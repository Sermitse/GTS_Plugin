#pragma once

namespace GTS {

	class ColliderData {
		public:
		void DisableCollisions();
		void EnableCollisions();

		void Activate();
		void UpdateCollisionFilter();

		protected:
		virtual std::vector<ColliderData*> GetChildren();
		std::vector<hkpWorldObject*> GetWorldObjects();
		std::vector<hkpRigidBody*> GetRigidBodies();
		std::vector<hkpPhantom*> GetPhantoms();

		void AddRB(hkpRigidBody* rb);
		void AddPhantom(hkpPhantom* phantom);

		private:
		std::unordered_map<hkpRigidBody*, hkRefPtr<hkpRigidBody>> rbs;
		std::unordered_map<hkpPhantom*, hkRefPtr<hkpPhantom>> phantoms;
	};
}
