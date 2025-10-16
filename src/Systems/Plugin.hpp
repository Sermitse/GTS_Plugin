#pragma once

namespace GTS {

	class Plugin {

		public:
		static bool IsInRaceMenu();
		static bool Enabled();
		static bool InGame();
		static bool Ready();
		static bool Live();
		static bool IsInBlockingMenu();
		static void SetInGame(bool value);
		static bool OnMainThread();
		static void SetOnMainThread(bool value);

		private:
		static inline std::atomic_bool m_enabled = std::atomic_bool(true);
		static inline std::atomic_bool m_inGame = std::atomic_bool(false);
		static inline std::atomic_bool m_onMainThread = std::atomic_bool(false);
	};
}
