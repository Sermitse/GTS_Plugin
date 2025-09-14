#include "Managers/Cameras/TP/Normal.hpp"
#include "Managers/Cameras/CamUtil.hpp"
#include "Managers/GtsSizeManager.hpp"
#include "Config/Config.hpp"

#include "Managers/SpectatorManager.hpp"

namespace GTS {

	static auto& CamSettings = Config::Camera.OffsetsNormal;

	NiPoint3 Normal::GetOffset(const NiPoint3& cameraPos) {


		return {
			CamSettings.f3NormalStand[0],
			CamSettings.f3NormalStand[1],
			CamSettings.f3NormalStand[2],
		};

	}

	NiPoint3 Normal::GetCombatOffset(const NiPoint3& cameraPos) {

		return {
			CamSettings.f3CombatStand[0],
			CamSettings.f3CombatStand[1],
			CamSettings.f3CombatStand[2],
		};

	}

	NiPoint3 Normal::GetOffsetProne(const NiPoint3& cameraPos) {

		return {
			CamSettings.f3NormalCrawl[0],
			CamSettings.f3NormalCrawl[1],
			CamSettings.f3NormalCrawl[2],
		};

	}

	NiPoint3 Normal::GetCombatOffsetProne(const NiPoint3& cameraPos)  {

		return {
			CamSettings.f3CombatCrawl[0],
			CamSettings.f3CombatCrawl[1],
			CamSettings.f3CombatCrawl[2],
		};

	}

	BoneTarget Normal::GetBoneTarget() {
		auto player = SpectatorManager::GetCameraTarget();
		auto& sizemanager = SizeManager::GetSingleton();
		CameraTracking Camera_Anim = sizemanager.GetTrackedBone(player);

		return GetBoneTargets(Camera_Anim, StringToEnum<LCameraTrackBone_t>(CamSettings.sCenterOnBone));
	}
}
