#pragma once

#include "Magic/Magic.hpp"

// Module that handles slow growth

namespace GTS {
	class SlowGrow : public Magic {
		public:
			using Magic::Magic;

			SlowGrow(ActiveEffect* effect);
			
			virtual void OnStart() override;

			virtual void OnUpdate() override;

			virtual void OnFinish() override;

			void Task_SlowGrowTask(Actor* caster);
			virtual std::string GetName() override;
		private:
			bool IsDual = false;
			Timer timer = Timer(2.33); // Run every 2.33s or as soon as we can
			Timer MoanTimer = Timer(6.00);

	};
}
