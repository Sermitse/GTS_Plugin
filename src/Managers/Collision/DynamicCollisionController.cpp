#include "Managers/Collision/DynamicCollisionController.hpp"
#include "Managers/Collision/DynamicCollisionUtils.hpp"
#include "Config/Config.hpp"

namespace {

	const float* gWorldScaleInverse = reinterpret_cast<float*>(RE::Offset::Havok::WorldScaleInverse.address());

	constexpr uint8_t Vertex18_Top = 9;
	constexpr uint8_t Vertex18_Bot = 8;
	constexpr std::array<uint8_t, 8> Vertex18_RingTop = { 1, 3, 4,  5,  7, 11, 13, 16 };
	constexpr std::array<uint8_t, 8> Vertex18_RingBot = { 0, 2, 6, 10, 12, 14, 15, 17 };

	constexpr uint8_t Vertex17_Bot = 8;
	constexpr std::array<uint8_t, 8> Vertex17_RingTop = { 1, 4, 12, 7, 3, 15, 5, 10 };
	constexpr std::array<uint8_t, 8> Vertex17_RingBot = { 0, 2, 11, 6, 14, 16, 13, 9 };

	//For Top Vertex
	constexpr std::string_view HeadBoneName = "NPC Head [Head]";

	//For Top Rings
	constexpr std::string_view UppderArmBoneRName = "NPC R UpperArm [RUar]";
	constexpr std::string_view UpperArmBoneLName = "NPC L UpperArm [LUar]";

	//For Bottom Rings
	constexpr std::string_view CalfBoneRName = "NPC R RearCalf [RrClf]";
	constexpr std::string_view CalfBoneLName = "NPC L RearCalf [LrClf]";

	float& Z(RE::hkVector4& v) noexcept { return v.quad.m128_f32[2]; }

	template <class Range>
	bool AllInRange(std::size_t n, const Range& idxs) noexcept {
		for (auto i : idxs) {
			if (static_cast<std::size_t>(i) >= n) return false;
		}
		return true;
	}

	void CheckAndCorrectCollapsedVertexShape(std::vector<RE::hkVector4>& a_modVerts)
	{
		const std::size_t n = a_modVerts.size();

		if (n == 18) {
			if (static_cast<std::size_t>(Vertex18_Top) >= n || static_cast<std::size_t>(Vertex18_Bot) >= n) return;
			if (!AllInRange(n, Vertex18_RingTop) || !AllInRange(n, Vertex18_RingBot))                       return;

			const float botPos = Z(a_modVerts[Vertex18_Bot]);

			// Nothing may be at or below botPos
			if (Z(a_modVerts[Vertex18_Top]) <= botPos) Z(a_modVerts[Vertex18_Top]) = botPos + 0.003f;
			for (uint8_t idx : Vertex18_RingTop) if (Z(a_modVerts[idx]) <= botPos) Z(a_modVerts[idx]) = botPos + 0.002f;
			for (uint8_t idx : Vertex18_RingBot) if (Z(a_modVerts[idx]) <= botPos) Z(a_modVerts[idx]) = botPos + 0.001f;

			const float topPos = Z(a_modVerts[Vertex18_Top]);

			// topRing must be below topPos
			for (uint8_t idx : Vertex18_RingTop)
				if (Z(a_modVerts[idx]) >= topPos) Z(a_modVerts[idx]) = topPos - 0.001f;

			// botRing must be below topRing
			float topRingMin = +FLT_MAX;
			for (uint8_t idx : Vertex18_RingTop) topRingMin = std::min(topRingMin, Z(a_modVerts[idx]));
			for (uint8_t idx : Vertex18_RingBot)
				if (Z(a_modVerts[idx]) >= topRingMin) Z(a_modVerts[idx]) = topRingMin - 0.001f;
		}
		else if (n == 17) {
			if (static_cast<std::size_t>(Vertex17_Bot) >= n) return;
			if (!AllInRange(n, Vertex17_RingTop) || !AllInRange(n, Vertex17_RingBot)) return;

			const float botPos = Z(a_modVerts[Vertex17_Bot]);

			// Nothing may be at or below botPos
			for (uint8_t idx : Vertex17_RingTop) if (Z(a_modVerts[idx]) <= botPos) Z(a_modVerts[idx]) = botPos + 0.002f;
			for (uint8_t idx : Vertex17_RingBot) if (Z(a_modVerts[idx]) <= botPos) Z(a_modVerts[idx]) = botPos + 0.001f;

			// botRing must be below topRing
			float topRingMin = +FLT_MAX;
			for (uint8_t idx : Vertex17_RingTop) topRingMin = std::min(topRingMin, Z(a_modVerts[idx]));
			for (uint8_t idx : Vertex17_RingBot)
				if (Z(a_modVerts[idx]) >= topRingMin) Z(a_modVerts[idx]) = topRingMin - 0.001f;
		}
	}

