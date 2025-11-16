#pragma once

#include "RE/B/BSTSingleton.h"
#include <dinput.h>

namespace RE {
	class BSDirectInputManager : public BSTSingletonSDM<BSDirectInputManager> {
	public:
		static BSDirectInputManager*    GetSingleton();

		IDirectInputDevice8A*           CreateDeviceWithGUID(GUID* a_guid);
		void                            GetDeviceState(IDirectInputDevice8A* a_device, std::uint32_t a_size, void* a_outData);
		void                            GetDeviceData(IDirectInputDevice8A* a_device, std::uint32_t* a_dataSize, DIDEVICEOBJECTDATA** a_outData);
		void                            ReleaseDevice(IDirectInputDevice8A* a_device);

		// members
		std::uint32_t             pad00;        // 00
		IDirectInput8A*           diInterface;  // 08
	};
	static_assert(sizeof(BSDirectInputManager) == 0x10);
}