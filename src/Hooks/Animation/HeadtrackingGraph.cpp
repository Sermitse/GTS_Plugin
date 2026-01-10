#include "Hooks/Animation/HeadTrackingGraph.hpp"
#include "Hooks/Util/HookUtil.hpp"

namespace {

	using namespace GTS;
        
	int ptrOffset = REL::Module::get().version().compare(SKSE::RUNTIME_SSE_1_6_629) == std::strong_ordering::less ? -0xB8 : -0xC0;

	void Headtracking_ManageSpineToggle(Actor* actor) {
		if (actor && actor->Is3DLoaded()) {
			// Player is handled inside HeadTracking.cpp -> SetGraphVariableBool Hook
			std::string taskname = std::format("SpineBool_{}", actor->formID);
			ActorHandle giantHandle = actor->CreateRefHandle();

			double Start = Time::WorldTimeElapsed();

			TaskManager::RunFor(taskname, 1.0f, [=](auto& progressData){
				if (!giantHandle) {
					return false;
				}

				double Finish = Time::WorldTimeElapsed();

				double timepassed = Finish - Start;
				if (timepassed > 0.10) {
					auto giant = giantHandle.get().get();

					bool Disable = !(AnimationVars::Crawl::IsCrawling(giant) || AnimationVars::Prone::IsProne(giant) ||
						AnimationVars::Crawl::GetIsHandStomping(actor) || 
						AnimationVars::Crawl::GetIsHandStompingStrong(actor));

					AnimationVars::Other::SetSpineRotationEnabled(giant, Disable);
					//log::info("Setting {} for {}", Disable, giant->GetDisplayFullName());
					return false;
				}
				return true;
			});
		}
	}
}


namespace Hooks {

	struct SetGraphVariableBool {

		static bool thunk(IAnimationGraphManagerHolder* a_graph, const BSFixedString& a_variableName, bool a_in) {

			bool result = a_in;

			{
				GTS_PROFILE_ENTRYPOINT("Animation::SetGraphVariableBool");

				//log::info("SetGraphVariableBool hooked");
				// Disable weird spine rotation during crawl/prone
				if (a_variableName == "bHeadTrackSpine") {
					// Done through hook since TDM seems to adjust it constantly
					auto actor = skyrim_cast<Actor*>(a_graph);
					if (actor) {

						bool ShouldDisable = (AnimationVars::Crawl::IsCrawling(actor) || AnimationVars::Prone::IsProne(actor) ||
							AnimationVars::Crawl::GetIsHandStomping(actor) || 
							AnimationVars::Crawl::GetIsHandStompingStrong(actor));

						if (ShouldDisable) {
							result = false;
						}
					}
				}
			}

			return func(a_graph, a_variableName, result);
		}

		FUNCTYPE_DETOUR func;

	};

	struct AddMovementFlagsSneak {

		static bool thunk(RE::ActorState* a_this, int16_t a_flag) {

			{
				GTS_PROFILE_ENTRYPOINT("AnimationHeadTrack::AddMovementFlagsSneak");

				//Adjust actor pointer based on game version, AE > .629 Changed Struct Layouts
				auto actor = SKSE::stl::adjust_pointer<RE::Actor>(a_this, ptrOffset);
				if (actor) {
					// Toggle spine HT on/off based on GTS state
					Headtracking_ManageSpineToggle(actor);
				}
			}

			return func(a_this, a_flag);
		}

		FUNCTYPE_CALL func;

	};

	struct RemoveMovementFlagsSneak {

		static bool thunk(RE::ActorState* a_this, int16_t a_flag) {

			{
				GTS_PROFILE_ENTRYPOINT("AnimationHeadTrack::RemoveMovementFlagsSneak");

				//Adjust actor pointer based on game version, AE > .629 Changed Struct Layouts
				auto actor = SKSE::stl::adjust_pointer<RE::Actor>(a_this, ptrOffset);
				if (actor) {
					// Toggle spine HT on/off based on GTS state
					Headtracking_ManageSpineToggle(actor);
				}
			}

			return func(a_this, a_flag);
		}

		FUNCTYPE_CALL func;

	};

	void Hook_HeadTrackingGraph::Install() {

		logger::info("Installing HeadTrackingGraph Hooks...");

		stl::write_detour<SetGraphVariableBool>(REL::RelocationID(32141, 32885, NULL));
		stl::write_call<AddMovementFlagsSneak>(REL::RelocationID(36926, 37951, NULL), REL::VariantOffset(0xE4, 0xA0, NULL));
		stl::write_call<RemoveMovementFlagsSneak>(REL::RelocationID(36926, 37951, NULL), REL::VariantOffset(0xEB, 0xB2, NULL));
	}

}