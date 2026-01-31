#include "Managers/Collision/DynamicCollisionController.hpp"
#include "Managers/Collision/DynamicCollisionUtils.hpp"
#include "Config/Config.hpp"

namespace {

	const float* gWorldScaleInverse = reinterpret_cast<float*>(RE::Offset::Havok::WorldScaleInverse.address());
	constexpr float maxUpdateScale = 50.0f;

	//TODO Some actors have less than 18 vertices, need to account for that
	constexpr uint8_t Vertex_Top = 9;
	constexpr uint8_t Vertex_Bot = 8;
	constexpr std::array<uint8_t, 8> Vertex_RingTop = { 1, 3, 4,  5,  7, 11, 13, 16 };
	constexpr std::array<uint8_t, 8> Vertex_RingBot = { 0, 2, 6, 10, 12, 14, 15, 17 };

	//For Top Vertex
	constexpr std::string_view HeadBoneName = "NPC Head [Head]";

	//For Top Rings
	constexpr std::string_view UppderArmBoneRName = "NPC R UpperArm [RUar]";
	constexpr std::string_view UpperArmBoneLName = "NPC L UpperArm [LUar]";

	//For Bottom Rings
	constexpr std::string_view CalfBoneRName = "NPC R RearCalf [RrClf]";
	constexpr std::string_view CalfBoneLName = "NPC L RearCalf [LrClf]";

	//TODO Needs fixing,
	//Bottom Coords are negative Z Top ones are positive
	void CheckAndCorrectCollapsedVertexShape(std::vector<RE::hkVector4>& a_modVerts) {
		
		if (a_modVerts.size() == 18) {

			constexpr float buffer = 0.05f;

			const float topPos = a_modVerts[Vertex_Top].quad.m128_f32[2];
			const float botPos = a_modVerts[Vertex_Bot].quad.m128_f32[2];
			const float topRing = a_modVerts[1].quad.m128_f32[2];
			const float botRing = a_modVerts[0].quad.m128_f32[2];

			//Bottom Ring
			if (botRing - botPos < buffer) {
				for (uint8_t idx : Vertex_RingBot) {
					a_modVerts[idx].quad.m128_f32[2] = botPos + buffer;
				}
			}

			//Top Ring
			if (topRing - botRing < buffer) {
				for (uint8_t idx : Vertex_RingTop) {
					a_modVerts[idx].quad.m128_f32[2] = botRing + buffer;
				}
			}

			//Top Vertex
			if (topPos - topRing < buffer) {
				a_modVerts[Vertex_Top].quad.m128_f32[2] = topRing + buffer;
			}
		}
	}

	template <class T>
	T* Clone(T* a_object) {

		if (!a_object) {
			return nullptr;
		}

		RE::NiCloningProcess cloningProcess{};

		{
			static constexpr auto One = RE::NiPoint3(1.0f, 1.0f, 1.0f);

			uintptr_t cloneMap = reinterpret_cast<uintptr_t>(&cloningProcess.cloneMap);
			void** value1 = reinterpret_cast<void**>(cloneMap + 0x18);
			*value1 = reinterpret_cast<void*>(RELOCATION_ID(501133, 359452).address());;

			uintptr_t processMap = reinterpret_cast<uintptr_t>(&cloningProcess.processMap);
			void** value2 = reinterpret_cast<void**>(processMap + 0x18);
			*value2 = reinterpret_cast<void*>(RELOCATION_ID(501132, 359451).address());

			cloningProcess.copyType = 1;
			cloningProcess.appendChar = '$';
			cloningProcess.scale = One;
		}

		auto clone = reinterpret_cast<T*>(a_object->Clone(cloningProcess));
		return clone;
	}
}

namespace GTS {

