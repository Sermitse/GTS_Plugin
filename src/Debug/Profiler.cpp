#include "Debug/Profiler.hpp"
#include "Managers/GtsManager.hpp"

#include "UI/ImGui/Lib/imgui.h"
#include "UI/ImGui/Core/ImFontManager.hpp"
#include "UI/GTSMenu.hpp"


namespace GTS {

	Profiler::Profiler(std::string_view name) : Name(std::string(name)), ThreadId(std::this_thread::get_id()), LastAccessed(Clock::now()) {}

	void Profiler::Start() {
		if (!Running.load(std::memory_order_relaxed)) {
			ClkBegin = Clock::now();
			Running.store(true);
			UpdateLastAccessed();
		}
	}

	void Profiler::Stop() {
		if (Running.load(std::memory_order_relaxed)) {
			this->Elapsed += RunningTime();
			Running.store(false);
			UpdateLastAccessed();
		}
	}

	void Profiler::Reset() {
		this->Elapsed = 0.0f;
		UpdateLastAccessed();
	}

	double Profiler::GetElapsed() const {
		if (this->IsRunning()) {
			return this->Elapsed + this->RunningTime();
		}
		return this->Elapsed;
	}

	bool Profiler::IsRunning() const {
		return Running.load(std::memory_order_relaxed);
	}

	double Profiler::RunningTime() const {
		if (Running.load(std::memory_order_relaxed)) {
			return std::chrono::duration_cast<Second>(Clock::now() - ClkBegin).count();
		}
		return 0;
	}

	std::string Profiler::GetName() {
		return this->Name;
	}

	std::thread::id Profiler::GetThreadId() const {
		return this->ThreadId;
	}

	void Profiler::UpdateLastAccessed() {
		LastAccessed = Clock::now();
	}

	bool Profiler::IsExpired(double expiration_time_seconds) const {
		auto now = Clock::now();
		auto elapsed = std::chrono::duration_cast<Second>(now - LastAccessed).count();
		return elapsed > expiration_time_seconds;
	}

	ProfilerHandle::ProfilerHandle(std::string_view name) : Name(std::string(name)) {
	#ifdef GTS_PROFILER_ENABLED
		Profilers::Start(name);
	#endif
	}

	ProfilerHandle::~ProfilerHandle() {
	#ifdef GTS_PROFILER_ENABLED
		Profilers::Stop(this->Name);
	#endif
	}

	EntrypointProfilerHandle::EntrypointProfilerHandle(std::string_view name) : Name(std::string(name)) {
	#ifdef GTS_PROFILER_ENABLED
		Profilers::StartEntrypoint(name);
	#endif
	}

	EntrypointProfilerHandle::~EntrypointProfilerHandle() {
	#ifdef GTS_PROFILER_ENABLED
		Profilers::StopEntrypoint(this->Name);
	#endif
	}

	void Profilers::Start(std::string_view a_name) {
		auto& me = Profilers::GetSingleton();
		auto thread_id = std::this_thread::get_id();
		auto key = std::string(a_name);

		auto& thread_data = me.thread_data[thread_id];
		thread_data.last_activity = std::chrono::steady_clock::now();
		thread_data.profilers.emplace(key, a_name);
		thread_data.profilers.at(key).Start();
	}

	void Profilers::Stop(std::string_view a_name) {
		auto& me = Profilers::GetSingleton();
		auto thread_id = std::this_thread::get_id();
		auto key = std::string(a_name);

		auto& thread_data = me.thread_data[thread_id];
		thread_data.last_activity = std::chrono::steady_clock::now();
		thread_data.profilers.emplace(key, a_name);
		thread_data.profilers.at(key).Stop();
	}

	void Profilers::StartEntrypoint(std::string_view a_name) {
		auto& me = Profilers::GetSingleton();
		auto thread_id = std::this_thread::get_id();
		auto key = std::string(a_name);

		auto& thread_data = me.thread_data[thread_id];
		thread_data.last_activity = std::chrono::steady_clock::now();
		thread_data.entrypoint_profilers.emplace(key, a_name);
		thread_data.entrypoint_profilers.at(key).Start();

		// Start total time tracking when any entrypoint starts
		if (me.AnyEntrypointRunning()) {
			me.TotalTime.Start();
		}
	}

