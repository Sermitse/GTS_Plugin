#include "Debug/DebugDraw.hpp"

#include "Config/Config.hpp"

#include "UI/DebugMenu.hpp"

#include "Debug/Util/DebugUtil.hpp"
#include "Debug/Util/DebugLine.hpp"
#include "Debug/Util/ObjectBound.hpp"

#include "Hooks/Other/Values.hpp"

namespace GTS {

	void DebugDraw::DrawLineForMS(const glm::vec3& from, const glm::vec3& to, int liftetimeMS, const glm::vec4& color, float lineThickness) {
		std::optional<std::reference_wrapper<std::unique_ptr<DebugUtil::DebugLine>>> oldLine = GetExistingLine(from, to, color, lineThickness);
		if (oldLine.has_value()) {
			oldLine->get()->From = from;
			oldLine->get()->To = to;
			oldLine->get()->DestroyTickCount = GetTickCount64() + liftetimeMS;
			oldLine->get()->LineThickness = lineThickness;
			return;
		}

		LinesToDraw.push_back(std::make_unique<DebugUtil::DebugLine>(
				from,
				to,
				color,
				lineThickness,
				GetTickCount64() + liftetimeMS
			)
		);
	}

	void DebugDraw::Update() {

		auto hud = GetHUD();

		if (!hud || !hud->uiMovie) {
			return;
		}

		CacheMenuData();
		ClearLines2D(hud->uiMovie);

		if (!LinesToDraw.empty()) {

			for (std::unique_ptr<DebugUtil::DebugLine>& line : LinesToDraw) {
				DrawLine3D(hud->uiMovie, line->From, line->To, line->fColor, line->LineThickness, line->Alpha);
			}

			std::erase_if(LinesToDraw, [](const auto& line) {
				return GetTickCount64() > line->DestroyTickCount;
			});
		}
	}

	void DebugDraw::DrawBoundsForMS(DebugUtil::ObjectBound objectBound, int liftetimeMS, const glm::vec4& color, float lineThickness) {

		const glm::vec3 boundRight = objectBound.GetBoundRightVectorRotated();
		const glm::vec3 boundForward = objectBound.GetBoundForwardVectorRotated();
		const glm::vec3 boundUp = objectBound.GetBoundUpVectorRotated();
		const glm::vec3 objectLocation = objectBound.worldBoundMin;

		// x axis
		glm::vec3 glmXStart1(objectLocation.x, objectLocation.y, objectLocation.z);
		glm::vec3 glmXEnd1(glmXStart1.x + boundRight.x, glmXStart1.y + boundRight.y, glmXStart1.z + boundRight.z);
		// + y
		glm::vec3 glmXStart2(objectLocation.x + boundUp.x, objectLocation.y + boundUp.y, objectLocation.z + boundUp.z);
		glm::vec3 glmXEnd2(glmXStart2.x + boundRight.x, glmXStart2.y + boundRight.y, glmXStart2.z + boundRight.z);
		// + z
		glm::vec3 glmXStart3(objectLocation.x + boundForward.x, objectLocation.y + boundForward.y, objectLocation.z + boundForward.z);
		glm::vec3 glmXEnd3(glmXStart3.x + boundRight.x, glmXStart3.y + boundRight.y, glmXStart3.z + boundRight.z);
		// + y + z
		glm::vec3 glmXStart4(objectLocation.x + boundUp.x + boundForward.x, objectLocation.y + boundUp.y + boundForward.y, objectLocation.z + boundUp.z + boundForward.z);
		glm::vec3 glmXEnd4(glmXStart4.x + boundRight.x, glmXStart4.y + boundRight.y, glmXStart4.z + boundRight.z);

		// y axis
		glm::vec3 glmYStart1(objectLocation.x, objectLocation.y, objectLocation.z);
		glm::vec3 glmYEnd1(glmYStart1.x + boundForward.x, glmYStart1.y + boundForward.y, glmYStart1.z + boundForward.z);
		// + z
		glm::vec3 glmYStart2(objectLocation.x + boundUp.x, objectLocation.y + boundUp.y, objectLocation.z + boundUp.z);
		glm::vec3 glmYEnd2(glmYStart2.x + boundForward.x, glmYStart2.y + boundForward.y, glmYStart2.z + boundForward.z);
		// + x
		glm::vec3 glmYStart3(objectLocation.x + boundRight.x, objectLocation.y + boundRight.y, objectLocation.z + boundRight.z);
		glm::vec3 glmYEnd3(glmYStart3.x + boundForward.x, glmYStart3.y + boundForward.y, glmYStart3.z + boundForward.z);
		// + z + x
		glm::vec3 glmYStart4(objectLocation.x + boundUp.x + boundRight.x, objectLocation.y + boundUp.y + boundRight.y, objectLocation.z + boundUp.z + boundRight.z);
		glm::vec3 glmYEnd4(glmYStart4.x + boundForward.x, glmYStart4.y + boundForward.y, glmYStart4.z + boundForward.z);

		// z axis
		glm::vec3 glmZStart1(objectLocation.x, objectLocation.y, objectLocation.z);
		glm::vec3 glmZEnd1(glmZStart1.x + boundUp.x, glmZStart1.y + boundUp.y, glmZStart1.z + boundUp.z);
		// + x
		glm::vec3 glmZStart2(objectLocation.x + boundRight.x, objectLocation.y + boundRight.y, objectLocation.z + boundRight.z);
		glm::vec3 glmZEnd2(glmZStart2.x + boundUp.x, glmZStart2.y + boundUp.y, glmZStart2.z + boundUp.z);
		// + y
		glm::vec3 glmZStart3(objectLocation.x + boundForward.x, objectLocation.y + boundForward.y, objectLocation.z + boundForward.z);
		glm::vec3 glmZEnd3(glmZStart3.x + boundUp.x, glmZStart3.y + boundUp.y, glmZStart3.z + boundUp.z);
		// + x + y
		glm::vec3 glmZStart4(objectLocation.x + boundRight.x + boundForward.x, objectLocation.y + boundRight.y + boundForward.y, objectLocation.z + boundRight.z + boundForward.z);
		glm::vec3 glmZEnd4(glmZStart4.x + boundUp.x, glmZStart4.y + boundUp.y, glmZStart4.z + boundUp.z);

		DrawLineForMS(glmZStart1, glmZEnd1, liftetimeMS, color, lineThickness);
		DrawLineForMS(glmZStart2, glmZEnd2, liftetimeMS, color, lineThickness);
		DrawLineForMS(glmZStart3, glmZEnd3, liftetimeMS, color, lineThickness);
		DrawLineForMS(glmZStart4, glmZEnd4, liftetimeMS, color, lineThickness);

		DrawLineForMS(glmYStart1, glmYEnd1, liftetimeMS, color, lineThickness);
		DrawLineForMS(glmYStart2, glmYEnd2, liftetimeMS, color, lineThickness);
		DrawLineForMS(glmYStart3, glmYEnd3, liftetimeMS, color, lineThickness);
		DrawLineForMS(glmYStart4, glmYEnd4, liftetimeMS, color, lineThickness);

		DrawLineForMS(glmXStart1, glmXEnd1, liftetimeMS, color, lineThickness);
		DrawLineForMS(glmXStart2, glmXEnd2, liftetimeMS, color, lineThickness);
		DrawLineForMS(glmXStart3, glmXEnd3, liftetimeMS, color, lineThickness);
		DrawLineForMS(glmXStart4, glmXEnd4, liftetimeMS, color, lineThickness);
	}

