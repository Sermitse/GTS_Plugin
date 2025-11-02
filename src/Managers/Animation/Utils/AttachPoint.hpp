#pragma once

#include "config/Config.hpp"

#include "Managers/HighHeel.hpp"
#include "Systems/Rays/Raycast.hpp"
#include "Debug/DebugDraw.hpp"

namespace GTS {

	static NiPoint3 CastRayDownwards_from(Actor* giant, std::string_view node) {
		bool success = false;
		auto object = find_node(giant, node);
		if (object) {
			NiPoint3 ray_start = object->world.translate;
			ray_start.z += 40.0f; // overrize .z with tiny .z + 40, so ray starts from above a bit
			NiPoint3 ray_direction(0.0f, 0.0f, -1.0f);

			float ray_length = 180 * get_visual_scale(giant);

			NiPoint3 endpos = CastRayStatics(giant, ray_start, ray_direction, ray_length, success);
			if (success) {
				return endpos;
			}
		}
		return NiPoint3(0.0f, 0.0f, 0.0f);
	}

	static NiPoint3 CastRayDownwards(Actor* tiny) {
		bool success = false;
		NiPoint3 ray_start = tiny->GetPosition();
		ray_start.z += 90.0f; // overrize .z with tiny .z + 90, so ray starts from above a bit
		NiPoint3 ray_direction(0.0f, 0.0f, -1.0f);

		float ray_length = 800;

		NiPoint3 endpos = CastRayStatics(tiny, ray_start, ray_direction, ray_length, success);
		if (success) {
			return endpos;
		}
		return tiny->GetPosition();
	}

	template<typename T, typename U>
	bool AttachTo_NoForceRagdoll(T& anyGiant, U& anyTiny, NiPoint3 point) {
		Actor* giant =  GetActorPtr(anyGiant);
		Actor* tiny =  GetActorPtr(anyTiny);

		if (!giant) {
			return false;
		}
		if (!tiny) {
			return false;
		}
		auto charcont = tiny->GetCharController();
		if (charcont) {
			charcont->SetLinearVelocityImpl((0.0f, 0.0f, 0.0f, 0.0f)); // Needed so Actors won't fall down.
		}
		tiny->SetPosition(point, true);
		return true;
	}
	template<typename T, typename U>
	bool AttachTo(T& anyGiant, U& anyTiny, NiPoint3 point) {
		Actor* giant =  GetActorPtr(anyGiant);
		Actor* tiny =  GetActorPtr(anyTiny);

		if (!giant) {
			return false;
		}
		if (!tiny) {
			return false;
		}

		tiny->SetPosition(point, true);

		ForceRagdoll(tiny, false);

		auto charcont = tiny->GetCharController();
		if (charcont) {
			charcont->SetLinearVelocityImpl((0.0f, 0.0f, 0.0f, 0.0f)); // Needed so Actors won't fall down.
		}

		if (IsDebugEnabled()) {
			DebugDraw::DrawSphere(glm::vec3(point.x, point.y, point.z), 6.0f, 40, {1.0f, 0.0f, 0.0f, 1.0f});
		}

		return true;
	}
	template<typename T, typename U>
	bool AttachTo(T& anyGiant, U& anyTiny, std::string_view bone_name) {
		Actor* giant = GetActorPtr(anyGiant);
		if (!giant) {
			return false;
		}
		auto bone = find_node(giant, bone_name);
		if (!bone) {
			return false;
		}
		return AttachTo(anyGiant, anyTiny, bone->world.translate);
	}

	template<typename T, typename U>
	bool AttachToObjectA(T& anyGiant, U& anyTiny) {
		return AttachTo(anyGiant, anyTiny, "AnimObjectA");
	}

	template<typename T, typename U>
	bool AttachToObjectB(T& anyGiant, U& anyTiny) {
		return AttachTo(anyGiant, anyTiny, "AnimObjectB");
	}

	template<typename T, typename U>
	bool AttachToObjectR(T& anyGiant, U& anyTiny) {
		return AttachTo(anyGiant, anyTiny, "AnimObjectR");
	}

