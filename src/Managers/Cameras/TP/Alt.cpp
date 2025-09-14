#include "Managers/Cameras/TP/Alt.hpp"
#include "Managers/Cameras/Camutil.hpp"
#include "Managers/GtsSizeManager.hpp"
#include "Config/Config.hpp"

#include "Managers/SpectatorManager.hpp"

namespace GTS {

	auto& CamSettings = Config::Camera.OffsetsAlt;

	NiPoint3 Alt::GetOffset(const NiPoint3& cameraPos) {

		return {
			CamSettings.f3NormalStand[0],
			CamSettings.f3NormalStand[1],
			CamSettings.f3NormalStand[2],
		};

	}

	NiPoint3 Alt::GetCombatOffset(const NiPoint3& cameraPos) {

		return {
			CamSettings.f3CombatStand[0],
			CamSettings.f3CombatStand[1],
			CamSettings.f3CombatStand[2],
		};

	}

	NiPoint3 Alt::GetOffsetProne(const NiPoint3& cameraPos) {

		return {
			CamSettings.f3NormalCrawl[0],
			CamSettings.f3NormalCrawl[1],
			CamSettings.f3NormalCrawl[2],
		};

	}

	NiPoint3 Alt::GetCombatOffsetProne(const NiPoint3& cameraPos) {

		return {
			CamSettings.f3CombatCrawl[0],
			CamSettings.f3CombatCrawl[1],
			CamSettings.f3CombatCrawl[2],
		};

	}

	BoneTarget Alt::GetBoneTarget() {
		auto player = SpectatorManager::GetCameraTarget();
		auto& sizemanager = SizeManager::GetSingleton();

		CameraTracking Camera_Anim = sizemanager.GetTrackedBone(player);
		return GetBoneTargets(Camera_Anim, StringToEnum<LCameraTrackBone_t>(CamSettings.sCenterOnBone));
	}
}