	void DebugDraw::DrawSphere(glm::vec3 origin, float radius, int liftetimeMS, const glm::vec4& color, float lineThickness) {
		DrawCircle(origin, radius, glm::vec3(0.0f, 0.0f, 0.0f), liftetimeMS, color, lineThickness);
		DrawCircle(origin, radius, glm::vec3(glm::half_pi<float>(), 0.0f, 0.0f), liftetimeMS, color, lineThickness);
	}

	void DebugDraw::DrawCircle(glm::vec3 origin, float radius, glm::vec3 eulerAngles, int liftetimeMS, const glm::vec4& color, float lineThickness) {

		glm::vec3 lastEndPos = DebugUtil::GetPointOnRotatedCircle(origin, radius, CIRCLE_NUM_SEGMENTS, static_cast<float>(CIRCLE_NUM_SEGMENTS - 1), eulerAngles);

		for (int i = 0; i <= CIRCLE_NUM_SEGMENTS; i++) {
			glm::vec3 currEndPos = DebugUtil::GetPointOnRotatedCircle(origin, radius, static_cast<float>(i), static_cast<float>(CIRCLE_NUM_SEGMENTS - 1), eulerAngles);
			DrawLineForMS(lastEndPos, currEndPos, liftetimeMS, color, lineThickness);
			lastEndPos = currEndPos;
		}
	}

	void DebugDraw::DrawHalfCircle(glm::vec3 origin, float radius, glm::vec3 eulerAngles, int liftetimeMS, const glm::vec4& color, float lineThickness) {

		glm::vec3 lastEndPos = DebugUtil::GetPointOnRotatedCircle(origin, radius, CIRCLE_NUM_SEGMENTS / 2, static_cast<float>(CIRCLE_NUM_SEGMENTS - 1), eulerAngles);

		for (int i = 0; i <= CIRCLE_NUM_SEGMENTS / 2; i++) {
			glm::vec3 currEndPos = DebugUtil::GetPointOnRotatedCircle(origin, radius, static_cast<float>(i), static_cast<float>(CIRCLE_NUM_SEGMENTS - 1), eulerAngles);
			DrawLineForMS(lastEndPos, currEndPos, liftetimeMS, color, lineThickness);
			lastEndPos = currEndPos;
		}
	}

