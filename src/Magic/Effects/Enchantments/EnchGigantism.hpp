#pragma once
#include "Magic/Magic.hpp"

namespace GTS {

	class Gigantism : public Magic {
		public:
		using Magic::Magic;
		virtual void OnStart() override;
		virtual void OnFinish() override;
		virtual std::string GetName() override;

		private:
		float magnitude = 0.0f;
		inline PSString AOGGuide = 
			"The Aspect Of Giantess is a powerful enchantment:\n"
		    "It increases maximum size, the power of the shrink spells and the size steal rate.\n"
			"It also affects all size-related damage\n"
			"the minimal shrink threshold (when balance mode is enabled)\n"
			"the rate and trigger chance of random growth, the power of shrink outburst and hit growth,\n"
			"and provides resistance against all hostile shrink sources.";
	};
}
