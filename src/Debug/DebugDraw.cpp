#include "Debug/DebugDraw.hpp"

#include "UI/DebugMenu.hpp"

#include "Debug/DrawUtils/DebugUtil.hpp"
#include "Debug/DrawUtils/DebugLine.hpp"
#include "Debug/DrawUtils/ObjectBound.hpp"

#include "Hooks/Other/Values.hpp"

namespace GTS {

	void DebugDraw::DrawLineForMS(const glm::vec3& from, const glm::vec3& to, int liftetimeMS, const glm::vec4& color, float lineThickness) {

		DebugUtil::DebugLine* oldLine = GetExistingLine(from, to, color, lineThickness);
		if (oldLine) {
			oldLine->From = from;
			oldLine->To = to;
			oldLine->DestroyTickCount = GetTickCount64() + liftetimeMS;
			oldLine->LineThickness = lineThickness;
			return;
		}

		DebugUtil::DebugLine* newLine = new DebugUtil::DebugLine(from, to, color, lineThickness, GetTickCount64() + liftetimeMS);
		LinesToDraw.push_back(newLine);
	}

	void DebugDraw::Update() {

		auto hud = GetHUD();

		if (!hud || !hud->uiMovie) {
			return;
		}

		CacheMenuData();
		ClearLines2D(hud->uiMovie);

		for (int i = 0; i < LinesToDraw.size(); i++) {

			DebugUtil::DebugLine* line = LinesToDraw[i];

			DrawLine3D(hud->uiMovie, line->From, line->To, line->fColor, line->LineThickness, line->Alpha);

			if (GetTickCount64() > line->DestroyTickCount) {
				LinesToDraw.erase(LinesToDraw.begin() + i);
				delete line;
				i--;
			}
		}
	}

