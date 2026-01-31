#pragma once
#include "Magic/Magic.hpp"

namespace GTS {

	class TinyCalamity : public Magic {

		public:
		using Magic::Magic;
		virtual void OnStart() override;
		virtual void OnUpdate() override;
		virtual void OnFinish() override;
		virtual std::string GetName() override;

		private:
		inline PSString TinyCalamityMessage = 
			"While Tiny Calamity is active, any size-related damage done will shrink your foes, but your max scale is limited."
			"You can perform most size-related actions (Vore, Grab, Hug Crush, etc.) "
			"while being same size with as your foe but performing them wastes some of Tiny Calamity's duration.";

	};
}
