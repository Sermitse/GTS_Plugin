#pragma once
#include "Magic/Magic.hpp"

namespace GTS {

	class RestoreSizeOther : public Magic {

		public:
		using Magic::Magic;
		virtual void OnStart() override;
		virtual std::string GetName() override;
	};
}
