#pragma once

#include "Managers/Collision/DynamicCollisionUtils.hpp"

namespace GTS {
	
	class DynamicCollisionController {
		
		public:

		DynamicCollisionController(const RE::ActorHandle& a_handle, const bhkCharacterController* a_controller, bool a_hasInSkeletonCollision);
		void Update();
		void MarkDestroyed();

		private:

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
			CharControllerKind controllerKind = CharControllerKind::Unknown;
			float maxSlopeDegrees = 0.0f;
			std::uint32_t maxSlopeRaw = 0;
			float stepHeight = 0.0f;
			float stepReach = 0.0f;
			float proxyMaxSolverSpeed = 0.0f;
			float rigidBodyMaxSlopeCosine = 0.0f;
		};

		// The controller fields this class owns, as they should be right now.
		struct ControllerFields {
			bool writeScale = false;
			float actorHeight = 0.0f;
			float scale = 0.0f;

			bool writeTraversal = false;
			std::uint32_t maxSlopeRaw = 0;      // bhkCharacterController::maxSlope, cos(angle) as float bits
			float stepHeight = 0.0f;
			float stepReach = 0.0f;
			float rigidBodyMaxSlopeCosine = 0.0f;
			float proxyMaxSolverSpeed = 0.0f;
		};

		const bhkCharacterController* m_controller                 = nullptr;   // The controller this instance was built for, compared by identity only
		std::atomic<bool> m_destroyed                              = false;     // Set when that controller is destroyed; an update snapshot may still hold this instance
		mutable const hkpConvexVerticesShape* m_ownedHull          = nullptr;   // Hull we installed last, the only one edited in place
		ActorHandle m_actor;                                                     // Handle to the actor this controller manages
		ShapeData m_originalData                                    = {};        // Original shape data for scaling reference
		NiPointer<bhkShape> m_uniqueShape                           = {};        // A unique copy of the game's collider shape for this actor. Needed to fix actors sharing the same collision shape object.
		absl::flat_hash_map<std::string, NiAVObject*> m_cachedBones = {};        // Cached bone pointers for quick access
		ActorState::ActorState1 m_lastActorState1                   = {};        // Last known actorstate
		float m_lastVisualScale                                     = 0.0f;      // Last known visual scale, Initialized as 0 to force update on first run
		float m_currentVisualScale                                  = 1.0f;      // Current visual scale
		bool m_traversalApplied                                     = false;     // Whether the traversal fields currently hold our values instead of the originals
		mutable hkRefPtr<hkReferencedObject> m_moppRoot              = {};        // Root of m_uniqueShape that m_moppShapes was collected from, held so those pointers stay valid
		mutable std::vector<hkpMoppBvTreeShape*> m_moppShapes        = {};        // MOPPs inside m_moppRoot, recompiled when the capsules under them change
		mutable float m_moppBuiltScale                              = std::numeric_limits<float>::quiet_NaN(); // Scale the capsules had at the last MOPP compile; NaN so the first update always compiles

		void AdjustBoneDrivenHuman() const;
		void AdjustScale() const;
		NiAVObject* FindBone(const std::string_view& a_name) const;
		std::vector<NiAVObject*> FindBones(const std::vector<std::string_view>& a_names) const;
		float GetDistanceBetweenBones(const std::pair<std::string_view, std::string_view>& a_names) const;
		static void ScaleCapsule(const CapsuleData& a_baseCapsule, hkpCapsuleShape* a_outCapsule, float a_scaleFactor);
		void RebuildMopps(bhkCharacterController* a_controller, float a_scale) const;
		ControllerFields GetWantedFields(bool a_writeScale) const;
		bool FieldsMatch(bhkCharacterController* a_controller, const ControllerFields& a_fields) const;
		void WriteFields(bhkCharacterController* a_controller, const ControllerFields& a_fields) const;
	};

}