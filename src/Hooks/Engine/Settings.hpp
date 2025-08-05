#pragma once


namespace Hooks {

	/*
	 * Contains RE'd offsets for the DAT values in skyrims ini settings.
	 */

	namespace LOD {

		//SE OFFSET: 141dea048
		static float* fLodDistance = reinterpret_cast<float*>(REL::RelocationID(507568, 378623, NULL).address());
		
		//SE OFFSET: 141dea060
		static float* fTalkingDistance = reinterpret_cast<float*>(REL::RelocationID(507570, 378626, NULL).address());
	};

	namespace Distance {

		static float* fActivatePickLength = reinterpret_cast<float*>(REL::RelocationID(502527, 370109, NULL).address());

		static float* fActivatePickRadius = reinterpret_cast<float*>(REL::RelocationID(502525, 370106, NULL).address());

	}

	namespace Camera {

		//SE OFFSET: 141df37d8
		static float* fVanityModeMaxDist = reinterpret_cast<float*>(REL::RelocationID(509878, 382627, NULL).address());

		//SE OFFSET: 141df37a8
		static float* fVanityModeMinDist = reinterpret_cast<float*>(REL::RelocationID(509874, 382621, NULL).address());

		//SE OFFSET: 141df3868
		static float* fMouseWheelZoomSpeed = reinterpret_cast<float*>(REL::RelocationID(509890, 382645, NULL).address());

		//SE OFFSET: 141df3850
		static float* fMouseWheelZoomIncrement = reinterpret_cast<float*>(REL::RelocationID(509888, 382642, NULL).address());
	
	};

}

