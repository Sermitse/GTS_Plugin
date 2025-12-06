#pragma once

namespace GTS {

	class ExplosionManager : public EventListener, public CInitSingleton <ExplosionManager> {
		public:
		virtual std::string DebugName() override;
		virtual void OnImpact(const Impact& impact) override;
	};

}