	void DebugDraw::DrawConvexVertices(const std::vector<glm::vec3>& vertices, glm::vec3 origin, const glm::mat4& transform, int liftetimeMS, const glm::vec4& color, float lineThickness) {
		if (vertices.empty()) return;

		// Pre-transform origin once
		glm::vec3 worldOrigin = glm::vec3(transform * glm::vec4(origin, 1.0f));
		glm::mat3 rotationScale(transform);

		auto apply_transform_with_origin = [&](const glm::vec3& vec) {
			return worldOrigin + rotationScale * vec;
		};

		// Remove degenerate vertices
		constexpr float VERTEX_EPSILON = 0.001f;
		std::vector<size_t> validIndices;
		validIndices.reserve(vertices.size());

		for (size_t i = 0; i < vertices.size(); ++i) {
			bool isDuplicate = false;
			for (size_t j : validIndices) {
				if (glm::distance(vertices[i], vertices[j]) < VERTEX_EPSILON) {
					isDuplicate = true;
					break;
				}
			}
			if (!isDuplicate) {
				validIndices.push_back(i);
			}
		}

		if (validIndices.size() < 3) return;

		auto validate_apex_and_ring = [&](int apexIdx, const std::array<int, 8>& ring) -> bool {
			if (apexIdx < 0 || apexIdx >= vertices.size()) return false;

			const glm::vec3& apex = vertices[apexIdx];
			glm::vec3 ringCentroid(0.0f);
			int validRingVerts = 0;

			for (int ringIdx : ring) {
				if (ringIdx >= 0 && ringIdx < vertices.size()) {
					ringCentroid += vertices[ringIdx];
					validRingVerts++;
				}
			}

			if (validRingVerts == 0) return false;
			ringCentroid /= static_cast<float>(validRingVerts);

			float separation = glm::distance(apex, ringCentroid);
			return separation > VERTEX_EPSILON * 2.0f;
		};

		auto is_ring_valid = [&](const std::array<int, 8>& ring) -> bool {
			int validCount = 0;
			for (int idx : ring) {
				if (idx >= 0 && idx < vertices.size()) {
					validCount++;
				}
			}
			return validCount >= 6;
		};

		// Character controller shape structure detection with validation
		if (vertices.size() == 18 && validIndices.size() >= 16) {
			constexpr std::array<int, 8> topRing = { 1, 4, 13, 7, 3, 16, 5, 11 };
			constexpr std::array<int, 8> bottomRing = { 0, 2, 12, 6, 15, 17, 14, 10 };
			constexpr int bottomApex = 8;
			constexpr int topApex = 9;

			bool topApexValid = validate_apex_and_ring(topApex, topRing);
			bool bottomApexValid = validate_apex_and_ring(bottomApex, bottomRing);
			bool ringsValid = is_ring_valid(topRing) && is_ring_valid(bottomRing);

			if (ringsValid) {

				// Fall through to generic handler
				// Pre-transform all ring vertices
				std::array<glm::vec3, 8> topRingWorld, bottomRingWorld;
				for (size_t i = 0; i < 8; ++i) {
					if (topRing[i] < vertices.size()) {
						topRingWorld[i] = apply_transform_with_origin(vertices[topRing[i]]);
					}
					if (bottomRing[i] < vertices.size()) {
						bottomRingWorld[i] = apply_transform_with_origin(vertices[bottomRing[i]]);
					}
				}

				glm::vec3 topApexPos = topApexValid ? apply_transform_with_origin(vertices[topApex]) : glm::vec3(0.0f);
				glm::vec3 bottomApexPos = bottomApexValid ? apply_transform_with_origin(vertices[bottomApex]) : glm::vec3(0.0f);

				// Draw top ring
				for (size_t i = 0; i < topRing.size(); ++i) {
					if (topRing[i] >= vertices.size()) continue;
					size_t next = (i + 1) % topRing.size();
					if (topRing[next] >= vertices.size()) continue;

					DrawLineForMS(topRingWorld[i], topRingWorld[next], liftetimeMS, color, lineThickness);
				}

				// Draw bottom ring
				for (size_t i = 0; i < bottomRing.size(); ++i) {
					if (bottomRing[i] >= vertices.size()) continue;
					size_t next = (i + 1) % bottomRing.size();
					if (bottomRing[next] >= vertices.size()) continue;

					DrawLineForMS(bottomRingWorld[i], bottomRingWorld[next], liftetimeMS, color, lineThickness);
				}

				// Draw vertical connections
				constexpr std::array<std::pair<int, int>, 8> ringConnections = { {
					{1, 0}, {4, 2}, {13, 12}, {7, 6},
					{3, 15}, {16, 17}, {5, 14}, {11, 10}
				} };

				for (size_t i = 0; i < ringConnections.size(); ++i) {
					//const auto& [topIdx, bottomIdx] = ringConnections[i];
					DrawLineForMS(topRingWorld[i], bottomRingWorld[i], liftetimeMS, color, lineThickness);
				}

				// Draw spokes from apexes
				if (topApexValid) {
					for (size_t i = 0; i < topRing.size(); ++i) {
						if (topRing[i] < vertices.size()) {
							DrawLineForMS(topApexPos, topRingWorld[i], liftetimeMS, color, lineThickness * 0.7f);
						}
					}
				}

				if (bottomApexValid) {
					for (size_t i = 0; i < bottomRing.size(); ++i) {
						if (bottomRing[i] < vertices.size()) {
							DrawLineForMS(bottomApexPos, bottomRingWorld[i], liftetimeMS, color, lineThickness * 0.7f);
						}
					}
				}

				// Draw cross-sections
				if (topRing[0] < vertices.size() && topRing[4] < vertices.size()) {
					DrawLineForMS(topRingWorld[0], topRingWorld[4], liftetimeMS, color * glm::vec4(1.0f, 1.0f, 1.0f, 0.5f), lineThickness * 0.5f);
				}
				if (bottomRing[0] < vertices.size() && bottomRing[4] < vertices.size()) {
					DrawLineForMS(bottomRingWorld[0], bottomRingWorld[4], liftetimeMS, color * glm::vec4(1.0f, 1.0f, 1.0f, 0.5f), lineThickness * 0.5f);
				}
				return;
			}
		}
		else if (vertices.size() == 17 && validIndices.size() >= 15) {
			constexpr std::array<int, 8> topRing = { 1, 4, 12, 7, 3, 15, 5, 10 };
			constexpr std::array<int, 8> bottomRing = { 0, 2, 11, 6, 14, 16, 13, 9 };
			constexpr int bottomApex = 8;

			bool bottomApexValid = validate_apex_and_ring(bottomApex, bottomRing);
			bool ringsValid = is_ring_valid(topRing) && is_ring_valid(bottomRing);

			if (!ringsValid) {
				// Fall through to generic handler
			}
			else {
				// Pre-transform all ring vertices
				std::array<glm::vec3, 8> topRingWorld, bottomRingWorld;
				for (size_t i = 0; i < 8; ++i) {
					if (topRing[i] < vertices.size()) {
						topRingWorld[i] = apply_transform_with_origin(vertices[topRing[i]]);
					}
					if (bottomRing[i] < vertices.size()) {
						bottomRingWorld[i] = apply_transform_with_origin(vertices[bottomRing[i]]);
					}
				}

				glm::vec3 bottomApexPos = bottomApexValid ? apply_transform_with_origin(vertices[bottomApex]) : glm::vec3(0.0f);

				// Draw rings
				for (size_t i = 0; i < topRing.size(); ++i) {
					if (topRing[i] >= vertices.size()) continue;
					size_t next = (i + 1) % topRing.size();
					if (topRing[next] >= vertices.size()) continue;

					DrawLineForMS(topRingWorld[i], topRingWorld[next], liftetimeMS, color, lineThickness);
				}

				for (size_t i = 0; i < bottomRing.size(); ++i) {
					if (bottomRing[i] >= vertices.size()) continue;
					size_t next = (i + 1) % bottomRing.size();
					if (bottomRing[next] >= vertices.size()) continue;

					DrawLineForMS(bottomRingWorld[i], bottomRingWorld[next], liftetimeMS, color, lineThickness);
				}

				// Vertical connections
				constexpr std::array<std::pair<int, int>, 8> ringConnections = {{
					{1,  0}, {4,   2}, {12, 11}, {7,  6},
					{3, 14}, {15, 16}, {5,  13}, {10, 9}
				}};

				for (size_t i = 0; i < ringConnections.size(); ++i) {
					DrawLineForMS(topRingWorld[i], bottomRingWorld[i], liftetimeMS, color, lineThickness);
				}

				// Bottom apex spokes
				if (bottomApexValid) {
					for (size_t i = 0; i < bottomRing.size(); ++i) {
						if (bottomRing[i] < vertices.size()) {
							DrawLineForMS(bottomApexPos, bottomRingWorld[i], liftetimeMS, color, lineThickness * 0.7f);
						}
					}
				}

				// Top ring cross-section
				if (topRing[0] < vertices.size() && topRing[4] < vertices.size()) {
					DrawLineForMS(topRingWorld[0], topRingWorld[4], liftetimeMS, color * glm::vec4(1.0f, 1.0f, 1.0f, 0.5f), lineThickness * 0.5f);
				}
				return;
			}
		}

		// Generic convex hull wireframe
		size_t vertCount = validIndices.size();

		// Pre-transform all valid vertices
		std::vector<glm::vec3> transformedVerts;
		transformedVerts.reserve(vertCount);
		for (size_t idx : validIndices) {
			transformedVerts.push_back(apply_transform_with_origin(vertices[idx]));
		}

		// Compute centroid
		glm::vec3 centroid(0.0f);
		for (const auto& v : transformedVerts) {
			centroid += v;
		}
		centroid /= static_cast<float>(vertCount);

		// Find seed vertex
		float maxDist = 0.0f;
		size_t seedIdx = 0;
		for (size_t i = 0; i < transformedVerts.size(); ++i) {
			const float dist = glm::distance(transformedVerts[i], centroid);
			if (dist > maxDist) {
				maxDist = dist;
				seedIdx = i;
			}
		}

		// Draw star pattern from seed
		const glm::vec3& seedPos = transformedVerts[seedIdx];
		for (size_t i = 0; i < transformedVerts.size(); ++i) {
			if (i != seedIdx) {
				DrawLineForMS(seedPos, transformedVerts[i], liftetimeMS, color * glm::vec4(1.0f, 1.0f, 1.0f, 0.6f), lineThickness * 0.6f);
			}
		}

		// Connect sequential vertices
		for (size_t i = 0; i < vertCount; ++i) {
			size_t next = (i + 1) % vertCount;
			DrawLineForMS(transformedVerts[i], transformedVerts[next], liftetimeMS, color, lineThickness);
		}

		// Add diagonal connections
		if (vertCount >= 4) {
			for (size_t i = 0; i < vertCount; i += 2) {
				size_t opposite = (i + vertCount / 2) % vertCount;
				DrawLineForMS(transformedVerts[i], transformedVerts[opposite], liftetimeMS, color * glm::vec4(1.0f, 1.0f, 1.0f, 0.4f), lineThickness * 0.5f);
			}
		}
	}

