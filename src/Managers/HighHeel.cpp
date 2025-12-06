#include "Managers/HighHeel.hpp"
#include "Managers/Animation/AnimationManager.hpp"



#include "Config/Config.hpp"

using namespace GTS;

struct SDTAAlteration {
	std::string name;
	std::vector<float> pos;
};

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
		if (evt.perk == Runtime::GetPerk(Runtime::PERK.GTSPerkHighHeels)) {
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



	void HighHeelManager::UpdateHHOffset(Actor* actor) {
		GTS_PROFILE_SCOPE("HHMgr: UpdateHHOffset");
		auto models = GetModelsForSlot(actor, BGSBipedObjectForm::BipedObjectSlot::kFeet);
		NiPoint3 result{};

		for (auto model : models) {
			if (!model) continue;

			VisitExtraData<NiFloatExtraData>(model, "HH_OFFSET",
				[&result](NiAVObject&, NiFloatExtraData& data) {
					result.z = std::fabs(data.value);
					return false;
				}
			);

			VisitExtraData<NiStringExtraData>(model, "SDTA", [&result](NiAVObject&, NiStringExtraData& data) {
				std::string_view sv = data.value;

				// Quick reject
				if (sv.find("\"pos\"") == std::string_view::npos) {
					return true;
				}

				// Sanitize only if needed (check for malformed float pattern)
				std::string sanitized;
				bool needsSanitization = false;

				// Quick scan for double-dot pattern
				for (size_t i = 0; i + 2 < sv.size(); ++i) {
					if (sv[i] == '.' && std::isdigit(static_cast<unsigned char>(sv[i + 1]))) {
						size_t j = i + 1;
						while (j < sv.size() && std::isdigit(static_cast<unsigned char>(sv[j]))) ++j;
						if (j < sv.size() && sv[j] == '.') {
							needsSanitization = true;
							break;
						}
					}
				}

				std::string_view parseView = sv;
				if (needsSanitization) {
					sanitized.reserve(sv.size());
					sanitized = sv;

					// Sanitize malformed floats
					size_t i = 0;
					while (i < sanitized.size()) {
						if (std::isdigit(static_cast<unsigned char>(sanitized[i]))) {
							size_t numStart = i;
							while (i < sanitized.size() && std::isdigit(static_cast<unsigned char>(sanitized[i]))) ++i;

							if (i < sanitized.size() && sanitized[i] == '.') {
								++i; // First dot
								if (i < sanitized.size() && std::isdigit(static_cast<unsigned char>(sanitized[i]))) {
									while (i < sanitized.size() && std::isdigit(static_cast<unsigned char>(sanitized[i]))) ++i;

									// Second dot (malformed)
									if (i < sanitized.size() && sanitized[i] == '.') {
										size_t endPos = i;
										while (endPos < sanitized.size() &&
											(std::isdigit(static_cast<unsigned char>(sanitized[endPos])) || sanitized[endPos] == '.')) {
											++endPos;
										}
										sanitized.erase(i, endPos - i);
										continue;
									}
								}
							}
						}
						++i;
					}
					parseView = sanitized;
				}

				// Try glaze parsing
				std::vector<SDTAAlteration> alterations;
				auto ec = glz::read_json(alterations, parseView);

				if (!ec) {
					for (const auto& alt : alterations) {
						if (alt.name == "NPC" && alt.pos.size() >= 3) {
							result = NiPoint3(alt.pos[0], alt.pos[1], alt.pos[2]);
							return false;
						}
					}
					return true;
				}

				// Fallback manual parser for malformed JSON
				size_t posStart = parseView.find("\"pos\":[");
				if (posStart == std::string_view::npos) {
					return true;
				}
				posStart += 7;
				size_t posEnd = parseView.find(']', posStart);
				if (posEnd == std::string_view::npos || posEnd == posStart) {
					return true;
				}

				const char* ptr = parseView.data() + posStart;
				const char* end = parseView.data() + posEnd;
				char* endptr;
				float vals[3];

				for (int i = 0; i < 3; ++i) {
					while (ptr < end && (*ptr == ',' || std::isspace(static_cast<unsigned char>(*ptr)))) ++ptr;
					if (ptr >= end) return true;

					vals[i] = std::strtof(ptr, &endptr);
					if (endptr == ptr || !std::isfinite(vals[i])) return true;
					ptr = endptr;
				}

				result = NiPoint3(vals[0], vals[1], vals[2]);
				return false;
			});
		}

		auto npcNodeScale = get_npcparentnode_scale(actor);
		static auto& me = GetSingleton();
		me.data.try_emplace(actor);
		auto& hhData = me.data[actor];
		hhData.lastBaseHHOffset = result * npcNodeScale;
		hhData.InitialHeelHeight = result.z;
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