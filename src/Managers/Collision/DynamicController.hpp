#pragma once

namespace GTS {
	
	class DynamicController {
		
		public:

		DynamicController(const RE::ActorHandle& a_handle);
		void Update();

		struct CapsuleData {
			float radius;
			hkVector4 vertexA;
			hkVector4 vertexB;
			bool isBumper = false;
		};

		struct ShapeData {
			std::vector<hkVector4> convexVerteces = {};
			std::vector<CapsuleData> capsules = {};
			float convexShapeRadius = 0.0f;
			float controllerActorHeight = 0.0f;
			float controllerActorScale = 0.0f;
			bool hasVertecesShape = false;
		};

		private:
		ActorHandle m_actor;                                                // Handle to the actor this controller manages
		ShapeData m_originalData                                    = {};   // Original shape data for scaling reference
		NiPointer<bhkShape> m_uniqueShape                           = {};   // A unique copy of the game's collider shape for this actor. Needed to fix actors sharing the same collision shape object.
		absl::flat_hash_map<std::string, NiAVObject*> m_cachedBones = {};   // Cached bone pointers for quick access
		ActorState::ActorState1 m_lastActorState1                   = {};   // Last known actorstate
		float m_lastVisualScale                                     = 0.0f; // Last known visual scale, Initialized as 0 to force update on first run
		float m_currentVisualScale                                  = 1.0f; // Current visual scale

		void AdjustBoneDriven();
		void AdjustScale() const;

	};

}