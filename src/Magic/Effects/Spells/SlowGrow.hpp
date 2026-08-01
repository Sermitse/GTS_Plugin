#pragma once
#include "Magic/Magic.hpp"

namespace GTS {

	class SlowGrow : public Magic {

		public:
		using Magic::Magic;
		SlowGrow(ActiveEffect* effect);
		virtual void OnStart() override;
		virtual void OnFinish() override;
		virtual std::string GetName() override;

		private:
		bool IsDual = false;
	};
}