	void DebugDraw::DrawConvexVertices(const RE::hkArray<RE::hkVector4>& hkVerts, glm::vec3 origin, const glm::mat4& transform, int liftetimeMS, const glm::vec4& color, float lineThickness) {
		std::vector<glm::vec3> vertices;
		vertices.reserve(hkVerts.size());

		for (int32_t i = 0; i < hkVerts.size(); ++i) {
			const RE::hkVector4& hkVert = hkVerts[i];
			vertices.emplace_back(hkVert.quad.m128_f32[0], hkVert.quad.m128_f32[1], hkVert.quad.m128_f32[2]);
		}

		DrawConvexVertices(vertices, origin, transform, liftetimeMS, color, lineThickness);
	}

	void DebugDraw::DrawCapsule(glm::vec3 start, glm::vec3 end, float radius, glm::mat4 transform, int liftetimeMS, const glm::vec4& color, float lineThickness, int longitudinal_steps, int latitude_steps) {
		
		constexpr float pi = glm::pi<float>();

		const glm::vec3 axis = end - start;
		float length = glm::length(axis);

		if (length < 1e-6f) {
			DrawSphere(start, radius, liftetimeMS, color, lineThickness);
			return;
		}

		glm::vec3 localZ = axis / length;
		glm::vec3 localX;
		glm::vec3 localY;

		if (localZ.z < -0.9999999f) {
			localX = glm::vec3(0.0f, -1.0f, 0.0f);
			localY = glm::vec3(-1.0f, 0.0f, 0.0f);
		}
		else {
			const float a = 1.0f / (1.0f + localZ.z);
			const float b = -localZ.x * localZ.y * a;

			localX = glm::vec3(
				1.0f - localZ.x * localZ.x * a,
				b,
				-localZ.x
			);

			localY = glm::vec3(
				b,
				1.0f - localZ.y * localZ.y * a,
				-localZ.y
			);

		}

		glm::mat3 basis = glm::mat3(transform);

		float scale = (glm::length(basis[0]) + glm::length(basis[1]) + glm::length(basis[2])) / 3.0f;

		const glm::vec3 worldX = glm::normalize(basis * localX) * radius * scale;
		const glm::vec3 worldY = glm::normalize(basis * localY) * radius * scale;
		const glm::vec3 worldZ = glm::normalize(basis * localZ) * radius * scale;

		const glm::vec3 worldStart = glm::vec3(transform * glm::vec4(start, 1.0f));
		const glm::vec3 worldEnd = glm::vec3(transform * glm::vec4(end, 1.0f));

		std::vector<float> cos_theta(longitudinal_steps + 1);
		std::vector<float> sin_theta(longitudinal_steps + 1);
		for (int i = 0; i <= longitudinal_steps; ++i) {
			float t = 2.0f * pi * i / longitudinal_steps;
			cos_theta[i] = std::cos(t);
			sin_theta[i] = std::sin(t);
		}

		std::vector<float> cos_lat(latitude_steps + 1);
		std::vector<float> sin_lat(latitude_steps + 1);
		for (int j = 0; j <= latitude_steps; ++j) {
			const float l = (pi * 0.5f) * j / latitude_steps;
			cos_lat[j] = std::cos(l);
			sin_lat[j] = std::sin(l);
		}

		auto compute_point = [&](int ti, int li, bool startHemi) -> glm::vec3 {
			const glm::vec3 ring = worldX * (cos_theta[ti] * cos_lat[li]) + worldY * (sin_theta[ti] * cos_lat[li]);
			const glm::vec3 cap = worldZ * sin_lat[li];

			return startHemi ? worldStart + ring - cap: worldEnd + ring + cap;
		};

		for (int i = 0; i < longitudinal_steps; ++i) {
			glm::vec3 p = compute_point(i, latitude_steps, true);

			for (int j = latitude_steps - 1; j >= 0; --j) {
				glm::vec3 n = compute_point(i, j, true);
				DrawLineForMS(p, n, liftetimeMS, color, lineThickness);
				p = n;
			}

			const glm::vec3 shaft = compute_point(i, 0, false);
			DrawLineForMS(p, shaft, liftetimeMS, color, lineThickness);
			p = shaft;

			for (int j = 1; j <= latitude_steps; ++j) {
				glm::vec3 n = compute_point(i, j, false);
				DrawLineForMS(p, n, liftetimeMS, color, lineThickness);
				p = n;
			}
		}

		for (int j = 1; j < latitude_steps; ++j) {
			glm::vec3 p = compute_point(longitudinal_steps, j, true);
			for (int i = 1; i <= longitudinal_steps; ++i) {
				glm::vec3 n = compute_point(i, j, true);
				DrawLineForMS(p, n, liftetimeMS, color, lineThickness);
				p = n;
			}
		}

		for (int j = 1; j < latitude_steps; ++j) {
			glm::vec3 p = compute_point(longitudinal_steps, j, false);
			for (int i = 1; i <= longitudinal_steps; ++i) {
				glm::vec3 n = compute_point(i, j, false);
				DrawLineForMS(p, n, liftetimeMS, color, lineThickness);
				p = n;
			}
		}

		glm::vec3 ps = compute_point(longitudinal_steps, 0, true);
		glm::vec3 pe = compute_point(longitudinal_steps, 0, false);

		for (int i = 1; i <= longitudinal_steps; ++i) {
			glm::vec3 ns = compute_point(i, 0, true);
			glm::vec3 ne = compute_point(i, 0, false);
			DrawLineForMS(ps, ns, liftetimeMS, color, lineThickness);
			DrawLineForMS(pe, ne, liftetimeMS, color, lineThickness);
			ps = ns;
			pe = ne;
		}
	}