	DynamicCollisionController::DynamicCollisionController(const ActorHandle& a_handle) {

		m_actor = a_handle;

		if (NiPointer<Actor> niActor = m_actor.get()) {
			if (Actor* actor = niActor.get()) {
				if (TESObjectCELL* cell = actor->GetParentCell()) {
					if (bhkWorld* world = cell->GetbhkWorld()) {

						//bhkCharacterController is the base class
						//bhkCharRigidBodyController is used by every actor except the player
						//the player has a bhkCharProxyController instead
						if (bhkCharacterController* controller = actor->GetCharController()) {

							BSWriteLockGuard lock(world->worldLock);

							//Disable bumper
							controller->bumperEnabled = true;
							const float height = actor->GetRelevantWaterHeight();
							controller->ToggleCharacterBumper(false);
							if (LOADED_REF_DATA* loadedData = actor->loadedData) {
								if (height > loadedData->relevantWaterHeight) {
									loadedData->relevantWaterHeight = height;
								}
							}

							m_originalData.maxSlope = GetControllerMaxSlope(controller);

							hkpConvexVerticesShape* convexShape = nullptr;
							std::vector<hkpCapsuleShape*> capsuleShapes = {};

							//Get all collider shapes and store a copy of their geometry
							//Used as a base reference for scaling
							if (GetShapes(controller, convexShape, capsuleShapes)) {

								if (convexShape) {

									//Make a copy of the original vertex collider vertex positions
									hkArray<hkVector4> Verteces{};
									convexShape->GetOriginalVertices(Verteces);

									m_originalData.convexVerteces.reserve(Verteces.size());
									for (const hkVector4& Vertex : Verteces) {
										m_originalData.convexVerteces.push_back(Vertex);
									}

									NiPoint3 originVert = HkVectorToNiPoint(Verteces[0]);
									originVert.z = 0.0f;
									m_originalData.convexShapeRadius = originVert.Length();
									m_originalData.hasVertecesShape = true;
								}

								if (!capsuleShapes.empty()) {
									for (hkpCapsuleShape* shape : capsuleShapes) {
										if (shape) {
											bool isBumper = false;
											if (bhkShape* userDat = shape->userData) {
												if (userDat->materialID == MATERIAL_ID::kCharacterBumper) {
													isBumper = true;
												}
											}
											m_originalData.capsules.push_back({
												shape->radius,
												shape->vertexA,
												shape->vertexB,
												isBumper,
											});
										}
									}
								}

								m_originalData.controllerActorHeight = controller->actorHeight;
								m_originalData.controllerActorScale = controller->scale;

								//NPC's Use bhkCharRigidBodyController, clone their capsule colliders as they are shared between all loaded actors with the same skeleton
								if (bhkCharRigidBodyController* RigidBodyController = skyrim_cast<bhkCharRigidBodyController*>(controller)){

									int8_t shapeIdx = 1;
									if (!RigidBodyController->shapes[shapeIdx]) shapeIdx = 0;
									if (!RigidBodyController->shapes[shapeIdx]) return;

									hkRefPtr rigidBody(static_cast<hkpCharacterRigidBody*>(RigidBodyController->characterRigidBody.referencedObject.get()));
									if (!rigidBody) {
										return;
									}

									if (hkpRigidBody* character = rigidBody->m_character) {
										if (const hkpShape* rootCollidableShape = character->collidable.shape) {
											// userData is an integer/pointer field; validate before constructing any NiPointer
											if (bhkShape* wrapperRaw = rootCollidableShape->userData) {
												NiPointer wrapper(wrapperRaw);
												if (wrapper) {
													// Havok shape the wrapper points at
													if (hkpShape* originalShape = static_cast<hkpShape*>(wrapper->referencedObject.get())) {
														// Deep clone (capsule leaf, list deep copy)
														if (hkpShape* clonedShape = DeepCloneShape(originalShape)) {
															// Clone the wrapper and bind it to the cloned Havok shape
															m_uniqueShape = NiPointer(Clone<bhkShape>(wrapper.get()));
															if (!m_uniqueShape) {
																clonedShape->RemoveReference(); // prevent leak if wrapper clone failed
																return;
															}

															m_uniqueShape->SetReferencedObject(clonedShape);

															// Install the cloned shape into the Havok character
															character->SetShape(clonedShape);

															// Replace controller wrapper entry
															RigidBodyController->shapes[shapeIdx]->DecRefCount();
															RigidBodyController->shapes[shapeIdx] = m_uniqueShape;

														}
													}
												}
											}
										}
									}
								}

								//Edge case for player. WW en VL form use capsules.
								if (bhkCharProxyController* ProxyController = skyrim_cast<bhkCharProxyController*>(controller)) {

									int8_t shapeIdx = 1;
									if (!ProxyController->shapes[shapeIdx]) shapeIdx = 0;
									if (!ProxyController->shapes[shapeIdx]) return;

									hkRefPtr charProxy(static_cast<hkpCharacterProxy*>(ProxyController->proxy.referencedObject.get()));
									if (!charProxy) {
										return;
									}

									hkpShapePhantom* phantom = charProxy->shapePhantom;
									if (!phantom) {
										return;
									}

									if (const hkpShape* rootCollidableShape = phantom->collidable.shape) {
										// userData is an integer/pointer field; validate before constructing any NiPointer
										if (bhkShape* wrapperRaw = rootCollidableShape->userData) {
											NiPointer wrapper(wrapperRaw);
											if (wrapper) {
												// Havok shape the wrapper points at
												if (hkpShape* originalShape = static_cast<hkpShape*>(wrapper->referencedObject.get())) {
													// Deep clone (capsule leaf, list deep copy)
													if (hkpShape* clonedShape = DeepCloneShape(originalShape)) {
														// Clone the wrapper and bind it to the cloned Havok shape
														m_uniqueShape = NiPointer(Clone<bhkShape>(wrapper.get()));
														if (!m_uniqueShape) {
															clonedShape->RemoveReference(); // prevent leak if wrapper clone failed
															return;
														}

														m_uniqueShape->SetReferencedObject(clonedShape);

														// Install the cloned shape into the Havok character
														phantom->SetShape(clonedShape);

														// Replace controller wrapper entry
														ProxyController->shapes[shapeIdx]->DecRefCount();
														ProxyController->shapes[shapeIdx] = m_uniqueShape;

													}
												}
											}
										}
									}
								}
							}
						}
					}
				}
			}
		}
	}

