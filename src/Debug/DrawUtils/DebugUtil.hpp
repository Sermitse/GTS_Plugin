#pragma once

#include <glm/ext.hpp>
#include <glm/glm.hpp>

namespace DebugUtil {

	glm::vec3 CompMult(const glm::vec3& a, const glm::vec3& b);
	glm::vec3 HkVecToGlmVec(const RE::hkVector4& vec);
	glm::vec3 HkToGlm(const RE::hkVector4& vec);
	glm::mat3 HkToGlm(const RE::hkRotation& mat);
	glm::mat4 HkToGlm(const RE::hkTransform& transform);
	glm::mat4 HkToGlm(const RE::hkQsTransform& transform);
	glm::vec3 ApplyTransform(glm::vec4 vec, const glm::mat4& mat);
	glm::vec3 ApplyTransform(glm::vec3 vec, const glm::mat4& mat);
	bool IsRoughlyEqual(float first, float second, float maxDif);
	glm::vec3 RotateVector(glm::quat quatIn, glm::vec3 vecIn);
	glm::vec3 GetCameraPos();
	glm::quat GetCameraRot();
	glm::vec3 NormalizeVector(glm::vec3 p);
	glm::vec3 GetForwardVector(glm::quat quatIn);
	bool IsPosBehindPlayerCamera(glm::vec3 pos);
	glm::vec3 GetPointOnRotatedCircle(glm::vec3 origin, float radius, float i, float maxI, glm::vec3 eulerAngles);
	glm::vec3 GetAnyPerpendicularUnitVector(const glm::vec3& vec);
	RE::NiPoint3 Glm2Ni(const glm::vec3& position);
	glm::vec3 Ni2Glm(const RE::NiPoint3& position);
	glm::mat4 Ni2Glm(const RE::NiTransform& transform);

	float RGBToHex(glm::vec3 rgb);
	float ConvertComponentR(float value);
	float ConvertComponentG(float value);
	float ConvertComponentB(float value);
}