	void DebugDraw::DrawBox(glm::vec3 origin, glm::vec3 halfExtents, glm::mat4 transform, int liftetimeMS, const glm::vec4& color, float lineThickness) {

		glm::vec3 p000 = DebugUtil::ApplyTransform(origin + DebugUtil::CompMult(glm::vec3(-1., -1., -1.), halfExtents), transform);
		glm::vec3 p100 = DebugUtil::ApplyTransform(origin + DebugUtil::CompMult(glm::vec3(1., -1., -1.), halfExtents), transform);
		glm::vec3 p101 = DebugUtil::ApplyTransform(origin + DebugUtil::CompMult(glm::vec3(1., -1., 1.), halfExtents), transform);
		glm::vec3 p001 = DebugUtil::ApplyTransform(origin + DebugUtil::CompMult(glm::vec3(-1., -1., 1.), halfExtents), transform);
		glm::vec3 p010 = DebugUtil::ApplyTransform(origin + DebugUtil::CompMult(glm::vec3(-1., 1., -1.), halfExtents), transform);
		glm::vec3 p110 = DebugUtil::ApplyTransform(origin + DebugUtil::CompMult(glm::vec3(1., 1., -1.), halfExtents), transform);
		glm::vec3 p111 = DebugUtil::ApplyTransform(origin + DebugUtil::CompMult(glm::vec3(1., 1., 1.), halfExtents), transform);
		glm::vec3 p011 = DebugUtil::ApplyTransform(origin + DebugUtil::CompMult(glm::vec3(-1., 1., 1.), halfExtents), transform);

		DrawLineForMS(p000, p100, liftetimeMS, color, lineThickness);
		DrawLineForMS(p100, p101, liftetimeMS, color, lineThickness);
		DrawLineForMS(p101, p001, liftetimeMS, color, lineThickness);
		DrawLineForMS(p001, p000, liftetimeMS, color, lineThickness);
		DrawLineForMS(p010, p110, liftetimeMS, color, lineThickness);
		DrawLineForMS(p110, p111, liftetimeMS, color, lineThickness);
		DrawLineForMS(p111, p011, liftetimeMS, color, lineThickness);
		DrawLineForMS(p011, p010, liftetimeMS, color, lineThickness);
		DrawLineForMS(p000, p010, liftetimeMS, color, lineThickness);
		DrawLineForMS(p001, p011, liftetimeMS, color, lineThickness);
		DrawLineForMS(p101, p111, liftetimeMS, color, lineThickness);
		DrawLineForMS(p100, p110, liftetimeMS, color, lineThickness);
	}

