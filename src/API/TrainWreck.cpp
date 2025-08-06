#include "TrainWreck.hpp"
#include "Impl/TrainWreckAPI.hpp"
#include "Version.hpp"
#include "git.h"

namespace GTS {

    void TrainWreck::Install() {

        auto err = TrainWreckAPI::register_section(
            TRAINWRECK_SECTION_CALL_STACK,
            TRAINWRECK_SECTION_BEFORE,
            "GTSPLUGIN",
            [](auto args) {
                //Just print info for now.
                auto twlog = TrainWreckAPI::Log(args->log_context);
                twlog.write_line(fmt::format("Version: {}", GTSPlugin::ModVersion.string()));
                twlog.write_line(fmt::format("Build Date: {} {}", __DATE__, __TIME__));
                twlog.write_line(fmt::format("Git Commit SHA1: {}", git_CommitSHA1()));
                twlog.write_line(fmt::format("Git Commit Date: {}", git_CommitDate()));
                twlog.write_line(fmt::format("Uncommited Changes: {}", (git_AnyUncommittedChanges() ? "Yes" : "No")));
            },
            nullptr
        );

        if (!err.has_value() || *err != TRAINWRECK_ERROR_OK) {
            logger::warn("Could not register trainwreck section");
        }
        logger::info("Registered TrainWreck section");
    }

}