#include "Managers/Morphs/MorphManager.hpp"
#include "API/Racemenu.hpp"
#include "Config/Config.hpp"

namespace GTS {

	// --------------------------------
	// Virtuals
	// --------

	std::string MorphManager::DebugName() {
		return "::MorphManager";
	}

	void MorphManager::ResetActor(Actor* a_actor) {
		if (!Racemenu::Loaded()) return;
		Racemenu::ClearMorphs(a_actor, GetMorphKey(kBreasts));
		Racemenu::ClearMorphs(a_actor, GetMorphKey(kBelly));
	}

	void MorphManager::ActorLoaded(Actor* a_actor) {
		if (!Racemenu::Loaded()) return;
		if (!a_actor) return;
		if (!a_actor->Get3D()) return;
		if (!IsHuman(a_actor)) return;
		if (!a_actor->IsPlayerRef() && !IsTeammate(a_actor)) return;

		if (auto* persistentData = Persistent::GetActorData(a_actor)) {
			if (auto* transientData = Transient::GetActorData(a_actor)) {
				// Initialize transient current scale from persistent target
				transientData->CurrentBreastsScale = persistentData->fBreastsScale;
				transientData->CurrentBellyScale = persistentData->fBellyScale;

				if (persistentData->fBreastsScale != 0.0f) {
					UpdateMorphsImediate(a_actor, kBreasts);
				}
				if (persistentData->fBellyScale != 0.0f) {
					UpdateMorphsImediate(a_actor, kBelly);
				}
			}
		}
	}

	void MorphManager::ActorUpdate(RE::Actor* a_actor) {
		if (!Racemenu::Loaded()) return;
		if (!State::InGame()) return;
		if (!a_actor) return;
		if (!a_actor->Get3D()) return;
		if (!IsHuman(a_actor)) return;
		if (!a_actor->IsPlayerRef() && !IsTeammate(a_actor)) return;

		auto* persistentData = Persistent::GetActorData(a_actor);
		auto* transientData = Transient::GetActorData(a_actor);
		if (!persistentData || !transientData) return;

		const float deltaTime = Time::WorldTimeDelta();
		constexpr float& EPS = Config::Gameplay.ActionSettings.fMorphEPS;
		bool needsUpdate = false;

		// Apply breast shrinking over time if enabled
		if (Config::Gameplay.ActionSettings.bShrinkBreastsOverTime && persistentData->fBreastsScale > 0.0f) {
			const float shrinkAmount = Config::Gameplay.ActionSettings.fBreastShrinkRate * deltaTime;
			persistentData->fBreastsScale = std::max(0.0f, persistentData->fBreastsScale - shrinkAmount);
			transientData->CurrentBreastsScale = std::max(0.0f, transientData->CurrentBreastsScale - shrinkAmount);
			if (std::abs(transientData->CurrentBreastsScale - transientData->LastAppliedBreastsScale) > EPS ||
				(transientData->CurrentBreastsScale == 0.0f && transientData->LastAppliedBreastsScale != 0.0f)) {
				SetMorph(a_actor, kBreasts, transientData->CurrentBreastsScale);
				transientData->LastAppliedBreastsScale = transientData->CurrentBreastsScale;
				needsUpdate = true;
			}
		}

		// Update breasts morph
		const float breastsDiff = std::abs(persistentData->fBreastsScale - transientData->CurrentBreastsScale);
		if (breastsDiff > EPS) {
			const float increment = (transientData->BreastsTransitionTime > 0.0f)
				? (breastsDiff / transientData->BreastsTransitionTime) * deltaTime
				: breastsDiff; // Instant if time is 0
			if (breastsDiff <= std::abs(increment)) {
				transientData->CurrentBreastsScale = persistentData->fBreastsScale;
			}
			else {
				transientData->CurrentBreastsScale += increment;
			}
			if (std::abs(transientData->CurrentBreastsScale - transientData->LastAppliedBreastsScale) > EPS ||
				(transientData->CurrentBreastsScale == 0.0f && transientData->LastAppliedBreastsScale != 0.0f)) {
				SetMorph(a_actor, kBreasts, transientData->CurrentBreastsScale);
				transientData->LastAppliedBreastsScale = transientData->CurrentBreastsScale;
				needsUpdate = true;
			}
		}

		// Apply belly shrinking used as a fallback 
		if (Config::Gameplay.ActionSettings.bShrinkBellyOverTime && persistentData->fBellyScale > 0.0f) {
			const float shrinkAmount = Config::Gameplay.ActionSettings.fBellyShrinkRate * deltaTime;
			persistentData->fBellyScale = std::max(0.0f, persistentData->fBellyScale - shrinkAmount);
			transientData->CurrentBellyScale = std::max(0.0f, transientData->CurrentBellyScale - shrinkAmount);
			if (std::abs(transientData->CurrentBellyScale - transientData->LastAppliedBellyScale) > EPS ||
				(transientData->CurrentBellyScale == 0.0f && transientData->LastAppliedBellyScale != 0.0f)) {
				SetMorph(a_actor, kBelly, transientData->CurrentBellyScale);
				transientData->LastAppliedBellyScale = transientData->CurrentBellyScale;
				needsUpdate = true;
			}
		}

		// Update belly morph
		const float bellyDiff = persistentData->fBellyScale - transientData->CurrentBellyScale;

		if (std::abs(bellyDiff) > EPS) {
			const float increment = (transientData->BellyTransitionTime > 0.0f)
				? (bellyDiff / transientData->BellyTransitionTime) * deltaTime
				: bellyDiff; // Instant if time is 0
			if (std::abs(bellyDiff) <= std::abs(increment)) {
				transientData->CurrentBellyScale = persistentData->fBellyScale;
			}
			else {
				transientData->CurrentBellyScale += increment;
			}
			if (std::abs(transientData->CurrentBellyScale - transientData->LastAppliedBellyScale) > EPS ||
				(transientData->CurrentBellyScale == 0.0f && transientData->LastAppliedBellyScale != 0.0f)) {
				SetMorph(a_actor, kBelly, transientData->CurrentBellyScale);
				transientData->LastAppliedBellyScale = transientData->CurrentBellyScale;
				needsUpdate = true;
			}
		}

		// Apply all morphs once if any changed
		if (needsUpdate) {
			Racemenu::ApplyMorphs(a_actor);
		}
	}

