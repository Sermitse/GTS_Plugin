#include "Managers/Collision/DynamicController.hpp"
#include "Managers/Collision/DynamicCollisionUtils.hpp"
#include "Config/Config.hpp"




namespace GTS {

	DynamicController::DynamicController(const ActorHandle& a_handle) {

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

							controller->bumperEnabled = true;
							ToggleCharacterBumper(actor, false);

							hkpConvexVerticesShape* convexShape = nullptr;
							std::vector<hkpCapsuleShape*> capsuleShapes = {};
							m_originalData.m_originalmaxSlope = controller->maxSlope;

							//Get all collider shapes and store a copy of their geometry
							//Used as a base reference for scaling
							if (GetShapes(controller, convexShape, capsuleShapes)) {

								if (convexShape) {

									//Make a copy of the original vertex collider vertex positions
									hkArray<hkVector4> Verteces{};
									convexShape->GetOriginalVertices(Verteces);

									m_originalData.m_convexVerteces.reserve(Verteces.size());
									for (const hkVector4& Vertex : Verteces) {
										m_originalData.m_convexVerteces.push_back(Vertex);
									}

									NiPoint3 originVert = HkVectorToNiPoint(Verteces[0]);
									originVert.z = 0.0f;
									m_originalData.m_convexRadius = originVert.Length();
									m_originalData.m_HasVertecesShape = true;
								}

								if (!capsuleShapes.empty()) {
									for (const hkpCapsuleShape* shape : capsuleShapes) {
										m_originalData.m_capsules.push_back({
											shape->radius,
											shape->vertexA,
											shape->vertexB,
											shape->userData->materialID == MATERIAL_ID::kCharacterBumper,
										});
									}
								}

								m_originalData.m_originalControllerHeight = controller->actorHeight;
								m_originalData.m_originalControllerScale = controller->scale;

								//NPC's Use bhkCharRigidBodyController, clone their capsule colliders as they are shared between all actors with the same skeleton
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
															m_clonedShape = NiPointer(Clone<bhkShape>(wrapper.get()));
															if (!m_clonedShape) {
																clonedShape->RemoveReference(); // prevent leak if wrapper clone failed
																return;
															}

															m_clonedShape->SetReferencedObject(clonedShape);

															// Install the cloned shape into the Havok character
															character->SetShape(clonedShape);

															// Replace controller wrapper entry
															RigidBodyController->shapes[shapeIdx]->DecRefCount();
															RigidBodyController->shapes[shapeIdx] = m_clonedShape;
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
	}

	void DynamicController::Update() {
		
		if (Actor* Target = m_actor.get().get()) {

			m_currentVisualScale = get_visual_scale(Target);
			bhkCharacterController* controller = Target->GetCharController();

			if (Target->IsPlayerRef() || IsTeammate(Target)) {
				AdjustPlayer();
				UpdateControllerData(controller, m_originalData, m_currentVisualScale);
				return;
			}

			if (ActorState* state = Target->AsActorState()) {
				if (ScaleorStateChange(state->actorState1, m_lastActorState1, m_currentVisualScale, m_lastVisualScale)) {
					m_lastVisualScale = m_currentVisualScale;
					m_lastActorState1 = state->actorState1;
					AdjustNPC();
					UpdateControllerData(controller, m_originalData, m_currentVisualScale);
				}
			}
		}
	}

	void DynamicController::AdjustPlayer() {
		
		GTS_PROFILE_SCOPE("DynamicController::AdjustPlayer");

		if (NiPointer<Actor> ni_actor = m_actor.get()) {
			if (Actor* actor = ni_actor.get()) {
				if (TESObjectCELL* cell = actor->GetParentCell()) {
					if (bhkWorld* world = cell->GetbhkWorld()){

						hkpListShape* ListShape              = nullptr;
						hkpCharacterProxy* CharProxy         = nullptr;
						hkpConvexVerticesShape* ConvexShape  = nullptr;
						hkpCharacterRigidBody* CharRigidBody = nullptr;

						std::vector<hkVector4> modifiedVerts = m_originalData.m_convexVerteces;
						const float& bottomZ = m_originalData.m_convexVerteces[Vertex_Bot].quad.m128_f32[2];

						if (GetConvexShape(actor->GetCharController(), CharProxy, CharRigidBody, ListShape, ConvexShape)) {

							const float distance = GetLargestBoneDistance(actor, m_cachedBones);

							const bool success =
								MoveVertZ(actor, modifiedVerts, Vertex_Top, HeadBoneName, m_cachedBones, bottomZ + (0.07f * m_currentVisualScale)) &&
								MoveRingsZAggregate(actor, modifiedVerts, Vertex_RingTop, { UppderArmBoneRName, UpperArmBoneLName }, m_cachedBones, bottomZ, distance) &&
								MoveRingsZAggregate(actor, modifiedVerts, Vertex_RingBot, { CalfBoneRName, CalfBoneLName }, m_cachedBones, bottomZ, distance) &&
								UpdatePlayerCapsuleScale(actor->GetCharController(), m_originalData, m_currentVisualScale);

							if (success){
								CorrectCollapsedVertexShape(modifiedVerts);
								CreateAndSetVerticesShape(world, ConvexShape, ListShape, CharProxy, CharRigidBody, modifiedVerts);
							}
						}
					}
				}
			}
		}
	}

	void DynamicController::AdjustNPC() const {

		GTS_PROFILE_SCOPE("DynamicController::AdjustNPC");

		if (NiPointer<Actor> ni_actor = m_actor.get()) {
			if (Actor* actor = ni_actor.get()) {
				if (TESObjectCELL* cell = actor->GetParentCell()) {
					if (bhkWorld* world = cell->GetbhkWorld()) {

						hkpListShape* ListShape = nullptr;
						hkpCharacterProxy* CharProxy = nullptr;
						hkpConvexVerticesShape* ConvexShape = nullptr;
						hkpCharacterRigidBody* CharRigidBody = nullptr;

						std::vector<hkVector4> modifiedVerts = m_originalData.m_convexVerteces;

						if (m_originalData.m_HasVertecesShape) {
							float widthMult = GetVerticesWidthMult(actor) * m_currentVisualScale * 0.90f;

							float heightMult = 1.0f;
							if (actor->IsSneaking()) {
								heightMult = 0.45f;
							}
							else if (actor->AsActorState()->IsSwimming()) {
								heightMult = 0.35f;
							}

							if (GetConvexShape(actor->GetCharController(), CharProxy, CharRigidBody, ListShape, ConvexShape)) {
								
								const float sZ = m_currentVisualScale * heightMult;
								const float zB0 = m_originalData.m_convexVerteces[Vertex_Bot].quad.m128_f32[2];

								auto ScaleZAboutBottom = [&](float z0) -> float {return zB0 + (z0 - zB0) * sZ;};

								modifiedVerts[Vertex_Top].quad.m128_f32[2] = ScaleZAboutBottom(m_originalData.m_convexVerteces[Vertex_Top].quad.m128_f32[2]);

								for (uint8_t idx : Vertex_RingTop) {
									const float z1 = ScaleZAboutBottom(m_originalData.m_convexVerteces[idx].quad.m128_f32[2]);
									modifiedVerts[idx] = NormalizeXYAndScale(m_originalData.m_convexVerteces[idx].quad, m_originalData.m_convexRadius * widthMult, z1);
								}

								for (uint8_t idx : Vertex_RingBot) {
									const float z1 = ScaleZAboutBottom(m_originalData.m_convexVerteces[idx].quad.m128_f32[2]);
									modifiedVerts[idx] = NormalizeXYAndScale(m_originalData.m_convexVerteces[idx].quad, m_originalData.m_convexRadius * widthMult, z1);
								}

								CorrectCollapsedVertexShape(modifiedVerts);
								CreateAndSetVerticesShape(world, ConvexShape, ListShape, CharProxy, CharRigidBody, modifiedVerts);
							}
						}
						UpdateCapsuleScale(m_clonedShape.get(), m_originalData, m_currentVisualScale);
					}
				}
			}
		}
	}
}