	void Profilers::StopEntrypoint(std::string_view a_name) {
		auto& me = Profilers::GetSingleton();
		auto thread_id = std::this_thread::get_id();
		auto key = std::string(a_name);

		auto& thread_data = me.thread_data[thread_id];
		thread_data.last_activity = std::chrono::steady_clock::now();
		thread_data.entrypoint_profilers.emplace(key, a_name);
		thread_data.entrypoint_profilers.at(key).Stop();

		// Stop total time tracking when no entrypoints are running
		if (!me.AnyEntrypointRunning()) {
			me.TotalTime.Stop();
		}
	}

	bool Profilers::AnyRunning() {
		for (auto& data : this->thread_data | views::values) {
			for (auto& profiler : data.profilers | std::views::values) {
				if (profiler.IsRunning()) {
					return true;
				}
			}
		}
		return false;
	}

	bool Profilers::AnyEntrypointRunning() {
		for (auto& data : this->thread_data | views::values) {
			for (auto& profiler : data.entrypoint_profilers | std::views::values) {
				if (profiler.IsRunning()) {
					return true;
				}
			}
		}
		return false;
	}

	void Profilers::CleanupExpiredThreads() {
		auto now = std::chrono::steady_clock::now();
		std::vector<std::thread::id> expired;

		for (auto& [tid, data] : thread_data) {
			auto elapsed = std::chrono::duration_cast<std::chrono::duration<double>>(now - data.last_activity).count();
			if (elapsed > thread_expiration_time) {
				expired.push_back(tid);
			}
		}

		for (auto& tid : expired) {
			thread_data.unsafe_erase(tid); // safe because no iterator involved
			thread_names.unsafe_erase(tid);
		}
	}

	std::string Profilers::GetThreadName(std::thread::id thread_id) {

		auto it = thread_names.find(thread_id);
		if (it != thread_names.end()) {
			return it->second;
		}

		// Generate a name for the thread
		std::ostringstream oss;
		oss << "Thread-" << thread_id;
		std::string name = oss.str();
		thread_names[thread_id] = name;
		return name;
	}

