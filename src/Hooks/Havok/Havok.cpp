#include "Hooks/Havok/Havok.hpp"
#include "Managers/OverkillManager.hpp"
#include "Hooks/Util/HookUtil.hpp"

using namespace GTS;

namespace {

	constexpr float tree_ignore_threshold = 16.0f;
	constexpr float actor_ignore_limit = 3.0f;

	COL_LAYER GetCollisionLayer(const std::uint32_t& collisionFilterInfo) {
		return static_cast<COL_LAYER>(collisionFilterInfo & 0x7F);
	}

	COL_LAYER GetCollisionLayer(const hkpCollidable* collidable) {
		if (collidable) {
			return GetCollisionLayer(collidable->broadPhaseHandle.collisionFilterInfo);
		} else {
			return COL_LAYER::kUnidentified;
		}
	}

	COL_LAYER GetCollisionLayer(const hkpCollidable& collidable) {
		return GetCollisionLayer(&collidable);
	}

	std::uint32_t GetCollisionSystem(const std::uint32_t& collisionFilterInfo) {
		return collisionFilterInfo >> 16;
	}

	std::uint32_t GetCollisionSystem(const hkpCollidable* collidable) {
		if (collidable) {
			return GetCollisionSystem(collidable->broadPhaseHandle.collisionFilterInfo);
		} else {
			return 0;
		}
	}

	std::uint32_t GetCollisionSystem(const hkpCollidable& collidable) {
		return GetCollisionSystem(&collidable);
	}

	TESObjectREFR* GetTESObjectREFR(const hkpCollidable* collidable) {
		if (collidable) {
			auto type = collidable->broadPhaseHandle.type;
			if (static_cast<RE::hkpWorldObject::BroadPhaseType>(type) == hkpWorldObject::BroadPhaseType::kEntity) {
				if (collidable->ownerOffset < 0) {
					hkpRigidBody* obj = collidable->GetOwner<hkpRigidBody>();
					if (obj) {
						return obj->GetUserData();
					}
				}
			} else if (static_cast<RE::hkpWorldObject::BroadPhaseType>(type) == hkpWorldObject::BroadPhaseType::kPhantom) {
				if (collidable->ownerOffset < 0) {
					hkpPhantom* obj = collidable->GetOwner<hkpPhantom>();
					if (obj) {
						return obj->GetUserData();
					}
				}
			}
		}
		return nullptr;
	}

	TESObjectREFR* GetTESObjectREFR(const hkpCollidable& collidable) {
		return GetTESObjectREFR(&collidable);
	}

	bool IsCollisionDisabledBetween(TESObjectREFR* actor, TESObjectREFR* otherActor) {
		if (!actor) {
			return false;
		}
		if (!otherActor) {
			return false;
		}


		Actor* actor_a = skyrim_cast<Actor*>(actor);
		Actor* actor_b = skyrim_cast<Actor*>(otherActor);

		if (actor_a && actor_b) {

			auto tranDataA = Transient::GetActorData(actor_a);
			if (tranDataA) {
				if (tranDataA->DisableColissionWith == otherActor) {
					return true;
				}
			}

			auto tranDataB = Transient::GetActorData(actor_b);
			if (tranDataB) {
				if (tranDataB->DisableColissionWith == actor) {
					return true;
				}
			}

			float gts_scale = get_visual_scale(actor_a);
			float tiny_scale = get_visual_scale(actor_b);

			float Scale_A = get_visual_scale(actor_a) * GetSizeFromBoundingBox(actor_a); // A is usually GTS, but can be TIny as well
			float Scale_B = get_visual_scale(actor_b) * GetSizeFromBoundingBox(actor_b); // B is usually a tiny but can be GTS as well

			float sizedifference_gts = Scale_A/Scale_B;

			if (gts_scale/tiny_scale < 1.0f) { // Switch actor roles
				//log::info("Roles switched");
				sizedifference_gts = Scale_B/Scale_A; // Rough fix for Tiny being the Gts sometimes
			}

			bool ignore = (sizedifference_gts >= actor_ignore_limit);
			bool busy = AnimationVars::General::IsGTSBusy(actor_a) && AnimationVars::General::IsGTSBusy(actor_b);
			bool grabbed = IsBeingHeld(actor_a, actor_b) || IsBeingHeld(actor_b, actor_a);
			if (ignore) {
				return true;
			}
			if (busy || grabbed) {
				return true;
			}
		}

		return false;
	}

	bool IsTreeCollisionDisabled(const hkpCollidable* a_collidableA, const hkpCollidable* a_collidableB) {

		auto colLayerA = GetCollisionLayer(a_collidableA);
		auto colLayerB = GetCollisionLayer(a_collidableB);

		bool Check_Tree = (colLayerA == COL_LAYER::kTrees || colLayerB == COL_LAYER::kTrees);

		if (Check_Tree) {
			auto obj_A = GetTESObjectREFR(a_collidableA);
			auto obj_B = GetTESObjectREFR(a_collidableB);
			
			if (obj_A && obj_B) {
				Actor* actor = skyrim_cast<Actor*>(obj_A);
				if (actor) {
					//log::info("A is actor");
					float tree_scale = static_cast<float>(obj_A->GetReferenceRuntimeData().refScale) / 100.0F;
					float actor_scale = get_visual_scale(actor);
					if (actor_scale/tree_scale >= tree_ignore_threshold) {
						//log::info("A: {}", obj_A->GetDisplayFullName());
						//log::info("B: {}", obj_B->GetDisplayFullName());
						//log::info("Ignoring collision");
						return true;
					}
				}
			}
		}
			
		return false;
	}

