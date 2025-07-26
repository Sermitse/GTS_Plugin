#include "Utils/MovementForce.hpp"

using namespace GTS;

// This file is outdated and doesn't work as expected, so all functions just return 1.0f for now 
// Needs rework

namespace {
	NodeMovementType Convert_To_MovementType(DamageSource Source) {

		NodeMovementType Type = NodeMovementType::Movement_None;

		switch (Source) {

			case DamageSource::HandDropRight:
			case DamageSource::HandSwipeRight:
			case DamageSource::HandSlamRight:
			case DamageSource::HandCrawlRight:
				Type = NodeMovementType::Movement_RightHand;
			break;	
			case DamageSource::HandDropLeft:
			case DamageSource::HandSwipeLeft:
			case DamageSource::HandSlamLeft:
			case DamageSource::HandCrawlLeft:
				Type = NodeMovementType::Movement_LeftHand;
			break;
			case DamageSource::WalkRight:
			case DamageSource::FootIdleR:
			case DamageSource::CrushedRight:
			case DamageSource::FootGrindedRight:
			case DamageSource::KickedRight:
			case DamageSource::KneeRight:
				Type = NodeMovementType::Movement_RightLeg;
			break;	
			case DamageSource::WalkLeft:
			case DamageSource::FootIdleL:
			case DamageSource::CrushedLeft:
			case DamageSource::FootGrindedLeft:
			case DamageSource::KickedLeft:
			case DamageSource::KneeLeft:
				Type = NodeMovementType::Movement_LeftLeg;
			break;	
			default: {
				return NodeMovementType::Movement_None;
			break;
			}
		}
		return Type;
	}
}

namespace GTS {

	void UpdatePlayerMovement() { // It is a good idea to keep it player exclusive to not calculate each single NPC in the scene which may vore FPS
		auto player = PlayerCharacter::GetSingleton();

		auto Data = Transient::GetSingleton().GetData(player);

		if (Data) {
			NiAVObject* Node_LeftFoot = find_node(player, "NPC L Foot [Lft ]");
			NiAVObject* Node_RightFoot = find_node(player, "NPC R Foot [Rft ]");
			
			if (player->IsSneaking() || IsCrawling(player)) {
				NiAVObject* Node_LeftHand = find_node(player, "NPC L Hand [LHnd]");
				NiAVObject* Node_RightHand = find_node(player, "NPC R Hand [RHnd]");
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

	float Get_Bone_Movement_Speed(Actor* actor, NodeMovementType type) {
		if (actor->formID != 0x14) {
			return 1.0f;
		}
		GTS_PROFILE_SCOPE("MovementForce: GetBoneMovementSpeed");
		auto Data = Transient::GetSingleton().GetData(actor);
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
				case NodeMovementType::Movement_LeftHand: 
					NodeMovementForce = Data->HandVelocity_L;
				break;
				case NodeMovementType::Movement_RightHand: 
					NodeMovementForce = Data->HandVelocity_R;
				break;
				case NodeMovementType::Movement_None:
					return 1.0f; // Always allow for actions that are supposed to stagger always
				break;
			}
		}
		
		if (NodeMovementForce > 0) {
			float NodeMovementForce_Clamped = std::clamp(NodeMovementForce / 10.0f, 0.0f, 1.0f);
			return NodeMovementForce_Clamped;
		}

		return 0.0f;
	}

	float Get_Bone_Movement_Speed(Actor* giant, DamageSource Source) {
		if (giant->formID != 0x14) {
			return 1.0f;
		}

		GTS_PROFILE_SCOPE("MovementForce: GetBoneMovementSpeed");
		NodeMovementType Type = Convert_To_MovementType(Source);

		return Get_Bone_Movement_Speed(giant, Type);
	}
}