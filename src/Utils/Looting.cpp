#include "Utils/Looting.hpp"

#include "Config/Config.hpp"

#include "Managers/AI/AIFunctions.hpp"
#include "Systems/Rays/Raycast.hpp"


using namespace GTS;

namespace {

	void RunScaleTask(const ObjectRefHandle& dropboxHandle, Actor* actor, const double Start, const float Scale, const bool soul, const NiPoint3 TotalPos) {

		const std::string taskname = std::format("Dropbox {}", actor->formID); // create task name for main task
		TaskManager::RunFor(taskname, 16, [=](auto&) { // Spawn loot piles
			if (!dropboxHandle) {
				return false;
			}
			const double Finish = Time::WorldTimeElapsed();
			const auto dropboxPtr = dropboxHandle.get().get();
			if (!dropboxPtr->Is3DLoaded()) {
				return true;
			}
			const auto dropbox3D = dropboxPtr->GetCurrent3D();
			if (!dropbox3D) {
				return true; // Retry next frame
			} else {
				double timepassed = Finish - Start;
				if (soul) {
					timepassed *= 1.33; // faster soul scale
				}
				const auto node = find_object_node(dropboxPtr, "GorePile_Obj");
				const auto trigger = find_object_node(dropboxPtr, "Trigger_Obj");
				if (node) {
					node->local.scale = (Scale * 0.33f) + static_cast<float>(timepassed*0.18);
					if (!soul) {
						node->world.translate.z = TotalPos.z;
					}
					update_node(node);
				}
				if (trigger) {
					trigger->local.scale = (Scale * 0.33f) + static_cast<float>(timepassed*0.18);
					if (!soul) {
						trigger->world.translate.z = TotalPos.z;
					}
					update_node(trigger);
				}
				if (node && node->local.scale >= Scale) { // disable collision once it is scaled enough
					return false; // End task
				}
				return true;
			}
		});
	}

	void RunAudioTask(const ObjectRefHandle& dropboxHandle, Actor* actor) {
		std::string taskname_sound = std::format("DropboxAudio {}", actor->formID);
		TaskManager::RunFor(taskname_sound, 6, [=](auto& progressData) {
			if (!dropboxHandle) {
				return false;
			}
			auto dropboxPtr = dropboxHandle.get().get();
			if (!dropboxPtr->Is3DLoaded()) {
				return true; // retry
			}
			auto dropbox3D = dropboxPtr->GetCurrent3D();
			if (!dropbox3D) {
				return true; // Retry next frame
			} else {
				Runtime::PlaySound(Runtime::SNDR.GTSSoundCrushDefault, dropboxPtr, 1.0f, 1.0f);
				return false;
			}
		});
	}
}

namespace GTS {

	NiPoint3 GetContainerSpawnLocation(Actor* giant, Actor* tiny) {
		bool success_first = false;
		bool success_second = false;
		NiPoint3 ray_start = tiny->GetPosition();
		ray_start.z += 40.0f; // overrize .z with giant .z + 40, so ray starts from above
		NiPoint3 ray_direction(0.0f, 0.0f, -1.0f);

		constexpr float ray_length = 40000.f;

		NiPoint3 pos = NiPoint3(0, 0, 0); // default pos
		NiPoint3 endpos = CastRayStatics(tiny, ray_start, ray_direction, ray_length, success_first);
		if (success_first) {
			return endpos;
		} else if (!success_first) {
			NiPoint3 ray_start_second = giant->GetPosition();
			ray_start_second.z += 40.0f;
			pos = CastRayStatics(giant, ray_start_second, ray_direction, ray_length, success_second);
			if (!success_second) {
				pos = giant->GetPosition();
				log::info("Ray cast failed");
				return pos;
			}
			return pos;
		}
		return pos;
	}

