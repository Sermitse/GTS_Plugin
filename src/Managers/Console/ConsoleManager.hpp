#pragma once 


namespace GTS {

    struct Command {
        std::function<void()> callback = nullptr;
        std::string desc;
        explicit Command(const std::function<void()>& callback, std::string desc) : callback(callback), desc(std::move(desc)) {}
    };

    class ConsoleManager : public CInitSingleton<ConsoleManager> {

        private:
        //default base command preffix
        const std::string Default_Preffix = "gts";

        static void CMD_Help();
        static void CMD_Version();
        static void CMD_Unlimited();

        public:

        static void Init() {
            logger::info("Loading Default Command List");
            RegisterCommand("help", CMD_Help, "Show this list");
            RegisterCommand("version", CMD_Version, "Show plugin version");
            RegisterCommand("unlimited", CMD_Unlimited, "Unlocks max size sliders");
        }

        static void RegisterCommand(std::string_view a_cmdName, const std::function<void()>& a_callback, const std::string& a_desc);
        static bool Process(const std::string& a_msg);


    private:
        std::map<std::string, Command> RegisteredCommands;
    };








}