	void DebugDraw::DrawTriangle(glm::vec3 pointA, glm::vec3 pointB, glm::vec3 pointC, const glm::mat4& transform, int liftetimeMS, const glm::vec4& color, float lineThickness) {

		auto apply_transform = [](glm::vec3 vec, const glm::mat4& mat) {
			return glm::vec3(mat * glm::vec4(vec, 1.0f));
		};

		DrawLineForMS(apply_transform(pointA, transform), apply_transform(pointB, transform), liftetimeMS, color, lineThickness);
		DrawLineForMS(apply_transform(pointB, transform), apply_transform(pointC, transform), liftetimeMS, color, lineThickness);
		DrawLineForMS(apply_transform(pointC, transform), apply_transform(pointA, transform), liftetimeMS, color, lineThickness);

	}

	std::optional<std::reference_wrapper<std::unique_ptr<DebugUtil::DebugLine>>> DebugDraw::GetExistingLine(const glm::vec3& from, const glm::vec3& to, const glm::vec4& color, float lineThickness) {
		for (int i = 0; i < LinesToDraw.size(); i++) {
			std::unique_ptr<DebugUtil::DebugLine>& line = LinesToDraw[i];

			if (
				DebugUtil::IsRoughlyEqual(from.x, line->From.x, DRAW_LOC_MAX_DIF) &&
				DebugUtil::IsRoughlyEqual(from.y, line->From.y, DRAW_LOC_MAX_DIF) &&
				DebugUtil::IsRoughlyEqual(from.z, line->From.z, DRAW_LOC_MAX_DIF) &&
				DebugUtil::IsRoughlyEqual(to.x, line->To.x, DRAW_LOC_MAX_DIF) &&
				DebugUtil::IsRoughlyEqual(to.y, line->To.y, DRAW_LOC_MAX_DIF) &&
				DebugUtil::IsRoughlyEqual(to.z, line->To.z, DRAW_LOC_MAX_DIF) &&
				DebugUtil::IsRoughlyEqual(lineThickness, line->LineThickness, DRAW_LOC_MAX_DIF) &&
				color == line->Color) {
				return line;
			}
		}

		return std::nullopt;
	}

