#pragma once 

namespace GTS {

    class ConsoleManager : public EventListener, public CInitSingleton<ConsoleManager> {

        private:
        struct Command {
            std::function<void()> callback = nullptr;
            std::string desc;
            explicit Command(const std::function<void()>& callback, std::string desc) : callback(callback), desc(std::move(desc)) {}
        };

        //default base command preffix
        inline static const std::string Default_Preffix = "gts";
        static inline std::map<std::string, Command> RegisteredCommands = {};

        static void CMD_Help();
        static void CMD_Version();
        static void CMD_Unlimited();

        public:
        static void Init();
        static void RegisterCommand(std::string_view a_cmdName, const std::function<void()>& a_callback, const std::string& a_desc);
        static bool Process(const std::string& a_msg);



        // Inherited via EventListener
        std::string DebugName() override;
        void DataReady() override;
    };








}