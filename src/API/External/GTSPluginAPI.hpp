#pragma once
#include <stdint.h>


/* GTSPlugin API
 * To use the API copy this file into your project and call RequestPluginAPI() at the earliset in the kPostPostLoad event.
 * It's however recommended to do this the kDataLoaded event instead.
 * 
 * Example:
 * 
 * static inline GTSPluginAPI::GTSPluginAPIV1* API = nullptr;
 * API = static_cast<GTSPluginAPI::GTSPluginAPIV1*>(GTSPluginAPI::RequestPluginAPI(GTSPluginAPI::InterfaceVersion::V1));
 * 
 * if(API) {
 *	   auto PlayerHandle = PlayerCharacter::GetSingleton()->CreateReferencehandle();
 *     if(API->SetTargetScale(PlayerHandle, 4.0f) == GTSPluginAPI::APIResult::kOK) {
 *         //Do Stuff
 *     }
 *     
 *     float VisScale = 0.0f;
 *     if(API->GetVisualScale(PlayerHandle, VisScale) == GTSPluginAPI::APIResult::kOK){
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
		/// <param name="a_actorHandle">Handle to the actor* you want to use</param>
		/// <param name="a_out">Return value</param>
		/// <returns>Success or failure</returns>
		virtual APIResult GetVisualScale(RE::ActorHandle a_actorHandle, float& a_out) noexcept = 0;


		/// <summary>
		/// Get an actor's maximum achievable scale.
		/// </summary>
		/// <param name="a_actorHandle">Handle to the actor* you want to use</param>
		/// <param name="a_out">Return value</param>
		/// <returns>Success or failure</returns>
		virtual APIResult GetMaxScale(RE::ActorHandle a_actorHandle, float& a_out) noexcept = 0;


		/// <summary>
		/// Get an actor's natural scale, this is the native game scale of the actor based on the ActorBase, Race and REFR scales.
		/// </summary>
		/// <param name="a_actorHandle">Handle to the actor* you want to use</param>
		/// <param name="a_out">Return value</param>
		/// <returns>Success or failure</returns>
		virtual APIResult GetNaturalScale(RE::ActorHandle a_actorHandle, float& a_out) noexcept = 0;
		
		
		/// <summary>
		/// Get an actor's target scale, this is the scale which the the actor is smoothly being changed to at the moment.
		/// </summary>
		/// <param name="a_actorHandle">Handle to the actor* you want to use</param>
		/// <param name="a_out">Return value</param>
		/// <returns>Success or failure</returns>
		virtual APIResult GetTargetScale(RE::ActorHandle a_actorHandle, float& a_out) noexcept = 0;
		
		
		/// <summary>
		/// Sets an actor's target scale to the specified value. The actor will grow towards the specified scale or the max allowed scale, (whichever one is lower).
		/// </summary>
		/// <param name="a_actorHandle">Handle to the actor* you want to use</param>
		/// <param name="a_scale">Target scale to grow/shrink towards</param>
		/// <returns>Success or failure</returns>
		virtual APIResult SetTargetScale(RE::ActorHandle a_actorHandle, float a_scale) noexcept = 0;
		
		
		/// <summary>
		/// Modifies an actor's target scale relative to their current one. The actor will grow towards the specified scale or the max allowed scale, (whichever one is lower).
		/// </summary>
		/// <param name="a_actorHandle">Handle to the actor* you want to use</param>
		/// <param name="a_modAmount">Modify amount</param>
		/// <returns>Success or failure</returns>
		virtual APIResult ModTargetScale(RE::ActorHandle a_actorHandle, float a_modAmount) noexcept = 0;

	};


	/// <summary>
	/// Request the the plugin interface object.
	/// Note: Should be done on or after kPostPostLoad at the earliest to ensure the dll has been loaded/Initialized by SKSE.
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