	void TransferInventory(Actor* from, Actor* to, const float scale, bool keepOwnership, bool removeQuestItems, DamageSource Cause, bool reset) {
		std::string name = std::format("TransferItems_{}_{}", from->formID, to->formID);

		bool reanimated = false; // shall we avoid transfering items or not.
		if (Cause != DamageSource::Vored) {
			reanimated = WasReanimated(from);
		}
		if (Runtime::IsRace(from, Runtime::RACE.IcewraithRace)) {
			reanimated = true;
		}
		// ^ we generally do not want to transfer loot in that case: 2 loot piles will spawn if actor was resurrected

		double Start = Time::WorldTimeElapsed();
		ActorHandle gianthandle = to->CreateRefHandle();
		ActorHandle tinyhandle = from->CreateRefHandle();

		const auto& Settings = Config::General;
		const bool PCLoot = Settings.bPlayerLootpiles;
		const bool NPCLoot = Settings.bFollowerLootpiles;

		double expectedtime = 0.15;
		if (IsDragon(from)) {
			expectedtime = 0.45; // Because dragons don't spawn loot right away...sigh...
		}

		if (reset) {
			StartActorResetTask(from); // reset actor data.
		}

		TaskManager::RunFor(name, 3.0f, [=](auto&) {
			if (!tinyhandle) {
				return false;
			}
			if (!gianthandle) {
				return false;
			}
			auto tiny = tinyhandle.get().get();
			auto giant = gianthandle.get().get();

			if (tiny && !tiny->IsDead()) {
				KillActor(giant, tiny); // just to make sure
			}

			float hp = GetAV(tiny, ActorValue::kHealth);

			if (tiny && (tiny->IsDead() || hp <= 0.0f)) {
				double Finish = Time::WorldTimeElapsed();
				double timepassed = Finish - Start;
				if (timepassed < expectedtime) {
					return true; // retry, not enough time has passed yet
				}

				if (giant->formID == 0x14 && !PCLoot) {
					TransferInventory_Normal(giant, tiny, removeQuestItems);
					return false;
				}
				if (giant->formID != 0x14 && !NPCLoot) {
					TransferInventory_Normal(giant, tiny, removeQuestItems);
					return false;
				}
				TransferInventoryToDropbox(giant, tiny, scale, removeQuestItems, Cause, reanimated);
				return false; // stop it, we started the looting of the Target.
			}
			return true;
		});
	}

	void TransferInventory_Normal(Actor* giant, Actor* tiny, bool removeQuestItems) {
		int32_t quantity = 1;

		for (auto &[a_object, invData]: tiny->GetInventory()) { // transfer loot
			if (a_object->GetPlayable() && a_object->GetFormType() != FormType::LeveledItem) {
				if ((!invData.second->IsQuestObject() || removeQuestItems)) {

					TESObjectREFR* ref = skyrim_cast<TESObjectREFR*>(tiny);
					if (ref) {
						auto changes = ref->GetInventoryChanges();
						if (changes) {
							//Clib version produces garbage values
							quantity = GetItemCount(changes, a_object); // obtain item count
						}
					}

					tiny->RemoveItem(a_object, quantity, ITEM_REMOVE_REASON::kRemove, nullptr, giant, nullptr, nullptr);
				}
			}
		}
	}

