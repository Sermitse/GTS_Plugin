#include "Systems/Misc/Tasks.hpp"

namespace GTS {
	//-----------
	// TASK
	//-----------

	Task::Task(const std::function<bool(const TaskUpdate&)>& tasking)
		: startTime(Time::WorldTimeElapsed()),
		  lastRunTime(Time::WorldTimeElapsed()),
		  tasking(tasking) {}

	bool Task::Update() {
		TaskUpdate update;
		double currentTime = Time::WorldTimeElapsed();

		if (this->initRun) {
			update = TaskUpdate{
				.runtime = currentTime - this->startTime,
				.delta = currentTime - this->lastRunTime,
			};
		}
		else {
			update = TaskUpdate{
				.runtime = 0.0,
				.delta = 0.0,
			};
			this->initRun = true;
		}

		this->lastRunTime = currentTime;

		return this->tasking(update);
	}

	//-----------
	// TASK FOR
	//-----------

	TaskFor::TaskFor(double duration, const std::function<bool(const TaskForUpdate&)>& tasking)
		: startTime(Time::WorldTimeElapsed()),
		  lastRunTime(Time::WorldTimeElapsed()),
		  tasking(tasking),
		  duration(duration) {}

	bool TaskFor::Update() {
		double currentTime = Time::WorldTimeElapsed();
		double currentRuntime = currentTime - this->startTime;

		double currentProgress = 0.0;
		if (this->duration > 0.0) {
			currentProgress = std::clamp(currentRuntime / this->duration, 0.0, 1.0);
		}

		TaskForUpdate update;

		if (this->initRun) {
			update = TaskForUpdate{
				.runtime = currentRuntime,
				.delta = currentTime - this->lastRunTime,
				.progress = currentProgress,
				.progressDelta = currentProgress - this->lastProgress,
			};
		}
		else {
			update = TaskForUpdate{
				.runtime = 0.0,
				.delta = 0.0,
				.progress = 0.0,
				.progressDelta = 0.0,
			};
			this->initRun = true;
		}

		this->lastRunTime = currentTime;
		this->lastProgress = currentProgress;

		bool shouldContinue = this->tasking(update);

		if (shouldContinue) {
			if (currentRuntime <= this->duration) {
				return true;
			}
		}

		return false;
	}

	//-----------
	// ONE SHOT
	//-----------

	Oneshot::Oneshot(const std::function<void(const OneshotUpdate&)>& tasking)
		: creationTime(Time::WorldTimeElapsed()),
		  tasking(tasking) {}

	bool Oneshot::Update() {
		double currentTime = Time::WorldTimeElapsed();

		OneshotUpdate update{
			.timeToLive = currentTime - this->creationTime,
		};

		this->tasking(update);

		return false;
	}

	//---------------
	// TASK MANAGER
	//---------------

	std::string TaskManager::DebugName() {
		return "::TaskManager";
	}

	std::string TaskManager::GenerateName(void* ptr) {
		return std::format("UNNAMED_{}", reinterpret_cast<std::uintptr_t>(ptr));
	}

	void TaskManager::Update() {
		std::vector<std::string> toRemove;
		toRemove.reserve(m_taskings.size());

		for (auto& [name, task] : m_taskings) {
			if (task->UpdateOn() == UpdateKind::Main) {
				if (!task->Update()) {
					toRemove.push_back(name);
				}
			}
		}

		for (auto& name : toRemove) {
			m_taskings.erase(name);
		}
	}

	void TaskManager::CameraUpdate() {
		std::vector<std::string> toRemove;
		toRemove.reserve(m_taskings.size());

		for (auto& [name, task] : m_taskings) {
			if (task->UpdateOn() == UpdateKind::Camera) {
				if (!task->Update()) {
					toRemove.push_back(name);
				}
			}
		}

		for (auto& name : toRemove) {
			m_taskings.erase(name);
		}
	}

	void TaskManager::HavokUpdate() {
		std::vector<std::string> toRemove;
		toRemove.reserve(m_taskings.size());

		for (auto& [name, task] : m_taskings) {
			if (task->UpdateOn() == UpdateKind::Havok) {
				if (!task->Update()) {
					toRemove.push_back(name);
				}
			}
		}

		for (auto& name : toRemove) {
			m_taskings.erase(name);
		}
	}

	void TaskManager::ChangeUpdate(std::string_view name, UpdateKind updateOn) {
		auto it = m_taskings.find(std::string(name));
		if (it != m_taskings.end()) {
			it->second->SetUpdateOn(updateOn);
		}
	}

	void TaskManager::Cancel(std::string_view name) {
		m_taskings.erase(std::string(name));
	}

	void TaskManager::Run(const std::function<bool(const TaskUpdate&)>& tasking) {
		auto task = std::make_unique<Task>(tasking);
		std::string name = GenerateName(task.get());

		auto [it, inserted] = m_taskings.try_emplace(name, std::move(task));
		if (!inserted) {
			logger::warn("Task '{}' already exists", name);
		}
	}

	void TaskManager::Run(std::string_view name, const std::function<bool(const TaskUpdate&)>& tasking) {
		auto task = std::make_unique<Task>(tasking);

		auto [it, inserted] = m_taskings.try_emplace(std::string(name), std::move(task));
		if (!inserted) {
			logger::warn("Task '{}' already exists", name);
		}
	}

	void TaskManager::RunFor(float duration, const std::function<bool(const TaskForUpdate&)>& tasking) {
		auto task = std::make_unique<TaskFor>(duration, tasking);
		std::string name = GenerateName(task.get());

		auto [it, inserted] = m_taskings.try_emplace(name, std::move(task));
		if (!inserted) {
			logger::warn("Task '{}' already exists", name);
		}
	}

	void TaskManager::RunFor(std::string_view name, float duration,
		const std::function<bool(const TaskForUpdate&)>& tasking) {

		auto task = std::make_unique<TaskFor>(duration, tasking);

		auto [it, inserted] = m_taskings.try_emplace(std::string(name), std::move(task));
		if (!inserted) {
			logger::warn("Task '{}' already exists", name);
		}
	}

	void TaskManager::RunOnce(const std::function<void(const OneshotUpdate&)>& tasking) {
		auto task = std::make_unique<Oneshot>(tasking);
		std::string name = GenerateName(task.get());

		auto [it, inserted] = m_taskings.try_emplace(name, std::move(task));
		if (!inserted) {
			logger::warn("Task '{}' already exists", name);
		}
	}

	void TaskManager::RunOnce(std::string_view name, const std::function<void(const OneshotUpdate&)>& tasking) {
		auto task = std::make_unique<Oneshot>(tasking);

		auto [it, inserted] = m_taskings.try_emplace(std::string(name), std::move(task));
		if (!inserted) {
			logger::warn("Task '{}' already exists", name);
		}
	}

	void TaskManager::CancelAllTasks() {
		m_taskings.clear();
		logger::info("Canceled all task manager tasks");
	}

	void TaskManager::OnGameLoaded() {
		CancelAllTasks();
	}
}