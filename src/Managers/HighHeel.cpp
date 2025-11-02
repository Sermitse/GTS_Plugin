#include "Managers/HighHeel.hpp"
#include "Managers/Animation/AnimationManager.hpp"



#include "Config/Config.hpp"

using json = nlohmann::json;
using namespace GTS;

namespace {
	bool DisableHighHeels(Actor* actor) {
		bool disable = (
			AnimationManager::HHDisabled(actor) || !Config::General.bEnableHighHeels ||
			BehaviorGraph_DisableHH(actor) || IsCrawling(actor) || 
			IsProning(actor)
		);
		return disable;
	}
	bool DisableOnFurniture(Actor* actor) {
		const auto ActorState = actor->AsActorState()->GetSitSleepState();
		bool DisableFurniture = Config::General.bHighheelsFurniture;
		bool Sleeping = false;
		bool Sitting = false;
		
		switch (ActorState) {
			case SIT_SLEEP_STATE::kIsSitting:
				Sitting = true;
			break;
			case SIT_SLEEP_STATE::kIsSleeping:
			case SIT_SLEEP_STATE::kWaitingForSleepAnim:
			case SIT_SLEEP_STATE::kWantToWake:
				Sleeping = true;
			break;
		}
		
		bool ShouldBeDisabled = (DisableFurniture && Sitting) || (Sleeping);
		return ShouldBeDisabled;
	}
}

namespace GTS {
	HighHeelManager& HighHeelManager::GetSingleton() noexcept {
		static HighHeelManager instance;
		return instance;
	}

	std::string HighHeelManager::DebugName() {
		return "::HighHeelManager";
	}

	void HighHeelManager::HavokUpdate() {
		GTS_PROFILE_SCOPE("HHMgr: HavokUpdate");
		auto actors = FindSomeActors("HHHavokUpdate", 1);
		for (auto actor: actors) {
			ApplyHH(actor, false);
		}
	}

	void HighHeelManager::ActorEquip(Actor* actor) {
		ActorHandle actorHandle = actor->CreateRefHandle();
		std::string taskname = std::format("ActorEquip_{}", actor->formID);

		TaskManager::RunOnce(taskname, [=](auto& update){
			if (!actorHandle) {
				return;
			}

			auto get_actor = actorHandle.get().get();
			this->ApplyHH(get_actor, true);
		});
	}
	void HighHeelManager::ActorLoaded(Actor* actor) {
		ActorHandle actorHandle = actor->CreateRefHandle();
		std::string taskname = std::format("ActorLoaded_{}", actor->formID);

		TaskManager::RunOnce(taskname, [=](auto& update){
			if (!actorHandle) {
				return;
			}

			auto get_actor = actorHandle.get().get();
			this->ApplyHH(get_actor, true);
		});
	}

	void HighHeelManager::OnAddPerk(const AddPerkEvent& evt) {
		//log::info("Add Perk fired");
		if (evt.perk == Runtime::GetPerk("GTSPerkHighHeels")) {
			for (auto actor: find_actors()) {
				if (actor) {
					this->data.try_emplace(actor);
					auto& hhData = this->data[actor];
					hhData.wasWearingHh = false;
				}
			}
		}
	}

	void HighHeelManager::ApplyHH(Actor* actor, bool force) {
		GTS_PROFILE_SCOPE("HHMgr: ApplyHH");
		if (actor) {

			if (!IsHumanoid(actor)) {
				return;
			}

			if (actor->Is3DLoaded()) {
				this->data.try_emplace(actor);
				auto& hhData = this->data[actor];
				float speedup = 1.0f;
				if (IsCrawling(actor) || IsProning(actor) || BehaviorGraph_DisableHH(actor)) {
					speedup = 4.0f; // To shift down a lot faster
				}
				else if (!IsGtsBusy(actor)) {
					speedup = 3.0f;
				}

				if (DisableHighHeels(actor) || DisableOnFurniture(actor)) {
					hhData.multiplier.target = 0.0f;
					hhData.multiplier.halflife = 1 / (AnimationManager::GetAnimSpeed(actor) * AnimationManager::GetHighHeelSpeed(actor) * speedup);
				} else {
					hhData.multiplier.target = 1.0f;
					hhData.multiplier.halflife = 1 / (AnimationManager::GetAnimSpeed(actor) * 1.0f * speedup);
					// Some GTS animations use smooth transitions between enabling/disabling HH and it looks ugly with halflife 0
					// Don't make halflife 0
				}

				if (!Config::General.bEnableHighHeels) {
					return;
				}

				GTS::HighHeelManager::UpdateHHOffset(actor);

				// With model scale do it in unscaled coords
				NiPoint3 new_hh = GTS::HighHeelManager::GetBaseHHOffset(actor) * hhData.multiplier.value;

				for (bool person: {false, true}) {
					auto npc_root_node = find_node(actor, "NPC", person);

					if (npc_root_node) {
						NiPoint3 current_value = npc_root_node->local.translate;
						NiPoint3 delta = current_value - new_hh;

						if (delta.Length() > 1e-5 || force) {
							npc_root_node->local.translate = new_hh;
							update_node(npc_root_node);
						}
						bool wasWearingHh = hhData.wasWearingHh;
						bool isWearingHH = fabs(new_hh.Length()) > 1e-4;
						if (isWearingHH != wasWearingHh) {
							// Just changed hh
							HighheelEquip hhEvent = HighheelEquip {
								.actor = actor,
								.equipping = isWearingHH,
								.hhLength = new_hh.Length(),
								.hhOffset = new_hh,
								.shoe = actor->GetWornArmor(BGSBipedObjectForm::BipedObjectSlot::kFeet),
							};
							EventDispatcher::DoHighheelEquip(hhEvent);
							hhData.wasWearingHh = isWearingHH;
						}
					}
				}
			}
		}
	}

