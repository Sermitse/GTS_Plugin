#pragma once

#include "Magic/Magic.hpp"

// Module that tracks Gigantism MGEF

namespace GTS {

	class Gigantism : public Magic {
		public:
			using Magic::Magic;

			virtual void OnStart() override;

			virtual void OnFinish() override;

			virtual std::string GetName() override;

		private:
			float magnitude = 0.0f;

			const char* const AOGGuide = "Aspect Of Giantess is a powerful enchantment: it increases your Maximal Size, affects power of the Shrink Spells and Size Steal Rate. It affects your size-related damage, minimal shrink threshold from quest/balance mode, growth rate and growth chance from Random Growth, power of Shrink Outburst and Hit Growth, as well as provides resistance against hostile shrink sources.";
	};
}
