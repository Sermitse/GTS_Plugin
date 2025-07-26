#pragma once


namespace Hooks {

	class Hook_Havok {
		public:
		static void Install();

		private:
		enum class CollisionFilterComparisonResult : uint8_t {
			Continue,  // Do not affect whether the two objects should collide
			Collide,   // Force the two objects to collide
			Ignore,    // Force the two objects to not collide
		};
	};
}
