#pragma once

namespace GTS {

	class ModEventManager : public CInitSingleton <ModEventManager> {
		public:
		using OnFootstep = SKSE::RegistrationSet<Actor*, std::string>;
		OnFootstep m_onfootstep;
		ModEventManager();

	};
}
