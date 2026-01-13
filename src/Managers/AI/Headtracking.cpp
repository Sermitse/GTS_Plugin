#include "Managers/AI/Headtracking.hpp"

using namespace GTS;

namespace {

	constexpr float REDUCTION_FACTOR = 0.44f;
	constexpr float PI = std::numbers::pi_v<float>;

	NiPoint3 HeadLocation(TESObjectREFR& obj, const float& scale) {
		NiPoint3 headOffset(0.0f, 0.0f, 0.0f);
		auto location = obj.GetPosition();
		auto asActor = skyrim_cast<Actor*>(&obj);
		if (asActor) {
			auto charCont = asActor->GetCharController();
			if (charCont) {
				headOffset.z = charCont->actorHeight * 70.0f * scale;// * get_natural_scale(asActor, true);
			}
		}
		return location + headOffset;
	}

	NiPoint3 HeadLocation(TESObjectREFR& obj) {
		float scale = 1.0f;
		auto asActor = skyrim_cast<Actor*>(&obj);
		if (asActor) {
			scale = get_visual_scale(asActor);
		}
		return HeadLocation(obj, scale);
	}

	NiPoint3 HeadLocation(TESObjectREFR* obj, const float& scale) {
		if (!obj) {
			return NiPoint3();
		} else {
			return HeadLocation(*obj, scale);
		}
	}

	NiPoint3 HeadLocation(TESObjectREFR* obj) {
		if (!obj) {
			return NiPoint3();
		} else {
			return HeadLocation(*obj);
		}
	}

	NiPoint3 HeadLocation(const ActorHandle& objRefr, const float& scale) {
		if (!objRefr) {
			return NiPoint3();
		} else {
			auto obj = objRefr.get().get();
			if (!obj) {
				return NiPoint3();
			}
			return HeadLocation(*obj, scale);
		}
	}

	NiPoint3 HeadLocation(const ActorHandle& objRefr) {
		if (!objRefr) {
			return NiPoint3();
		} else {
			auto obj = objRefr.get().get();
			if (!obj) {
				return NiPoint3();
			}
			return HeadLocation(*obj);
		}
	}

	NiPoint3 HeadLocation(const ObjectRefHandle& objRefr, const float& scale) {
		if (!objRefr) {
			return NiPoint3();
		} else {
			auto obj = objRefr.get().get();
			if (!obj) {
				return NiPoint3();
			}
			return HeadLocation(*obj, scale);
		}
	}
	NiPoint3 HeadLocation(const ObjectRefHandle& objRefr) {
		if (!objRefr) {
			return NiPoint3();
		} else {
			auto obj = objRefr.get().get();
			if (!obj) {
				return NiPoint3();
			}
			return HeadLocation(*obj);
		}
	}

	// Rotate spine to look at an actor either leaning back or looking down
	void RotateSpine(Actor* giant, Actor* tiny, HeadtrackingData& data) {
		if (giant->formID == 0x14) {
			return;
		}

		float finalAngle = 0.0f;
		if (tiny) { // giant is the actor that is looking, tiny is the one that is being looked at (Player for example)
			//log::info("Tiny is: {}", tiny->GetDisplayFullName());

			/*
			float Collision_PitchMult = 0.0f;
			if (AnimationVars::Other::IsCollisionInstalled(giant)) { //Detects 'Precision' mod
				Collision_PitchMult = AnimationVars::Other::CollisionPitchMult(giant); // If true, obtain value to apply it
				//log::info("Collision Pitch Mult: {}", Collision_PitchMult);
			}
			*/

			auto dialoguetarget = giant->GetActorRuntimeData().dialogueItemTarget;
			if (dialoguetarget) {
				// In dialogue
				if (giant != tiny) { // Just to make sure
					// With valid look at target
					AnimationVars::General::SetIsInDialogue(giant, true);
					auto meHead = HeadLocation(giant);
					//log::info("  - meHead: {}", Vector2Str(meHead));
					auto targetHead = HeadLocation(tiny);
					//log::info("  - targetHead: {}", Vector2Str(targetHead));
					auto directionToLook = targetHead - meHead;
					//log::info("  - directionToLook: {}", Vector2Str(directionToLook));
					directionToLook = directionToLook * (1/directionToLook.Length());
					//log::info("  - Norm(directionToLook): {}", Vector2Str(directionToLook));
					NiPoint3 upDirection = NiPoint3(0.0f, 0.0f, 1.0f);
					auto sinAngle = directionToLook.Dot(upDirection);
					//log::info("  - cosAngle: {}", sinAngle);
					auto angleFromUp = fabs(acos(sinAngle) * 180.0f / PI);
					//log::info("  - angleFromUp: {}", angleFromUp);
					float angleFromForward = -(angleFromUp - 90.0f) * REDUCTION_FACTOR;
					//log::info("  - angleFromForward: {}", angleFromForward);

					finalAngle = std::clamp(angleFromForward * REDUCTION_FACTOR, -60.f, 60.f);
					//log::info("  - finalAngle: {}", finalAngle);
				}
			
			}
			else {
				// Not in dialog
				if (fabs(data.spineSmooth.value) < 1e-3) {
					// Finihed smoothing back to zero
					AnimationVars::General::SetIsInDialogue(giant, false); // Disallow
					//log::info("Setting InDialogue to false");
				}
			}
			//log::info("Pitch Override of {} is {}", giant->GetDisplayFullName(), data.spineSmooth.value);
		}
		data.spineSmooth.target = finalAngle;
		AnimationVars::General::SetPitchOverride(giant, data.spineSmooth.value);
	}
}

namespace GTS {

	std::string Headtracking::DebugName() {
		return "::Headtracking";
	}

	void Headtracking::Update() {
		for (auto actor: find_actors()) {
			this->data.try_emplace(actor->formID);
			if (actor->formID == 0x14 || IsTeammate(actor)) {
				SpineUpdate(actor);
			}
		}
	}

	void Headtracking::SpineUpdate(Actor* me) {
		GTS_PROFILE_SCOPE("Headtracking: SpineUpdate");
		if (me->formID == 0x14) {
			return;
		}
		auto ai = me->GetActorRuntimeData().currentProcess;
		Actor* tiny = nullptr;
		if (ai) {
			auto targetObjHandle = ai->GetHeadtrackTarget();
			if (targetObjHandle) {
				auto target = targetObjHandle.get().get();
				auto asActor = skyrim_cast<Actor*>(target);
				if (asActor) {
					tiny = asActor;
				}
			}
		}
		this->data.try_emplace(me->formID);
		RotateSpine(me, tiny, this->data.at(me->formID));
	}
}