	template<typename T, typename U>
	NiPoint3 AttachToObjectB_GetCoords(T& anyGiant, U& anyTiny) {
		Actor* giant = GetActorPtr(anyGiant);
		if (!giant) {
			return NiPoint3(0,0,0);
		}
		Actor* tiny = GetActorPtr(anyTiny);
		if (!tiny) {
			return NiPoint3(0,0,0);
		}

		auto ObjectA = find_node(giant, "AnimObjectB");
		if (!ObjectA) {
			return NiPoint3(0,0,0);
		}

		NiPoint3 coords = ObjectA->world.translate;//foot->world*(rotMat*point);
		coords.z = CastRayDownwards(tiny).z; // Cast ray down to get precise ground position
		return coords;
		//return false;
	}

	template<typename T, typename U>
	NiPoint3 AttachToUnderFoot(T& anyGiant, U& anyTiny, bool right_leg) {

		constexpr std::string_view leftFootLookup = "NPC L Foot [Lft ]";
		constexpr std::string_view rightFootLookup = "NPC R Foot [Rft ]";
		constexpr std::string_view leftCalfLookup = "NPC L Calf [LClf]";
		constexpr std::string_view rightCalfLookup = "NPC R Calf [RClf]";
		constexpr std::string_view leftToeLookup = "AnimObjectB";
		constexpr std::string_view rightToeLookup = "AnimObjectB";
		//constexpr std::string_view bodyLookup = "NPC Spine1 [Spn1]";

		Actor* giant = GetActorPtr(anyGiant);
		if (!giant) {
			return NiPoint3(0,0,0);
		}
		Actor* tiny = GetActorPtr(anyTiny);
		if (!tiny) {
			return NiPoint3(0,0,0);
		}

		NiPoint3 hhOffsetbase = HighHeelManager::GetBaseHHOffset(giant);

		std::string_view FootLookup = leftFootLookup;
		std::string_view CalfLookup = leftCalfLookup;
		std::string_view ToeLookup = leftToeLookup;

		if (right_leg) {
			FootLookup = rightFootLookup;
			CalfLookup = rightCalfLookup;
			ToeLookup = rightToeLookup;
		} 

		auto Foot = find_node(giant, FootLookup);
		auto Calf = find_node(giant, CalfLookup);
		auto Toe = find_node(giant, ToeLookup);

		if (!Foot) {
			return NiPoint3(0,0,0);
		}
		if (!Calf) {
			return NiPoint3(0,0,0);
		}
		if (!Toe) {
			log::info("Toe not found!");
			return NiPoint3(0,0,0);
		}
		NiMatrix3 footRotMat;
		{
			NiAVObject* foot = Foot;
			NiAVObject* calf = Calf;
			NiAVObject* toe = Toe;

			NiTransform inverseFoot = foot->world.Invert();
			NiPoint3 forward = inverseFoot*toe->world.translate;
			forward = forward / forward.Length();

			NiPoint3 up = inverseFoot*calf->world.translate;
			up = up / up.Length();

			NiPoint3 side = up.UnitCross(forward);
			forward = side.UnitCross(up); // Reorthonalize

			footRotMat = NiMatrix3(side, forward, up);
		}

		float hh = hhOffsetbase[2];
		// Make a list of points to check
		float Forward = 8 - (hh * 0.6f);
		float UpDown = 9;


		std::vector<NiPoint3> points = {
			NiPoint3(0, Forward, -(UpDown + hh * 0.65f)),
		};
		std::tuple<NiAVObject*, NiMatrix3> Coords(Foot, footRotMat);

		for (const auto& [foot, rotMat]: {Coords}) {
			std::vector<NiPoint3> footPoints = {};
			for (NiPoint3 point: points) {
				footPoints.push_back(foot->world*(rotMat*point));
				NiPoint3 coords = Foot->world.translate;//foot->world*(rotMat*point);
				coords.z = CastRayDownwards(tiny).z; // Cast ray down to get precise ground position
				return coords;
				//return AttachTo(anyGiant, anyTiny, coords);
			}
		}
		//return false
		return NiPoint3(0,0,0);
	}


