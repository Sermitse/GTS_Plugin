#pragma once

namespace GTS {

	class TrainWreck final : public EventListener, public CInitSingleton< TrainWreck> {
		public:
		static void Install();
		std::string DebugName() override;
		void OnPluginPostLoad() override;
	};

}