	void DynamicCollisionController::Update() {
		
		if (NiPointer<Actor> niTarget = m_actor.get()) {
			if (Actor* Target = niTarget.get()) {
				if (Target->Is3DLoaded()) {
					if (bhkCharacterController* controller = Target->GetCharController()) {

						m_currentVisualScale = get_visual_scale(Target);

						if (IsHuman(Target)) {
							if (Target->IsPlayerRef() || (Config::Collision.bEnableBoneDrivenCollisionUpdatesFollowers && IsTeammate(Target))) {
								AdjustBoneDrivenHuman();
								UpdateControllerScaleAndSlope(controller, m_originalData, m_currentVisualScale);

								m_lastVisualScale = 0.0f; // Set it to 0 to force an update if followers are switched to simple scaling

								if (Config::Collision.bDrawDebugShapes) {
									DrawCollisionShapes(Target, true);
								}

								return;
							}
						}

						if (ActorState* state = Target->AsActorState()) {

							const bool ShouldUpdate = abs(m_currentVisualScale - m_lastVisualScale) > 1e-4 ||
								(state->actorState1.swimming != m_lastActorState1.swimming || state->actorState1.sneaking != m_lastActorState1.sneaking);

							if (ShouldUpdate) {
								AdjustScale();
								UpdateControllerScaleAndSlope(controller, m_originalData, m_currentVisualScale);
							}
							if (Config::Collision.bDrawDebugShapes) {
								DrawCollisionShapes(Target, false);
							}

							m_lastActorState1 = state->actorState1;
						}
						m_lastVisualScale = m_currentVisualScale;
					}
				}
			}
		}
	}

