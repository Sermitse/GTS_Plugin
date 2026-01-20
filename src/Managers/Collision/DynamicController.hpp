#pragma once

namespace GTS {
	
	class DynamicController {
		
		public:
		DynamicController(const RE::ActorHandle& a_handle);
		void Update();
		void AdjustPlayer();
		void AdjustNPC() const;

		struct CapsuleData {
			float m_radius;
			hkVector4 m_pointA;
			hkVector4 m_pointB;
			bool m_isBumper = false;
		};

		struct ShapeData {
			std::vector<hkVector4> m_convexVerteces = {};
			std::vector<CapsuleData> m_capsules = {};
			float m_convexRadius = 0.0f;
			float m_originalControllerHeight = 0.0f;
			float m_originalControllerScale = 0.0f;
			uint32_t m_originalmaxSlope;
			bool m_HasVertecesShape = false;
		};


		private:


		ActorHandle m_actor;

		ShapeData m_originalData = {};
		NiPointer<bhkShape> m_clonedShape = {};
		absl::flat_hash_map<std::string, NiAVObject*> m_cachedBones = {};
		float m_lastVisualScale = 0.0f;
		float m_currentVisualScale = 1.0f;
		ActorState::ActorState1 m_lastActorState1 = {};
	};

}