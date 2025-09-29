#include "RE/M/MenuCursor.hpp"

namespace RE {

	MenuCursor* MenuCursor::GetSingleton() {
		REL::Relocation<MenuCursor**> singleton{ REL::RelocationID(517043, 403551) };
		return *singleton;
	}
}