	void DynamicCollisionController::AdjustBoneDrivenHuman() {
		
		GTS_PROFILE_SCOPE("DynamicCollisionController::AdjustBoneDrivenHuman");

		// Bone driven updates only work with convex vertex shape colliders
		if (!m_originalData.hasVertecesShape) return;

		if (NiPointer<Actor> ni_actor = m_actor.get()) {
			if (Actor* actor = ni_actor.get()) {
				if (bhkCharacterController* controller = actor->GetCharController()) {
					if (TESObjectCELL* cell = actor->GetParentCell()) {
						if (bhkWorld* world = cell->GetbhkWorld()) {

							std::vector<hkVector4> modifiedVerts = m_originalData.convexVerteces;
							const float& bottomZ = m_originalData.convexVerteces[Vertex_Bot].quad.m128_f32[2];
							const float vertexRingWidthMult = GetVerticesWidthMult(actor, true);
							//const float vertexRingHBoneDst = GetDistanceBetweenBones({ UppderArmBoneRName, UpperArmBoneLName });

							if (m_originalData.hasVertecesShape && m_originalData.convexVerteces.size() != 18) {
								logger::trace("Entity {} has unexpected vertex shape data size {}", actor->GetDisplayFullName(), m_originalData.convexVerteces.size());
							}

							if (m_originalData.convexVerteces.size() != 18) return;

							if (m_currentVisualScale > maxUpdateScale) {
								return; //Stop updating past this scale. While Havok can handle larger the larger shape causes massive lag due to all the collision checks.
							}

							/*if (vertexRingHBoneDst < 0.0f) {
								return;
							}*/

							// ---- Top vertex
							{
								NiAVObject* headBone = FindBone(HeadBoneName);
								if (!headBone) return;

								const NiPoint3 BonePos = (headBone->world.translate - actor->GetPosition()) / *gWorldScaleInverse;
								modifiedVerts[Vertex_Top].quad.m128_f32[2] = BonePos.z + bottomZ + (0.05f * m_currentVisualScale); //Bone position + small offset Correction
							}

							// ---- Upper Ring
							{
								std::vector<NiAVObject*> boneList = FindBones({UppderArmBoneRName, UpperArmBoneLName});
								if (boneList.empty()) return;

								float aggregateBoneZPos = 0.0f;

								for (NiAVObject* const& bone : boneList) {
									if (bone) aggregateBoneZPos += bone->world.translate.z;
								}
								aggregateBoneZPos = ((aggregateBoneZPos / static_cast<float>(boneList.size())) - actor->GetPosition().z) / *gWorldScaleInverse;

								// Adjust ring vertices
								if (aggregateBoneZPos >= 0.0f) {
									for (uint8_t idx : Vertex_RingTop) {
										modifiedVerts[idx] = ScaleRingWidth(m_originalData.convexVerteces[idx].quad, m_originalData.convexShapeRadius * vertexRingWidthMult, aggregateBoneZPos + bottomZ);
										//modifiedVerts[idx] = ScaleRingWidth(modifiedVerts[idx].quad, (vertexRingHBoneDst / *gWorldScaleInverse) * vertexRingWidthMult, aggregateBoneZPos + bottomZ);
									}
								}
								
							}

							// ---- Lower Ring
							{
								std::vector<NiAVObject*> boneList = FindBones({CalfBoneRName, CalfBoneLName});

								if (boneList.empty()) return;

								float aggregateBoneZPos = 0.0f;
	
								for (NiAVObject* const& bone : boneList) {
									if (bone) aggregateBoneZPos += bone->world.translate.z;
								}
								aggregateBoneZPos = ((aggregateBoneZPos / static_cast<float>(boneList.size())) - actor->GetPosition().z) / *gWorldScaleInverse;

								// Adjust ring vertices
								if (aggregateBoneZPos >= 0.0f) {
									for (uint8_t idx : Vertex_RingBot) {
										modifiedVerts[idx] = ScaleRingWidth(m_originalData.convexVerteces[idx].quad, m_originalData.convexShapeRadius * vertexRingWidthMult, aggregateBoneZPos + bottomZ);
										//modifiedVerts[idx] = ScaleRingWidth(modifiedVerts[idx].quad, (vertexRingHBoneDst / *gWorldScaleInverse) * vertexRingWidthMult, aggregateBoneZPos + bottomZ);
									}
								}
							}

							//CheckAndCorrectCollapsedVertexShape(modifiedVerts);

							// Set new shape
							{
								BSWriteLockGuard lock(world->worldLock);
								SetNewVerticesShape(controller, modifiedVerts);
							}

							// ---- Bumper Capsule
							{
								// NPC's always have a verteces shape + unique capsule bumper. If the size somehow is not 1, then its not an npc.
								if (m_originalData.capsules.size() != 1) return; // Actor has no capsules to update
								if (!m_uniqueShape.get()) return;
								std::vector<hkpCapsuleShape*> CurrentCapsules{};

								{
									BSReadLockGuard lock(world->worldLock);
									if (!GetCapsulesFromShape(m_uniqueShape.get(), CurrentCapsules)) return; //Should always be 1.
									if (CurrentCapsules.size() != m_originalData.capsules.size())  return;
								}

								{
									BSWriteLockGuard lock(world->worldLock);
									for (size_t i = 0; i < CurrentCapsules.size(); ++i) {
										ScaleCapsule(m_originalData.capsules[i], CurrentCapsules[i], m_currentVisualScale);
									}
								}
							}
						}
					}
				}
			}
		}
	}

