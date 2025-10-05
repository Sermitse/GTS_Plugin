#include "DebugUtil.hpp"

namespace DebugUtil {

	glm::vec3 CompMult(const glm::vec3& a, const glm::vec3& b) {
		return glm::vec3(a[0] * b[0], a[1] * b[1], a[2] * b[2]);
	}

	glm::vec3 HkVecToGlmVec(const RE::hkVector4& vec) {
		return glm::vec3(vec.quad.m128_f32[0], vec.quad.m128_f32[1], vec.quad.m128_f32[2]);
	}

	glm::vec3 HkToGlm(const RE::hkVector4& vec) {
		return HkVecToGlmVec(vec) * World::WorldScaleInverse();
	}

	glm::mat3 HkToGlm(const RE::hkRotation& mat) {
		return glm::mat3(
			HkVecToGlmVec(mat.col0),
			HkVecToGlmVec(mat.col1),
			HkVecToGlmVec(mat.col2)
		);
	}

	glm::mat4 HkToGlm(const RE::hkTransform& transform) {
		return glm::mat4(
			glm::vec4(HkVecToGlmVec(transform.rotation.col0), 0.0f),
			glm::vec4(HkVecToGlmVec(transform.rotation.col1), 0.0f),
			glm::vec4(HkVecToGlmVec(transform.rotation.col2), 0.0f),
			glm::vec4(HkToGlm(transform.translation), 1.0f)
		);
	}

	glm::mat4 HkToGlm(const RE::hkQsTransform& transform) {
		glm::quat quat = glm::quat(HkVecToGlmVec(transform.rotation.vec));
		glm::mat4 rotMat = glm::mat4_cast(quat);
		return glm::translate(rotMat * glm::scale(glm::mat4(1.0f), HkVecToGlmVec(transform.scale)), HkToGlm(transform.translation));
	}

	glm::vec3 ApplyTransform(glm::vec4 vec, const glm::mat4& mat) {
		return glm::vec3(mat * vec);
	}

	glm::vec3 ApplyTransform(glm::vec3 vec, const glm::mat4& mat) {
		return glm::vec3(mat * glm::vec4(vec, 1.0f));
	}

	bool DebugUtil::IsRoughlyEqual(float first, float second, float maxDif) {
		return abs(first - second) <= maxDif;
	}

	glm::vec3 RotateVector(glm::quat quatIn, glm::vec3 vecIn) {
		float num = quatIn.x * 2.0f;
		float num2 = quatIn.y * 2.0f;
		float num3 = quatIn.z * 2.0f;
		float num4 = quatIn.x * num;
		float num5 = quatIn.y * num2;
		float num6 = quatIn.z * num3;
		float num7 = quatIn.x * num2;
		float num8 = quatIn.x * num3;
		float num9 = quatIn.y * num3;
		float num10 = quatIn.w * num;
		float num11 = quatIn.w * num2;
		float num12 = quatIn.w * num3;
		glm::vec3 result;
		result.x = (1.0f - (num5 + num6)) * vecIn.x + (num7 - num12) * vecIn.y + (num8 + num11) * vecIn.z;
		result.y = (num7 + num12) * vecIn.x + (1.0f - (num4 + num6)) * vecIn.y + (num9 - num10) * vecIn.z;
		result.z = (num8 - num11) * vecIn.x + (num9 + num10) * vecIn.y + (1.0f - (num4 + num5)) * vecIn.z;
		return result;
	}

	glm::vec3 GetCameraPos() {
		auto playerCam = RE::PlayerCamera::GetSingleton();
		return glm::vec3(playerCam->pos.x, playerCam->pos.y, playerCam->pos.z);
	}

	glm::quat GetCameraRot() {
		auto playerCam = RE::PlayerCamera::GetSingleton();

		auto cameraState = playerCam->currentState.get();
		if (!cameraState) {
			return glm::quat();
		}

		RE::NiQuaternion niRotation;
		cameraState->GetRotation(niRotation);

		return glm::quat(niRotation.w, niRotation.x, niRotation.y, niRotation.z);
	}

	glm::vec3 NormalizeVector(glm::vec3 p) {
		return glm::normalize(p);
	}

	glm::vec3 GetForwardVector(glm::quat quatIn) {
		// rotate Skyrim's base forward vector (positive Y forward) by quaternion
		return RotateVector(quatIn, glm::vec3(0.0f, 1.0f, 0.0f));
	}

	bool IsPosBehindPlayerCamera(glm::vec3 pos) {
		auto cameraPos = GetCameraPos();
		auto cameraRot = GetCameraRot();
		auto toTarget = NormalizeVector(pos - cameraPos);
		auto cameraForward = NormalizeVector(GetForwardVector(cameraRot));
		auto angleDif = abs(glm::length(toTarget - cameraForward));

		// root_two is the diagonal length of a 1x1 square. When comparing normalized forward
		// vectors, this accepts an angle of 90 degrees in all directions
		return angleDif > glm::root_two<float>();
	}

	glm::vec3 GetPointOnRotatedCircle(glm::vec3 origin, float radius, float i, float maxI, glm::vec3 eulerAngles) {
		float currAngle = (i / maxI) * glm::two_pi<float>();
		glm::vec3 targetPos(radius * cos(currAngle), radius * sin(currAngle), 0.0f);
		auto targetPosRotated = RotateVector(eulerAngles, targetPos);
		return glm::vec3(targetPosRotated.x + origin.x, targetPosRotated.y + origin.y, targetPosRotated.z + origin.z);
	}

	glm::vec3 GetAnyPerpendicularUnitVector(const glm::vec3& vec) {
		if (vec.y != 0.0f || vec.z != 0.0f) {
			return glm::vec3(1, 0, 0);
		}
		return glm::vec3(0, 1, 0);

	}

	RE::NiPoint3 Glm2Ni(const glm::vec3& position) {
		return RE::NiPoint3(position[0], position[1], position[2]);
	}

	glm::vec3 Ni2Glm(const RE::NiPoint3& position) {
		return glm::vec3{ position.x, position.y, position.z };
	}

	glm::mat4 Ni2Glm(const RE::NiTransform& transform) {
		return glm::mat4(
			glm::vec4(transform.rotate.entry[0][0], transform.rotate.entry[0][1], transform.rotate.entry[0][2], 0.0f),
			glm::vec4(transform.rotate.entry[1][0], transform.rotate.entry[1][1], transform.rotate.entry[1][2], 0.0f),
			glm::vec4(transform.rotate.entry[2][0], transform.rotate.entry[2][1], transform.rotate.entry[2][2], 0.0f),
			glm::vec4(transform.translate[0], transform.translate[1], transform.translate[2], 1.0f)
		);
	}

	float RGBToHex(glm::vec3 rgb) {
		return ConvertComponentR(rgb.r * 255) + ConvertComponentG(rgb.g * 255) + ConvertComponentB(rgb.b * 255);
	}

	float ConvertComponentR(float value) {
		return (value * 0xffff) + value;
	}

	float ConvertComponentG(float value) {
		return (value * 0xff) + value;
	}

	float ConvertComponentB(float value) {
		return value;
	}
}
