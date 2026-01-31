#pragma once

namespace AnimationVars::Helpers {

	[[nodiscard]]
	constexpr __forceinline bool GetBool(const RE::Actor* a_actor, const RE::BSFixedString& a_var) {
		bool result = false;

		if (a_actor) {
			if (a_actor->GetGraphVariableBool(a_var, result)) {
				return result;
			}
		}

		return false;
	}

	[[nodiscard]]
	constexpr __forceinline bool SetBool(RE::Actor* a_actor, const RE::BSFixedString& a_var, bool a_value) {
		return a_actor && a_actor->SetGraphVariableBool(a_var, a_value);
	}

	[[nodiscard]]
	constexpr __forceinline float GetFloat(const RE::Actor* a_actor, const RE::BSFixedString& a_var) {
		float result = 0.0f;

		if (a_actor) {
			if (a_actor->GetGraphVariableFloat(a_var, result)) {
				return result;
			}
		}
		return 0.0f;
	}

	[[nodiscard]]
	constexpr __forceinline bool SetFloat(RE::Actor* a_actor, const RE::BSFixedString& a_var, float a_value) {
		return a_actor && a_actor->SetGraphVariableFloat(a_var, a_value);
	}

	[[nodiscard]]
	constexpr __forceinline float GetInt(const RE::Actor* a_actor, const RE::BSFixedString& a_var) {
		int result = 0;

		if (a_actor) {
			if (a_actor->GetGraphVariableInt(a_var, result)) {
				return result;
			}
		}
		return 0;
	}

	[[nodiscard]]
	constexpr __forceinline bool SetInt(RE::Actor* a_actor, const RE::BSFixedString& a_var, int a_value) {
		return a_actor && a_actor->SetGraphVariableInt(a_var, a_value);
	}

}