	template<typename T, typename U>
	bool AttachToHand(T& anyGiant, U& anyTiny) {
		Actor* giant = GetActorPtr(anyGiant);
		if (!giant) {
			return false;
		}
		auto FingerA = find_node(giant, "NPC L Finger02 [LF02]");
		if (!FingerA) {
			log::info("FingerA not found");
			return false;
		}
		auto FingerB = find_node(giant, "NPC L Finger30 [LF30]");
		if (!FingerB) {
			log::info("FingerB not found");
			return false;
		}
		NiPoint3 coords = (FingerA->world.translate + FingerB->world.translate) / 2.0f;
		coords.z -= 3.0f;
		return AttachTo(anyGiant, anyTiny, coords);
	}

	template<typename T, typename U>
	bool HugAttach(T& anyGiant, U& anyTiny) {
		Actor* giant = GetActorPtr(anyGiant);
		if (!giant) {
			return false;
		}
		Actor* tiny = GetActorPtr(anyTiny);
		if (!tiny) {
			return false;
		}
		auto targetRootA = find_node(giant, "AnimObjectA");
		if (!targetRootA) {
			return false;
		}
		auto targetA = targetRootA->world.translate;

		float scaleFactor = get_visual_scale(tiny) / get_visual_scale(giant);

		NiPoint3 targetB = NiPoint3();
		std::vector<std::string_view> bone_names = {
			"NPC L Finger02 [LF02]",
			"NPC R Finger02 [RF02]",
			"L Breast02",
			"R Breast02"
		};
		std::uint32_t bone_count = static_cast<uint32_t>(bone_names.size());
		for (auto bone_name: bone_names) {
			auto bone = find_node(giant, bone_name);
			if (!bone) {
				Notify("Error: Breast Nodes could not be found.");
				Notify("Suggestion: install XP32 skeleton.");
				return false;
			}
			targetB += (bone->world * NiPoint3()) * (1.0f/bone_count);
		}

		// scaleFactor = std::clamp(scaleFactor, 0.0f, 1.0f);
		auto targetPoint = targetA*(scaleFactor) + targetB*(1.0f - scaleFactor);
		if (IsDebugEnabled()) {
			DebugDraw::DrawSphere(glm::vec3(targetA.x, targetA.y, targetA.z), 2.0f, 40, {1.0f, 0.0f, 0.0f, 1.0f});
			DebugDraw::DrawSphere(glm::vec3(targetB.x, targetB.y, targetB.z), 2.0f, 40, {0.0f, 1.0f, 0.0f, 1.0f});
			DebugDraw::DrawSphere(glm::vec3(targetPoint.x, targetPoint.y, targetPoint.z), 2.0f, 40, {0.0f, 0.0f, 1.0f, 1.0f});
		}

		/*if (Attachment_GetTargetNode(giant) == AttachToNode::ObjectA) {
			return AttachToObjectA(giant, tiny);
		}*/

		return AttachTo(anyGiant, anyTiny, targetPoint);
	}

