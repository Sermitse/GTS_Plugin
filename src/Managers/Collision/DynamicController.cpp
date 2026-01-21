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
									for (const hkpCapsuleShape* shape : capsuleShapes) {
										m_originalData.capsules.push_back({
											shape->radius,
											shape->vertexA,
											shape->vertexB,
											shape->userData->materialID == MATERIAL_ID::kCharacterBumper,
										});
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

			if (m_currentVisualScale > 50.f) {
				return; //Stop updating past this scale. While Havok can handle larger the large shape causes massive lag due to all the collision checks.
			}

			if (Target->IsPlayerRef() || (Config::Collision.bEnableBoneDrivenCollisionUpdatesFollowers && IsTeammate(Target))) {
				AdjustBoneDriven();
				UpdateControllerScaleAndSlope(controller, m_originalData, m_currentVisualScale);
				m_lastVisualScale = 0.0f; // Set it to 0 to force an update if followers are switched to simple scaling
				if (Config::Collision.bDrawDebugShapes) {
					DrawCollisionShapes(Target, true);
				}

				return;
			}

			if (ActorState* state = Target->AsActorState()) {

				if (Config::Collision.bDrawDebugShapes) {
					DrawCollisionShapes(Target, false);
				}

				if (ScaleorStateChange(state->actorState1, m_lastActorState1, m_currentVisualScale, m_lastVisualScale)) {
					AdjustScale();
					UpdateControllerScaleAndSlope(controller, m_originalData, m_currentVisualScale);
				}
				m_lastActorState1 = state->actorState1;
			}
			m_lastVisualScale = m_currentVisualScale;
		}
	}

	void DynamicController::AdjustBoneDriven() {
		
		GTS_PROFILE_SCOPE("DynamicController::AdjustBoneDriven");

		if (NiPointer<Actor> ni_actor = m_actor.get()) {
			if (Actor* actor = ni_actor.get()) {
				if (TESObjectCELL* cell = actor->GetParentCell()) {
					if (bhkWorld* world = cell->GetbhkWorld()){

						hkpListShape* ListShape              = nullptr;
						hkpCharacterProxy* CharProxy         = nullptr;
						hkpConvexVerticesShape* ConvexShape  = nullptr;
						hkpCharacterRigidBody* CharRigidBody = nullptr;

						std::vector<hkVector4> modifiedVerts = m_originalData.convexVerteces;
						const float& bottomZ = m_originalData.convexVerteces[Vertex_Bot].quad.m128_f32[2];

						{
							BSReadLockGuard lock(world->worldLock);
							if (!GetConvexShape(actor->GetCharController(), CharProxy, CharRigidBody, ListShape, ConvexShape)) {
								return;
							}
						}

						const float distance = GetLargestBoneDistance(actor, m_cachedBones);
						const bool success =
						MoveVertZ(actor, modifiedVerts, Vertex_Top, HeadBoneName, m_cachedBones, bottomZ + (0.05f * m_currentVisualScale))           && // Top vertex
						MoveRings(actor, modifiedVerts, Vertex_RingTop, { UppderArmBoneRName, UpperArmBoneLName }, m_cachedBones, bottomZ, distance) && // Upper ring
						MoveRings(actor, modifiedVerts, Vertex_RingBot, { CalfBoneRName, CalfBoneLName }, m_cachedBones, bottomZ, distance)          && // Lower ring
						ScaleBumper(actor->GetCharController(), m_originalData, m_currentVisualScale);                                                 // Set bumper capsule scale

						if (success){

							CheckAndCorrectCollapsedVertexShape(modifiedVerts);

							{
								BSWriteLockGuard lock(world->worldLock);
								SetNewVerticesShape(ConvexShape, ListShape, CharProxy, CharRigidBody, modifiedVerts);
							}
						}
					}
				}
			}
		}
	}

	void DynamicController::AdjustScale() const {

		GTS_PROFILE_SCOPE("DynamicController::AdjustScale");

		if (NiPointer<Actor> ni_actor = m_actor.get()) {
			if (Actor* actor = ni_actor.get()) {
				if (TESObjectCELL* cell = actor->GetParentCell()) {
					if (bhkWorld* world = cell->GetbhkWorld()) {

						hkpListShape* ListShape = nullptr;
						hkpCharacterProxy* CharProxy = nullptr;
						hkpConvexVerticesShape* ConvexShape = nullptr;
						hkpCharacterRigidBody* CharRigidBody = nullptr;

						std::vector<hkVector4> modifiedVerts = m_originalData.convexVerteces;
						const float fClampedScale = std::clamp(Config::Collision.fMSimpleDrivenColliderMinScale, Config::Collision.fMSimpleDrivenColliderMaxScale, m_currentVisualScale);
						if (m_originalData.hasVertecesShape) {
							float widthMult = GetVerticesWidthMult(actor, false) * fClampedScale;

							float heightMult = 1.0f;
							if (AnimationVars::Crawl::IsCrawling(actor) && Runtime::HasKeyword(actor, Runtime::KYWD.ActorTypeNPC)){
								heightMult = Config::Collision.fSimpleDrivenHeightMultCrawling;
							}
							else if (actor->IsSneaking()) {
								heightMult = Config::Collision.fSimpleDrivenHeightMultSneaking;
							}
							else if (actor->AsActorState()->IsSwimming()) {
								heightMult = Config::Collision.fSimpleDrivenHeightMultSwimming;
							}

							{
								BSReadLockGuard lock(world->worldLock);
								if (!GetConvexShape(actor->GetCharController(), CharProxy, CharRigidBody, ListShape, ConvexShape)) {
									return;
								}
							}

							const float sZ = fClampedScale * heightMult;
							const float& zB0 = m_originalData.convexVerteces[Vertex_Bot].quad.m128_f32[2];

							auto ScaleZFromBottom = [&](float z0) -> float {return zB0 + (z0 - zB0) * sZ;};

							modifiedVerts[Vertex_Top].quad.m128_f32[2] = ScaleZFromBottom(m_originalData.convexVerteces[Vertex_Top].quad.m128_f32[2]);

							for (uint8_t idx : Vertex_RingTop) {
								const float z1 = ScaleZFromBottom(m_originalData.convexVerteces[idx].quad.m128_f32[2]);
								modifiedVerts[idx] = ScaleRingWidth(m_originalData.convexVerteces[idx].quad, m_originalData.convexShapeRadius * widthMult, z1);
							}

							for (uint8_t idx : Vertex_RingBot) {
								const float z1 = ScaleZFromBottom(m_originalData.convexVerteces[idx].quad.m128_f32[2]);
								modifiedVerts[idx] = ScaleRingWidth(m_originalData.convexVerteces[idx].quad, m_originalData.convexShapeRadius * widthMult, z1);
							}

							CheckAndCorrectCollapsedVertexShape(modifiedVerts);
							{
								BSWriteLockGuard lock(world->worldLock);
								SetNewVerticesShape(ConvexShape, ListShape, CharProxy, CharRigidBody, modifiedVerts);
							}

						}
						{
							BSWriteLockGuard lock(world->worldLock);
							UpdateCapsuleScale(m_uniqueShape.get(), m_originalData, fClampedScale);
						}
					}
				}
			}
		}
	}
}