	void DebugDraw::DrawLine3D(const GPtr<GFxMovieView>& movie, glm::vec3 from, glm::vec3 to, float color, float lineThickness, float alpha) {

		if (DebugUtil::IsPosBehindPlayerCamera(from) && DebugUtil::IsPosBehindPlayerCamera(to)) {
			return;
		}

		glm::vec2 screenLocFrom = WorldToScreenLoc(movie, from);
		glm::vec2 screenLocTo = WorldToScreenLoc(movie, to);

		DrawLine2D(movie, screenLocFrom, screenLocTo, color, lineThickness, alpha);
	}

	void DebugDraw::DrawLine3D(const GPtr<GFxMovieView>& movie, glm::vec3 from, glm::vec3 to, glm::vec4 color, float lineThickness) {
		DrawLine3D(movie, from, to, DebugUtil::RGBToHex(glm::vec3(color.r, color.g, color.b)), lineThickness, color.a * 100.0f);
	}

	void DebugDraw::DrawLine2D(const GPtr<GFxMovieView>& movie, glm::vec2 from, glm::vec2 to, float color, float lineThickness, float alpha) {

		// all parts of the line are off screen - don't need to draw them

		if (!IsOnScreen(from, to)) {
			return;
		}

		FastClampToScreen(from);
		FastClampToScreen(to);

		GFxValue argsLineStyle[3]{ lineThickness, color, alpha };
		movie->Invoke("lineStyle", nullptr, argsLineStyle, 3);

		GFxValue argsStartPos[2]{ from.x, from.y };
		movie->Invoke("moveTo", nullptr, argsStartPos, 2);

		GFxValue argsEndPos[2]{ to.x, to.y };
		movie->Invoke("lineTo", nullptr, argsEndPos, 2);

		movie->Invoke("endFill", nullptr, nullptr, 0);
	}

