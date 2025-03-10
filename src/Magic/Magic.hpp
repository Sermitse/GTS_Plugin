#pragma once
// Module that handles footsteps

namespace GTS {

	template <class T>
	T* find_form(std::string_view lookup_id) {
		// From https://github.com/Exit-9B/MCM-Helper/blob/a39b292909923a75dbe79dc02eeda161763b312e/src/FormUtil.cpp
		std::string lookup_id_str(lookup_id);
		std::istringstream ss{ lookup_id_str };
		std::string plugin, id;

		std::getline(ss, plugin, '|');
		std::getline(ss, id);
		RE::FormID relativeID;
		std::istringstream{ id } >> std::hex >> relativeID;
		const auto dataHandler = RE::TESDataHandler::GetSingleton();
		return dataHandler ? dataHandler->LookupForm<T>(relativeID, plugin) : nullptr;
	}

	class Magic {
		public:
			virtual void OnStart();
			virtual void OnUpdate();
			virtual void OnFinish();

			virtual std::string GetName();

			void poll();

			[[nodiscard]] Actor* GetTarget() const;
			[[nodiscard]] Actor* GetCaster() const;

			[[nodiscard]] ActiveEffect* GetActiveEffect() const;
			[[nodiscard]] EffectSetting* GetBaseEffect() const;

			void Dispel() const;
			[[nodiscard]] bool IsDualCasting() const;

			[[nodiscard]] inline bool DualCasted() const {
				return this->dual_casted;
			}

			[[nodiscard]] bool HasDuration() const;

			Magic(ActiveEffect* effect);

			[[nodiscard]] inline bool IsFinished() const {
				return this->state == State::CleanUp;
			}

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
		} else {
			return nullptr;
		}
	}

	class MagicManager : public EventListener {
		public:


			[[nodiscard]] static MagicManager& GetSingleton() noexcept;

			virtual std::string DebugName() override;
			virtual void Update() override;
			virtual void Reset() override;
			virtual void DataReady() override;

			void ProcessActiveEffects(Actor* actor);

			template<class MagicCls>
			void RegisterMagic(std::string_view tag) {
				auto magic = Runtime::GetMagicEffect(tag);
				if (magic) {
					this->factories.try_emplace(magic,new MagicFactory<MagicCls>());
				}
			}

			void PrintReport();
		private:
			std::map<ActiveEffect*, std::unique_ptr<Magic> > active_effects;
			std::unordered_map<EffectSetting*, std::unique_ptr<MagicFactoryBase> > factories;

			std::uint64_t numberOfEffects = 0;
			std::uint64_t numberOfOurEffects = 0;
	};

	template<class MagicCls>
	void RegisterMagic(std::string_view tag) {
		MagicManager::GetSingleton().RegisterMagic(tag);
	}
}