	void CollisionPrints(const hkpCollidable* collidableA, const hkpCollidable* collidableB) {
		
		auto ObjectA = GetTESObjectREFR(collidableA);
		auto ObjectB = GetTESObjectREFR(collidableB);

		if (ObjectA && ObjectB) {
			auto Layer_A = GetCollisionLayer(collidableA);
			auto Layer_B = GetCollisionLayer(collidableB);
			if (ObjectA->formID == 0x14 || ObjectB->formID == 0x14) {
				log::info("{} is colliding with {}", ObjectA->GetDisplayFullName(), ObjectB->GetDisplayFullName());
				log::info("{} Collision Layer: {}", ObjectA->GetDisplayFullName(), Layer_A);
				log::info("{} Collision Layer: {}", ObjectB->GetDisplayFullName(), Layer_B);
			}
		}
	}

	void Throw_DoDamage(TESObjectREFR* victim_ref, TESObjectREFR* aggressor_ref, float speed) {
		float damage = speed * Damage_Throw_Collision;

		Actor* victim = skyrim_cast<Actor*>(victim_ref);
		Actor* aggressor = skyrim_cast<Actor*>(aggressor_ref);

		if (victim && aggressor) {
			InflictSizeDamage(aggressor, victim, damage);

			std::string task = std::format("ThrowTiny {}", victim->formID);
			ActorHandle giantHandle = aggressor->CreateRefHandle();
			ActorHandle tinyHandle = victim->CreateRefHandle();

			log::info("Inflicting throw damage for {}: {}", victim->GetDisplayFullName(), damage);

			TaskManager::RunOnce(task, [=](auto& update){
				if (!giantHandle) {
					return;
				}
				if (!tinyHandle) {
					return;
				}
				
				auto giant = giantHandle.get().get();
				auto tiny = tinyHandle.get().get();
				float health = GetAV(tiny, ActorValue::kHealth);
				if (health <= 1.0f || tiny->IsDead()) {
					OverkillManager::GetSingleton().Overkill(giant, tiny);
				}
			});
		}
	}
}

namespace Hooks {

	struct ProcessHavokHitJobs {

		static void thunk(void* a_unk01) {

			func(a_unk01);

			{
				GTS_PROFILE_ENTRYPOINT("Havok::ProcessHavokHitJobs");
				EventDispatcher::DoHavokUpdate();
			}

		}
		FUNCTYPE_CALL func;
	};

	struct IsCollisionEnabled {

		static inline size_t funcIndex = 0x01;
		
		// Credit: FlyingParticle for code on getting the TESObjectREFR
		// maxsu. for IsCollisionEnabled idea
		static bool* thunk(hkpCollidableCollidableFilter* a_this, bool* a_result, const hkpCollidable* a_collidableA, const hkpCollidable* a_collidableB) {


			a_result = func(a_this, a_result, a_collidableA, a_collidableB);

			{
				GTS_PROFILE_ENTRYPOINT("Havok::IsCollisionEnabled");

				if (*a_result) {
					auto colLayerA = GetCollisionLayer(a_collidableA);
					auto colLayerB = GetCollisionLayer(a_collidableB);

					//CollisionPrints(a_collidableA, a_collidableB);

					if (IsTreeCollisionDisabled(a_collidableA, a_collidableB)) {
						*a_result = false;
					}

					bool Check_A = (colLayerA == COL_LAYER::kBiped || colLayerA == COL_LAYER::kCharController || colLayerA == COL_LAYER::kDeadBip || colLayerA == COL_LAYER::kBipedNoCC);
					bool Check_B = (colLayerB == COL_LAYER::kBiped || colLayerB == COL_LAYER::kCharController || colLayerB == COL_LAYER::kDeadBip || colLayerB == COL_LAYER::kBipedNoCC);

					if (Check_A || Check_B) {
						auto objA = GetTESObjectREFR(a_collidableA);
						auto objB = GetTESObjectREFR(a_collidableB);

						if (objA && objB && objA != objB) {
							if (IsCollisionDisabledBetween(objA, objB)) {
								*a_result = false;
							}
						}
					}
				}
			}

			return a_result;

		}

		FUNCTYPE_VFUNC func;

	};

	void Hook_Havok::Install() {

		logger::info("Installing Havok Hooks...");

		stl::write_call<ProcessHavokHitJobs>(REL::RelocationID(38112, 39068, NULL),REL::VariantOffset(0x104, 0xFC, NULL));
		stl::write_vfunc<IsCollisionEnabled>(VTABLE_bhkCollisionFilter[1]);

	}
}