#pragma once

namespace GTS {
	void UpdatePlayerMovement();
	float Get_Bone_Movement_Speed(Actor* actor, NodeMovementType type);
	float Get_Bone_Movement_Speed(Actor* giant, DamageSource Source);
};