#include "Magic/Magic.hpp"

#include "Magic/Effects/Spells/Absorb_Effect.hpp"
#include "Magic/Effects/Spells/GrowthSpurt.hpp"
#include "Magic/Effects/Spells/Grow_Other.hpp"
#include "Magic/Effects/Spells/Growth.hpp"
#include "Magic/Effects/Spells/Shrink.hpp"
#include "Magic/Effects/Spells/Restore_Size.hpp"
#include "Magic/Effects/Spells/Restore_Size_Other.hpp"
#include "Magic/Effects/Spells/Shrink_Foe.hpp"
#include "Magic/Effects/Spells/Shrink_Other.hpp"
#include "Magic/Effects/Spells/Slow_Grow.hpp"

#include "Magic/Effects/Shouts/TinyCalamity.hpp"

#include "Magic/Effects/Enchantments/EnchGigantism.hpp"
#include "Magic/Effects/Enchantments/Sword_Of_Size.hpp"

#include "Magic/Effects/Potions/MightPotion.hpp"
#include "Magic/Effects/Potions/GrowthPotion.hpp"
#include "Magic/Effects/Potions/MaxSizePotion.hpp"
#include "Magic/Effects/Potions/SizeHunger.hpp"
#include "Magic/Effects/Potions/EssencePotion.hpp"
#include "Magic/Effects/Potions/ExperiencePotion.hpp"
#include "Magic/Effects/Poisons/Poison_Of_Shrinking.hpp"
#include "Magic/Effects/Potions/ShrinkResistPotion.hpp"
#include "Magic/Effects/Potions/ShrinkPotion.hpp"


namespace GTS {

	void Magic::OnStart() {}

	void Magic::OnUpdate() {}

	void Magic::OnFinish() {}

	std::string Magic::GetName() {
		return "";
	}

	Magic::Magic(ActiveEffect* effect) : activeEffect(effect) {
		if (this->activeEffect) {
			auto spell = this->activeEffect->spell;
			this->effectSetting = this->activeEffect->GetBaseObject();
			MagicTarget* m_target = this->activeEffect->target;
			if (m_target) {
				if (m_target->MagicTargetIsActor()) {
					this->target = skyrim_cast<Actor*>(m_target);
				}
			}
			if (this->activeEffect->caster) {
				this->caster = this->activeEffect->caster.get().get();
			}
			this->hasDuration = this->HasDuration();
		}
	}

	bool Magic::HasDuration() const {
		if (this->activeEffect) {
			auto spell = this->activeEffect->spell;
			if (spell) {
				// switch (this->activeEffect->spell->GetSpellType()) {
				// 	case  MagicSystem::SpellType::kEnchantment: {
				// 		return false;
				// 	}
				// }
				switch (spell->GetCastingType()) {
					case  MagicSystem::CastingType::kConstantEffect: {
						return false;
					}
				}
			}
		}
		auto effectSetting = this->effectSetting;
		if (effectSetting) {
			if (effectSetting->data.flags.all(EffectSetting::EffectSettingData::Flag::kNoDuration)) {
				return false;
			}
		}
		return true;
	}

	void Magic::poll() {

		switch (this->state) {
			case State::Init:
			{
				this->dual_casted = this->IsDualCasting();

				this->state = State::Start;
				break;
			}
			case State::Start: {
				this->OnStart();
				this->state = State::Update;
				break;
			}
			case State::Update: {
				if (this->activeEffect->flags & ActiveEffect::Flag::kInactive) {
					break;
				}
				this->OnUpdate();
				if ((this->activeEffect->flags & ActiveEffect::Flag::kDispelled)
				    || (this->hasDuration && (this->activeEffect->elapsedSeconds >= this->activeEffect->duration))) {
					this->state = State::Finish;
				}
				break;
			}
			case State::Finish: {
				this->OnFinish();
				this->state = State::CleanUp;
				break;
			}
			case State::CleanUp: {
				break;
			}
		}
	}

	Actor* Magic::GetTarget() const {
		return this->target;
	}

	Actor* Magic::GetCaster() const {
		return this->caster;
	}

	ActiveEffect* Magic::GetActiveEffect() const {
		return this->activeEffect;
	}

