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

	MagicManager& MagicManager::GetSingleton() noexcept {
		static MagicManager instance;
		return instance;
	}

	void MagicManager::ProcessActiveEffects(Actor* actor) {

		auto effect_list = actor->AsMagicTarget()->GetActiveEffectList();

		if (!effect_list) {
			return;
		}

		for (auto effect: (*effect_list)) {
			this->numberOfEffects += 1;
			if (!this->active_effects.contains(effect)) {
				EffectSetting* base_spell = effect->GetBaseObject();
				auto factorySearch = this->factories.find(base_spell);
				if (factorySearch != this->factories.end()) {
					auto &[key, factory] = (*factorySearch);
					auto magic_effect = factory->MakeNew(effect);
					if (magic_effect) {
						this->active_effects.try_emplace(effect, magic_effect);
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
			this->ProcessActiveEffects(actor);
		}

		for (auto i = this->active_effects.begin(); i != this->active_effects.end();) {
			this->numberOfOurEffects += 1;
			auto& magic = (*i);
			magic.second->poll();
			if (magic.second->IsFinished()) {
				i = this->active_effects.erase(i);
			} else {
				++i;
			}
		}
	}

	void MagicManager::Reset() {
		this->active_effects.clear();
	}

	void MagicManager::DataReady() {

		// Potions
		
		RegisterMagic<SizeHunger>("GTSPotionEffectSizeHunger");

		RegisterMagic<MaxSizePotion>("GTSPotionEffectSizeLimitWeak");
		RegisterMagic<MaxSizePotion>("GTSPotionEffectSizeLimitNormal");
		RegisterMagic<MaxSizePotion>("GTSPotionEffectSizeLimitStrong");
		RegisterMagic<MaxSizePotion>("GTSPotionEffectSizeLimitExtreme");
		RegisterMagic<MaxSizePotion>("GTSAlchEffectSizeLimit");

		RegisterMagic<MightPotion>("GTSPotionEffectMightWeak");
		RegisterMagic<MightPotion>("GTSPotionEffectMightNormal");
		RegisterMagic<MightPotion>("GTSPotionEffectMightStrong");
		RegisterMagic<MightPotion>("GTSPotionEffectMightExtreme");
		RegisterMagic<MightPotion>("GTSAlchEffectMight");

		RegisterMagic<ExperiencePotion>("GTSPotionEffectSizeExperienceBasic");

		RegisterMagic<EssencePotion>("GTSPotionEffectEssenceWeak");
		RegisterMagic<EssencePotion>("GTSPotionEffectEssenceNormal");
		RegisterMagic<EssencePotion>("GTSPotionEffectEssenceStrong");
		RegisterMagic<EssencePotion>("GTSPotionEffectEssenceExtreme");
		RegisterMagic<EssencePotion>("GTSAlchEffectEssence");

		RegisterMagic<ShrinkResistPotion>("GTSPotionEffectResistShrinkWeak");
		RegisterMagic<ShrinkResistPotion>("GTSPotionEffectResistShrinkNormal");
		RegisterMagic<ShrinkResistPotion>("GTSPotionEffectResistShrinkStrong");
		RegisterMagic<ShrinkResistPotion>("GTSPotionEffectResistShrinkExtreme");
		RegisterMagic<ShrinkResistPotion>("GTSAlchEffectResistShrink");

		RegisterMagic<GrowthPotion>("GTSPotionEffectGrowthWeak");
		RegisterMagic<GrowthPotion>("GTSPotionEffectGrowthNormal");
		RegisterMagic<GrowthPotion>("GTSPotionEffectGrowthStrong");
		RegisterMagic<GrowthPotion>("GTSPotionEffectGrowthExtreme");
		RegisterMagic<GrowthPotion>("GTSAlchEffectGrowth");

		RegisterMagic<ShrinkPotion>("GTSPoisonEffectShrinking");
		RegisterMagic<Shrink_Poison>("GTSPotionEffectSizeDrain");

		RegisterMagic<ShrinkFoe>("GTSEffectShrinkEnemy");
		RegisterMagic<ShrinkFoe>("GTSEffectShrinkEnemyAOE");
		RegisterMagic<ShrinkFoe>("GTSEffectShrinkOtherAOEMastery");
		RegisterMagic<ShrinkFoe>("GTSEffectShrinkBolt");
		RegisterMagic<ShrinkFoe>("GTSEffectShrinkStorm");

		RegisterMagic<Gigantism>("GTSEnchGigantism");
		RegisterMagic<SwordOfSize>("GTSEnchSwordAbsorbSize");

		RegisterMagic<SlowGrow>("GTSEffectSlowGrowth");
		RegisterMagic<SlowGrow>("GTSEffectSlowGrowthDual");
		RegisterMagic<TinyCalamity>("GTSEffectTinyCalamity");
		RegisterMagic<Growth>("GTSEffectGrowth");
		RegisterMagic<Growth>("GTSEffectGrowthAdept");
		RegisterMagic<Growth>("GTSEffectGrowthExpert");
		RegisterMagic<Shrink>("GTSEffectShrink");
		RegisterMagic<GrowOther>("GTSEffectGrowAlly");
		RegisterMagic<GrowOther>("GTSEffectGrowAllyAdept");
		RegisterMagic<GrowOther>("GTSEffectGrowAllyExpert");

		RegisterMagic<ShrinkOther>("GTSEffectShrinkAlly");
		RegisterMagic<ShrinkOther>("GTSEffectShrinkAllyAdept");
		RegisterMagic<ShrinkOther>("GTSEffectShrinkAllyExpert");

		RegisterMagic<RestoreSize>("GTSEffectRestoreSize");
		RegisterMagic<RestoreSizeOther>("GTSEffectRestoreSizeOther");

		RegisterMagic<Absorb>("GTSEffectAbsorb");
		RegisterMagic<Absorb>("GTSEffectAbsorbTrue");

		RegisterMagic<GrowthSpurt>("GTSEffectGrowthSpurt1");
		RegisterMagic<GrowthSpurt>("GTSEffectGrowthSpurt2");
		RegisterMagic<GrowthSpurt>("GTSEffectGrowthSpurt3");
	}
}
