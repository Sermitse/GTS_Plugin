
#include "Config/Config.hpp"

#include "Debug/DebugDraw.hpp"

#include "Managers/Rumble.hpp"
#include "Managers/Animation/Utils/AnimationUtils.hpp"
#include "Managers/ExplosionManager.hpp"

namespace GTS {
	
	void SpawnHearts(Actor* giant, Actor* tiny, float Z, float scale, bool hugs, NiPoint3 CustomPos) {

		bool Allow = Config::General.bShowHearts;

		if (Allow) {
			NiPoint3 Position = NiPoint3(0, 0, 0);
			if (CustomPos.Length() < 0.01f) {
				Position = GetHeartPosition(giant, tiny, hugs);
			}
			else {
				Position = CustomPos;
			}

			if (Z > 0) {
				Position.z -= Z * get_visual_scale(giant);
			}

			SpawnCustomParticle(giant, ParticleType::Hearts, Position, "NPC COM [COM ]", get_visual_scale(giant) * scale);
		}
	}

	void SpawnCustomParticle(Actor* actor, ParticleType Type, NiPoint3 spawn_at_point, std::string_view spawn_at_node, float scale_mult) {
		if (actor) {
			float scale = scale_mult * GetSizeFromBoundingBox(actor);

			if (actor->IsDead()) {
				scale *= 0.33f;
			}

			auto node = find_node(actor, spawn_at_node);
			if (!node) {
				log::info("!Node: {}", spawn_at_node);
				return;
			}
			const char* particle_path = "None";
			switch (Type) {
			case ParticleType::Red:
				particle_path = "GTS/Magic/Life_Drain.nif";
				break;
			case ParticleType::DarkRed:
				particle_path = "GTS/Magic/Life_Drain_Dark.nif";
				break;
			case ParticleType::Green:
				particle_path = "GTS/Magic/Slow_Grow.nif";
				break;
			case ParticleType::Blue:
				particle_path = "GTS/Magic/Soul_Drain.nif";
				break;
			case ParticleType::Hearts:
				particle_path = "GTS/Magic/Hearts.nif";
				break;
			case ParticleType::None:
				return;
				break;
			}
			NiPoint3 pos = NiPoint3(); // Empty point 3

			if (spawn_at_point.Length() > 1.0f) {
				pos = spawn_at_point;
			}
			else {
				pos = node->world.translate;
			}

			SpawnParticle(actor, 4.60f, particle_path, NiMatrix3(), pos, scale, 7, nullptr);
		}
	}

	void DoDustExplosion(Actor* giant, float modifier, FootEvent kind, std::string_view node) {
		auto& explosion = ExplosionManager::GetSingleton();

		std::vector<NiAVObject*> points = { find_node(giant, node) };

		Impact impact_data = Impact{
			.actor = giant,
			.kind = kind,
			.scale = get_visual_scale(giant),
			.modifier = modifier,
			.nodes = points,
		};
		explosion.OnImpact(impact_data); // Play explosion
	}

	void SpawnParticle(Actor* actor, float lifetime, const char* modelName, const NiMatrix3& rotation, const NiPoint3& position, float scale, std::uint32_t flags, NiAVObject* target) {
		auto cell = actor->GetParentCell();
		if (cell) {
			BSTempEffectParticle::Spawn(cell, lifetime, modelName, rotation, position, scale, flags, target);
		}
	}