	EffectSetting* Magic::GetBaseEffect() const {
		return this->effectSetting;
	}

	void Magic::Dispel() const {
		if (this->activeEffect) {
			this->activeEffect->Dispel(false); // Not forced
			// Seems to be CTD prone for some reason, best to not use it
		}
	}

	bool Magic::IsDualCasting() const {
		if (this->caster) {
			auto casting_type = GetActiveEffect()->castingSource;
			if (casting_type == MagicSystem::CastingSource::kLeftHand || casting_type == MagicSystem::CastingSource::kRightHand) {
				auto source = this->caster->GetMagicCaster(casting_type);
				if (source) {
					return source->GetIsDualCasting();
				}
			}
		}
		return false;
	}

	void MagicManager::ProcessActiveEffects(Actor* a_actor) {

		auto effect_list = a_actor->AsMagicTarget()->GetActiveEffectList();

		if (!effect_list) {
			return;
		}

		for (auto effect: (*effect_list)) {
			numberOfEffects += 1;
			if (!active_effects.contains(effect)) {
				EffectSetting* base_spell = effect->GetBaseObject();
				auto factorySearch = factories.find(base_spell);
				if (factorySearch != factories.end()) {
					auto &[key, factory] = (*factorySearch);
					auto magic_effect = factory->MakeNew(effect);
					if (magic_effect) {
						active_effects.try_emplace(effect, magic_effect);
					}
				}
			}
		}
	}

	std::string MagicManager::DebugName() {
		return "::MagicManager";
	}

	void MagicManager::Update() {

		for (auto actor: find_actors()) {
			ProcessActiveEffects(actor);
		}

		for (auto i = active_effects.begin(); i != this->active_effects.end();) {
			numberOfOurEffects += 1;
			auto& magic = (*i);
			magic.second->poll();
			if (magic.second->IsFinished()) {
				i = active_effects.erase(i);
			} 
			else {
				++i;
			}
		}
	}

	void MagicManager::Reset() {
		this->active_effects.clear();
	}

