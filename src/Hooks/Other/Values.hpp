#pragma once


namespace Hooks {

	/*
	 * RE'd offsets for the DAT values in skyrims ini settings.
	 */

	namespace LOD {
		static float* fLodDistance =               reinterpret_cast<float*>(REL::RelocationID(507568, 378623, NULL).address());
		static float* fTalkingDistance =           reinterpret_cast<float*>(REL::RelocationID(507570, 378626, NULL).address());
	};

	namespace Distance {
		static float* fActivatePickLength =        reinterpret_cast<float*>(REL::RelocationID(502527, 370109, NULL).address());
		static float* fActivatePickRadius =        reinterpret_cast<float*>(REL::RelocationID(502525, 370106, NULL).address());
	}

	namespace Camera {
		static float* fVanityModeMaxDist =         reinterpret_cast<float*>(REL::RelocationID(509878, 382627, NULL).address());
		static float* fVanityModeMinDist =         reinterpret_cast<float*>(REL::RelocationID(509874, 382621, NULL).address());
		static float* fMouseWheelZoomSpeed =       reinterpret_cast<float*>(REL::RelocationID(509890, 382645, NULL).address());
		static float* fMouseWheelZoomIncrement =   reinterpret_cast<float*>(REL::RelocationID(509888, 382642, NULL).address());
	}

	/*
	 * World
	 */

	namespace World {
		static RE::GMatrix3D* RawWorldToCamMatrix =    reinterpret_cast<RE::GMatrix3D*>     (REL::RelocationID(519579, 406126, NULL).address());
		static RE::NiRect<float>* RawViewPort =        reinterpret_cast<RE::NiRect<float>*> (REL::RelocationID(519618, 406160, NULL).address());

		static float* RawWorldScale =                  reinterpret_cast<float*>(REL::RelocationID(231896, 188105, NULL).address());
		static float* RawWorldScaleInverse =           reinterpret_cast<float*>(REL::RelocationID(230692, 187407, NULL).address());
	}

}