	void DynamicCollisionController::AdjustScale() const {

		GTS_PROFILE_SCOPE("DynamicCollisionController::AdjustScale");

		if (NiPointer<Actor> ni_actor = m_actor.get()) {
			if (Actor* actor = ni_actor.get()) {
				if (bhkCharacterController* controller = actor->GetCharController()) {
					if (TESObjectCELL* cell = actor->GetParentCell()) {
						if (bhkWorld* world = cell->GetbhkWorld()) {

							std::vector<hkVector4> modifiedVerts = m_originalData.convexVerteces;
							const float fClampedScale = std::clamp(m_currentVisualScale, Config::Collision.fMSimpleDrivenColliderMinScale, Config::Collision.fMSimpleDrivenColliderMaxScale);

							if (m_currentVisualScale > maxUpdateScale) {
								return; //Stop updating past this scale. While Havok can handle larger the larger shape causes massive lag due to all the collision checks.
							}

							if (m_originalData.hasVertecesShape && m_originalData.convexVerteces.size() != 18) {
								logger::trace("Entity {} has unexpected vertex shape data size {}", actor->GetDisplayFullName(), m_originalData.convexVerteces.size());
							}

							// ---- Vertex Shape | Some Creatures also use vertex shapes
							if (m_originalData.hasVertecesShape && m_originalData.convexVerteces.size() == 18) {
								float widthMult = GetVerticesWidthMult(actor, false) * fClampedScale;

								float heightMult = 1.0f;
								if (AnimationVars::Crawl::IsCrawling(actor) && Runtime::HasKeyword(actor, Runtime::KYWD.ActorTypeNPC)) {
									heightMult = Config::Collision.fSimpleDrivenHeightMultCrawling;
								}
								else if (actor->IsSneaking()) {
									heightMult = Config::Collision.fSimpleDrivenHeightMultSneaking;
								}
								else if (actor->AsActorState()->IsSwimming()) {
									heightMult = Config::Collision.fSimpleDrivenHeightMultSwimming;
								}

								const float sZ = fClampedScale * heightMult;
								const float& zB0 = m_originalData.convexVerteces[Vertex_Bot].quad.m128_f32[2];
								auto ScaleZFromBottom = [&](float z0) -> float {return zB0 + (z0 - zB0) * sZ; };

								// ---- Top vertex
								modifiedVerts[Vertex_Top].quad.m128_f32[2] = ScaleZFromBottom(m_originalData.convexVerteces[Vertex_Top].quad.m128_f32[2]);

								// ---- Upper ring
								for (uint8_t idx : Vertex_RingTop) {
									const float z1 = ScaleZFromBottom(m_originalData.convexVerteces[idx].quad.m128_f32[2]);
									modifiedVerts[idx] = ScaleRingWidth(m_originalData.convexVerteces[idx].quad, m_originalData.convexShapeRadius * widthMult, z1);
								}

								// ---- Lower ring
								for (uint8_t idx : Vertex_RingBot) {
									const float z1 = ScaleZFromBottom(m_originalData.convexVerteces[idx].quad.m128_f32[2]);
									modifiedVerts[idx] = ScaleRingWidth(m_originalData.convexVerteces[idx].quad, m_originalData.convexShapeRadius * widthMult, z1);
								}

								//CheckAndCorrectCollapsedVertexShape(modifiedVerts);

								// Set new shape
								{
									BSWriteLockGuard lock(world->worldLock);
									SetNewVerticesShape(controller, modifiedVerts);
								}
							}

							// ---- Capsule Shapes
							{

								if (m_originalData.capsules.empty()) return; // Actor has no capsules to update
								if (!m_uniqueShape.get()) return;
								std::vector<hkpCapsuleShape*> CurrentCapsules{};
								CurrentCapsules.reserve(6); //Some actors have up to 6 capsules

								{
									BSReadLockGuard lock(world->worldLock);
									if (!GetCapsulesFromShape(m_uniqueShape.get(), CurrentCapsules)) return; //Should always be 1.
									if (CurrentCapsules.size() != m_originalData.capsules.size())  return;
								}

								{
									BSWriteLockGuard lock(world->worldLock);
									for (size_t i = 0; i < CurrentCapsules.size(); ++i) {
										ScaleCapsule(m_originalData.capsules[i], CurrentCapsules[i], fClampedScale);
									}
								}
							}
						}
					}
				}
			}
		}
	}

