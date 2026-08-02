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


	// Identifies one independent, simultaneously-active screen-shake pulse:
	// the actor driving it plus an optional channel to distinguish several
	// pulses from the same actor (e.g. one per rumbling node - a body loop
	// vs. a foot impact). See Rumbling::QueueScreenShake.
	struct ShakeChannelKey {
		FormID source;
		const void* channel;

		bool operator==(const ShakeChannelKey& other) const noexcept {
			return source == other.source && channel == other.channel;
		}
	};

	struct ShakeChannelKeyHash {
		size_t operator()(const ShakeChannelKey& key) const noexcept {
			size_t h1 = std::hash<FormID>{}(key.source);
			size_t h2 = std::hash<const void*>{}(key.channel);
			// standard boost::hash_combine-style mix
			return h1 ^ (h2 + 0x9e3779b9u + (h1 << 6) + (h1 >> 2));
		}
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

		// Queues a raw camera/controller shake pulse to be combined with every
		// other pulse still active this frame, instead of poking the native
		// shake_controller/shake_camera_at_node functions directly. Those two
		// just SET the current shake magnitude - they don't add on top of
		// whatever is already running - so calling them from multiple
		// independent sources means whichever call happens most recently
		// always wins, regardless of intensity. Routing every source through
		// this queue and combining them once per tick (see UpdateScreenShake)
		// fixes that.
		//
		// `source` identifies the actor driving this pulse (its FormID).
		// `channel` further distinguishes independent, simultaneously-active
		// pulses from the SAME actor - e.g. a continuous body-rumble loop and
		// a brief foot-impact are different channels; without this they'd
		// collide on the same slot and one would overwrite the other's
		// position/duration instead of the two summing independently. Pass
		// nullptr when there's only ever one pulse in flight for that actor.
		// Different (source, channel) pairs still sum normally in
		// UpdateScreenShake. Insert/update/erase for a given (source,
		// channel) pair are O(1) amortized (see _screenShakeQueue below) -
		// safe to call once per rumbling node per actor per tick even with
		// many simultaneously-shaking actors on screen.
		static void QueueScreenShake(FormID source, const void* channel, float intensity, float duration);

	private:

		void UpdateScreenShake();

		struct ShakeRequest {
			float intensity;
			double startTime;
			float duration;
		};
		mutable std::mutex _shakeLock;
		// Keyed by (actor, channel) instead of scanned linearly by a vector -
		// requeuing an already-active channel (the common case: any source
		// that re-fires every tick, like ActorRumbleData::Update) is an O(1)
		// amortized lookup+update instead of an O(n) scan over every other
		// currently-active pulse from every actor.
		std::unordered_map<ShakeChannelKey, ShakeRequest, ShakeChannelKeyHash> _screenShakeQueue;
		mutable std::mutex _lock;
		std::unordered_map<FormID, ActorRumbleData> data;
	};


	void ApplyShake(Actor* caster, float power, float radius);
	void ApplyShakeAtNode(Actor* caster, float power, std::string_view node, bool ignore_scaling = false);
	// `channel` lets callers that already know they're driving several
	// independent, simultaneous pulses for the same caster (e.g.
	// ActorRumbleData::Update, one call per rumbling node) keep those pulses
	// from colliding in the screen-shake queue - see Rumbling::QueueScreenShake.
	// Leave it nullptr for simple one-shot callers.
	void ApplyShakeAtPoint(Actor* caster, float power, const NiPoint3& coords, float duration_override, bool ignore_scaling = false, const void* channel = nullptr);
}