	void DebugDraw::DrawBoundsForMS(DebugUtil::ObjectBound objectBound, int liftetimeMS, const glm::vec4& color, float lineThickness) {

		auto boundRight = objectBound.GetBoundRightVectorRotated();
		auto boundForward = objectBound.GetBoundForwardVectorRotated();
		auto boundUp = objectBound.GetBoundUpVectorRotated();

		auto objectLocation = objectBound.worldBoundMin;

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

		DebugDraw::DrawLineForMS(glmZStart1, glmZEnd1, liftetimeMS, color, lineThickness);
		DebugDraw::DrawLineForMS(glmZStart2, glmZEnd2, liftetimeMS, color, lineThickness);
		DebugDraw::DrawLineForMS(glmZStart3, glmZEnd3, liftetimeMS, color, lineThickness);
		DebugDraw::DrawLineForMS(glmZStart4, glmZEnd4, liftetimeMS, color, lineThickness);

		DebugDraw::DrawLineForMS(glmYStart1, glmYEnd1, liftetimeMS, color, lineThickness);
		DebugDraw::DrawLineForMS(glmYStart2, glmYEnd2, liftetimeMS, color, lineThickness);
		DebugDraw::DrawLineForMS(glmYStart3, glmYEnd3, liftetimeMS, color, lineThickness);
		DebugDraw::DrawLineForMS(glmYStart4, glmYEnd4, liftetimeMS, color, lineThickness);

		DebugDraw::DrawLineForMS(glmXStart1, glmXEnd1, liftetimeMS, color, lineThickness);
		DebugDraw::DrawLineForMS(glmXStart2, glmXEnd2, liftetimeMS, color, lineThickness);
		DebugDraw::DrawLineForMS(glmXStart3, glmXEnd3, liftetimeMS, color, lineThickness);
		DebugDraw::DrawLineForMS(glmXStart4, glmXEnd4, liftetimeMS, color, lineThickness);
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

	void DebugDraw::DrawCapsule(glm::vec3 start, glm::vec3 end, float radius, glm::mat4 transform, int liftetimeMS, const glm::vec4& color, float lineThickness) {

		// From https://gamedev.stackexchange.com/questions/162426/how-to-draw-a-3d-capsule
		// Local basis

		float pi = glm::pi<float>();
		glm::vec3 axis = end - start;
		float length = glm::length(axis);
		glm::vec3 localZ = axis / length;
		glm::vec3 localX = DebugUtil::GetAnyPerpendicularUnitVector(localZ);
		glm::vec3 localY = glm::cross(localZ, localX);
		localX = glm::cross(localY, localZ); // Reorthonalize

		auto shaft_point = [&](float u, float v) {
			return start +
				localX * glm::cos(2 * pi * u) * radius +
				localY * glm::sin(2 * pi * u) * radius +
				localZ * v * length;
			};

		auto start_hemi_point = [&](float u, float v) {
			float latitude = (pi / 2) * (v - 1);
			return start +
				localX * cos(2 * pi * u) * cos(latitude) * radius +
				localY * sin(2 * pi * u) * cos(latitude) * radius +
				localZ * sin(latitude) * radius;
			};

		auto end_hemi_point = [&](float u, float v) {
			float latitude = (pi / 2) * v;
			return end +
				localX * cos(2 * pi * u) * cos(latitude) * radius +
				localY * sin(2 * pi * u) * cos(latitude) * radius +
				localZ * sin(latitude) * radius;
			};

		auto apply_transform = [](glm::vec3 vec, const glm::mat4& mat) {
			return glm::vec3(mat * glm::vec4(vec, 1.0f));
		};

		// Loop1
		DrawLineForMS(apply_transform(shaft_point(0.0f, 0.0f), transform), apply_transform(shaft_point(0.0f, 1.0f), transform), liftetimeMS, color, lineThickness);
		DrawLineForMS(apply_transform(shaft_point(0.5f, 0.0f), transform), apply_transform(shaft_point(0.5f, 1.0f), transform), liftetimeMS, color, lineThickness);

		// Loop2
		DrawLineForMS(apply_transform(shaft_point(0.25f, 0.0f), transform), apply_transform(shaft_point(0.25f, 1.0f), transform), liftetimeMS, color, lineThickness);
		DrawLineForMS(apply_transform(shaft_point(0.75f, 0.0f), transform), apply_transform(shaft_point(0.75f, 1.0f), transform), liftetimeMS, color, lineThickness);

		// Start hemi
		constexpr int STEPS = 20;
		glm::vec3 prev_point = apply_transform(start_hemi_point(0.0f, 0.0f), transform);
		for (int i = 1; i < STEPS; i++) {
			float x = (1.0f / STEPS * i);
			float v = glm::sin(x * pi);
			float u = (x < 0.5f) ? 0.0f : 0.5f;
			glm::vec3 next_point = apply_transform(start_hemi_point(u, v), transform);
			DrawLineForMS(prev_point, next_point, liftetimeMS, color, lineThickness);
			prev_point = next_point;
		}

		prev_point = apply_transform(start_hemi_point(0.0f, 0.25f), transform);
		for (int i = 1; i < STEPS; i++) {
			float x = (1.0f / STEPS * i);
			float v = glm::sin(x * pi);
			float u = (x < 0.5f) ? 0.25f : 0.75f;
			glm::vec3 next_point = apply_transform(start_hemi_point(u, v), transform);
			DrawLineForMS(prev_point, next_point, liftetimeMS, color, lineThickness);
			prev_point = next_point;
		}

		// end hemi
		prev_point = apply_transform(end_hemi_point(0.0f, 0.0f), transform);
		for (int i = 1; i < STEPS; i++) {
			float x = (1.0f / STEPS * i);
			float v = glm::sin(x * pi);
			float u = (x < 0.5f) ? 0.0f : 0.5f;
			glm::vec3 next_point = apply_transform(end_hemi_point(u, v), transform);
			DrawLineForMS(prev_point, next_point, liftetimeMS, color, lineThickness);
			prev_point = next_point;
		}

		prev_point = apply_transform(end_hemi_point(0.0f, 0.25f), transform);
		for (int i = 1; i < STEPS; i++) {
			float x = (1.0f / STEPS * i);
			float v = glm::sin(x * pi);
			float u = (x < 0.5f) ? 0.25f : 0.75f;
			glm::vec3 next_point = apply_transform(end_hemi_point(u, v), transform);
			DrawLineForMS(prev_point, next_point, liftetimeMS, color, lineThickness);
			prev_point = next_point;
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

	DebugUtil::DebugLine* DebugDraw::GetExistingLine(const glm::vec3 & from, const glm::vec3 & to, const glm::vec4 & color, float lineThickness) {
		for (int i = 0; i < LinesToDraw.size(); i++) {
			DebugUtil::DebugLine* line = LinesToDraw[i];

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

		return nullptr;
	}

	void DebugDraw::DrawLine3D(const RE::GPtr<RE::GFxMovieView>& movie, glm::vec3 from, glm::vec3 to, float color, float lineThickness, float alpha) {

		if (DebugUtil::IsPosBehindPlayerCamera(from) && DebugUtil::IsPosBehindPlayerCamera(to)) {
			return;
		}

		glm::vec2 screenLocFrom = WorldToScreenLoc(movie, from);
		glm::vec2 screenLocTo = WorldToScreenLoc(movie, to);

		DrawLine2D(movie, screenLocFrom, screenLocTo, color, lineThickness, alpha);
	}

	void DebugDraw::DrawLine3D(const RE::GPtr<RE::GFxMovieView>& movie, glm::vec3 from, glm::vec3 to, glm::vec4 color, float lineThickness) {
		DrawLine3D(movie, from, to, DebugUtil::RGBToHex(glm::vec3(color.r, color.g, color.b)), lineThickness, color.a * 100.0f);
	}

	void DebugDraw::DrawLine2D(const RE::GPtr<RE::GFxMovieView>& movie, glm::vec2 from, glm::vec2 to, float color, float lineThickness, float alpha) {

		// all parts of the line are off screen - don't need to draw them

		if (!IsOnScreen(from, to)) {
			return;
		}

		FastClampToScreen(from);
		FastClampToScreen(to);

		RE::GFxValue argsLineStyle[3]{ lineThickness, color, alpha };
		movie->Invoke("lineStyle", nullptr, argsLineStyle, 3);

		RE::GFxValue argsStartPos[2]{ from.x, from.y };
		movie->Invoke("moveTo", nullptr, argsStartPos, 2);

		RE::GFxValue argsEndPos[2]{ to.x, to.y };
		movie->Invoke("lineTo", nullptr, argsEndPos, 2);

		movie->Invoke("endFill", nullptr, nullptr, 0);
	}

	void DebugDraw::DrawLine2D(const RE::GPtr<RE::GFxMovieView>& movie, glm::vec2 from, glm::vec2 to, glm::vec4 color, float lineThickness) {
		DrawLine2D(movie, from, to, DebugUtil::RGBToHex(glm::vec3(color.r, color.g, color.b)), lineThickness, color.a * 100.0f);
	}

	void DebugDraw::ClearLines2D(const RE::GPtr<RE::GFxMovieView>& movie) {
		movie->Invoke("clear", nullptr, nullptr, 0);
	}

	RE::GPtr<RE::IMenu> DebugDraw::GetHUD() {
		return RE::UI::GetSingleton()->GetMenu(DebugMenu::MENU_NAME);
	}

	void DebugDraw::FastClampToScreen(glm::vec2& point) {

		if (point.x < 0.0f) {
			float overshootX = abs(point.x);
			if (overshootX > CLAMP_MAX_OVERSHOOT) {
				point.x += overshootX - CLAMP_MAX_OVERSHOOT;
			}
		}
		else if (point.x > ScreenResX) {
			float overshootX = point.x - ScreenResX;
			if (overshootX > CLAMP_MAX_OVERSHOOT) {
				point.x -= overshootX - CLAMP_MAX_OVERSHOOT;
			}
		}

		if (point.y < 0.0f) {
			float overshootY = abs(point.y);
			if (overshootY > CLAMP_MAX_OVERSHOOT) {
				point.y += overshootY - CLAMP_MAX_OVERSHOOT;
			}
		}
		else if (point.y > ScreenResY) {
			float overshootY = point.y - ScreenResY;
			if (overshootY > CLAMP_MAX_OVERSHOOT) {
				point.y -= overshootY - CLAMP_MAX_OVERSHOOT;
			}
		}
	}

	glm::vec2 DebugDraw::WorldToScreenLoc(const RE::GPtr<RE::GFxMovieView>& movie, glm::vec3 worldLoc) {

		glm::vec2 screenLocOut;
		RE::NiPoint3 niWorldLoc(worldLoc.x, worldLoc.y, worldLoc.z);

		float zVal;

		RE::NiCamera::WorldPtToScreenPt3(Hooks::World::RawWorldToCamMatrix->data, *Hooks::World::RawViewPort, niWorldLoc, screenLocOut.x, screenLocOut.y, zVal, 1e-5f);
		RE::GRectF rect = movie->GetVisibleFrameRect();

		screenLocOut.x = rect.left + (rect.right - rect.left) * screenLocOut.x;
		screenLocOut.y = 1.0f - screenLocOut.y;  // Flip y for Flash coordinate system
		screenLocOut.y = rect.top + (rect.bottom - rect.top) * screenLocOut.y;

		return screenLocOut;
	}

	void DebugDraw::CacheMenuData() {

		if (CachedMenuData) {
			return;
		}

		RE::GPtr<RE::IMenu> menu = RE::UI::GetSingleton()->GetMenu(DebugMenu::MENU_NAME);
		if (!menu || !menu->uiMovie) {
			return;
		}

		RE::GRectF rect = menu->uiMovie->GetVisibleFrameRect();

		ScreenResX = abs(rect.left - rect.right);
		ScreenResY = abs(rect.top - rect.bottom);

		CachedMenuData = true;
		log::debug("DebugDraw::CacheMenuData");

	}

	bool DebugDraw::IsOnScreen(glm::vec2 from, glm::vec2 to) {
		return IsOnScreen(from) || IsOnScreen(to);
	}

	bool DebugDraw::IsOnScreen(glm::vec2 point) {
		return (point.x <= ScreenResX && point.x >= 0.0f && point.y <= ScreenResY && point.y >= 0.0f);
	}
}