	void Profilers::DisplayThreadTable(const std::string& thread_name, ThreadProfilerData& data, double total_time) {
		// Entrypoint Profilers for this thread
		if (!data.entrypoint_profilers.empty()) {
			if (ImGui::TreeNodeEx(("Entrypoints##" + thread_name).c_str(), ImGuiTreeNodeFlags_DefaultOpen)) {
				if (ImGui::BeginTable(("EntrypointTable##" + thread_name).c_str(), 3,
					ImGuiTableFlags_Borders |
					ImGuiTableFlags_HighlightHoveredColumn |
					ImGuiTableFlags_Sortable |
					ImGuiTableFlags_BordersOuter |
					ImGuiTableFlags_SizingFixedFit)) {

					ImGui::TableSetupColumn("Name", ImGuiTableColumnFlags_DefaultSort | ImGuiTableColumnFlags_WidthStretch);
					ImGui::TableSetupColumn("CPU Time", ImGuiTableColumnFlags_PreferSortAscending | ImGuiTableColumnFlags_WidthFixed);
					ImGui::TableSetupColumn("% Total", ImGuiTableColumnFlags_PreferSortAscending | ImGuiTableColumnFlags_WidthFixed);
					ImGui::TableHeadersRow();

					// Gather & sort
					std::vector<std::pair<const std::string, Profiler>*> entries;
					entries.reserve(data.entrypoint_profilers.size());
					for (auto& kv : data.entrypoint_profilers)
						entries.push_back(&kv);

					if (auto specs = ImGui::TableGetSortSpecs(); specs && specs->SpecsCount > 0) {
						auto& spec = specs->Specs[0];
						bool asc = spec.SortDirection == ImGuiSortDirection_Ascending;
						ranges::sort(entries, [&](auto* a, auto* b) {
							switch (spec.ColumnIndex) {
								case 0: return asc ? (a->first < b->first) : (a->first > b->first);
								case 1: {
									double ea = a->second.GetElapsed();
									double eb = b->second.GetElapsed();
									return asc ? (ea < eb) : (ea > eb);
								}
								case 2: {
									double ea = a->second.GetElapsed();
									double eb = b->second.GetElapsed();
									double pa = total_time > 0 ? ea / total_time : 0.0;
									double pb = total_time > 0 ? eb / total_time : 0.0;
									return asc ? (pa < pb) : (pa > pb);
								}
								default: break;
							}
							return false;
						});
						specs->SpecsDirty = false;
					}

					// Clip & draw
					ImGuiListClipper clipper; clipper.Begin(entries.size());
					while (clipper.Step()) {
						for (int i = clipper.DisplayStart; i < clipper.DisplayEnd; i++) {
							auto* ent = entries[i];
							double e = ent->second.GetElapsed();
							double pct = total_time > 0 ? e / total_time * 100.0 : 0.0;

							ImGui::TableNextRow();
							ImGui::TableSetColumnIndex(0);
							ImGui::TextUnformatted(ent->first.c_str());
							ImGui::TableSetColumnIndex(1);
							ImGui::Text("%.3fms", e * 1000);
							ImGui::TableSetColumnIndex(2);
							ImGui::Text("%.2f%%", pct);

							ent->second.Reset();
						}
					}

					ImGui::EndTable();
				}
				ImGui::TreePop();
			}
		}

		// Regular Profilers for this thread
		if (!data.profilers.empty()) {
			if (ImGui::TreeNode(("Scoped##" + thread_name).c_str())) {
				if (ImGui::BeginTable(("ProfilerTable##" + thread_name).c_str(), 4,
					ImGuiTableFlags_Borders |
					ImGuiTableFlags_HighlightHoveredColumn |
					ImGuiTableFlags_Sortable |
					ImGuiTableFlags_BordersOuter |
					ImGuiTableFlags_SizingFixedFit)) {

					ImGui::TableSetupColumn("Name", ImGuiTableColumnFlags_DefaultSort | ImGuiTableColumnFlags_WidthStretch);
					ImGui::TableSetupColumn("CPU Time", ImGuiTableColumnFlags_PreferSortAscending | ImGuiTableColumnFlags_WidthFixed);
					ImGui::TableSetupColumn("% DLL", ImGuiTableColumnFlags_PreferSortAscending | ImGuiTableColumnFlags_WidthFixed);
					ImGui::TableSetupColumn("% Frame", ImGuiTableColumnFlags_PreferSortAscending | ImGuiTableColumnFlags_WidthFixed);
					ImGui::TableHeadersRow();

					// Compute per-frame delta
					double current = Time::WorldTimeElapsed();
					static double last = current;
					double delta = current - last;
					last = current;

					// Gather entries
					std::vector<std::pair<const std::string, Profiler>*> entries;
					entries.reserve(data.profilers.size());
					for (auto& kv : data.profilers)
						entries.push_back(&kv);

					if (auto specs = ImGui::TableGetSortSpecs(); specs && specs->SpecsCount > 0) {
						ranges::sort(entries, [&](auto* a, auto* b) {
							for (int si = 0; si < specs->SpecsCount; ++si) {
								auto& spec = specs->Specs[si];
								bool asc = spec.SortDirection == ImGuiSortDirection_Ascending;
								switch (spec.ColumnIndex) {
									case 0:
										if (a->first != b->first) return asc ? (a->first < b->first) : (a->first > b->first);
										break;

									case 1:{
										double ea = a->second.GetElapsed();
										double eb = b->second.GetElapsed();
										if (ea != eb) return asc ? (ea < eb) : (ea > eb);
									} break;

									case 2:{
										double ea = a->second.GetElapsed();
										double eb = b->second.GetElapsed();
										double pa = total_time > 0 ? ea / total_time : 0.0;
										double pb = total_time > 0 ? eb / total_time : 0.0;
										if (pa != pb) return asc ? (pa < pb) : (pa > pb);
									} break;

									case 3: {
										double ea = a->second.GetElapsed();
										double eb = b->second.GetElapsed();
										double fr = delta > 0 ? ea / delta : 0.0;
										double frb = delta > 0 ? eb / delta : 0.0;
										if (fr != frb) return asc ? (fr < frb) : (fr > frb);
									} break;
									default: break;
								}
							}
							return false;
						});
						specs->SpecsDirty = false;
					}

					ImGuiListClipper clipper; clipper.Begin(entries.size());
					while (clipper.Step()) {
						for (int i = clipper.DisplayStart; i < clipper.DisplayEnd; i++) {
							auto* ent = entries[i];
							double e = ent->second.GetElapsed();
							double dllp = total_time > 0 ? e / total_time * 100.0 : 0.0;
							double frp = delta > 0 ? e / delta * 100.0 : 0.0;

							ImGui::TableNextRow();
							ImGui::TableSetColumnIndex(0); ImGui::TextUnformatted(ent->first.c_str());
							ImGui::TableSetColumnIndex(1); ImGui::Text("%.3fms", e * 1000);
							ImGui::TableSetColumnIndex(2); ImGui::Text("%.2f%%", dllp);
							ImGui::TableSetColumnIndex(3); ImGui::Text("%.2f%%", frp);

							ent->second.Reset();
						}
					}

					ImGui::EndTable();
				}
				ImGui::TreePop();
			}
		}
	}