	void SpawnDustParticle(Actor* giant, Actor* tiny, std::string_view node, float size) {
		auto result = find_node(giant, node);
		if (result) {
			BGSExplosion* base_explosion = Runtime::GetExplosion(Runtime::EXPL.GTSExplosionDraugr);
			if (base_explosion) {
				NiPointer<TESObjectREFR> instance_ptr = giant->PlaceObjectAtMe(base_explosion, false);
				if (!instance_ptr) {
					return;
				}
				TESObjectREFR* instance = instance_ptr.get();
				if (!instance) {
					return;
				}
				Explosion* explosion = instance->AsExplosion();
				if (!explosion) {
					return;
				}
				explosion->SetPosition(result->world.translate);
				explosion->GetExplosionRuntimeData().radius *= 3 * get_visual_scale(tiny) * size;
				explosion->GetExplosionRuntimeData().imodRadius *= 3 * get_visual_scale(tiny) * size;
				explosion->GetExplosionRuntimeData().unkB8 = nullptr;
				explosion->GetExplosionRuntimeData().negativeVelocity *= 0.0f;
				explosion->GetExplosionRuntimeData().unk11C *= 0.0f;
			}
		}
	}

	void ShrinkOutburstExplosion(Actor* giant, bool WasHit) {
		if (!giant) {
			return;
		}
		auto node = find_node(giant, "NPC Pelvis [Pelv]");
		if (!node) {
			return;
		}
		bool DarkArts_Legendary = Runtime::HasPerk(giant, Runtime::PERK.GTSPerkDarkArtsLegendary);
		bool DarkArts1 = Runtime::HasPerk(giant, Runtime::PERK.GTSPerkDarkArtsAug1);

		NiPoint3 NodePosition = node->world.translate;

		float gigantism = std::clamp(1.0f + Ench_Aspect_GetPower(giant), 1.0f, 2.5f); // Capped for Balance reasons, don't want to Annihilate entire map if hacking
		float giantScale = get_visual_scale(giant);

		float radius_mult = 1.0f;
		float explosion = 0.5f;
		float shrink = 0.38f;
		float radius = 1.0f;

		if (WasHit) {
			radius_mult += 0.25f;
			shrink += 0.22f;
		}
		if (DarkArts1) {
			radius_mult += 0.30f;
			shrink *= 1.3f;
		}

		if (DarkArts_Legendary) {
			radius_mult += 0.15f;
			shrink *= 1.30f;
		}

		explosion *= radius_mult;
		radius *= radius_mult;


		constexpr float BASE_DISTANCE = 84.0f;
		float CheckDistance = BASE_DISTANCE * giantScale * radius;

		Runtime::PlaySoundAtNode(Runtime::SNDR.GTSSoundShrinkOutburst, giant, explosion, "NPC Pelvis [Pelv]");
		Rumbling::For("ShrinkOutburst", giant, Rumble_Misc_ShrinkOutburst, 0.15f, "NPC COM [COM ]", 0.60f, 0.0f);

		SpawnParticle(giant, 6.00f, "GTS/Shouts/ShrinkOutburst.nif", NiMatrix3(), NodePosition, giantScale * explosion * 3.0f, 7, nullptr); // Spawn effect

		if (IsDebugEnabled() && (giant->formID == 0x14 || IsTeammate(giant))) {
			DebugDraw::DrawSphere(glm::vec3(NodePosition.x, NodePosition.y, NodePosition.z), CheckDistance, 600, { 0.0f, 1.0f, 0.0f, 1.0f });
		}

		NiPoint3 giantLocation = giant->GetPosition();
		for (auto otherActor : find_actors()) {
			if (otherActor != giant) {
				NiPoint3 actorLocation = otherActor->GetPosition();
				if ((actorLocation - giantLocation).Length() < BASE_DISTANCE * giantScale * radius * 3) {
					int nodeCollisions = 0;

					auto model = otherActor->GetCurrent3D();

					if (model) {
						VisitNodes(model, [&nodeCollisions, NodePosition, CheckDistance](NiAVObject& a_obj) {
							float distance = (NodePosition - a_obj.world.translate).Length();
							if (distance < CheckDistance) {
								nodeCollisions += 1;
								return false;
							}
							return true;
							});
					}
					if (nodeCollisions > 0) {
						ShrinkOutburst_Shrink(giant, otherActor, shrink, gigantism);
					}
				}
			}
		}
	}


}