	template <class T>
	T* Clone(T* a_object, RE::NiPoint3 a_scale = RE::NiPoint3(1.0f, 1.0f, 1.0f)) {

		if (!a_object) {
			return nullptr;
		}

		RE::NiCloningProcess cloningProcess{};

		{

			uintptr_t cloneMap = reinterpret_cast<uintptr_t>(&cloningProcess.cloneMap);
			void** value1 = reinterpret_cast<void**>(cloneMap + 0x18);
			*value1 = reinterpret_cast<void*>(Hooks::RelocationIDEx(501133, 359452, 359452, NULL).address());

			uintptr_t processMap = reinterpret_cast<uintptr_t>(&cloningProcess.processMap);
			void** value2 = reinterpret_cast<void**>(processMap + 0x18);
			*value2 = reinterpret_cast<void*>(Hooks::RelocationIDEx(501132, 359451, 359451, NULL).address());

			cloningProcess.copyType = 1;
			cloningProcess.appendChar = '$';
			cloningProcess.scale = a_scale;
		}

		// Clone() hands back an NiObject*, and nothing guarantees the clone really is
		// a T. netimmerse_cast walks the NiRTTI chain and yields nullptr on mismatch
		// instead of a mistyped pointer.
		auto clone = netimmerse_cast<T*>(a_object->Clone(cloningProcess));
		return clone;
	}
}

namespace GTS {

