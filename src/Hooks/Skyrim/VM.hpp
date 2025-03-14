#pragma once

namespace Hooks {
	
	class Hook_VM {
		public:
			static void Hook();
		private:
			static void SendEvent(VirtualMachine* a_this, VMHandle a_handle, const BSFixedString& a_eventName, IFunctionArguments* a_args);
			static inline REL::Relocation<decltype(SendEvent)> _SendEvent;
	};
}
