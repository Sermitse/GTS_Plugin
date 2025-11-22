#pragma once

namespace GTS {

	class ModEventManager : public CInitSingleton <ModEventManager> {
		public:
		using OnFootstep = RegistrationSet<Actor*, std::string>;
		OnFootstep m_onfootstep;
		ModEventManager();

	};
}
