#pragma once
#include "Managers/Contact/ContactListener.hpp"

namespace GTS {

	class ContactManager : public EventListener, public CInitSingleton <ContactManager> {
		public:
		virtual std::string DebugName() override;
		virtual void HavokUpdate() override;
		void UpdateCameraContacts();

		private:
		ContactListener listener{};
	};
}
