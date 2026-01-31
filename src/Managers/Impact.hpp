#pragma once

namespace GTS {

	class ImpactManager {
		public:
		static void HookProcessEvent(BGSImpactManager* impact, const BGSFootstepEvent* a_event, BSTEventSource<BGSFootstepEvent>* a_eventSource);
	};
}
