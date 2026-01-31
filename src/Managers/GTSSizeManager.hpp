#pragma once

namespace GTS {

	struct SizeManagerData {
		float AspectOfGiantess = 0.0f;

		float SizeHungerBonus = 0.0f;
		float HitGrowth = 0.0f;
		float GrowthTimer = 0.0f;
		float GrowthSpurtSize = 0.0f;

		float NormalDamage = 1.0f; // 0
		float SprintDamage = 1.0f; // 1
		float FallDamage = 1.0f; // 2
		float HHDamage = 1.0f; // 3
		float SizeVulnerability = 0.0f;

		CameraTracking TrackedBone = CameraTracking::None;
		CameraTracking PreviousBone = CameraTracking::None;
		float Camera_HalfLife = 0.05f;
	};

	class SizeManager : public EventListener, public CInitSingleton<SizeManager> {
		public:

		virtual std::string DebugName() override;
		virtual void Reset() override;
		virtual void ResetActor(Actor* actor) override;
		virtual void OnGameLoaded() override;

		SizeManagerData& GetData(Actor* actor);

		void SetEnchantmentBonus(Actor* actor, float amt);
		float GetEnchantmentBonus(Actor* actor);
		void ModEnchantmentBonus(Actor* actor, float amt);

		void SetSizeHungerBonus(Actor* actor, float amt);
		float GetSizeHungerBonus(Actor* actor);
		void ModSizeHungerBonus(Actor* actor, float amt);

		void SetGrowthSpurt(Actor* actor, float amt);
		float GetGrowthSpurt(Actor* actor);
		void ModGrowthSpurt(Actor* actor, float amt);

		static void SetSizeAttribute(Actor* actor, float amt, SizeAttribute attribute);
		static float GetSizeAttribute(Actor* actor, SizeAttribute attribute);

		void SetSizeVulnerability(Actor* actor, float amt);
		float GetSizeVulnerability(Actor* actor);
		void ModSizeVulnerability(Actor* actor, float amt);

		CameraTracking GetPreviousBone(Actor* actor);
		void SetPreviousBone(Actor* actor);
	
		void SetTrackedBone(Actor* actor, bool enable, CameraTracking Bone);
		CameraTracking GetTrackedBone(Actor* actor);

		void SetCameraHalflife(Actor* actor, CameraTracking Bone);
		float GetCameraHalflife(Actor* actor);

		static bool BalancedMode();

		private: 
		std::unordered_map<Actor*, SizeManagerData> sizeData;
	};
}
