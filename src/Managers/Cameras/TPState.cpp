#include "Managers/Cameras/TPState.hpp"

#include "Config/Config.hpp"

#include "Managers/Cameras/CamUtil.hpp"
#include "Managers/GtsSizeManager.hpp"
#include "Debug/DebugDraw.hpp"

using namespace GTS;

namespace {

	float Modify_HalfLife() {
		auto player = PlayerCharacter::GetSingleton();
		auto& sizemanager = SizeManager::GetSingleton();
		float result = sizemanager.GetCameraHalflife(player);
		return result;
	}
}

namespace GTS {


	NiPoint3 ThirdPersonCameraState::GetPlayerLocalOffset(const NiPoint3& cameraPos) {
		NiPoint3 pos = NiPoint3();
		auto player = GetCameraActor();
		if (player) {
			auto scale = get_visual_scale(player);
			auto boneTarget = this->GetBoneTarget();
			if (!boneTarget.boneNames.empty()) {
				auto player = GetCameraActor();
				if (player) {
					auto rootModel = player->Get3D(false);
					if (rootModel) {
						auto playerTrans = rootModel->world;
						playerTrans.scale = rootModel->parent ? rootModel->parent->world.scale : 1.0f;  // Only do translation/rotation
						auto transform = playerTrans.Invert();
						NiPoint3 lookAt = ComputeLookAt(boneTarget.zoomScale);
						NiPoint3 localLookAt = transform*lookAt;
						this->SpringSmoothScale.halflife = Modify_HalfLife();
						this->SpringSmoothedBonePos.halflife = Modify_HalfLife();
						this->SpringSmoothScale.target = scale;
						pos += localLookAt * -1 * this->SpringSmoothScale.value;

						std::vector<NiAVObject*> bones = {};
						for (auto bone_name: boneTarget.boneNames) {
							auto node = find_node(player, bone_name);
							if (node) {
								bones.push_back(node);
							} else {
								log::error("Bone not found for camera target: {}", bone_name);
							}
						}

						NiPoint3 bonePos = NiPoint3();
						auto bone_count = bones.size();
						for (auto bone: bones) {
							auto worldPos = bone->world * NiPoint3();
							if (IsDebugEnabled()) {
								DebugDraw::DrawSphere(glm::vec3(worldPos.x, worldPos.y, worldPos.z), 1.0f, 10, {1.0f, 1.0f, 0.0f, 1.0f});
							}
							auto localPos = transform * worldPos;
							bonePos += localPos * (1.0f/bone_count);
						}
						NiPoint3 worldBonePos = playerTrans * bonePos;
						if (IsDebugEnabled()) {
							DebugDraw::DrawSphere(glm::vec3(worldBonePos.x, worldBonePos.y, worldBonePos.z), 1.0f, 10, {0.0f, 1.0f, 0.0f, 1.0f});
						}
						SpringSmoothedBonePos.target = bonePos;
						pos += SpringSmoothedBonePos.value;
					}
				}
			}
		}
		return pos;
	}

	NiPoint3 ThirdPersonCameraState::GetPlayerLocalOffsetCrawling(const NiPoint3& cameraPos) {
		NiPoint3 pos = this->GetPlayerLocalOffset(cameraPos);
		auto player = GetCameraActor();
		if (player) {
			auto scale = get_visual_scale(player);
			pos += this->CrawlAdjustment(cameraPos)*scale;
		}
		return pos;
	}

	BoneTarget ThirdPersonCameraState::GetBoneTarget() {
		return {};
	}

	NiPoint3 ThirdPersonCameraState::CrawlAdjustment(const NiPoint3& cameraPos) {
		float proneFactor = 0.0;

		if (GetBoneTarget().boneNames.empty()) {
			proneFactor = Config::Camera.fTPCrawlHeightMult;
		}

		NiPoint3 result = NiPoint3();
		result.z = -cameraPos.z * proneFactor;
		return result;
	}
}