	void TransferInventoryToDropbox(Actor* giant, Actor* actor, const float scale, bool removeQuestItems, DamageSource Cause, bool Resurrected) {
		bool soul = false;
		float Scale = std::clamp(scale, 0.10f, 4.4f);

		if (Resurrected) {
			return;
		}

		RuntimeData::RuntimeEntry<TESObjectCONT>* container = nullptr;
		std::string name = std::format("{} remains", actor->GetDisplayFullName());

		if (IsMechanical(actor)) {
			container = &Runtime::CONT.GTSDropboxMechanical;
		} 
		else if (Cause == DamageSource::Vored) { // Always spawn soul on vore
			container = &Runtime::CONT.GTSDropboxSoul;
			name = std::format("{} Soul Remains", actor->GetDisplayFullName());
			soul = true;
		} 
		else if (LessGore()) { // Always Spawn soul if Less Gore is on
			container = &Runtime::CONT.GTSDropboxSoul;
			name = std::format("Crushed Soul of {} ", actor->GetDisplayFullName());
			soul = true;
		} 
		else if (IsInsect(actor, false)) {
			container = &Runtime::CONT.GTSDropboxInsect;
			name = std::format("Remains of {}", actor->GetDisplayFullName());
		} 
		else if (IsLiving(actor)) {
			container = &Runtime::CONT.GTSDropboxGore; // spawn normal dropbox
		}
		else {
			container = &Runtime::CONT.GTSDropboxUndead;
		}

		NiPoint3 TotalPos = GetContainerSpawnLocation(giant, actor); // obtain goal of container position by doing ray-cast
		if (DebugDraw::CanDraw()) {
			DebugDraw::DrawSphere(glm::vec3(TotalPos.x, TotalPos.y, TotalPos.z), 8.0f, 6000, {1.0f, 1.0f, 0.0f, 1.0f});
		}
		auto dropbox = Runtime::PlaceContainerAtPos(actor, TotalPos, *container); // Place chosen container

		if (!dropbox) {
			return;
		}
		double Start = Time::WorldTimeElapsed();
		dropbox->SetDisplayName(name, false); // Rename container to match chosen name

		ObjectRefHandle dropboxHandle = dropbox->CreateRefHandle();

		if (Cause == DamageSource::Overkill) { // Play audio that won't disappear if source of loot transfer is Overkill
			RunAudioTask(dropboxHandle, actor); // play sound
		}
		if (dropboxHandle) {
			float scale_up = std::clamp(Scale, 0.10f, 1.0f);
			TotalPos.z += (200.0f - (200.0f * scale_up)); // move it a bit upwards
			RunScaleTask(dropboxHandle, actor, Start, Scale, soul, TotalPos); // Scale our pile over time
		}
		MoveItemsTowardsDropbox(actor, dropboxHandle.get().get(), removeQuestItems); // Launch transfer items task with a bit of delay
	}

	void MoveItemsTowardsDropbox(Actor* actor, TESObjectREFR* dropbox, bool removeQuestItems) {
		int32_t quantity = 1;
		for (auto &[a_object, invData]: actor->GetInventory()) { // transfer loot
			if (a_object->GetPlayable() && a_object->GetFormType() != FormType::LeveledItem) { // We don't want to move Leveled Items
				if ((!invData.second->IsQuestObject() || removeQuestItems)) {

					TESObjectREFR* ref = skyrim_cast<TESObjectREFR*>(actor);
					if (ref) {
						//log::info("Transfering item: {}, looking for quantity", a_object->GetName());
						auto changes = ref->GetInventoryChanges();
						if (changes) {
							quantity = GetItemCount(changes, a_object); // obtain item count
						}
					}
					quantity = std::max(0, quantity);
					//log::info("Transfering item: {}, quantity: {}", a_object->GetName(), quantity);
					if (dropbox) {
						actor->RemoveItem(a_object, quantity, ITEM_REMOVE_REASON::kRemove, nullptr, dropbox, nullptr, nullptr);
					}
				}
			}
		}
	}

	void MoveItems(const ActorHandle& giantHandle, const ActorHandle& tinyHandle, FormID ID, DamageSource Cause) {
		std::string taskname = std::format("MoveItems_{}", ID);
		TaskManager::RunOnce(taskname, [=](auto&){
			if (!tinyHandle || !giantHandle) {
				return;
			}
			auto giant = giantHandle.get().get();
			auto tiny = tinyHandle.get().get();
			float scale = get_visual_scale(tiny) * GetSizeFromBoundingBox(tiny);
			TransferInventory(tiny, giant, scale, false, true, Cause, true);
			// ^ transferInventory>TransferInventoryToDropBox also plays crush audio on loot pile
			// Works like that because Audio very often disappears on actors, so it's easier to play it on the object
		});
    }
}