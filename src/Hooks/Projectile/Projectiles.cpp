#include "Hooks/Projectile/Projectiles.hpp"
#include "Hooks/Util/HookUtil.hpp"

using namespace GTS;

namespace {
	
	void ScaleExplosion(Explosion* explosion) {
		auto causer = explosion->GetExplosionRuntimeData().actorOwner;
		if (causer) {
			auto cause = causer.get().get();
			if (cause) {
				if (IsDragon(cause) || IsGiant(cause)) {
					log::info("Scaling Explosion");
					explosion->GetExplosionRuntimeData().radius *= get_visual_scale(cause);
				}
			}
		}
	}

	void ArrowImpact(Projectile* projectile) {
		auto owner = projectile->GetProjectileRuntimeData().shooter;
		auto node = projectile->Get3D2();
		if (node) {
			if (owner) {
				auto owner_get = skyrim_cast<Actor*>(owner.get().get());
				if (owner_get) {
					auto player = PlayerCharacter::GetSingleton();
					float distance = player->GetPosition().Length();
					//log::info("Owner_get found: {}", owner_get->GetDisplayFullName());
					float scaling = get_visual_scale(owner_get);
					float shake_power = scaling * (node->world.translate.Length() / distance);

					shake_camera_at_node(node->world.translate, shake_power, 1.5f);

					auto cell = projectile->GetParentCell();

					if (cell) {
						// Crashes if called like that, too lazy to fix it
						log::info("Spawning footstep.nif");
						BSTempEffectParticle::Spawn(cell, 6.0f, "GTS/Effects/Footstep.nif", node->world.rotate, node->world.translate, scaling, 7, nullptr);
					}
				}
			}
		}
	}
	void ScaleProjectile(Projectile* projectile, float speed_limit, bool apply_speed) {

		//Probably impossible, but while testing the killfeed this ptr was 0xFFFFFFFFFFFFFFFF  according to the debugger
		if (!projectile || reinterpret_cast<uintptr_t>(projectile) == 0xFFFFFFFFFFFFFFFF) {
			return;
		}

		auto node = projectile->Get3D2();
		if (!node) {
			node = projectile->Get3D1(false);
			if (!node) {
				log::info("3d1: fp");
				node = projectile->Get3D1(true);
			}
		}
		if (!node) {
			node = projectile->Get3D();
		} 
		if (node) {
			//log::info("Node found");
			auto owner = projectile->GetProjectileRuntimeData().shooter;
			if (owner) {
				//log::info("Owner found");
				auto owner_get = skyrim_cast<Actor*>(owner.get().get());
				if (owner_get) {
					//log::info("Owner_get found: {}", owner_get->GetDisplayFullName());
					float scaling = std::clamp(get_visual_scale(owner_get), 0.02f, 1.0f); // not bigger than 1.0x
					node->local.scale *= scaling;

					if (apply_speed) {
						float speed_scaling = std::clamp(scaling, 0.10f, speed_limit);
						projectile->GetProjectileRuntimeData().speedMult *= speed_scaling;
						
						//projectile->GetProjectileRuntimeData().power *= speed_scaling; // No idea what it does
					}

					auto spell = projectile->GetProjectileRuntimeData().spell;
					projectile->GetProjectileRuntimeData().scale *= scaling;
					if (spell) {
						auto effect = skyrim_cast<SpellItem*>(spell);
						if (effect) {
							log::info("Effect found!");
							effect->data.range *= scaling;
						}
					}
					auto explosion = projectile->GetProjectileRuntimeData().explosion;
					if (explosion) {
						//explosion->data.radius *= scaling;
						// Scales only visuals. Damage zone is still the same
					}
				}
			}
		}
	}
}

namespace Hooks {

	struct InitializeExplosion {

		static constexpr size_t funcIndex = 0x90;

		// Attempts to scale explosions that come from dragons. Doesn't work s expected.
		template<int ID>
		static void thunk(RE::Explosion* a_this, TESObjectCELL& a_cell) {

			func<ID>(a_this, a_cell);

			{
				GTS_PROFILE_ENTRYPOINT_UNIQUE("Projectile::InitializeExplosion", ID);
				ScaleExplosion(a_this);
			}

		}

		template<int ID>
		FUNCTYPE_VFUNC_UNIQUE func;

	};

	struct GetLinearVelocity {

		static constexpr size_t funcIndex = 0x86;

		// Unused
		template<int ID>
		static void thunk(RE::Projectile* a_this, RE::NiPoint3& a_outVelocity) {

			{
				GTS_PROFILE_ENTRYPOINT_UNIQUE("Projectile::GetLinearVelocity", ID);
			}

			func<ID>(a_this, a_outVelocity);
		}

		template<int ID>
		FUNCTYPE_VFUNC_UNIQUE func;

	};

	struct Handle3DLoaded {

		static constexpr size_t funcIndex = 0xC0;

		// Scales projectiles once (when it first spawns). Affects only visuals.
		template<int ID>
		static void thunk(RE::Projectile* a_this) {

			func<ID>(a_this);

			{
				GTS_PROFILE_ENTRYPOINT_UNIQUE("Projectile::Handle3DLoaded", ID);
				ScaleProjectile(a_this, 1.0f, false);
			}

		}

		template<int ID>
		FUNCTYPE_VFUNC_UNIQUE func;
	};

	struct AddImpact {

		static constexpr size_t funcIndex = 0xBD;

		// Better not call it, spawning Footstep.nif causes ctd
		template<int ID>
		static void thunk(RE::Projectile* a_this, TESObjectREFR* a_ref, const NiPoint3& a_targetLoc, const NiPoint3& a_velocity, hkpCollidable* a_collidable, std::int32_t a_arg6, std::uint32_t a_arg7) {

			func<ID>(a_this, a_ref, a_targetLoc, a_velocity, a_collidable, a_arg6, a_arg7);

			{
				GTS_PROFILE_ENTRYPOINT_UNIQUE("Projectile::AddImpact", ID);
				//ArrowImpact(a_this);
			}

		}

		template<int ID>
		FUNCTYPE_VFUNC_UNIQUE func;
	};

	void Hook_Projectiles::Install() {

		logger::info("Installing Projectile Hooks...");

		//Explosion Initialize
		//stl::write_vfunc_unique<InitializeExplosion, 0>(VTABLE_Explosion[0]);

		//Projectile Velocity
		//stl::write_vfunc_unique<GetLinearVelocity, 0>(VTABLE_Projectile[0]);

		//Projectile 3D Loaed
		stl::write_vfunc_unique<Handle3DLoaded, 0>(VTABLE_ArrowProjectile[0]);
		//stl::write_vfunc_unique<Handle3DLoaded, 1>(VTABLE_MissileProjectile[0]);
		//stl::write_vfunc_unique<Handle3DLoaded, 2>(VTABLE_BeamProjectile[0]);
		//stl::write_vfunc_unique<Handle3DLoaded, 3>(VTABLE_BarrierProjectile[0]);
		//stl::write_vfunc_unique<Handle3DLoaded, 4>(VTABLE_ConeProjectile[0]);
		//stl::write_vfunc_unique<Handle3DLoaded, 5>(VTABLE_FlameProjectile[0]);

		//Projectile Impact
		//stl::write_vfunc_unique<AddImpact, 0>(VTABLE_ArrowProjectile[0]);

	}

}