#pragma once

namespace GTS {

	enum class RumbleState {
		RampingUp,
		Rumbling,
		RampingDown,
		Still,
	};

	class RumbleData {
	public:
		RumbleData(float intensity, float duration, float halflife, float shake_duration, bool ignore_scaling, std::string_view node);

		void ChangeTargetIntensity(float intensity);
		void ChangeDuration(float duration);

		RumbleState state;

		float duration;
		float shake_duration;
		bool ignore_scaling;
		Spring currentIntensity;
		std::string node;
		double startTime;
	};


	class ActorRumbleData {
	public:
		ActorRumbleData(Actor* giant);

		void Update();
		void Start(std::string_view tag, float intensity, float duration, float halflife, float shake_duration, bool ignore_scaling, std::string_view node);
		void Stop(std::string_view tag);
		bool Empty() const;

		ActorHandle giant;
		Timer delay;
		std::unordered_map<std::string, RumbleData> tags;
	};


	class Rumbling : public EventListener, public CInitSingleton<Rumbling> {

	public:

		virtual std::string DebugName() override;

		virtual void Reset() override;
		virtual void ResetActor(Actor* actor) override;
		virtual void Update() override;

		ActorRumbleData& GetRumbleData(Actor* giant);

		static void Start(std::string_view tag, Actor* giant, float intensity, float halflife, std::string_view node);
		static void Start(std::string_view tag, Actor* giant, float intensity, float halflife);
		static void Stop(std::string_view tag, Actor* giant);
		static void For(std::string_view tag, Actor* giant, float intensity, float halflife, std::string_view node, float duration, float shake_duration, bool ignore_scaling = false);
		static void Once(std::string_view tag, Actor* giant, float intensity, float halflife, std::string_view node, float shake_duration, bool ignore_scaling = false);
		static void Once(std::string_view tag, Actor* giant, float intensity, float halflife, bool ignore_scaling = false);

	private:

		mutable std::mutex _lock;
		std::unordered_map<FormID, ActorRumbleData> data;
	};


	void ApplyShake(Actor* caster, float modifier, float radius);
	void ApplyShakeAtNode(Actor* caster, float modifier, std::string_view node, bool ignore_scaling = false);
	void ApplyShakeAtPoint(Actor* caster, float modifier, const NiPoint3& coords, float duration_override, bool ignore_scaling = false);
}