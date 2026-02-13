#pragma once
// Module that handles footsteps

namespace GTS {

	class Magic {
		public:
		virtual void OnStart();
		virtual void OnUpdate();
		virtual void OnFinish();
		virtual std::string GetName();

		[[nodiscard]] Actor* GetTarget() const;
		[[nodiscard]] Actor* GetCaster() const;
		[[nodiscard]] ActiveEffect* GetActiveEffect() const;
		[[nodiscard]] EffectSetting* GetBaseEffect() const;
		[[nodiscard]] bool IsDualCasting() const;
		[[nodiscard]] bool DualCasted() const;
		[[nodiscard]] bool HasDuration() const;
		[[nodiscard]] bool IsFinished() const;

		void Poll();
		void Dispel() const;

		Magic(ActiveEffect* effect);



		private:

		enum State {
			Init,
			Start,
			Update,
			Finish,
			CleanUp,
		};

		State state = State::Init;
		Actor* target = nullptr;
		Actor* caster = nullptr;
		ActiveEffect* activeEffect = nullptr;
		EffectSetting* effectSetting = nullptr;
		bool dual_casted = false;
		bool hasDuration = false;
	};

	class MagicFactoryBase {
		public:
		virtual Magic* MakeNew(ActiveEffect* effect) const = 0;
		virtual ~MagicFactoryBase() = default;
	};

	template<class MagicCls>
	class MagicFactory : public MagicFactoryBase {
		public:
		virtual Magic* MakeNew(ActiveEffect* effect)  const override;
	};

	template<class MagicCls>
	Magic* MagicFactory<MagicCls>::MakeNew(ActiveEffect* effect) const {
		if (effect) {
			return new MagicCls(effect);
		}
		return nullptr;
	}

	class MagicManager : public EventListener, public CInitSingleton<MagicManager> {
		
		public:
		virtual std::string DebugName() override;
		virtual void Update() override;
		virtual void Reset() override;
		virtual void DataReady() override;

		static void ProcessActiveEffects(Actor* a_actor);

		template<class MagicCls>
		static void RegisterMagic(const std::string_view& a_tag) {
			if (auto magic = Runtime::GetMagicEffect(a_tag)) {
				factories.try_emplace(magic,new MagicFactory<MagicCls>());
			}
		}

		template<class MagicCls>
		static void RegisterMagic(const RuntimeData::RuntimeEntry<RE::EffectSetting>& a_entry) {
			if (auto magic = Runtime::GetMagicEffect(a_entry)) {
				factories.try_emplace(magic, new MagicFactory<MagicCls>());
			}
		}	

		private:
		static inline std::unordered_map<ActiveEffect*, std::unique_ptr<Magic>> active_effects;
		static inline std::unordered_map<EffectSetting*, std::unique_ptr<MagicFactoryBase>> factories;
		static inline std::uint64_t numberOfEffects = 0;
		static inline std::uint64_t numberOfOurEffects = 0;
	};

}
