#include "FunctionHook.hpp"


// Deprecated

namespace Hooks {

	constexpr uint32_t MaxRetry = 3;

	void FunctionHook<void>::Attach(void** target, void* hook) {
		
		uintptr_t base = REL::Module::get().base();
		logger::debug("Attaching function hook to address {:#X} (offset from image base of {:#X} by {:#X}...",
		           reinterpret_cast<uintptr_t>(*target), base, reinterpret_cast<uintptr_t>(*target) - base);
		for (std::size_t i = 0; i < 3; ++i) {
			auto result = DetourTransactionBegin();
			if (result != NO_ERROR) {
				logger::error("Failed to start transaction for unknown reason (error code {}).", result);
				throw std::runtime_error("");
			}
			logger::trace("Initiated transaction for function hook...");
			result = DetourUpdateThread(GetCurrentThread());
			switch (result) {
				case NO_ERROR:
					logger::trace("Function hook transaction thread information written...");
					break;
				case ERROR_NOT_ENOUGH_MEMORY:
					DetourTransactionAbort();
					logger::error(
						"Not enough memory to obtain thread information for function hook transaction, "
						"aborting operation.");
					throw std::runtime_error("");
				default:
					DetourTransactionAbort();
					logger::error("Failed to obtain transaction thread information for unknown reason (error code {}).",
					           result);
					throw std::runtime_error("");
			}

			result = DetourAttach(target, hook);

			switch (result) {
				case NO_ERROR:
					logger::trace("Function hook attached successfully in transaction, committing transaction...");
					break;
				case ERROR_INVALID_BLOCK:
					DetourTransactionAbort();
					logger::error(
						"The target function is too small to attach a function hook, aborting operation.");
					throw std::runtime_error("");
				case ERROR_INVALID_HANDLE:
					DetourTransactionAbort();
					logger::error("The target function does refer to a valid address, aborting operation.");
					throw std::runtime_error("");
				case ERROR_NOT_ENOUGH_MEMORY:
					DetourTransactionAbort();
					logger::error("Not enough memory to attach function hook; aborting operation.");
					throw std::runtime_error("");
				default:
					DetourTransactionAbort();
					logger::error("Failed to attach function hook for unknown reason (error code {}).", result);
					throw std::runtime_error("");
			}

			result = DetourTransactionCommit();

			switch (result) {
				case NO_ERROR:
					logger::debug("Function hook to address {} committed, function hook is now active.",
					           reinterpret_cast<uintptr_t>(*target));
					return;
				case ERROR_INVALID_DATA:
					logger::warn(
						"Detected target of function hook was modified by another thread before committing "
						"the hook. Retrying...");
					continue;
				default:
					DetourTransactionAbort();
					logger::error("Failed to commit function hook for unknown reason (error code {}).", result);
					throw std::runtime_error("");
			}
		}

		logger::error("Unable to commit function hook after {} retries without "
		           "another thread modifying the target function. Operation aborted.",
		           MaxRetry);
		throw std::runtime_error("");
	}

	void FunctionHook<void>::Detach(void** target, void* hook) {
		uintptr_t base = REL::Module::get().base();
		logger::debug("Detaching function hook from address {:#X} (offset from image base of {:#X} by {:#X}...",
		           reinterpret_cast<uintptr_t>(*target), base, reinterpret_cast<uintptr_t>(*target) - base);
		for (std::size_t i = 0; i < MaxRetry; ++i) {
			auto result = DetourTransactionBegin();
			if (result != NO_ERROR) {
				logger::error("Failed to start transaction for unknown reason (error code {}).", result);
				throw std::runtime_error("");
			}
			logger::trace("Initiated transaction for function hook...");
			result = DetourUpdateThread(GetCurrentThread());
			switch (result) {
				case NO_ERROR:
					logger::trace("Function hook transaction thread information written...");
					break;
				case ERROR_NOT_ENOUGH_MEMORY:
					DetourTransactionAbort();
					logger::error(
						"Not enough memory to obtain thread information for function hook transaction, "
						"aborting operation.");
					throw std::runtime_error("");
				default:
					DetourTransactionAbort();
					logger::error("Failed to obtain transaction thread information for unknown reason (error code {}).",
					           result);
					throw std::runtime_error("");
			}
			result = DetourDetach(target, hook);
			if (result != NO_ERROR) {
				DetourTransactionAbort();
				logger::error("Failed to detach function hook for unknown reason (error code {}).", result);
				throw std::runtime_error("");
			}
			logger::trace("Function hook detached successfully in transaction, committing transaction...");
			result = DetourTransactionCommit();
			switch (result) {
				case NO_ERROR:
					logger::debug("Function hook detachment for address {} committed, function hook has been removed.",
					           reinterpret_cast<uintptr_t>(*target));
					return;
				case ERROR_INVALID_DATA:
					logger::warn(
						"Detected target of function hook was modified by another thread before committing "
						"the hook. Retrying...");
					continue;
				default:
					DetourTransactionAbort();
					logger::error("Failed to commit function hook detachment for unknown reason (error code {}).", result);
					throw std::runtime_error("");
			}
		}
		logger::error("Unable to commit function hook detachment after {} retries "
		           "without another thread modifying the target function. Operation aborted.",MaxRetry);
		throw std::runtime_error("");
	}

}