	void Profilers::DisplayReport() {
		static auto& Instance = Profilers::GetSingleton();

		if (!DrawProfiler) {
			// Reset display snapshot
			{
				std::lock_guard lock(snapshot_mutex);
				display_snapshot.is_valid = false;
			}
			return;
		}

		// Always capture snapshot and reset (every frame)
		DisplaySnapshot new_snapshot;
		new_snapshot.total_time = Instance.TotalTime.GetElapsed();

		for (auto& [tid, data] : Instance.thread_data) {
			auto& thread_snap = new_snapshot.threads[tid];
			thread_snap.last_activity = data.last_activity;

			for (auto& [name, prof] : data.profilers) {
				thread_snap.profilers.emplace_back(name, prof.GetElapsed());
			}

			for (auto& [name, prof] : data.entrypoint_profilers) {
				thread_snap.entrypoint_profilers.emplace_back(name, prof.GetElapsed());
			}
		}

		new_snapshot.is_valid = true;

		// Only update display snapshot every interval
		auto now = std::chrono::steady_clock::now();
		double elapsed = std::chrono::duration_cast<std::chrono::duration<double>>(now - last_display_update).count();

		if (elapsed >= update_interval) {
			last_display_update = now;
			Instance.CleanupExpiredThreads();

			std::lock_guard lock(snapshot_mutex);
			display_snapshot = std::move(new_snapshot);
		}

		// Always reset every frame
		for (auto& data : Instance.thread_data | views::values) {
			for (auto& kv : data.profilers | views::values) kv.Reset();
			for (auto& kv : data.entrypoint_profilers | views::values) kv.Reset();
		}
		Instance.TotalTime.Reset();

		// Display from snapshot
		DisplaySnapshot current_snapshot;
		{
			std::lock_guard lock(snapshot_mutex);
			if (!display_snapshot.is_valid) {
				return;
			}
			current_snapshot = display_snapshot;
		}

		ImFontManager::Push(ImFontManager::ActiveFontType::kSubText);

		if (!ImGui::Begin("Profiler Report", nullptr, ImGuiWindowFlags_None)) {
			ImGui::End();
			ImFontManager::Pop();
			return;
		}

		if (ImGui::IsWindowCollapsed()) {
			ImGui::End();
			ImFontManager::Pop();
			return;
		}

		// Display summary
		ImGui::Text("Total DLL Time: %.3fms", current_snapshot.total_time * 1000);
		ImGui::SameLine(); ImGui::Text("FPS: %.2f", ImGui::GetIO().Framerate);
		ImGui::SameLine(); ImGui::Text("Loaded Actors: %d", GtsManager::LoadedActorCount);
		ImGui::SameLine(); ImGui::Text("Threads: %zu", current_snapshot.threads.size());

		if (ImGui::Button("Settings")) {
			ImGui::OpenPopup("ProfilerSettings");
		}

		if (ImGui::BeginPopup("ProfilerSettings")) {
			float expiration = static_cast<float>(Instance.thread_expiration_time);
			if (ImGui::SliderFloat("Thread Expiration (s)", &expiration, 5.0f, 300.0f, "%.1f")) {
				Instance.thread_expiration_time = expiration;
			}
			float interval = static_cast<float>(update_interval);
			if (ImGui::SliderFloat("Update Interval (s)", &interval, 0.1f, 2.0f, "%.2f")) {
				update_interval = interval;
			}
			ImGui::EndPopup();
		}

		ImGui::SameLine();
		if (ImGui::Button("Clear")) {
			std::lock_guard lock(snapshot_mutex);
			display_snapshot = DisplaySnapshot{};
		}

		if (!current_snapshot.threads.empty()) {
			ImGui::Separator();

			std::vector<std::pair<std::thread::id, const DisplaySnapshot::ThreadSnapshot*>> sorted_threads;
			for (auto& [tid, data] : current_snapshot.threads) {
				sorted_threads.emplace_back(tid, &data);
			}
			ranges::sort(sorted_threads, [&](auto const& a, auto const& b) {
				return Instance.GetThreadName(a.first) < Instance.GetThreadName(b.first);
				});

			for (auto& [tid, data] : sorted_threads) {
				std::string name = Instance.GetThreadName(tid);
				DisplayThreadTableFromSnapshot(name, tid, *data, current_snapshot.total_time);
			}
		}

		ImGui::End();
		ImFontManager::Pop();
	}

