#include "API/SmoothCam.hpp"

namespace {
	bool Smoothcam_HaveCamera = false;
}

namespace GTS {


	void SmoothCam::Register() {

		logger::info("Registering Smoothcam API");

		if (!Loaded()) {
			if (!SmoothCamAPI::RegisterInterfaceLoaderCallback(SKSE::GetMessagingInterface(), [](void* interfaceInstance, SmoothCamAPI::InterfaceVersion interfaceVersion) {
				if (interfaceVersion >= SmoothCamAPI::InterfaceVersion::V3) {
					SmoothCamAPI = reinterpret_cast<SmoothCamAPI::IVSmoothCam3*>(interfaceInstance);
					logger::info("Obtained SmoothCamAPI");
				}
				else {
					logger::warn("Unable to acquire requested SmoothCamAPI interface version");
				}
				})) {
				logger::warn("SmoothCamAPI::RegisterInterfaceLoaderCallback reported an error");
			}
			if (!SmoothCamAPI::RequestInterface(
				SKSE::GetMessagingInterface(),
				SmoothCamAPI::InterfaceVersion::V3)) {
				//Set back to null incase it got set but requesting the interface failed.
				SmoothCamAPI = nullptr;
				logger::warn("SmoothCamAPI::RequestInterface reported an error");
			}
		}
	}


	void SmoothCam::RequestConrol() {

		if (Loaded()) {
			if (!SmoothCamAPI->IsCameraEnabled()) {
				//Camera is disabled, We don't need to do anything
				Smoothcam_HaveCamera = false;
			}

			if (!Smoothcam_HaveCamera) {
				auto res = SmoothCamAPI->RequestCameraControl(SKSE::GetPluginHandle());
				if (res == SmoothCamAPI::APIResult::OK || res == SmoothCamAPI::APIResult::AlreadyGiven) {
					Smoothcam_HaveCamera = true;
				}
			}
		}
	}

	void SmoothCam::ReturnControl() {

		if (Loaded()) {
			if (Smoothcam_HaveCamera) {
				SmoothCamAPI->ReleaseCameraControl(SKSE::GetPluginHandle());
				Smoothcam_HaveCamera = false;
			}
		}
	}

	bool SmoothCam::Enabled() {
		auto Enable = Loaded() ? SmoothCamAPI->IsCameraEnabled() : false;
		if (!Enable && Smoothcam_HaveCamera) {
			Smoothcam_HaveCamera = false;
		}
		return Enable;
	}

	bool SmoothCam::HaveCamera() {
		return Smoothcam_HaveCamera;
	}

	std::string SmoothCam::DebugName() {
		return "::SmoothCamAPI";
	}

	void SmoothCam::DataReady() {
		Register();
	}
}