	// --------------------------------
	// Methods
	// --------
	
	//Main method to alter morphs
	void MorphManager::AlterMorph(Actor* a_actor, Category a_type, Action a_action, float a_value, UpdateKind a_kind, float a_transitionTime) {
		if (!Racemenu::Loaded()) return;
		if (!a_actor) return;
		if (!IsHuman(a_actor)) return;
		if (!a_actor->IsPlayerRef() && !IsTeammate(a_actor)) return;

		auto* persistentData = Persistent::GetActorData(a_actor);
		auto* transientData = Transient::GetActorData(a_actor);
		if (!persistentData || !transientData) return;

		auto LUpdateMorphTarget = [&](float& persistentTarget, float& transientCurrent, float& transitionTime) {

			switch (a_action) {
				case kSet:    persistentTarget = a_value;  break;
				case kModify: persistentTarget += a_value; break;
			}

			if (persistentTarget <= 0) {
				persistentTarget = 0.0f;
			}

			if (a_kind == kInstant) {
				transientCurrent = persistentTarget;
				transitionTime = 0.0f;
			}
			else {
				transitionTime = a_transitionTime;
			}
		};

		switch (a_type) {

			case kBreasts: {
				LUpdateMorphTarget(
					persistentData->fBreastsScale,
					transientData->CurrentBreastsScale,
					transientData->BreastsTransitionTime
				);
			} break;
			case kBelly: {
				LUpdateMorphTarget(
					persistentData->fBellyScale,
					transientData->CurrentBellyScale,
					transientData->BellyTransitionTime
				);
			} break;
			case kAll: {
				LUpdateMorphTarget(
					persistentData->fBreastsScale,
					transientData->CurrentBreastsScale,
					transientData->BreastsTransitionTime
				);
				LUpdateMorphTarget(
					persistentData->fBellyScale,
					transientData->CurrentBellyScale,
					transientData->BellyTransitionTime
				);
			} break;
		}

		if (a_kind == kInstant) {
			UpdateMorphsImediate(a_actor, a_type);
		}
	}

	void MorphManager::UpdateMorphsImediate(Actor* a_actor, Category a_type) {
		if (!Racemenu::Loaded()) return;
		auto* transientData = Transient::GetActorData(a_actor);
		if (!transientData) return;

		float currentScale = 0.0f;
		switch (a_type) {
			case kBreasts: currentScale = transientData->CurrentBreastsScale; break;
			case kBelly:   currentScale = transientData->CurrentBellyScale;   break;
			case kAll: {
				SetMorph(a_actor, kBreasts, transientData->CurrentBreastsScale);
				SetMorph(a_actor, kBelly, transientData->CurrentBellyScale);
				Racemenu::ApplyMorphs(a_actor);
				return;
			}
		}

		SetMorph(a_actor, a_type, currentScale);
		Racemenu::ApplyMorphs(a_actor);
	}

	void MorphManager::ResetMorphs(Actor* a_actor) {

		if (auto* persistentData = Persistent::GetActorData(a_actor)) {
			persistentData->fBreastsScale = 0.0f;
			persistentData->fBellyScale = 0.0f;
		}

		if (auto* transientData = Transient::GetActorData(a_actor)) {
			transientData->CurrentBreastsScale = 0.0f;
			transientData->CurrentBellyScale = 0.0f;
		}

		Racemenu::ClearMorphs(a_actor, GetMorphKey(kBreasts));
		Racemenu::ClearMorphs(a_actor, GetMorphKey(kBelly));
	}

	void MorphManager::HandleCategoryDataChange(Category a_type) {
		if (!Racemenu::Loaded()) return;
		// Clear morphs for the specified category
		if (a_type == kAll) {
			Racemenu::ClearKeyOnAllActors(GetMorphKey(kBreasts));
			Racemenu::ClearKeyOnAllActors(GetMorphKey(kBelly));
		}
		else {
			Racemenu::ClearKeyOnAllActors(GetMorphKey(a_type));
		}

		for (const auto& actor : find_actors()) {
			// Instantly set to whatever current scale is for all the currently loaded actors
			UpdateMorphsImediate(actor, a_type);
		}
	}

	void MorphManager::SetMorph(Actor* a_actor, Category a_type, float a_scale) {
		if (!Racemenu::Loaded()) return;
		GameplayMorphSettings_t* settings = nullptr;

		switch (a_type) {
			case kBreasts: settings = &Config::Gameplay.ActionSettings.MorphListBreasts; break;
			case kBelly:   settings = &Config::Gameplay.ActionSettings.MorphListBelly;   break;
			default: return;
		}

		if (!settings) return;

		const char* morphKey = GetMorphKey(a_type);
		for (size_t i = 0; i < settings->MorphNames.size(); i++) {
			if (!settings->MorphNames[i].empty()) {
				Racemenu::SetMorph(a_actor, settings->MorphNames[i].c_str(), settings->MorphScales[i] * settings->fMultiplier * a_scale, morphKey, false);
			}
		}
	}

	const char* MorphManager::GetMorphKey(Category a_type) {
		const auto name = magic_enum::enum_name(a_type);
		return name.data();
	}

}