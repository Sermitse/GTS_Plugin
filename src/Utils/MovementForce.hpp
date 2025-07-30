#pragma once

namespace GTS {
	void UpdateBoneMovementData(Actor* a_Giant);
	float Get_Bone_Movement_Speed(Actor* actor, NodeMovementType type);
	float Get_Bone_Movement_Speed(Actor* giant, DamageSource Source);
};