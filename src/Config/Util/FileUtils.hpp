#pragma once

namespace GTS {

    class FileUtils {

        public:

        static inline const std::filesystem::path _basePath = std::filesystem::current_path() / R"(Data/SKSE/Plugins/GTSPlugin)";
        static inline const std::filesystem::path _exportsDir = _basePath / "Exports";

        static std::string GetTimestamp();
        static bool EnsureExportDirectoryExists();
        static std::vector<std::filesystem::path> GetExportedFiles();
        static bool DeleteExport(const std::filesystem::path& a_exportPath);
        static bool CleanOldExports(int a_keepCount);
        static std::filesystem::path GetExportPath(const std::string& a_fileName);
        static void CopyLegacySettings(const std::filesystem::path& a_fullFilePath);
        static bool CheckOrCreateFile(const std::filesystem::path& a_fullFilePath);
    };
}