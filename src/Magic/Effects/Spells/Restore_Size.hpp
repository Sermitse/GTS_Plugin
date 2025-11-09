#pragma once

#include "Magic/Magic.hpp"

namespace GTS {

	class RestoreSize : public Magic {
		public:
			using Magic::Magic;

			virtual void OnStart() override;

			virtual std::string GetName() override;

		private:
			Timer timer = Timer(2.33); // Run every 2.33s or as soon as we can
	};
}
