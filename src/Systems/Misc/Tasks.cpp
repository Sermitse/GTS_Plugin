#include "Systems/Misc/Tasks.hpp"

namespace GTS {

	//-----------
	// TASK
	//-----------

	Task::Task(const std::function<bool(const TaskUpdate&)>& tasking) : startTime(Time::WorldTimeElapsed()), lastRunTime(Time::WorldTimeElapsed()), tasking(tasking) {}

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
				.runtime = 0.0f,
				.delta = 0.0f,
			};
			this->initRun = true;
		}
		this->lastRunTime = currentTime;
		return this->tasking(update);
	}

	//-----------
	// TASK FOR
	//-----------

	TaskFor::TaskFor(double duration, const std::function<bool(const TaskForUpdate&)>& tasking) : startTime(Time::WorldTimeElapsed()), lastRunTime(Time::WorldTimeElapsed()), tasking(tasking), duration(duration) {}

	bool TaskFor::Update() {
		double currentTime = Time::WorldTimeElapsed();
		double currentRuntime = currentTime - this->startTime;
		double currentProgress = std::clamp(currentRuntime / this->duration, 0.0, 1.0);

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
		if (!this->tasking(update)) {
			return false;
		}
		else {
			return currentRuntime <= this->duration;
		}
	}

	//-----------
	// ONE SHOT
	//-----------

	Oneshot::Oneshot(const std::function<void(const OneshotUpdate&)>& tasking) : creationTime(Time::WorldTimeElapsed()), tasking(tasking) {}

	bool Oneshot::Update() {
		double currentTime = Time::WorldTimeElapsed();
		auto update = OneshotUpdate{
			.timeToLive = currentTime - this->creationTime,
		};
		this->tasking(update);
		return false;
	}

	//-----------
	// BASE TASK
	//-----------

	UpdateKind BaseTask::UpdateOn() const {
		return this->updateOnKind;
	}

	void BaseTask::SetUpdateOn(UpdateKind updateOn) {
		this->updateOnKind = updateOn;
	}

	//---------------
	// TASK MANAGER
	//---------------

	std::string TaskManager::DebugName() {
		return "::TaskManager";
	}

	void TaskManager::Update() {
		std::vector<std::string> toRemove = {};
		for (auto& [name, task]: this->m_taskings) {
			if (task->UpdateOn() == UpdateKind::Main) {
				if (!task->Update()) {
					toRemove.push_back(name);
				}
			}
		}
		for (auto task: toRemove) {
			this->m_taskings.erase(task);
		}

	}

	void TaskManager::CameraUpdate() {
		std::vector<std::string> toRemove = {};
		for (auto& [name, task]: this->m_taskings) {
			if (task->UpdateOn() == UpdateKind::Camera) {
				if (!task->Update()) {
					toRemove.push_back(name);
				}
			}
		}

		for (auto task: toRemove) {
			this->m_taskings.erase(task);
		}

	}

	void TaskManager::HavokUpdate() {
		std::vector<std::string> toRemove = {};
		for (auto& [name, task]: this->m_taskings) {
			if (task->UpdateOn() == UpdateKind::Havok) {
				if (!task->Update()) {
					toRemove.push_back(name);
				}
			}
		}

		for (auto task: toRemove) {
			this->m_taskings.erase(task);
		}
	}

	void TaskManager::PapyrusUpdate() {
		std::vector<std::string> toRemove = {};
		for (auto& [name, task]: this->m_taskings) {
			if (task->UpdateOn() == UpdateKind::Papyrus) {
				if (!task->Update()) {
					toRemove.push_back(name);
				}
			}
		}

		for (auto task: toRemove) {
			this->m_taskings.erase(task);
		}
	}

	void TaskManager::ChangeUpdate(std::string_view name, UpdateKind updateOn) {
		try {
			m_taskings.at(std::string(name))->SetUpdateOn(updateOn);
		}
		catch (const std::out_of_range&) {}
	}

	void TaskManager::Cancel(std::string_view name) {
		m_taskings.erase(std::string(name));
	}

	void TaskManager::Run(const std::function<bool(const TaskUpdate&)>& tasking) {
		auto task = new Task(tasking);
		std::string name = std::format("UNNAMED_{}", *reinterpret_cast<std::uintptr_t*>(task));
		m_taskings.try_emplace(name, task);
	}

	void TaskManager::Run(std::string_view name, const std::function<bool(const TaskUpdate&)>& tasking) {
		m_taskings.try_emplace(std::string(name), new Task(tasking));
	}

	void TaskManager::RunFor(float duration, const std::function<bool(const TaskForUpdate&)>& tasking) {
		auto task = new TaskFor(duration, tasking);
		std::string name = std::format("UNNAMED_{}", *reinterpret_cast<std::uintptr_t*>(task));
		m_taskings.try_emplace(name, task);
	}

	void TaskManager::RunFor(std::string_view name, float duration,
		const std::function<bool(const TaskForUpdate&)>& tasking) {
		m_taskings.try_emplace(std::string(name), new TaskFor(duration, tasking));
	}

	void TaskManager::RunOnce(const std::function<void(const OneshotUpdate&)>& tasking) {
		auto task = new Oneshot(tasking);
		std::string name = std::format("UNNAMED_{}", *reinterpret_cast<std::uintptr_t*>(task));
		m_taskings.try_emplace(name, task);
	}

	void TaskManager::RunOnce(std::string_view name, const std::function<void(const OneshotUpdate&)>& tasking) {
		m_taskings.try_emplace(std::string(name), new Oneshot(tasking));
	}

	void TaskManager::CancelAllTasks() {
		m_taskings.clear();
		log::info("Canceled all task manager tasks");
	}

	void TaskManager::OnGameLoaded() {
		CancelAllTasks(); // just in case
	}
}
