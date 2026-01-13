#include "Utils/MovementForce.hpp"
#include "Config/Config.hpp"

using namespace GTS;

// This file is outdated and doesn't work as expected, so all functions just return 1.0f for now 
// Needs rework

namespace {

	bool IsAllowed(Actor* a_Giant) {
		const bool IsPlayer = a_Giant->IsPlayerRef(); // Always allow Player
		const bool ConfigLock = Config::AI.bRecordBoneSpeedData; // IF ON, also allow NPC's
		
		if (IsPlayer || ConfigLock) {
			return true;
		}
		return false;
	}

	NodeMovementType Convert_To_MovementType(DamageSource Source) {

		NodeMovementType Type;

		switch (Source) {

			case DamageSource::HandDropRight:
			case DamageSource::HandSwipeRight:
			case DamageSource::HandSlamRight:
			case DamageSource::HandCrawlRight: {
				Type = NodeMovementType::Movement_RightHand;
				break;
			}
			case DamageSource::HandDropLeft:
			case DamageSource::HandSwipeLeft:
			case DamageSource::HandSlamLeft:
			case DamageSource::HandCrawlLeft: {
				Type = NodeMovementType::Movement_LeftHand;
				break;
			}
			case DamageSource::WalkRight:
			case DamageSource::FootIdleR:
			case DamageSource::CrushedRight:
			case DamageSource::FootGrindedRight:
			case DamageSource::KickedRight:
			case DamageSource::KneeRight: {
				Type = NodeMovementType::Movement_RightLeg;
				break;
			}
			case DamageSource::WalkLeft:
			case DamageSource::FootIdleL:
			case DamageSource::CrushedLeft:
			case DamageSource::FootGrindedLeft:
			case DamageSource::KickedLeft:
			case DamageSource::KneeLeft: {
				Type = NodeMovementType::Movement_LeftLeg;
				break;
			}

			default: {
				return NodeMovementType::Movement_None;
			}
		}
		return Type;
	}
}

namespace GTS {

	void UpdateBoneMovementData(Actor* a_Giant) { // It is a good idea to keep it player/follower exclusive to not calculate each single NPC in the scene which may vore FPS
		if (a_Giant && a_Giant->Is3DLoaded()) {
			if (!IsAllowed(a_Giant)) {
				return; // Do nothing in that case
			}

			if (auto Data = Transient::GetActorData(a_Giant)) {
				NiAVObject* Node_LeftFoot = find_node(a_Giant, "NPC L Foot [Lft ]");
				NiAVObject* Node_RightFoot = find_node(a_Giant, "NPC R Foot [Rft ]");
				
				if (a_Giant->IsSneaking() || AnimationVars::Crawl::IsCrawling(a_Giant)) {
					NiAVObject* Node_LeftHand = find_node(a_Giant, "NPC L Hand [LHnd]");
					NiAVObject* Node_RightHand = find_node(a_Giant, "NPC R Hand [RHnd]");
					if (Node_LeftHand && Node_RightHand) {
						Data->POSCurrentHandL = Node_LeftHand->world.translate;
						Data->POSCurrentHandR = Node_RightHand->world.translate;

						Data->HandVelocity_L = (Data->POSCurrentHandL - Data->POSLastHandL).Length();
						Data->HandVelocity_R = (Data->POSCurrentHandR - Data->POSLastHandR).Length();

						Data->POSLastHandL = Data->POSCurrentHandL;
						Data->POSLastHandR = Data->POSCurrentHandR;
					}
				}

				if (Node_LeftFoot && Node_RightFoot) {
					Data->POSCurrentLegL = Node_LeftFoot->world.translate;
					Data->POSCurrentLegR = Node_RightFoot->world.translate;

					Data->FootVelocity_L = (Data->POSCurrentLegL - Data->POSLastLegL).Length();
					Data->FootVelocity_R = (Data->POSCurrentLegR - Data->POSLastLegR).Length();

					Data->POSLastLegL = Data->POSCurrentLegL;
					Data->POSLastLegR = Data->POSCurrentLegR;
				}
			}
		}
	}

	float Get_Bone_Movement_Speed(Actor* actor, NodeMovementType type) {
		if (!IsAllowed(actor)) {
			return 1.0f;
		}
		GTS_PROFILE_SCOPE("MovementForce: GetBoneMovementSpeed");
		auto Data = Transient::GetActorData(actor);
		float NodeMovementForce = 0.0f;
		
		if (Data) {
			switch (type) {
				case NodeMovementType::Movement_LeftLeg: {
					NodeMovementForce = Data->FootVelocity_L;
					break;
				}
				case NodeMovementType::Movement_RightLeg: {
					NodeMovementForce = Data->FootVelocity_R;
					break;
				}
				case NodeMovementType::Movement_LeftHand: {
					NodeMovementForce = Data->HandVelocity_L;
					break;
				}
				case NodeMovementType::Movement_RightHand: {
					NodeMovementForce = Data->HandVelocity_R;
					break;
				}
				case NodeMovementType::Movement_None: return 1.0f; // Always allow for actions that are supposed to stagger always
			}
		}
		
		if (NodeMovementForce > 0) {
			float NodeMovementForce_Clamped = std::clamp(NodeMovementForce / 10.0f, 0.0f, 1.0f);
			return NodeMovementForce_Clamped;
		}

		return 0.0f;
	}

	float Get_Bone_Movement_Speed(Actor* giant, DamageSource Source) {
		if (!IsAllowed(giant)) {
			return 1.0f;
		}

		GTS_PROFILE_SCOPE("MovementForce: GetBoneMovementSpeed");
		NodeMovementType Type = Convert_To_MovementType(Source);

		return Get_Bone_Movement_Speed(giant, Type);
	}
}