	// This seems to be the most expensive HH function so far, not sure how to optimize it properly
	void HighHeelManager::UpdateHHOffset(Actor* actor) {
		GTS_PROFILE_SCOPE("HHMgr: UpdateHHOffset");
		auto models = GetModelsForSlot(actor, BGSBipedObjectForm::BipedObjectSlot::kFeet);
		NiPoint3 result = NiPoint3();
		for (auto model: models) {
			if (model) {
				VisitExtraData<NiFloatExtraData>(model, "HH_OFFSET", [&result](NiAVObject& currentnode, NiFloatExtraData& data) {
					result.z = fabs(data.value);
					return false;
				});
				VisitExtraData<NiStringExtraData>(model, "SDTA", [&result](NiAVObject& currentnode, NiStringExtraData& data) {
					std::string stringDataStr = data.value;
					try{
						std::stringstream jsonData(stringDataStr);
						json j = json::parse(jsonData);
						for (const auto& alteration: j) {
							if (alteration.contains("name") && alteration.contains("pos") && alteration["name"] == "NPC" && alteration["pos"].size() > 2) {
								auto p = alteration["pos"].template get<std::vector<float> >();
								result = NiPoint3(p[0], p[1], p[2]);
								return false;
							}
						}
						return true;
					} catch (const json::exception& e) {
						//log::warn("JSON parse error: {}. Using alternate method", e.what());

						auto posStart = stringDataStr.find("\"pos\":[");
						if (posStart == std::string::npos) {
							//log::warn("Pos not found in string. High Heel will be disabled");
							return true;
						}

						posStart += 7;
						auto posEnd = stringDataStr.find("]", posStart);

						if (posEnd != std::string::npos && posStart != posEnd) {
								
							std::string posString = stringDataStr.substr(posStart, posEnd - posStart);

							auto posValueStart = 0;
							auto posValueEnd = static_cast<int>(posString.find(",", posValueStart));
							
							float pos_x = static_cast<float>(std::stod(posString.substr(posValueStart, posValueEnd - posValueStart)));
							
							posValueStart = posValueEnd + 1;
							posValueEnd = static_cast<int>(posString.find(",", posValueStart));
							float pos_y = static_cast<float>(std::stod(posString.substr(posValueStart, posValueEnd - posValueStart)));
							
							posValueStart = posValueEnd + 1;
							float pos_z = static_cast<float>(std::stod(posString.substr(posValueStart)));

							result = NiPoint3(pos_x, pos_y, pos_z);
							return false;
						}

						return true;
					} catch (const std::exception& e) {
						//log::warn("Error while parsing the JSON data: {}", e.what());
						return true;
					}
				});
			}
		}
		//log::info("Base HHOffset: {}", Vector2Str(result));
		auto npcNodeScale = get_npcparentnode_scale(actor);

		auto& me = HighHeelManager::GetSingleton();
		me.data.try_emplace(actor);
		auto& hhData = me.data[actor];
		hhData.lastBaseHHOffset = result * npcNodeScale; // Record hh height that is affected by natural scale for .z offset of model
		hhData.InitialHeelHeight = result.z; // Record initial hh height for hh damage boost
	}

	// Unscaled base .z offset of HH, takes Natural Scale into account
	NiPoint3 HighHeelManager::GetBaseHHOffset(Actor* actor) {  
		GTS_PROFILE_SCOPE("HHMgr: GetBaseHHOffset");
		auto& me = HighHeelManager::GetSingleton();
		me.data.try_emplace(actor);
		auto& hhData = me.data[actor];
		return hhData.lastBaseHHOffset;
	}

	NiPoint3 HighHeelManager::GetHHOffset(Actor* actor) { // Scaled .z offset of HH
		GTS_PROFILE_SCOPE("HHMgr: GetHHOffset");
		auto Scale = get_visual_scale(actor);
		return HighHeelManager::GetBaseHHOffset(actor) * Scale;
	}

	float HighHeelManager::GetInitialHeelHeight(Actor* actor) { // Get raw heel height, used in damage bonus for HH perk
		GTS_PROFILE_SCOPE("HH: GetInitHeight");
		auto& me = HighHeelManager::GetSingleton();
		me.data.try_emplace(actor);
		auto& hhData = me.data[actor];
		return hhData.InitialHeelHeight * 0.01f;
	}

	float HighHeelManager::GetHHMultiplier(Actor* actor) {
		auto& me = HighHeelManager::GetSingleton();
		me.data.try_emplace(actor);
		auto& hhData = me.data[actor];
		return hhData.multiplier.value;
	}

	bool HighHeelManager::IsWearingHH(Actor* actor) {
		return HighHeelManager::GetBaseHHOffset(actor).Length() > 1e-3;
	}
}