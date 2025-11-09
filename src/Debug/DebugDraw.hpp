#pragma once

#include "Debug/Util/DebugLine.hpp"
#include "Debug/Util/ObjectBound.hpp"

namespace GTS {

	class DebugDraw {

		public:
		static inline bool CachedMenuData = false;
		static inline float ScreenResX = 0.0f;
		static inline float ScreenResY = 0.0f;
		static inline std::vector<DebugUtil::DebugLine*> LinesToDraw = {};
		static inline bool DEBUG_API_REGISTERED = false;
		static constexpr int CIRCLE_NUM_SEGMENTS = 32;
		static constexpr float DRAW_LOC_MAX_DIF = 1.0f;
		static constexpr float CLAMP_MAX_OVERSHOOT = 10000.0f;

		static void Update();

		static RE::GPtr<RE::IMenu> GetHUD();

		static void DrawLine2D(const RE::GPtr<RE::GFxMovieView>& movie, glm::vec2 from, glm::vec2 to, float color, float lineThickness, float alpha);
		static void DrawLine2D(const RE::GPtr<RE::GFxMovieView>& movie, glm::vec2 from, glm::vec2 to, glm::vec4 color, float lineThickness);
		static void ClearLines2D(const RE::GPtr<RE::GFxMovieView>& movie);

		static void DrawLine3D(const RE::GPtr<RE::GFxMovieView>& movie, glm::vec3 from, glm::vec3 to, float color, float lineThickness, float alpha);
		static void DrawLine3D(const RE::GPtr<RE::GFxMovieView>& movie, glm::vec3 from, glm::vec3 to, glm::vec4 color, float lineThickness);

		static void DrawLineForMS(const glm::vec3& from, const glm::vec3& to, int liftetimeMS = 10, const glm::vec4& color = { 1.0f, 0.0f, 0.0f, 1.0f }, float lineThickness = 1);
		static void DrawBoundsForMS(DebugUtil::ObjectBound objectBound, int liftetimeMS = 10, const glm::vec4& color = { 1.0f, 0.0f, 0.0f, 1.0f }, float lineThickness = 1);
		static void DrawSphere(glm::vec3, float radius, int liftetimeMS = 10, const glm::vec4& color = { 1.0f, 0.0f, 0.0f, 1.0f }, float lineThickness = 1);
		static void DrawCircle(glm::vec3, float radius, glm::vec3 eulerAngles, int liftetimeMS = 10, const glm::vec4& color = { 1.0f, 0.0f, 0.0f, 1.0f }, float lineThickness = 1);
		static void DrawHalfCircle(glm::vec3, float radius, glm::vec3 eulerAngles, int liftetimeMS = 10, const glm::vec4& color = { 1.0f, 0.0f, 0.0f, 1.0f }, float lineThickness = 1);
		static void DrawCapsule(glm::vec3 start, glm::vec3 end, float radius, glm::mat4 transform, int liftetimeMS = 10, const glm::vec4& color = { 1.0f, 0.0f, 0.0f, 1.0f }, float lineThickness = 1);
		static void DrawTriangle(glm::vec3 pointA, glm::vec3 pointB, glm::vec3 pointC, const glm::mat4& transform, int liftetimeMS = 10, const glm::vec4& color = { 1.0f, 0.0f, 0.0f, 1.0f }, float lineThickness = 1);
		static void DrawBox(glm::vec3 origin, glm::vec3 halfExtents, glm::mat4 transform, int liftetimeMS = 10, const glm::vec4& color = { 1.0f, 0.0f, 0.0f, 1.0f }, float lineThickness = 1);

		static glm::vec2 WorldToScreenLoc(const RE::GPtr<RE::GFxMovieView>& movie, glm::vec3 worldLoc);
		static void FastClampToScreen(glm::vec2& point);
		static bool IsOnScreen(glm::vec2 from, glm::vec2 to);
		static bool IsOnScreen(glm::vec2 point);
		static void CacheMenuData();

		private:
		static DebugUtil::DebugLine* GetExistingLine(const glm::vec3& from, const glm::vec3& to, const glm::vec4& color, float lineThickness);
	};
}