	void MagicManager::DataReady() {

		// -------- Potions
		RegisterMagic<MaxSizePotion>(Runtime::MGEF.GTSPotionEffectSizeLimitWeak);
		RegisterMagic<MaxSizePotion>(Runtime::MGEF.GTSPotionEffectSizeLimitNormal);
		RegisterMagic<MaxSizePotion>(Runtime::MGEF.GTSPotionEffectSizeLimitStrong);
		RegisterMagic<MaxSizePotion>(Runtime::MGEF.GTSPotionEffectSizeLimitExtreme);
		RegisterMagic<MaxSizePotion>(Runtime::MGEF.GTSAlchEffectSizeLimit);

		RegisterMagic<MightPotion>(Runtime::MGEF.GTSPotionEffectMightWeak);
		RegisterMagic<MightPotion>(Runtime::MGEF.GTSPotionEffectMightNormal);
		RegisterMagic<MightPotion>(Runtime::MGEF.GTSPotionEffectMightStrong);
		RegisterMagic<MightPotion>(Runtime::MGEF.GTSPotionEffectMightExtreme);
		RegisterMagic<MightPotion>(Runtime::MGEF.GTSAlchEffectMight);

		RegisterMagic<EssencePotion>(Runtime::MGEF.GTSPotionEffectEssenceWeak);
		RegisterMagic<EssencePotion>(Runtime::MGEF.GTSPotionEffectEssenceNormal);
		RegisterMagic<EssencePotion>(Runtime::MGEF.GTSPotionEffectEssenceStrong);
		RegisterMagic<EssencePotion>(Runtime::MGEF.GTSPotionEffectEssenceExtreme);
		RegisterMagic<EssencePotion>(Runtime::MGEF.GTSAlchEffectEssence);

		RegisterMagic<ShrinkResistPotion>(Runtime::MGEF.GTSPotionEffectResistShrinkWeak);
		RegisterMagic<ShrinkResistPotion>(Runtime::MGEF.GTSPotionEffectResistShrinkNormal);
		RegisterMagic<ShrinkResistPotion>(Runtime::MGEF.GTSPotionEffectResistShrinkStrong);
		RegisterMagic<ShrinkResistPotion>(Runtime::MGEF.GTSPotionEffectResistShrinkExtreme);
		RegisterMagic<ShrinkResistPotion>(Runtime::MGEF.GTSAlchEffectResistShrink);

		RegisterMagic<GrowthPotion>(Runtime::MGEF.GTSPotionEffectGrowthWeak);
		RegisterMagic<GrowthPotion>(Runtime::MGEF.GTSPotionEffectGrowthNormal);
		RegisterMagic<GrowthPotion>(Runtime::MGEF.GTSPotionEffectGrowthStrong);
		RegisterMagic<GrowthPotion>(Runtime::MGEF.GTSPotionEffectGrowthExtreme);
		RegisterMagic<GrowthPotion>(Runtime::MGEF.GTSAlchEffectGrowth);

		RegisterMagic<ShrinkPotion>(Runtime::MGEF.GTSPoisonEffectShrinking);
		RegisterMagic<Shrink_Poison>(Runtime::MGEF.GTSPotionEffectSizeDrain);

		RegisterMagic<ExperiencePotion>(Runtime::MGEF.GTSPotionEffectSizeExperienceBasic);
		RegisterMagic<SizeHunger>(Runtime::MGEF.GTSPotionEffectSizeHunger);

		// -------- Enchantments
		RegisterMagic<Gigantism>(Runtime::MGEF.GTSEnchGigantism);
		RegisterMagic<SwordOfSize>(Runtime::MGEF.GTSEnchSwordAbsorbSize);

		// -------- Shouts
		RegisterMagic<TinyCalamity>(Runtime::MGEF.GTSEffectTinyCalamity);

		RegisterMagic<GrowthSpurt>(Runtime::MGEF.GTSEffectGrowthSpurt1);
		RegisterMagic<GrowthSpurt>(Runtime::MGEF.GTSEffectGrowthSpurt2);
		RegisterMagic<GrowthSpurt>(Runtime::MGEF.GTSEffectGrowthSpurt3);

		RegisterMagic<Absorb>(Runtime::MGEF.GTSEffectAbsorb);
		RegisterMagic<Absorb>(Runtime::MGEF.GTSEffectAbsorbTrue);

		// -------- Spells
		RegisterMagic<ShrinkFoe>(Runtime::MGEF.GTSEffectShrinkEnemy);
		RegisterMagic<ShrinkFoe>(Runtime::MGEF.GTSEffectShrinkEnemyAOE);
		RegisterMagic<ShrinkFoe>(Runtime::MGEF.GTSEffectShrinkOtherAOEMastery);
		RegisterMagic<ShrinkFoe>(Runtime::MGEF.GTSEffectShrinkBolt);
		RegisterMagic<ShrinkFoe>(Runtime::MGEF.GTSEffectShrinkStorm);

		RegisterMagic<SlowGrow>(Runtime::MGEF.GTSEffectSlowGrowth);
		RegisterMagic<SlowGrow>(Runtime::MGEF.GTSEffectSlowGrowthDual);
	
		RegisterMagic<Growth>(Runtime::MGEF.GTSEffectGrowth);
		RegisterMagic<Growth>(Runtime::MGEF.GTSEffectGrowthAdept);
		RegisterMagic<Growth>(Runtime::MGEF.GTSEffectGrowthExpert);

		RegisterMagic<GrowOther>(Runtime::MGEF.GTSEffectGrowAlly);
		RegisterMagic<GrowOther>(Runtime::MGEF.GTSEffectGrowAllyAdept);
		RegisterMagic<GrowOther>(Runtime::MGEF.GTSEffectGrowAllyExpert);

		RegisterMagic<ShrinkOther>(Runtime::MGEF.GTSEffectShrinkAlly);
		RegisterMagic<ShrinkOther>(Runtime::MGEF.GTSEffectShrinkAllyAdept);
		RegisterMagic<ShrinkOther>(Runtime::MGEF.GTSEffectShrinkAllyExpert);

		RegisterMagic<RestoreSize>(Runtime::MGEF.GTSEffectRestoreSize);
		RegisterMagic<RestoreSizeOther>(Runtime::MGEF.GTSEffectRestoreSizeOther);

		RegisterMagic<Shrink>(Runtime::MGEF.GTSEffectShrink);
	}
}
