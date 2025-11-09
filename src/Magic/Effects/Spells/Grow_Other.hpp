#pragma once

#include "Magic/Magic.hpp"

namespace GTS {

	class GrowOther : public Magic {

		public:
			using Magic::Magic;

			virtual void OnUpdate() override;

			virtual std::string GetName() override;

	};
}
