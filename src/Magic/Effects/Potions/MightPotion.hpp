#pragma once
#include "Magic/Magic.hpp"

namespace GTS {

	class MightPotion : public Magic {

		public:
		using Magic::Magic;
		virtual void OnStart() override;
		virtual void OnFinish() override;
		virtual std::string GetName() override;
		MightPotion(ActiveEffect* effect);

		private:
		float Power = 0.0f;
	};
}