#include "managers/GtsSizeManager.hpp"
#include "managers/CrushManager.hpp"
#include "managers/GtsManager.hpp"
#include "managers/Attributes.hpp"
#include "managers/highheel.hpp"
#include "managers/InputManager.hpp"
#include "managers/Rumble.hpp"
#include "magic/effects/common.hpp"
#include "utils/actorUtils.hpp"
#include "data/persistent.hpp"
#include "data/runtime.hpp"
#include "scale/scale.hpp"
#include "data/time.hpp"
#include "profiler.hpp"
#include "timer.hpp"
#include "node.hpp"

#include <random>


using namespace Gts;
using namespace RE;
using namespace REL;
using namespace SKSE;

namespace {
	const float LAUNCH_COOLDOWN = 0.8f;
	const float DAMAGE_COOLDOWN = 2.0f;
	const float HANDDAMAGE_COOLDOWN = 0.6f;
	const float THIGHDAMAGE_COOLDOWN = 1.2f;
	const float HEALTHGATE_COOLDOWN = 60.0f;
	const float SCARE_COOLDOWN = 6.0f;
	const float BUTTCRUSH_COOLDOWN = 30.0f;
	const float HUGS_COOLDOWN = 8.0f;

	float Calculate_Halflife(CameraTracking Bone) {
		if (Bone == CameraTracking::Thigh_Crush) { // Thigh Crushing
			return 0.15f;
		} else if (Bone == CameraTracking::VoreHand_Right || Bone == CameraTracking::Hand_Left || Bone == CameraTracking::Hand_Right) { // Voring / using hands
			return 0.10f;
		} else if (Bone == CameraTracking::ObjectA || Bone == CameraTracking::ObjectB) { // pretty much vore/ hands too
			return 0.10f;
		} else if (Bone == CameraTracking::R_Foot || Bone == CameraTracking::L_Foot) { // Feet
			return 0.08f;
		} else if (Bone == CameraTracking::Butt || Bone == CameraTracking::Mid_Butt_Legs) { // Butt
			return 0.08f;
		} else if (Bone == CameraTracking::Breasts_02) { // Breasts
			return 0.10f;
		} else if (Bone == CameraTracking::Knees) { // Knees
			return 0.10f;
		} else if (Bone == CameraTracking::Finger_Right || Bone == CameraTracking::Finger_Left) {
			return 0.08f;
		} else {
			return 0.05f;
		}
	}

	float Get_LifeForceBonus(Actor* giant) {
		float bonus = 0.0f;
		auto data = Transient::GetSingleton().GetActorData(giant);
		if (data) {
			bonus = data->Perk_lifeForceStolen;
		}
		return bonus;
	}
}

namespace Gts {
	SizeManager& SizeManager::GetSingleton() noexcept {
		static SizeManager instance;
		return instance;
	}

	std::string SizeManager::DebugName() {
		return "SizeManager";
	}

	void SizeManager::SetEnchantmentBonus(Actor* actor, float amt) {
		if (!actor) {
			return;
		}
		this->GetData(actor).AspectOfGiantess = amt;
	}

	float SizeManager::GetEnchantmentBonus(Actor* actor) {
		if (!actor) {
			return 0.0f;
		}
		float EB = std::clamp(this->GetData(actor).AspectOfGiantess, 0.0f, 1000.0f);
		EB += Get_LifeForceBonus(actor);

		return EB;
	}

	void SizeManager::ModEnchantmentBonus(Actor* actor, float amt) {
		if (!actor) {
			return;
		}
		this->GetData(actor).AspectOfGiantess += amt;
	}

	//=================Size Hunger

	void SizeManager::SetSizeHungerBonus(Actor* actor, float amt) {
		if (!actor) {
			return;
		}
		this->GetData(actor).SizeHungerBonus = amt;
	}

	float SizeManager::GetSizeHungerBonus(Actor* actor) {
		if (!actor) {
			return 0.0f;
		}
		float SHB = std::clamp(this->GetData(actor).SizeHungerBonus, 0.0f, 1000.0f);
		return SHB;
	}

	void SizeManager::ModSizeHungerBonus(Actor* actor, float amt) {
		if (!actor) {
			return;
		}
		this->GetData(actor).SizeHungerBonus += amt;
	}

	//==================Growth Spurt

	void SizeManager::SetGrowthSpurt(Actor* actor, float amt) {
		if (!actor) {
			return;
		}
		this->GetData(actor).GrowthSpurtSize = amt;
	}

	float SizeManager::GetGrowthSpurt(Actor* actor) {
		if (!actor) {
			return 0.0f;
		}
		float GS = clamp (0.0f, 999999.0f, this->GetData(actor).GrowthSpurtSize);
		return GS;
	}

	void SizeManager::ModGrowthSpurt(Actor* actor, float amt) {
		if (!actor) {
			return;
		}
		this->GetData(actor).GrowthSpurtSize += amt;
	}

