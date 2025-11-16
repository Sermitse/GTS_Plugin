#pragma once
#include "Magic/Magic.hpp"

namespace GTS {

	class SizeHunger : public Magic {

		public:
		using Magic::Magic;
		virtual void OnStart() override;
		virtual void OnFinish() override;
		virtual std::string GetName() override;
	};
}