	NiAVObject* DynamicCollisionController::FindBone(const std::string_view& a_name) {

		if (NiPointer<Actor> niActor = m_actor.get()) {
			if (Actor* actor = niActor.get()) {
				//// Try to get from cached bones first
				//auto it = m_cachedBones.find(a_name);
				//if (it != m_cachedBones.end()) {
				//	return it->second;
				//}

				// Find bone and cache it
				if (NiAVObject* bone = find_node(actor, a_name, false)) {
					//m_cachedBones[a_name] = bone;
					return bone;
				}
			}
		}
		return nullptr;
	}

	std::vector<NiAVObject*> DynamicCollisionController::FindBones(const std::vector<std::string_view>& a_names) {

		std::vector<NiAVObject*> boneList = {};
		boneList.reserve(2); //Usually 2

		for (const std::string_view& boneName : a_names) {
			if (NiAVObject* Bone = FindBone(boneName)) {
				boneList.push_back(Bone);
			}
		}

		return boneList;
	}

	float DynamicCollisionController::GetDistanceBetweenBones(const std::pair<std::string_view, std::string_view>& a_names) {
		if (NiAVObject* boneA = FindBone(a_names.first)) {
			if (NiAVObject* boneB = FindBone(a_names.second)) {
				return abs(boneA->world.translate.GetDistance(boneB->world.translate)) / 2.f;
			}
		}
		return -1.0f;
	}

	void DynamicCollisionController::ScaleCapsule(const CapsuleData& a_baseCapsule, hkpCapsuleShape* a_outCapsule, float a_scaleFactor) {

		if (!a_outCapsule) return;

		const float& r0 = a_baseCapsule.radius;

		// Original endpoints
		const float& a0x = a_baseCapsule.vertexA.quad.m128_f32[0];
		const float& a0y = a_baseCapsule.vertexA.quad.m128_f32[1];
		const float& a0z = a_baseCapsule.vertexA.quad.m128_f32[2];
		const float& b0x = a_baseCapsule.vertexB.quad.m128_f32[0];
		const float& b0y = a_baseCapsule.vertexB.quad.m128_f32[1];
		const float& b0z = a_baseCapsule.vertexB.quad.m128_f32[2];

		const float bottom0 = std::min(a0z, b0z) - r0;

		// Scale radius + endpoints about origin
		const float r1 = r0 * a_scaleFactor;

		const float a1x = a0x * a_scaleFactor;
		const float a1y = a0y * a_scaleFactor;
		float a1z = a0z * a_scaleFactor;

		const float b1x = b0x * a_scaleFactor;
		const float b1y = b0y * a_scaleFactor;
		float b1z = b0z * a_scaleFactor;

		if (a_baseCapsule.isBumper) {
			const float bottom1 = std::min(a1z, b1z) - r1;
			const float dz = bottom0 - bottom1;

			a1z += dz;
			b1z += dz;
		}

		a_outCapsule->radius = r1;
		a_outCapsule->vertexA.quad.m128_f32[0] = a1x;
		a_outCapsule->vertexA.quad.m128_f32[1] = a1y;
		a_outCapsule->vertexA.quad.m128_f32[2] = a1z;
		a_outCapsule->vertexB.quad.m128_f32[0] = b1x;
		a_outCapsule->vertexB.quad.m128_f32[1] = b1y;
		a_outCapsule->vertexB.quad.m128_f32[2] = b1z;
	}

	void DynamicCollisionController::UpdateControllerScaleAndSlope(bhkCharacterController* a_controller, const ShapeData& a_origData, float a_currentScale) {
		constexpr float maxSlopeAtScale = 5.0f;
		const float normalizedScale = std::clamp((a_currentScale - 1.0f) / maxSlopeAtScale, 0.0f, 1.0f);
		const float newSlope = std::lerp(a_origData.maxSlope, 89.0f, normalizedScale);

		a_controller->actorHeight = a_origData.controllerActorHeight * a_currentScale;
		a_controller->scale = a_currentScale;
		SetControllerMaxSlope(a_controller, newSlope);
	}

}