	void DebugDraw::DrawLine2D(const GPtr<GFxMovieView>& movie, glm::vec2 from, glm::vec2 to, glm::vec4 color, float lineThickness) {
		DrawLine2D(movie, from, to, DebugUtil::RGBToHex(glm::vec3(color.r, color.g, color.b)), lineThickness, color.a * 100.0f);
	}

	void DebugDraw::ClearLines2D(const GPtr<GFxMovieView>& movie) {
		movie->Invoke("clear", nullptr, nullptr, 0);
	}

	GPtr<IMenu> DebugDraw::GetHUD() {
		return UI::GetSingleton()->GetMenu(DebugMenu::MENU_NAME);
	}

	void DebugDraw::FastClampToScreen(glm::vec2& point) {

		if (point.x < 0.0f) {
			const float overshootX = abs(point.x);
			if (overshootX > CLAMP_MAX_OVERSHOOT) {
				point.x += overshootX - CLAMP_MAX_OVERSHOOT;
			}
		}
		else if (point.x > ScreenResX) {
			const float overshootX = point.x - ScreenResX;
			if (overshootX > CLAMP_MAX_OVERSHOOT) {
				point.x -= overshootX - CLAMP_MAX_OVERSHOOT;
			}
		}

		if (point.y < 0.0f) {
			const float overshootY = abs(point.y);
			if (overshootY > CLAMP_MAX_OVERSHOOT) {
				point.y += overshootY - CLAMP_MAX_OVERSHOOT;
			}
		}
		else if (point.y > ScreenResY) {
			const float overshootY = point.y - ScreenResY;
			if (overshootY > CLAMP_MAX_OVERSHOOT) {
				point.y -= overshootY - CLAMP_MAX_OVERSHOOT;
			}
		}
	}

	glm::vec2 DebugDraw::WorldToScreenLoc(const GPtr<GFxMovieView>& movie, glm::vec3 worldLoc) {

		glm::vec2 screenLocOut;
		const NiPoint3 niWorldLoc(worldLoc.x, worldLoc.y, worldLoc.z);

		float zVal;

		NiCamera::WorldPtToScreenPt3(Hooks::World::RawWorldToCamMatrix->data, *Hooks::World::RawViewPort, niWorldLoc, screenLocOut.x, screenLocOut.y, zVal, 1e-5f);
		GRectF rect = movie->GetVisibleFrameRect();

		screenLocOut.x = rect.left + (rect.right - rect.left) * screenLocOut.x;
		screenLocOut.y = 1.0f - screenLocOut.y;  // Flip y for Flash coordinate system
		screenLocOut.y = rect.top + (rect.bottom - rect.top) * screenLocOut.y;

		return screenLocOut;
	}

	void DebugDraw::CacheMenuData() {

		if (CachedMenuData) {
			return;
		}

		GPtr<IMenu> menu = UI::GetSingleton()->GetMenu(DebugMenu::MENU_NAME);
		if (!menu || !menu->uiMovie) {
			return;
		}

		GRectF rect = menu->uiMovie->GetVisibleFrameRect();

		ScreenResX = abs(rect.left - rect.right);
		ScreenResY = abs(rect.top - rect.bottom);

		LinesToDraw.reserve(4096);

		CachedMenuData = true;
		logger::debug("DebugDraw::CacheMenuData");

	}

	bool DebugDraw::IsOnScreen(glm::vec2 from, glm::vec2 to) {
		return IsOnScreen(from) || IsOnScreen(to);
	}

	bool DebugDraw::IsOnScreen(glm::vec2 point) {
		return (point.x <= ScreenResX && point.x >= 0.0f && point.y <= ScreenResY && point.y >= 0.0f);
	}

	bool DebugDraw::CanDraw() {
		return Config::Advanced.bShowOverlay;
	}

	bool DebugDraw::CanDraw(RE::Actor* a_actor, DrawTarget a_draw_target) {
		if (Config::Advanced.bShowOverlay) {
			switch (a_draw_target) {
				default:
				case DrawTarget::kPlayerOnly: {
					return a_actor->IsPlayerRef();
				}
				case DrawTarget::kPlayerAndFollowers: {
					return a_actor->IsPlayerRef() || IsTeammate(a_actor);
				}
				case DrawTarget::kAnyGTS: {
					return a_actor->IsPlayerRef() || IsTeammate(a_actor) || EffectsForEveryone(a_actor);
				}
				case DrawTarget::kAll: {
					return true;
				}
			}
		}
		return false;
	}
}
