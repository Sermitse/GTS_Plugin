#pragma once

namespace GTS {

    class FileUtils {

        public:

        static inline const std::filesystem::path _basePath = std::filesystem::current_path() / R"(Data/SKSE/Plugins/GTSPlugin)";
        static inline const std::filesystem::path _exportsDir = _basePath / "Exports";

        static std::string GetTimestamp() {
            using namespace std::chrono;
            auto now = system_clock::now();
            auto unix_seconds = duration_cast<seconds>(now.time_since_epoch()).count();
            return std::to_string(unix_seconds);
        }

        static bool EnsureExportDirectoryExists() {
            std::error_code ec;
            if (!std::filesystem::exists(_exportsDir, ec)) {
                if (!std::filesystem::create_directories(_exportsDir, ec)) {
                    logger::error("Failed to create export directory: {}", ec.message());
                    return false;
                }
            }
            return true;
        }

        std::vector<std::filesystem::path> GetExportedFiles() const {
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
            ranges::sort(exports, [](const auto& a, const auto& b) {
                std::error_code err;
                auto timeA = std::filesystem::last_write_time(a, err);
                auto timeB = std::filesystem::last_write_time(b, err);
                return timeA > timeB;
            });

            return exports;
        }

        static bool DeleteExport(const std::filesystem::path& exportPath) {
            std::error_code ec;
            bool result = std::filesystem::remove(exportPath, ec);
            if (result) {
                logger::info("Deleted export: {}", exportPath.string());
            }
            else {
                logger::error("Failed to delete export {}: {}", exportPath.string(), ec.message());
            }
            return result;
        }

        bool CleanOldExports(int keepCount) const {
            auto exports = GetExportedFiles();
            if (exports.size() <= static_cast<size_t>(keepCount)) {
                return true; // Nothing to clean
            }

            bool allDeleted = true;
            for (size_t i = keepCount; i < exports.size(); ++i) {
                if (!DeleteExport(exports[i])) {
                    allDeleted = false;
                }
            }
            logger::info("Cleaned {} old export files", exports.size() - keepCount);
            return allDeleted;
        }

        static std::filesystem::path GetExportPath(const std::string& filename) {
            return _exportsDir / filename;
        }

        static void CopyLegacySettings(const std::filesystem::path& legacyPath) {

            if (!std::filesystem::exists(legacyPath) || !EnsureExportDirectoryExists()) {
                return;
            }
            try {
                auto destinationPath = _exportsDir / "LegacySettings.toml";
                std::filesystem::copy_file(legacyPath, destinationPath, std::filesystem::copy_options::overwrite_existing);
                std::filesystem::remove(legacyPath);
            }
            catch (const std::filesystem::filesystem_error& e) {
                logger::error("CopyLegacySettings() FS error {}", e.what());
            }
        }

        static bool CheckOrCreateFile(const std::filesystem::path& a_file) {
            try {
                // Check if the file exists
                if (std::filesystem::exists(a_file)) {
                    return true;
                }

                // Create parent directories if they don't exist
                if (std::filesystem::create_directories(a_file.parent_path())) {
                    logger::critical("Plugin folder was mising and was created, MOD BROKEN.");
                    ReportAndExit("The GTSPlugin folder was missing and had to be created.\n"
                        "This indicates that the mod was not installed correctly.\n"
                        "The mod will not work if the Font Folder and Runtime.toml are missing.\n"
                        "The game will now close");
                }

                // Try to create the file
                std::ofstream file(a_file);
                file.exceptions(std::ofstream::failbit);
                if (file) {
                    file.close();
                    logger::warn("Configuration file did not exist but was successfully created");
                    return true;
                }

                return false;
            }
            catch (const std::filesystem::filesystem_error& e) {
                logger::error("CheckFile() Filesystem error: {}", e.what());
                return false;
            }
            catch (const std::exception& e) {
                logger::error("CheckFile() -> Exception: {}", e.what());
                return false;
            }
            catch (...) {
                logger::error("CheckFile() -> Unknown Exception");
                return false;
            }
        }
    };
}