	DynamicCollisionController::DynamicCollisionController(const RE::ActorHandle& a_handle, const bhkCharacterController* a_controller, bool a_hasInSkeletonCollision) {

		m_actor = a_handle;
		m_controller = a_controller;

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

							m_originalData.controllerKind = GetControllerKind(controller);
							m_originalData.maxSlopeDegrees = GetControllerMaxSlope(controller);
							m_originalData.maxSlopeRaw = controller->maxSlope;
							m_originalData.stepHeight = GetControllerStepHeight(controller);
							m_originalData.stepReach = GetControllerStepReach(controller);

							if (const hkpCharacterRigidBody* rigidBody = GetControllerRigidBody(controller, m_originalData.controllerKind)) {
								m_originalData.rigidBodyMaxSlopeCosine = rigidBody->m_maxSlopeCosine;
							}
							else if (const hkpCharacterProxy* proxy = GetControllerProxy(controller, m_originalData.controllerKind)) {
								m_originalData.proxyMaxSolverSpeed = proxy->maxCharacterSpeedForSolver;
							}

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

								//Fixes edge cases for creatures whom don't have an i- skeleton collision shape.
								//As the game DOES correctly scale them on character init.
								//Because of this, this normaly ends up double scaling the collider so it needs to be accounted for.
								//We only care for actors which don't have an in skeleton shape and don't use a hkpConvexVerticesShape as well.
								//Creatures who have a ConvexShape appear to not get scaled
								//We effectively normalize scale here by premtively shrinking the nodes based on the inverse of visual_scale.
								float capsuleBaseScale = 1.0f;
								if (!a_hasInSkeletonCollision && !convexShape && Runtime::HasKeyword(actor, Runtime::KYWD.CreatureKeyword)) {
									const float InitialScale = get_3d_scale(actor); //get_visual_scale is not yet ready at this point due to ModScale's caching system, so get the values directly.
									capsuleBaseScale = (InitialScale > 1e-4f) ? (1.0f / InitialScale) : 1.0f;
								}
								const __m128 capsuleBaseScaleV = _mm_set_ps(0.0f, capsuleBaseScale, capsuleBaseScale, capsuleBaseScale); // leave W=0

								for (hkpCapsuleShape* shape : capsuleShapes) {
									if (shape) {
										bool isBumper = false;
										if (bhkShape* userDat = shape->userData) {
											if (userDat->materialID == MATERIAL_ID::kCharacterBumper) {
												isBumper = true;
											}
										}

										m_originalData.capsules.push_back({
											shape->radius * capsuleBaseScale,
											hkVector4{ _mm_mul_ps(shape->vertexA.quad, capsuleBaseScaleV) },
											hkVector4{ _mm_mul_ps(shape->vertexB.quad, capsuleBaseScaleV) },
											isBumper,
										});
									}
								}

								m_originalData.controllerActorHeight = controller->actorHeight;
								m_originalData.controllerActorScale = controller->scale;

								//---------------------- NPC'S With Capsule(List)/MOPP Shapes

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
														// Deep clone (capsule leaf, list deep copy). Any other root, like a bare hull, is the
														// controller's own already (the game builds the hull per controller), and wrapping it again
														// would leave two wrappers fighting over its userData.
														if (hkpShape* clonedShape = IsDeepCloneable(originalShape) ? DeepCloneShape(originalShape) : nullptr) {
															// Clone the wrapper and bind it to the cloned Havok shape
															m_uniqueShape = NiPointer(Clone<bhkShape>(wrapper.get()));
															if (!m_uniqueShape) {
																clonedShape->RemoveReference(); // prevent leak if wrapper clone failed
																return;
															}

															m_uniqueShape->SetReferencedObject(clonedShape);

															// Install the cloned shape into the Havok character
															character->SetShape(clonedShape);
															//No manual DecRefCount: the NiPointer assignment below already releases the old wrapper.
															//RigidBodyController->shapes[shapeIdx]->DecRefCount();
															RigidBodyController->shapes[shapeIdx] = m_uniqueShape;

															//The wrapper and the body or phantom each took their own reference, so ours is released.
															clonedShape->RemoveReference();

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
										// userData is an integer/pointer field. Validate before constructing any NiPointer
										if (bhkShape* wrapperRaw = rootCollidableShape->userData) {
											NiPointer wrapper(wrapperRaw);
											if (wrapper) {
												// Havok shape the wrapper points at
												if (hkpShape* originalShape = static_cast<hkpShape*>(wrapper->referencedObject.get())) {
													// Deep clone (capsule leaf, list deep copy). Any other root, like a bare hull, is the
													// controller's own already (the game builds the hull per controller), and wrapping it again
													// would leave two wrappers fighting over its userData.
													if (hkpShape* clonedShape = IsDeepCloneable(originalShape) ? DeepCloneShape(originalShape) : nullptr) {
														// Clone the wrapper and bind it to the cloned Havok shape
														m_uniqueShape = NiPointer(Clone<bhkShape>(wrapper.get()));
														if (!m_uniqueShape) {
															clonedShape->RemoveReference(); // prevent leak if wrapper clone failed
															return;
														}

														m_uniqueShape->SetReferencedObject(clonedShape);

														// Install the cloned shape into the Havok character
														phantom->SetShape(clonedShape);

														//No manual DecRefCount: the NiPointer assignment below already releases the old wrapper.
														//ProxyController->shapes[shapeIdx]->DecRefCount();
														ProxyController->shapes[shapeIdx] = m_uniqueShape;

														//The wrapper and the body or phantom each took their own reference, so ours is released.
														clonedShape->RemoveReference();

													}
												}
											}
										}
									}
								}

							}
							logger::trace("DCC .ctor on 0x{:X} | {} | HasSkeletonShapes: {} | IsCreature: {} | GameScale: {} | VisualScale: {} | HasConvexShape: {}", actor->formID, actor->GetDisplayFullName(), a_hasInSkeletonCollision, Runtime::HasKeyword(actor, Runtime::KYWD.CreatureKeyword), actor->GetBaseHeight(), get_3d_scale(actor), convexShape != nullptr);
						}
					}
				}

			}
		}

		

	}

	void DynamicCollisionController::MarkDestroyed() {
		m_destroyed.store(true);
	}

	void DynamicCollisionController::Update() {

		if (m_destroyed.load()) {
			return;
		}

		if (NiPointer<Actor> niTarget = m_actor.get()) {
			if (Actor* Target = niTarget.get()) {
				if (Target->Is3DLoaded()) {

					//Holding a reference keeps the controller alive for this update, wherever its last owner lets go.
					//The identity check keeps this instance off a controller that replaced the one it was built for.
					NiPointer<bhkCharacterController> controllerRef{ Target->GetCharController() };
					bhkCharacterController* controller = controllerRef.get();

					//Player and Bone driven ActorTypeNPC followers
					if (controller && controller == m_controller) {

						m_currentVisualScale = get_visual_scale(Target);

						//Havok's step reads these fields on its own threads, so they are written under the world lock,
						//and only when they differ.
						const auto updateControllerFields = [&](bool a_updateScale) {
							const ControllerFields wanted = GetWantedFields(a_updateScale && Target->IsPlayerRef());

							if (!FieldsMatch(controller, wanted)) {
								TESObjectCELL* cell = Target->GetParentCell();
								bhkWorld* world = cell ? cell->GetbhkWorld() : nullptr;
								if (!world) {
									return;
								}

								BSWriteLockGuard lock(world->worldLock);
								WriteFields(controller, wanted);
							}

							if (wanted.writeTraversal) {
								m_traversalApplied = Config::Collision.bScaleTraversal;
							}
						};

						if (m_originalData.hasVertecesShape && m_originalData.convexVerteces.size() == 18) { // Bone driven updates only for humanoids with 18-vertex shapes

							if (IsHuman(Target)) {
								if ((Target->IsPlayerRef() && Target->GetActorBase()->GetSex() != SEX::kMale) || (Config::Collision.bEnableBoneDrivenCollisionUpdatesFollowers && IsTeammate(Target))) {

									AdjustBoneDrivenHuman(); // Clamped inside function
									updateControllerFields(true);
									m_lastVisualScale = 0.0f; // Set it to 0 to force an update if followers are switched to simple scaling

									if (Config::Collision.bDrawDebugShapes) {
										DrawCollisionShapes(Target, true);
									}

									return;
								}
							}
						}

						//Everything else
						if (ActorState* state = Target->AsActorState()) {

							const bool ShouldUpdate = abs(m_currentVisualScale - m_lastVisualScale) > 1e-4 ||
								(state->actorState1.swimming != m_lastActorState1.swimming || state->actorState1.sneaking != m_lastActorState1.sneaking);

							if (ShouldUpdate) {
								AdjustScale(); // It's clamped by Config::Collision.fMSimpleDrivenColliderMaxScale anyway
							}

							updateControllerFields(ShouldUpdate);

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

	void DynamicCollisionController::AdjustBoneDrivenHuman() const {
		
		GTS_PROFILE_SCOPE("DynamicCollisionController::AdjustBoneDrivenHuman");

		//Stop updating past a certain scale, the shape breaks below around 0.15, just clamp it a bit higher.
		const float clampedScale = std::clamp(m_currentVisualScale, 0.20f, Config::Collision.fDynamicColliderMaxUpdateScale);

		// Bone driven updates only work with convex vertex shape colliders
		if (!m_originalData.hasVertecesShape) return;

		if (NiPointer<Actor> ni_actor = m_actor.get()) {
			if (Actor* actor = ni_actor.get()) {
				if (bhkCharacterController* controller = actor->GetCharController()) {
					if (TESObjectCELL* cell = actor->GetParentCell()) {
						if (bhkWorld* world = cell->GetbhkWorld()) {

							std::vector<hkVector4> modifiedVerts = m_originalData.convexVerteces;
							const float& bottomZ = m_originalData.convexVerteces[Vertex18_Bot].quad.m128_f32[2];
							const float vertexRingWidthMult = GetVerticesWidthMult(actor, true) * clampedScale;
							//const float vertexRingHBoneDst = GetDistanceBetweenBones({ UppderArmBoneRName, UpperArmBoneLName });

							/*if (m_originalData.hasVertecesShape && m_originalData.convexVerteces.size() != 18) {
								logger::trace("Actor {} has unexpected vertex shape data count {}", actor->GetDisplayFullName(), m_originalData.convexVerteces.size());
							}*/

							/*if (vertexRingHBoneDst < 0.0f) {
								return;
							}*/

							// ---- Top vertex
							{
								NiAVObject* headBone = FindBone(HeadBoneName);
								if (!headBone) return;

								const NiPoint3 BonePos = (headBone->world.translate - actor->GetPosition()) / *gWorldScaleInverse;
								modifiedVerts[Vertex18_Top].quad.m128_f32[2] = BonePos.z + bottomZ + (0.05f * clampedScale); //Bone position + small offset Correction
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
									for (uint8_t idx : Vertex18_RingTop) {
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
									for (uint8_t idx : Vertex18_RingBot) {
										modifiedVerts[idx] = ScaleRingWidth(m_originalData.convexVerteces[idx].quad, m_originalData.convexShapeRadius * vertexRingWidthMult, aggregateBoneZPos + bottomZ);
										//modifiedVerts[idx] = ScaleRingWidth(modifiedVerts[idx].quad, (vertexRingHBoneDst / *gWorldScaleInverse) * vertexRingWidthMult, aggregateBoneZPos + bottomZ);
									}
								}
							}

							CheckAndCorrectCollapsedVertexShape(modifiedVerts);

							// Set new shape
							{
								BSWriteLockGuard lock(world->worldLock);
								m_ownedHull = SetNewVerticesShape(controller, modifiedVerts, GetScaledConvexRadius(m_currentVisualScale), m_ownedHull);
								if (auto tranData = Transient::GetActorData(actor)) {
									tranData->cachedConvexVerticesShape = modifiedVerts;
								}
							}

							// ---- Bumper Capsule
							{
								// NPC's always have a verteces shape + unique capsule bumper. If the size somehow is not 1, then its not an npc.
								if (m_originalData.capsules.size() != 1) return; // Actor has no capsules to update
								if (!m_uniqueShape.get()) return;
								std::vector<hkpCapsuleShape*> CurrentCapsules{};


								{
									BSWriteLockGuard lock(world->worldLock);
									if (!GetCapsulesFromShape(m_uniqueShape.get(), CurrentCapsules)) return; //Should always be 1.
									if (CurrentCapsules.size() != m_originalData.capsules.size())  return;

									for (size_t i = 0; i < CurrentCapsules.size(); ++i) {
										ScaleCapsule(m_originalData.capsules[i], CurrentCapsules[i], clampedScale);
									}
									RebuildMopps(controller, clampedScale);
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

							/*if (m_originalData.hasVertecesShape && m_originalData.convexVerteces.size() != 18) {
								logger::trace("Actor {} has unexpected vertex shape data count {}", actor->GetDisplayFullName(), m_originalData.convexVerteces.size());
							}*/

							// ---- Vertex Shape | Some Creatures also use vertex shapes
							if (m_originalData.hasVertecesShape) {

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

								if (m_originalData.convexVerteces.size() == 18) {

									const float& zB0 = m_originalData.convexVerteces[Vertex18_Bot].quad.m128_f32[2];
									auto ScaleZFromBottom = [&](float z0) -> float {return zB0 + (z0 - zB0) * sZ; };

									// ---- Top vertex
									modifiedVerts[Vertex18_Top].quad.m128_f32[2] = ScaleZFromBottom(m_originalData.convexVerteces[Vertex18_Top].quad.m128_f32[2]);

									// ---- Upper ring
									for (uint8_t idx : Vertex18_RingTop) {
										const float z1 = ScaleZFromBottom(m_originalData.convexVerteces[idx].quad.m128_f32[2]);
										modifiedVerts[idx] = ScaleRingWidth(m_originalData.convexVerteces[idx].quad, m_originalData.convexShapeRadius * widthMult, z1);
									}

									// ---- Lower ring
									for (uint8_t idx : Vertex18_RingBot) {
										const float z1 = ScaleZFromBottom(m_originalData.convexVerteces[idx].quad.m128_f32[2]);
										modifiedVerts[idx] = ScaleRingWidth(m_originalData.convexVerteces[idx].quad, m_originalData.convexShapeRadius * widthMult, z1);
									}
								}
								else if (m_originalData.convexVerteces.size() == 17) {

									const float& zB0 = m_originalData.convexVerteces[Vertex17_Bot].quad.m128_f32[2];
									auto ScaleZFromBottom = [&](float z0) -> float {return zB0 + (z0 - zB0) * sZ; };

									// ---- Upper ring
									for (uint8_t idx : Vertex17_RingTop) {
										const float z1 = ScaleZFromBottom(m_originalData.convexVerteces[idx].quad.m128_f32[2]);
										modifiedVerts[idx] = ScaleRingWidth(m_originalData.convexVerteces[idx].quad, m_originalData.convexShapeRadius * widthMult, z1);
									}

									// ---- Lower ring
									for (uint8_t idx : Vertex17_RingBot) {
										const float z1 = ScaleZFromBottom(m_originalData.convexVerteces[idx].quad.m128_f32[2]);
										modifiedVerts[idx] = ScaleRingWidth(m_originalData.convexVerteces[idx].quad, m_originalData.convexShapeRadius * widthMult, z1);
									}
								}

								CheckAndCorrectCollapsedVertexShape(modifiedVerts);

								// Set new shape
								{
									BSWriteLockGuard lock(world->worldLock);
									m_ownedHull = SetNewVerticesShape(controller, modifiedVerts, GetScaledConvexRadius(m_currentVisualScale), m_ownedHull);
									if (auto tranData = Transient::GetActorData(actor)) {
										tranData->cachedConvexVerticesShape = modifiedVerts;
									}
								}
							}

							// ---- Capsule Shapes
							{

								if (m_originalData.capsules.empty()) return; // Actor has no capsules to update
								if (!m_uniqueShape.get()) return;
								std::vector<hkpCapsuleShape*> CurrentCapsules{};
								CurrentCapsules.reserve(6); //Some actors have up to 6 capsules

								{
									BSWriteLockGuard lock(world->worldLock);
									if (!GetCapsulesFromShape(m_uniqueShape.get(), CurrentCapsules)) return; //Should always be 1.
									if (CurrentCapsules.size() != m_originalData.capsules.size())  return;

									for (size_t i = 0; i < CurrentCapsules.size(); ++i) {
										ScaleCapsule(m_originalData.capsules[i], CurrentCapsules[i], fClampedScale);
									}
									RebuildMopps(controller, fClampedScale);
								}
							}
						}
					}
				}
			}
		}
		
	}

	NiAVObject* DynamicCollisionController::FindBone(const std::string_view& a_name) const {

		if (NiPointer<Actor> niActor = m_actor.get()) {
			if (Actor* actor = niActor.get()) {
				//Disabled for now, don't know if stale pointers are an issue here
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

	std::vector<NiAVObject*> DynamicCollisionController::FindBones(const std::vector<std::string_view>& a_names) const {

		std::vector<NiAVObject*> boneList = {};
		boneList.reserve(2); //Usually 2

		for (const std::string_view& boneName : a_names) {
			if (NiAVObject* Bone = FindBone(boneName)) {
				boneList.push_back(Bone);
			}
		}

		return boneList;
	}

	float DynamicCollisionController::GetDistanceBetweenBones(const std::pair<std::string_view, std::string_view>& a_names) const {
		if (NiAVObject* boneA = FindBone(a_names.first)) {
			if (NiAVObject* boneB = FindBone(a_names.second)) {
				return abs(boneA->world.translate.GetDistance(boneB->world.translate)) / 2.f;
			}
		}
		return -1.0f;
	}

	void DynamicCollisionController::ScaleCapsule(const CapsuleData& a_baseCapsule, hkpCapsuleShape* a_outCapsule, float a_scaleFactor) {

		if (!a_outCapsule) return;

		const float r0 = a_baseCapsule.radius;
		const float bottom0 = std::min(a_baseCapsule.vertexA.quad.m128_f32[2], a_baseCapsule.vertexB.quad.m128_f32[2]) - r0;

		const __m128 scale = _mm_set1_ps(a_scaleFactor);

		// Scale both vertices in parallel: [x, y, z, w] * scaleFactor
		__m128 a1 = _mm_mul_ps(a_baseCapsule.vertexA.quad, scale);
		__m128 b1 = _mm_mul_ps(a_baseCapsule.vertexB.quad, scale);

		if (a_baseCapsule.isBumper) {
			const float r1 = r0 * a_scaleFactor;
			const float bottom1 = std::min(a1.m128_f32[2], b1.m128_f32[2]) - r1;
			const float dz = bottom0 - bottom1;

			// Add dz only to the Z lane: [0, 0, dz, 0]
			const __m128 dzVec = _mm_set_ps(0.0f, dz, 0.0f, 0.0f); // [w, z, y, x]
			a1 = _mm_add_ps(a1, dzVec);
			b1 = _mm_add_ps(b1, dzVec);
		}

		a_outCapsule->radius = r0 * a_scaleFactor;
		a_outCapsule->vertexA.quad = a1;
		a_outCapsule->vertexB.quad = b1;
	}


	// TODO: Check the performance implications of mopp recompiles.
	// The capsules are a function of the scale alone, so the trees only need compiling when it changed.
	//
	// UpdateShape is havok's notice that a shape changed in place. Checked on 
	// 1.7.104 that it skips a null modifier, recomputes the body's radius and invalidates its bounding volume.
	// It leaves agents alone, so a bv-tree agent keeps the child list it cached for the other body until that
	// body leaves its cached AABB.
	// TODO: A child that grew into an overlap is picked up late by up to that margin; SetShape(root) would reset the agents, at the cost of the contacts on every growing frame.
	// Phantoms have no UpdateShape (the slot only returns). The proxy's integrate rebuilds their AABB.
	void DynamicCollisionController::RebuildMopps(bhkCharacterController* a_controller, float a_scale) const {

		if (!m_uniqueShape) {
			return;
		}

		//Collected again whenever the wrapper's shape changes, so the pointers never outlive the shape they
		//point into. Holding the root keeps them valid until then.
		if (m_uniqueShape->referencedObject.get() != m_moppRoot.get()) {
			m_moppRoot = m_uniqueShape->referencedObject;
			m_moppShapes.clear();
			GetMoppShapes(static_cast<hkpShape*>(m_moppRoot.get()), m_moppShapes);
			m_moppBuiltScale = std::numeric_limits<float>::quiet_NaN();
		}

		if (m_moppShapes.empty() || a_scale == m_moppBuiltScale) {
			return;
		}

		for (hkpMoppBvTreeShape* mopp : m_moppShapes) {
			RebuildMoppCode(mopp);
		}
		m_moppBuiltScale = a_scale;

		if (hkpCharacterRigidBody* rigidBody = GetControllerRigidBody(a_controller, m_originalData.controllerKind)) {
			if (hkpRigidBody* character = rigidBody->m_character) {
				character->UpdateShape(nullptr);
			}
		}
	}

	DynamicCollisionController::ControllerFields DynamicCollisionController::GetWantedFields(bool a_writeScale) const {

		ControllerFields fields{};

		if (a_writeScale) {
			fields.writeScale = true;
			fields.actorHeight = m_originalData.controllerActorHeight * m_currentVisualScale;
			fields.scale = m_currentVisualScale;
		}

		if (Config::Collision.bScaleTraversal) {

			const float scale = std::clamp(m_currentVisualScale, 1.0f, std::max(1.0f, Config::Collision.fTraversalMaxScale));
			const float ramp = std::max(1e-4f, Config::Collision.fTraversalSlopeRampScale);
			const float normalizedScale = std::clamp((scale - 1.0f) / ramp, 0.0f, 1.0f);
			const float targetDegrees = std::clamp(Config::Collision.fTraversalMaxSlopeDegrees, 1.0f, 89.0f);
			const float degrees = std::lerp(m_originalData.maxSlopeDegrees, targetDegrees, normalizedScale);
			const float radians = degrees * (std::numbers::pi_v<float> / 180.0f);
			const float stepHeight = m_originalData.stepHeight * std::lerp(1.0f, scale, std::clamp(Config::Collision.fTraversalStepScaling, 0.0f, 1.0f));

			fields.writeTraversal = true;
			fields.maxSlopeRaw = EncodeControllerMaxSlope(degrees);
			fields.stepHeight = stepHeight;

			// The game derives reach from the slope once in the controller ctor, so a changed
			// slope leaves it stale unless we rewrite it too.
			fields.stepReach = stepHeight / std::tan(radians);

			// bhkCharacterController::maxSlope only reaches the proxy path. On the rigid body the
			// limit Bethesda left at havok's default 60 degrees is this one.
			fields.rigidBodyMaxSlopeCosine = std::cos(radians);

			// Above this the simplex solver stops solving planes together and lets the character
			// penetrate instead. A giant's normal walking speed clears the stock 10 on its own.
			// keepDistance is left alone on purpose. It is the gap the proxy holds from every
			// surface, so scaling it lifts the character off the ground by that much.
			fields.proxyMaxSolverSpeed = m_originalData.proxyMaxSolverSpeed * scale;
		}
		else if (m_traversalApplied) {
			fields.writeTraversal = true;
			fields.maxSlopeRaw = m_originalData.maxSlopeRaw;
			fields.stepHeight = m_originalData.stepHeight;
			fields.stepReach = m_originalData.stepReach;
			fields.rigidBodyMaxSlopeCosine = m_originalData.rigidBodyMaxSlopeCosine;
			fields.proxyMaxSolverSpeed = m_originalData.proxyMaxSolverSpeed;
		}

		return fields;
	}

	bool DynamicCollisionController::FieldsMatch(bhkCharacterController* a_controller, const ControllerFields& a_fields) const {

		if (a_fields.writeScale && (a_controller->actorHeight != a_fields.actorHeight || a_controller->scale != a_fields.scale)) {
			return false;
		}

		if (!a_fields.writeTraversal) {
			return true;
		}

		if (a_controller->maxSlope != a_fields.maxSlopeRaw ||
			GetControllerStepHeight(a_controller) != a_fields.stepHeight ||
			GetControllerStepReach(a_controller) != a_fields.stepReach) {
			return false;
		}

		if (const hkpCharacterRigidBody* rigidBody = GetControllerRigidBody(a_controller, m_originalData.controllerKind)) {
			return rigidBody->m_maxSlopeCosine == a_fields.rigidBodyMaxSlopeCosine;
		}
		if (const hkpCharacterProxy* proxy = GetControllerProxy(a_controller, m_originalData.controllerKind)) {
			return proxy->maxCharacterSpeedForSolver == a_fields.proxyMaxSolverSpeed;
		}
		return true;
	}

	void DynamicCollisionController::WriteFields(bhkCharacterController* a_controller, const ControllerFields& a_fields) const {

		if (a_fields.writeScale) {
			a_controller->actorHeight = a_fields.actorHeight;
			a_controller->scale = a_fields.scale;
		}

		if (!a_fields.writeTraversal) {
			return;
		}

		a_controller->maxSlope = a_fields.maxSlopeRaw;
		SetControllerStepHeight(a_controller, a_fields.stepHeight);
		SetControllerStepReach(a_controller, a_fields.stepReach);

		if (hkpCharacterRigidBody* rigidBody = GetControllerRigidBody(a_controller, m_originalData.controllerKind)) {
			rigidBody->m_maxSlopeCosine = a_fields.rigidBodyMaxSlopeCosine;
		}
		else if (hkpCharacterProxy* proxy = GetControllerProxy(a_controller, m_originalData.controllerKind)) {
			proxy->maxCharacterSpeedForSolver = a_fields.proxyMaxSolverSpeed;
		}
	}

}