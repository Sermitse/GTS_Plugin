#pragma once

namespace ImGuiEx {

	void Bitfield32(const char* a_label, uint32_t* a_bitfield);
	void Bitfield64(const char* a_label, uint64_t* a_bitfield);
	bool CheckBox(const char* a_label, bool* a_state, const char* a_Tooltip = nullptr, bool a_disabled = false);

}