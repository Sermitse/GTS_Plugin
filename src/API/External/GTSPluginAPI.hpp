#pragma once
#include <stdint.h>


/* GTSPlugin API
 * To use the API copy this file into your project and call RequestPluginAPI() in kPostPostLoad or later.
 * 
 * Example:
 * 
 * static inline GTSPluginAPI::GTSPluginAPIV1* API = nullptr;
 * API = static_cast<GTSPluginAPI::GTSPluginAPIV1*>(GTSPluginAPI::RequestPluginAPI(GTSPluginAPI::InterfaceVersion::V1));
 * 
 * if(API) {
 * 
 *     if(API->SetTargetScale(PlayerCharacter::GetSingleton(), 4.0f) == GTSPluginAPI::APIResult::kOK) {
 *         //Do Stuff
 *     }
 *     
 *     float VisScale = 0.0f;
 *     if(API->GetVisualScale(PlayerCharacter::GetSingleton(), VisScale) == GTSPluginAPI::APIResult::kOK){
 *			SKSE::log::info("VisScale is {}, VisScale);
 *     }
 * }
 */

namespace GTSPluginAPI {

	enum class InterfaceVersion : uint8_t {
		V1 = 1,
	};

	enum class APIResult : uint8_t {
		kOk,
		kFail
	};

	// Interface V1
	class GTSPluginAPIV1 {
		public:
		virtual ~GTSPluginAPIV1() noexcept = default;

		/// <summary>
		/// Get an actor's current (at the moment of the call) scale.
		/// </summary>
		/// <param name="a_actor">Pointer to the actor object.</param>
		/// <param name="a_out">Return value</param>
		/// <returns>Success or failure</returns>
		virtual APIResult GetVisualScale(RE::Actor* a_actor, float& a_out) noexcept = 0;


		/// <summary>
		/// Get an actor's maximum achievable scale.
		/// </summary>
		/// <param name="a_actor">Pointer to the actor object.</param>
		/// <param name="a_out">Return value</param>
		/// <returns>Success or failure</returns>
		virtual APIResult GetMaxScale(RE::Actor* a_actor, float& a_out) noexcept = 0;


		/// <summary>
		/// Get an actor's natural scale, this is the native game scale of the actor based on the ActorBase, Race and REFR scales.
		/// </summary>
		/// <param name="a_actor">Pointer to the actor object.</param>
		/// <param name="a_out">Return value</param>
		/// <returns>Success or failure</returns>
		virtual APIResult GetNaturalScale(RE::Actor* a_actor, float& a_out) noexcept = 0;
		
		
		/// <summary>
		/// Get an actor's target scale, this is the scale which the the actor is smoothly being changed to at the moment.
		/// </summary>
		/// <param name="a_actor">Pointer to the actor object.</param>
		/// <param name="a_out">Return value</param>
		/// <returns>Success or failure</returns>
		virtual APIResult GetTargetScale(RE::Actor* a_actor, float& a_out) noexcept = 0;
		
		
		/// <summary>
		/// Sets an actor's target scale to the specified value. The actor will grow towards the specified scale or the max allowed scale, (whichever one is lower).
		/// </summary>
		/// <param name="a_actor">Pointer to the actor object.</param>
		/// <param name="a_scale">Target scale to grow/shrink towards</param>
		/// <returns>Success or failure</returns>
		virtual APIResult SetTargetScale(RE::Actor* a_actor, float a_scale) noexcept = 0;
		
		
		/// <summary>
		/// Modifies an actor's target scale relative to their current one. The actor will grow towards the specified scale or the max allowed scale, (whichever one is lower).
		/// </summary>
		/// <param name="a_actor">Pointer to the actor object.</param>
		/// <param name="a_modAmount">Modify amount</param>
		/// <returns>Success or failure</returns>
		virtual APIResult ModTargetScale(RE::Actor* a_actor, float a_modAmount) noexcept = 0;

		/// <summary>
		/// Retrieves the current animation slowdown multiplier for the specified actor.
		/// The multiplier is calculated using a soft-core potential function based on the actor's
		/// visual scale, clamped to a configurable lower bound. Returns 1.0 (no slowdown) if dynamic
		/// animation speed is disabled, the actor is in a sit/sleep state, or the actor is performing
		/// a GTS animation with full-speed override enabled. Player and teammate actors have an
		/// additional configurable scalar applied to their result.
		/// </summary>
		/// <param name="a_actor">Pointer to the actor object.</param>
		/// <param name="a_multiplier">Out parameter receiving the animation speed multiplier [lowerBound, 1.0].</param>
		/// <returns>Success or failure</returns>
		virtual APIResult GetAnimationSlowdown(RE::Actor* a_actor, float& a_multiplier) noexcept = 0;

		/// <summary>
		/// Retrieves the five soft-core potential parameters used to compute animation slowdown,
		/// as configured in the advanced settings (fAnimSpeedSoftCore). The parameters map to
		/// [k, n, s, o, a] of the SoftPotential struct, in that order.
		/// </summary>
		/// <param name="a_args">Out array of 5 floats receiving [k, n, s, o, a].</param>
		/// <returns>Success or failure</returns>
		virtual APIResult GetAnimationSlowdownArgs(std::array<float, 5>& a_args) noexcept = 0;

	};

	/// <summary>
	/// Request the the plugin interface object.
	/// Note: Should be done on, or after kPostPostLoad at the earliest to ensure the main dll has been loaded/Initialized by SKSE.
	/// If aquiring fails use the kDataLoaded event instead.
	/// </summary>
	/// <param name="a_interfaceVersion">The interface version to request</param>
	/// <returns>The pointer to the API singleton, or null if the request somehow failed</returns>
	[[nodiscard]] inline void* RequestPluginAPI(const InterfaceVersion a_interfaceVersion = InterfaceVersion::V1)
	{
		typedef void* (*Singleton)(const InterfaceVersion interfaceVersion);

		auto handle = GetModuleHandleW(L"GtsPlugin.dll");
		Singleton SingletonPtr = reinterpret_cast<Singleton>(GetProcAddress(handle, "RequestPluginAPI"));
		if (SingletonPtr) {
			return SingletonPtr(a_interfaceVersion);
		}
		return nullptr;
	}
}