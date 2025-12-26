#pragma once

namespace Hooks {

	class Hook_Actor {
		public:
		static void Install();
		private:
		static void PatchFollowMoveSpeed();
	};
}
