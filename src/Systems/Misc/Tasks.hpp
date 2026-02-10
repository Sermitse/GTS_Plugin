#pragma once

namespace GTS {

	enum class UpdateKind {
		Main,
		Camera,
		Havok,
		Bone,
		Papyrus,
	};

	// A `Task` that runs once in the next frame
	struct OneshotUpdate {
		// Total time since creation before run
		double timeToLive;
	};

	class BaseTask {

		public:
		virtual bool Update() = 0;
		UpdateKind UpdateOn() const;
		void SetUpdateOn(UpdateKind updateOn);

		protected:
		UpdateKind updateOnKind = UpdateKind::Main;
	};

	class Oneshot : public BaseTask {
		public:
		Oneshot(const std::function<void(const OneshotUpdate&)>& tasking);
		virtual bool Update() override;

		private:
		double creationTime = 0.0;
		std::function<void(const OneshotUpdate&)> tasking;
	};

	// A `Task` that runs until it returns false
	struct TaskUpdate {
		double runtime;  // Total runtime in seconds
		double delta;    // Time delta since last runtime
	};

	class Task : public BaseTask {
		public:
		Task(const std::function<bool(const TaskUpdate&)>& tasking);
		virtual bool Update() override;

		private:
		bool initRun = false;
		double startTime = 0.0;
		double lastRunTime = 0.0;
		std::function<bool(const TaskUpdate&)> tasking;
	};

	struct TaskForUpdate {
		double runtime;         // Total runtime in seconds
		double delta;           // Time delta since last runtime
		double progress;        // How close to completion on a scale of 0.0...1.0
		double progressDelta;   // How much progress has been gained since last time
	};

	// A `TaskFor` runs until it returns false OR the duration has elapsed
	class TaskFor : public BaseTask {
		public:
		TaskFor(double duration, const std::function<bool(const TaskForUpdate&)>& tasking);
		virtual bool Update() override;

		private:
		bool initRun = false;
		double startTime = 0.0;
		double lastRunTime = 0.0;
		double lastProgress = 0.0;
		std::function<bool(const TaskForUpdate&)> tasking;
		double duration;
	};

	class TaskManager : public EventListener, public CInitSingleton<TaskManager> {

		public:
		std::string DebugName() override;
		virtual void Update() override;
		virtual void CameraUpdate() override;
		virtual void HavokUpdate() override;
		virtual void PapyrusUpdate() override;
		virtual void OnGameLoaded() override;

		static void ChangeUpdate(std::string_view name, UpdateKind updateOn);
		static void Cancel(std::string_view name);
		static void Run(const std::function<bool(const TaskUpdate&)>& tasking);
		static void Run(std::string_view name, const std::function<bool(const TaskUpdate&)>& tasking);
		static void RunFor(float duration, const std::function<bool(const TaskForUpdate&)>& tasking);
		static void RunFor(std::string_view name, float duration, const std::function<bool(const TaskForUpdate&)>& tasking);
		static void RunOnce(const std::function<void(const OneshotUpdate&)>& tasking);
		static void RunOnce(std::string_view name, const std::function<void(const OneshotUpdate&)>& tasking);
		static void CancelAllTasks();

		static inline std::unordered_map<std::string, BaseTask*> m_taskings;
	};
}
