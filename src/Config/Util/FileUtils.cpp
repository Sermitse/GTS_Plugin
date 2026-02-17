#include "Config/Util/FileUtils.hpp"

namespace GTS {

	std::string FileUtils::GetTimestamp() {
		using namespace std::chrono;
		auto now = system_clock::now();
		auto unix_seconds = duration_cast<seconds>(now.time_since_epoch()).count();
		return std::to_string(unix_seconds);
	}

	bool FileUtils::EnsureExportDirectoryExists() {
		std::error_code ec;
		if (!std::filesystem::exists(_exportsDir, ec)) {
			if (!std::filesystem::create_directories(_exportsDir, ec)) {
				logger::error("Failed to create export directory: {}", ec.message());
				return false;
			}
		}
		return true;
	}

	std::vector<std::filesystem::path> FileUtils::GetExportedFiles() {
		std::vector<std::filesystem::path> exports;
		if (!std::filesystem::exists(_exportsDir)) {
			return exports;
		}

		std::error_code ec;
		for (const auto& entry : std::filesystem::directory_iterator(_exportsDir, ec)) {
			if (entry.is_regular_file() && entry.path().extension() == ".toml") {
				exports.push_back(entry.path());
			}
		}

		// Sort by modification time (newest first)
		std::ranges::sort(exports, [](const auto& a, const auto& b) {
			std::error_code err;
			auto timeA = std::filesystem::last_write_time(a, err);
			auto timeB = std::filesystem::last_write_time(b, err);
			return timeA > timeB;
		});

		return exports;
	}

	bool FileUtils::DeleteExport(const std::filesystem::path& a_exportPath) {
		std::error_code ec;
		bool result = std::filesystem::remove(a_exportPath, ec);
		if (result) {
			logger::info("Deleted export: {}", a_exportPath.string());
		}
		else {
			logger::error("Failed to delete export {}: {}", a_exportPath.string(), ec.message());
		}
		return result;
	}

	bool FileUtils::CleanOldExports(int a_keepCount) {
		auto exports = GetExportedFiles();
		if (exports.size() <= static_cast<size_t>(a_keepCount)) {
			return true; // Nothing to clean
		}

		bool allDeleted = true;
		for (size_t i = a_keepCount; i < exports.size(); ++i) {
			if (!DeleteExport(exports[i])) {
				allDeleted = false;
			}
		}
		logger::info("Cleaned {} old export files", exports.size() - a_keepCount);
		return allDeleted;
	}

	std::filesystem::path FileUtils::GetExportPath(const std::string& a_fileName) {
		return _exportsDir / a_fileName;
	}

	void FileUtils::CopyLegacySettings(const std::filesystem::path& a_fullFilePath) {

		if (!std::filesystem::exists(a_fullFilePath) || !EnsureExportDirectoryExists()) {
			return;
		}
		try {
			auto destinationPath = _exportsDir / "LegacySettings.toml";
			std::filesystem::copy_file(a_fullFilePath, destinationPath, std::filesystem::copy_options::overwrite_existing);
			std::filesystem::remove(a_fullFilePath);
		}
		catch (const std::filesystem::filesystem_error& e) {
			logger::error("CopyLegacySettings() FS error {}", e.what());
		}
	}

	bool FileUtils::CheckOrCreateFile(const std::filesystem::path& a_fullFilePath) {
		namespace fs = std::filesystem;

		try {

			if (a_fullFilePath.empty()) {
				logger::error("CheckOrCreateFile: empty path");
				return false;
			}

			std::error_code ec{};

			// Normalize for logging/debugging (do not require existence)
			fs::path full = a_fullFilePath;
			fs::path parent = full.parent_path();

			logger::debug("CheckOrCreateFile: target='{}' parent='{}'",
				full.string(), parent.string());

			// Fast path: file already exists
			ec.clear();
			if (fs::exists(full, ec)) {
				if (ec) {
					logger::error("CheckOrCreateFile: exists() failed for '{}' ({})",
						full.string(), ec.message());
					return false;
				}

				ec.clear();
				if (fs::is_regular_file(full, ec)) {
					if (ec) {
						logger::error("CheckOrCreateFile: is_regular_file() failed for '{}' ({})",
							full.string(), ec.message());
						return false;
					}
					logger::debug("CheckOrCreateFile: file exists");
					return true;
				}

				// Exists but isn't a regular file
				ec.clear();
				if (fs::is_directory(full, ec)) {
					logger::error("CheckOrCreateFile: '{}' exists but is a directory", full.string());
					return false;
				}
				logger::error("CheckOrCreateFile: '{}' exists but is not a regular file", full.string());
				return false;
			}
			else if (ec) {
				logger::error("CheckOrCreateFile: exists() failed for '{}' ({})",
					full.string(), ec.message());
				return false;
			}

			// Parent directory handling
			if (parent.empty()) {
				logger::warn("CheckOrCreateFile: parent path is empty; using current working directory");
			}
			else {
				ec.clear();
				const bool parentExists = fs::exists(parent, ec);
				if (ec) {
					logger::error("CheckOrCreateFile: exists(parent) failed for '{}' ({})",
						parent.string(), ec.message());
					return false;
				}

				if (parentExists) {
					ec.clear();
					if (!fs::is_directory(parent, ec) || ec) {
						logger::error("CheckOrCreateFile: parent '{}' exists but is not a directory{}",
							parent.string(),
							ec ? fmt::format(" ({})", ec.message()) : "");
						return false;
					}
				}
				else {
					ec.clear();
					const bool created = fs::create_directories(parent, ec);
					if (ec) {
						logger::error("CheckOrCreateFile: create_directories('{}') failed ({})",
							parent.string(), ec.message());
						return false;
					}

					if (created) {
						logger::critical("CheckOrCreateFile: plugin folder was missing and was created: '{}'", parent.string());
						ReportAndExit(
							"The GTSPlugin folder was missing and had to be created.\n"
							"This indicates that the mod was not installed correctly.\n"
							"The mod will not work if it's contents are missing."
						);
					}
				}
			}

			// Create file: use explicit open so exceptions happen at open(), not at exceptions()
			{
				std::ofstream file;
				file.exceptions(std::ios::failbit | std::ios::badbit);

				try {
					file.open(full, std::ios::out | std::ios::binary | std::ios::trunc);
					file.close();
				}
				catch (const std::ios_base::failure& e) {
					// Enrich with errno-style info if possible
					logger::error("CheckOrCreateFile: ofstream open/close failed for '{}': {}",
						full.string(), e.what());
					return false;
				}
			}

			// Verify it now exists and is a file
			ec.clear();
			if (!fs::exists(full, ec) || ec) {
				logger::error("CheckOrCreateFile: file creation verification failed for '{}'{}",
					full.string(),
					ec ? fmt::format(" ({})", ec.message()) : "");
				return false;
			}

			ec.clear();
			if (!fs::is_regular_file(full, ec) || ec) {
				logger::error("CheckOrCreateFile: '{}' created but is not a regular file{}",
					full.string(),
					ec ? fmt::format(" ({})", ec.message()) : "");
				return false;
			}

			logger::info("CheckOrCreateFile: created '{}'", full.string());
			return true;
		}
		catch (const std::filesystem::filesystem_error& e) {
			logger::error("CheckOrCreateFile: filesystem_error: {} (path1='{}' path2='{}')",
				e.what(), e.path1().string(), e.path2().string());
			return false;
		}
		catch (const std::exception& e) {
			logger::error("CheckOrCreateFile: exception: {}", e.what());
			return false;
		}
		catch (...) {
			logger::error("CheckOrCreateFile: unknown exception");
			return false;
		}
	}

}