	void Profilers::DisplayThreadTableFromSnapshot(const std::string& thread_name, std::thread::id tid,
		const DisplaySnapshot::ThreadSnapshot& data, double total_time) {

		double thread_total = 0.0;
		for (const auto& t : data.entrypoint_profilers | views::values) thread_total += t;

		const char* worstEPName = "(none)";
		double worstEPTime = 0.0;
		for (auto& [n, t] : data.entrypoint_profilers) {
			if (t > worstEPTime) {
				worstEPTime = t; worstEPName = n.c_str();
			}
		}

		const char* worstSName = "(none)";
		double worstSTime = 0.0;
		for (auto& [n, t] : data.profilers) {
			if (t > worstSTime) {
				worstSTime = t; worstSName = n.c_str();
			}
		}

		char header_id[128];
		std::snprintf(header_id, sizeof(header_id), "%s##%zu", thread_name.c_str(), std::hash<std::thread::id>{}(tid));
		bool open = ImGui::TreeNode(header_id);

		ImGui::SameLine();
		ImGui::Text("(Total: %.3fms)", thread_total * 1000);
		ImGui::SameLine();
		ImGui::Text("Worst Entrypoint: %s (%.3fms)", worstEPName, worstEPTime * 1000);
		ImGui::SameLine();
		ImGui::Text("Worst Scoped: %s (%.3fms)", worstSName, worstSTime * 1000);

		if (!open) return;

		// Entrypoints
		if (!data.entrypoint_profilers.empty()) {
			if (ImGui::TreeNodeEx(("Entrypoints##" + thread_name).c_str(), ImGuiTreeNodeFlags_DefaultOpen)) {
				if (ImGui::BeginTable(("EntrypointTable##" + thread_name).c_str(), 3,
					ImGuiTableFlags_Borders | ImGuiTableFlags_HighlightHoveredColumn |
					ImGuiTableFlags_Sortable | ImGuiTableFlags_BordersOuter | ImGuiTableFlags_SizingFixedFit)) {

					ImGui::TableSetupColumn("Name", ImGuiTableColumnFlags_DefaultSort | ImGuiTableColumnFlags_WidthStretch);
					ImGui::TableSetupColumn("CPU Time", ImGuiTableColumnFlags_PreferSortAscending | ImGuiTableColumnFlags_WidthFixed);
					ImGui::TableSetupColumn("% Total", ImGuiTableColumnFlags_PreferSortAscending | ImGuiTableColumnFlags_WidthFixed);
					ImGui::TableHeadersRow();

					std::vector<std::pair<std::string, double>> entries = data.entrypoint_profilers;

					if (auto specs = ImGui::TableGetSortSpecs(); specs && specs->SpecsCount > 0) {
						auto& spec = specs->Specs[0];
						bool asc = spec.SortDirection == ImGuiSortDirection_Ascending;
						ranges::sort(entries, [&](auto& a, auto& b) {
							switch (spec.ColumnIndex) {
								case 0: return asc ? (a.first < b.first) : (a.first > b.first);
								case 1: return asc ? (a.second < b.second) : (a.second > b.second);
								case 2:
								{
									double pa = total_time > 0 ? a.second / total_time : 0.0;
									double pb = total_time > 0 ? b.second / total_time : 0.0;
									return asc ? (pa < pb) : (pa > pb);
								}
								default:break;
							}
							return false;
						});
						specs->SpecsDirty = false;
					}

					ImGuiListClipper clipper; clipper.Begin(entries.size());
					while (clipper.Step()) {
						for (int i = clipper.DisplayStart; i < clipper.DisplayEnd; i++) {
							auto& [name, elapsed] = entries[i];
							double pct = total_time > 0 ? elapsed / total_time * 100.0 : 0.0;

							ImGui::TableNextRow();
							ImGui::TableSetColumnIndex(0); ImGui::TextUnformatted(name.c_str());
							ImGui::TableSetColumnIndex(1); ImGui::Text("%.3fms", elapsed * 1000);
							ImGui::TableSetColumnIndex(2); ImGui::Text("%.2f%%", pct);
						}
					}
					ImGui::EndTable();
				}
				ImGui::TreePop();
			}
		}

		// Scoped profilers
		if (!data.profilers.empty()) {
			if (ImGui::TreeNode(("Scoped##" + thread_name).c_str())) {
				if (ImGui::BeginTable(("ProfilerTable##" + thread_name).c_str(), 4,
					ImGuiTableFlags_Borders | ImGuiTableFlags_HighlightHoveredColumn |
					ImGuiTableFlags_Sortable | ImGuiTableFlags_BordersOuter | ImGuiTableFlags_SizingFixedFit)) {

					ImGui::TableSetupColumn("Name", ImGuiTableColumnFlags_DefaultSort | ImGuiTableColumnFlags_WidthStretch);
					ImGui::TableSetupColumn("CPU Time", ImGuiTableColumnFlags_PreferSortAscending | ImGuiTableColumnFlags_WidthFixed);
					ImGui::TableSetupColumn("% DLL", ImGuiTableColumnFlags_PreferSortAscending | ImGuiTableColumnFlags_WidthFixed);
					ImGui::TableSetupColumn("% Frame", ImGuiTableColumnFlags_PreferSortAscending | ImGuiTableColumnFlags_WidthFixed);
					ImGui::TableHeadersRow();

					double current = Time::WorldTimeElapsed();
					static double last = current;
					double delta = current - last;
					last = current;

					std::vector<std::pair<std::string, double>> entries = data.profilers;

					if (auto specs = ImGui::TableGetSortSpecs(); specs && specs->SpecsCount > 0) {
						auto& spec = specs->Specs[0];
						bool asc = spec.SortDirection == ImGuiSortDirection_Ascending;
						ranges::sort(entries, [&](auto& a, auto& b) {
							switch (spec.ColumnIndex) {
								case 0: return asc ? (a.first < b.first) : (a.first > b.first);
								case 1: return asc ? (a.second < b.second) : (a.second > b.second);
								case 2:
								{
									double pa = total_time > 0 ? a.second / total_time : 0.0;
									double pb = total_time > 0 ? b.second / total_time : 0.0;
									return asc ? (pa < pb) : (pa > pb);
								}
								case 3:
								{
									double fa = delta > 0 ? a.second / delta : 0.0;
									double fb = delta > 0 ? b.second / delta : 0.0;
									return asc ? (fa < fb) : (fa > fb);
								}
								default: break;
							}
							return false;
						});
						specs->SpecsDirty = false;
					}

					ImGuiListClipper clipper; clipper.Begin(entries.size());
					while (clipper.Step()) {
						for (int i = clipper.DisplayStart; i < clipper.DisplayEnd; i++) {
							auto& [name, elapsed] = entries[i];
							double dllp = total_time > 0 ? elapsed / total_time * 100.0 : 0.0;
							double frp = delta > 0 ? elapsed / delta * 100.0 : 0.0;

							ImGui::TableNextRow();
							ImGui::TableSetColumnIndex(0); ImGui::TextUnformatted(name.c_str());
							ImGui::TableSetColumnIndex(1); ImGui::Text("%.3fms", elapsed * 1000);
							ImGui::TableSetColumnIndex(2); ImGui::Text("%.2f%%", dllp);
							ImGui::TableSetColumnIndex(3); ImGui::Text("%.2f%%", frp);
						}
					}
					ImGui::EndTable();
				}
				ImGui::TreePop();
			}
		}
		ImGui::TreePop();
	}
}
