#pragma once

// Critically Damped Springs
namespace GTS {

	class SpringBase {
		public:
		virtual void Update(float delta) = 0;

		protected:
		static void UpdateValues(float& value, const float& target, float & velocity, const float& halflife, const float& dt);
	};

	class Spring : public SpringBase {
		public:
		float value = 0.0f;
		float target = 0.0f;
		float velocity = 0.0f;
		float halflife = 1.0f;

		void Update(float delta) override;

		Spring();
		Spring(float initial, float halflife);

		~Spring();
	};

	class Spring3 : public SpringBase {
		public:
		NiPoint3 value = NiPoint3(0.0f, 0.0f, 0.0f);
		NiPoint3 target = NiPoint3(0.0f, 0.0f, 0.0f);
		NiPoint3 velocity = NiPoint3(0.0f, 0.0f, 0.0f);
		float halflife = 1.0f;

		void Update(float delta) override;

		Spring3();
		Spring3(NiPoint3 initial, float halflife);

		~Spring3();
	};

	class SpringHolder : public EventListener, public CInitSingleton<SpringHolder> {
		public:
		static void AddSpring(SpringBase* spring);
		static void RemoveSpring(SpringBase* spring);

		virtual std::string DebugName() override;
		virtual void Update() override;

		static inline std::unordered_set<SpringBase*> springs;
	};
}