	//================Size-Related Damage
	void SizeManager::SetSizeAttribute(Actor* actor, float amt, SizeAttribute attribute) {
		if (!actor) {
			return;
		}
		auto Persistent = Persistent::GetSingleton().GetData(actor);
		if (Persistent) {
			switch (attribute) {
				case SizeAttribute::Normal:
					Persistent->NormalDamage = amt;
				break;
				case SizeAttribute::Sprint:
					Persistent->SprintDamage = amt;
				break;
				case SizeAttribute::JumpFall: 
					Persistent->FallDamage = amt;
				break;
				case SizeAttribute::HighHeel:
					Persistent->HHDamage = amt;
				break;
			}
		}
	}

	float SizeManager::GetSizeAttribute(Actor* actor, SizeAttribute attribute) {
		if (!actor) {
			return 1.0f;
		}
		auto Persistent = Persistent::GetSingleton().GetData(actor);
		if (Persistent) {
			float Normal = clamp (1.0f, 999999.0f, Persistent->NormalDamage);
			float Sprint = clamp (1.0f, 999999.0f, Persistent->SprintDamage);
			float Fall = clamp (1.0f, 999999.0f, Persistent->FallDamage);
			float HH = clamp (1.0f, 999999.0f, Persistent->HHDamage);
			switch (attribute) {
				case SizeAttribute::Normal: 
					return Normal;
				break;
				case SizeAttribute::Sprint:
					return Sprint;
				break;
				case SizeAttribute::JumpFall:
					return Fall;
				break;
				case SizeAttribute::HighHeel:
					return GetHighHeelsBonusDamage(actor, false);
				break;
				}
			}
		return 1.0f;
	}

	//===============Size-Related Attribute End


	//===============Size-Vulnerability

	void SizeManager::SetSizeVulnerability(Actor* actor, float amt) {
		if (actor) {
			auto Transient = Transient::GetSingleton().GetData(actor);
			if (Transient) {
				Transient->SizeVulnerability = amt;
			}
		}
	}

	float SizeManager::GetSizeVulnerability(Actor* actor) {
		if (actor) {
			auto Transient = Transient::GetSingleton().GetData(actor);
			if (Transient) {
				return std::clamp(Transient->SizeVulnerability, 0.0f, 0.35f);
			}
		}
		return 0.0f;
	}

	void SizeManager::ModSizeVulnerability(Actor* actor, float amt) {
		if (actor) {
			auto Transient = Transient::GetSingleton().GetData(actor);
			if (Transient) {
				if (Transient->SizeVulnerability < 0.35f) {
					Transient->SizeVulnerability += amt;
				}
			}
		}
	}
	//===============Size-Vulnerability

	//===============Hit Growth

	float SizeManager::GetHitGrowth(Actor* actor) {
		if (!actor) {
			return 0.0f;
		}
		auto Persistent = Persistent::GetSingleton().GetData(actor);
		if (!Persistent) {
			return 0.0f;
		}
		
		return Persistent->AllowHitGrowth;
	}

	void SizeManager::SetHitGrowth(Actor* actor, float allow) {
		if (!actor) {
			return;
		}
		auto Persistent = Persistent::GetSingleton().GetData(actor);
		if (Persistent) {
			for (auto follower: FindTeammates()) {
				if (follower) {
					auto TeammateData = Persistent::GetSingleton().GetData(follower);
					if (TeammateData) {
						TeammateData->AllowHitGrowth = allow; // Affect teammates as well
					}
				}
			}

			Persistent->AllowHitGrowth = allow; // Affect the player
		}
	}

	//===============Size-Vulnerability

	//===============Camera Stuff
	CameraTracking SizeManager::GetPreviousBone(Actor* actor) {
		return this->GetData(actor).PreviousBone;
	}
	void SizeManager::SetPreviousBone(Actor* actor) {
		this->GetData(actor).PreviousBone = this->GetData(actor).TrackedBone;
	}
	void SizeManager::SetTrackedBone(Actor* actor, bool enable, CameraTracking Bone) {
		if (!enable) {
			Bone = CameraTracking::None;
		}
		SetCameraHalflife(actor, Bone);
		SetCameraOverride(actor, enable);
		this->GetData(actor).TrackedBone = Bone;
	}

	CameraTracking SizeManager::GetTrackedBone(Actor* actor) {
		return this->GetData(actor).TrackedBone;
	}


	//==============Half life stuff
	void SizeManager::SetCameraHalflife(Actor* actor, CameraTracking Bone) {
		this->GetData(actor).Camera_HalfLife = Calculate_Halflife(Bone);
	}

	float SizeManager::GetCameraHalflife(Actor* actor) {
		return this->GetData(actor).Camera_HalfLife;
	}
	//

	//===============Balance Mode
	float SizeManager::BalancedMode()
	{
		if (Runtime::GetBool("BalanceMode")) {
			return 2.0f;
		} else {
			return 1.0f;
		}
	}

	SizeManagerData& SizeManager::GetData(Actor* actor) {
		this->sizeData.try_emplace(actor);
		return this->sizeData.at(actor);
	}

	void SizeManager::Reset() {
		TaskManager::CancelAllTasks(); // just in case, to avoid CTD
		this->sizeData.clear();
	}

	void SizeManager::ResetActor(Actor* actor) {
		if (actor) {
			this->sizeData.erase(actor);
		}
	}
}