	template<typename T, typename U>
	bool AttachToCleavage(T& anyGiant, U& anyTiny) {
		Actor* giant = GetActorPtr(anyGiant);
		if (!giant) {
			return false;
		}
		Actor* tiny = GetActorPtr(anyTiny);
		if (!tiny) {
			return false;
		}

		std::vector<std::string_view> bone_names = {
			"L Breast02",
			"R Breast02"
		};

		std::vector<std::string_view> center_bone_names = {
			"L Breast01",
			"R Breast01"
		};

		std::vector<std::string_view> up_bone_names = {
			"NPC L Clavicle [LClv]",
			"NPC R Clavicle [RClv]"
		};

		NiPoint3 clevagePos = NiPoint3();
		NiPoint3 centerBonePos = NiPoint3();
		NiPoint3 upBonePos = NiPoint3();

		for (auto bone_name: bone_names) {
			auto bone = find_node(giant, bone_name);
			if (!bone) {
				Notify("ERROR: Breast 02 bones not found");
				Notify("Install 3BB/XPMS32");
				return false;
			}
			if (IsDebugEnabled()) {
				DebugDraw::DrawSphere(glm::vec3(bone->world.translate.x, bone->world.translate.y, bone->world.translate.z), 2.0f, 10, {1.0f, 1.0f, 1.0f, 1.0f});
			}
			clevagePos += (bone->world * NiPoint3()) * (1.0f / bone_names.size());
		}

		// Center bone
		for (auto bone_name: center_bone_names) {
			auto bone = find_node(giant, bone_name);
			if (!bone) {
				Notify("ERROR: Breast 01 bones not found");
				Notify("Install 3BB/XPMS32");
				return false;
			}
			if (IsDebugEnabled()) {
				DebugDraw::DrawSphere(glm::vec3(bone->world.translate.x, bone->world.translate.y, bone->world.translate.z), 2.0f, 10, {1.0f, 1.0f, 1.0f, 1.0f});
			}
			centerBonePos += bone->world.translate  * (1.0f / center_bone_names.size());
		}

		// Up bone
		for (auto bone_name: up_bone_names) {
			auto bone = find_node(giant, bone_name);
			if (!bone) {
				Notify("ERROR: Clavicle bones not found");
				Notify("Install 3BB/XPMS32");
				return false;
			}
			if (IsDebugEnabled()) {
				DebugDraw::DrawSphere(glm::vec3(bone->world.translate.x, bone->world.translate.y, bone->world.translate.z), 2.0f, 10, {1.0f, 1.0f, 1.0f, 1.0f});
			}
			upBonePos += bone->world.translate  * (1.0f / up_bone_names.size());
		}

		// Forward
		NiPoint3 forward = (clevagePos - centerBonePos);
		forward.Unitize();
		// Up
		NiPoint3 up = (upBonePos - centerBonePos);
		up.Unitize();
		// Sideways
		NiPoint3 sideways = up.Cross(forward);
		sideways.Unitize();
		// Reorthorg
		forward = up.Cross(sideways * -1.0f);
		forward.Unitize();
		
		NiMatrix3 breastRotation = NiMatrix3(sideways, forward, up);


		// Manual offsets
		float difference = GetSizeDifference(giant, tiny, SizeType::GiantessScale, false, false) * 0.15f;

		const auto Offsets = Config::Gameplay.ActionSettings.f2CleavageOffset;

		float offset_Z = Offsets.at(0) * get_visual_scale(giant);
		float offset_Y = Offsets.at(1) * get_visual_scale(giant);


		// FIX tiny falling into breasts based on size
		offset_Y += difference;
		offset_Z += difference;

		// Sermite: Offset adjustment HERE
		NiPoint3 offset = NiPoint3(0.0f, offset_Y, offset_Z);

		

		// Global space offset
		NiPoint3 globalOffset = breastRotation * offset;

		// rotate tiny to face the same direction as gts
		if (tiny->formID == 0x14 && IsFirstPerson()) {
			// do nothing
		} else {
			tiny->data.angle.z = giant->data.angle.z;
		}

		clevagePos += globalOffset;

		if (IsDebugEnabled()) {
			DebugDraw::DrawSphere(glm::vec3(clevagePos.x, clevagePos.y, clevagePos.z), 2.0f, 10, {1.0f, 0.0f, 0.0f, 1.0f});
		}

		if (IsCleavageZIgnored(giant)) {
			auto objectB = find_node(giant, "AnimObjectB");
			if (objectB) {
				clevagePos.z = objectB->world.translate.z;
			}
		} /*else {
			clevagePos.z *= Animation_Cleavage::GetWeightedBreastMovement(giant, tiny);
		}*/

		return AttachTo(anyGiant, anyTiny, clevagePos);
	}
}
