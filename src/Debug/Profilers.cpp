#include "Debug/Profilers.hpp"

#include "Managers/GTSManager.hpp"
#include "UI/Core/ImFontManager.hpp"
#include "UI/GTSMenu.hpp"

#ifdef GTS_PROFILER_ENABLED

namespace GTS {

	//---------
	// CTOR
	//---------

	ProfilerHandle Profilers::Profile(std::string_view name) {
		return { name };
	}

	EntrypointProfilerHandle Profilers::ProfileEntrypoint(std::string_view name) {
		return { name };
	}

	//-------------
	// Start/Stop
	//-------------

	void Profilers::Start(std::string_view a_name) {

		auto thread_id = std::this_thread::get_id();
		auto key = std::string(a_name);

		thread_data[thread_id].last_activity = std::chrono::steady_clock::now();
		thread_data[thread_id].profilers.emplace(key, a_name);
		thread_data[thread_id].profilers.at(key).Start();
	}

	void Profilers::Stop(std::string_view a_name) {

		auto thread_id = std::this_thread::get_id();
		auto key = std::string(a_name);

		thread_data[thread_id].last_activity = std::chrono::steady_clock::now();
		thread_data[thread_id].profilers.emplace(key, a_name);
		thread_data[thread_id].profilers.at(key).Stop();
	}

	void Profilers::StartEntrypoint(std::string_view a_name) {

		auto thread_id = std::this_thread::get_id();
		auto key = std::string(a_name);

		thread_data[thread_id].last_activity = std::chrono::steady_clock::now();
		thread_data[thread_id].entrypoint_profilers.emplace(key, a_name);
		thread_data[thread_id].entrypoint_profilers.at(key).Start();

		// Start total time tracking when any entrypoint starts
		if (AnyEntrypointRunning()) {
			TotalTime.Start();
		}
	}

	void Profilers::StopEntrypoint(std::string_view a_name) {

		auto thread_id = std::this_thread::get_id();
		auto key = std::string(a_name);

		thread_data[thread_id].last_activity = std::chrono::steady_clock::now();
		thread_data[thread_id].entrypoint_profilers.emplace(key, a_name);
		thread_data[thread_id].entrypoint_profilers.at(key).Stop();

		// Stop total time tracking when no entrypoints are running
		if (!AnyEntrypointRunning()) {
			TotalTime.Stop();
		}
	}

	//-----------------
	// Draw Report
	//-----------------

	void Profilers::DisplayThreadTable(const std::string& thread_name, ThreadProfilerData& data, double total_time) {

		// Entrypoint Profilers for this thread
		if (!data.entrypoint_profilers.empty()) {

			if (ImGui::TreeNodeEx(("Entrypoints##" + thread_name).c_str(), ImGuiTreeNodeFlags_DefaultOpen)) {

				if (ImGui::BeginTable(("EntrypointTable##" + thread_name).c_str(), 3,
					ImGuiTableFlags_Borders                |
					ImGuiTableFlags_HighlightHoveredColumn |
					ImGuiTableFlags_Sortable               |
					ImGuiTableFlags_BordersOuter           |
					ImGuiTableFlags_SizingFixedFit)) {

					ImGui::TableSetupColumn("Name", ImGuiTableColumnFlags_DefaultSort | ImGuiTableColumnFlags_WidthStretch);
					ImGui::TableSetupColumn("CPU Time", ImGuiTableColumnFlags_PreferSortAscending | ImGuiTableColumnFlags_WidthFixed);
					ImGui::TableSetupColumn("% Total", ImGuiTableColumnFlags_PreferSortAscending | ImGuiTableColumnFlags_WidthFixed);
					ImGui::TableHeadersRow();

					// Gather & sort
					std::vector<std::pair<const std::string, Profiler>*> entries;
					entries.reserve(data.entrypoint_profilers.size());

					for (auto& kv : data.entrypoint_profilers) {
						entries.push_back(&kv);
					}

					if (auto specs = ImGui::TableGetSortSpecs(); specs && specs->SpecsCount > 0) {

						const auto& spec = specs->Specs[0];
						const bool asc = spec.SortDirection == ImGuiSortDirection_Ascending;

						std::ranges::sort(entries, [&](auto* a, auto* b) {

							switch (spec.ColumnIndex) {

								case 0:
								{
									return asc ? (a->first < b->first) : (a->first > b->first);
								}

								case 1:
								{
									double ea = a->second.GetElapsed();
									double eb = b->second.GetElapsed();
									return asc ? (ea < eb) : (ea > eb);
								}

								case 2:
								{
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
					ImGuiListClipper clipper;
					clipper.Begin(entries.size());

					while (clipper.Step()) {
						for (int i = clipper.DisplayStart; i < clipper.DisplayEnd; i++) {
							auto* ent = entries[i];
							const double e = ent->second.GetElapsed();
							const double pct = total_time > 0 ? e / total_time * 100.0 : 0.0;

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

					ImGuiTableFlags_Borders                |
					ImGuiTableFlags_HighlightHoveredColumn |
					ImGuiTableFlags_Sortable               |
					ImGuiTableFlags_BordersOuter           |
					ImGuiTableFlags_SizingFixedFit)) {

					ImGui::TableSetupColumn("Name", ImGuiTableColumnFlags_DefaultSort | ImGuiTableColumnFlags_WidthStretch);
					ImGui::TableSetupColumn("CPU Time", ImGuiTableColumnFlags_PreferSortAscending | ImGuiTableColumnFlags_WidthFixed);
					ImGui::TableSetupColumn("% DLL", ImGuiTableColumnFlags_PreferSortAscending | ImGuiTableColumnFlags_WidthFixed);
					ImGui::TableSetupColumn("% Frame", ImGuiTableColumnFlags_PreferSortAscending | ImGuiTableColumnFlags_WidthFixed);
					ImGui::TableHeadersRow();

					// Compute per-frame delta
					const double current = Time::WorldTimeElapsed();
					static double last = current;
					const double delta = current - last;
					last = current;

					// Gather entries
					std::vector<std::pair<const std::string, Profiler>*> entries;
					entries.reserve(data.profilers.size());

					for (auto& kv : data.profilers) {
						entries.push_back(&kv);
					}

					if (auto specs = ImGui::TableGetSortSpecs(); specs && specs->SpecsCount > 0) {
						std::ranges::sort(entries, [&](auto* a, auto* b) {
							for (int si = 0; si < specs->SpecsCount; ++si) {
								const auto& spec = specs->Specs[si];
								const bool asc = spec.SortDirection == ImGuiSortDirection_Ascending;
								switch (spec.ColumnIndex) {

									case 0:
									{
										if (a->first != b->first) return asc ? (a->first < b->first) : (a->first > b->first);
										
									} break;

									case 1:
									{
										double ea = a->second.GetElapsed();
										double eb = b->second.GetElapsed();
										if (ea != eb) return asc ? (ea < eb) : (ea > eb);
									} break;

									case 2:
									{
										double ea = a->second.GetElapsed();
										double eb = b->second.GetElapsed();
										double pa = total_time > 0 ? ea / total_time : 0.0;
										double pb = total_time > 0 ? eb / total_time : 0.0;
										if (pa != pb) return asc ? (pa < pb) : (pa > pb);
									} break;

									case 3:
									{
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

					ImGuiListClipper clipper;
					clipper.Begin(entries.size());

					while (clipper.Step()) {
						for (int i = clipper.DisplayStart; i < clipper.DisplayEnd; i++) {
							auto* ent = entries[i];
							const double e = ent->second.GetElapsed();
							const double dllp = total_time > 0 ? e / total_time * 100.0 : 0.0;
							const double frp = delta > 0 ? e / delta * 100.0 : 0.0;

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

	void Profilers::DisplayThreadTableFromSnapshot(const std::string& thread_name, std::thread::id tid, const DisplaySnapshot::ThreadSnapshot& data, double total_time) {

		double thread_total = 0.0;
		for (const auto& t : data.entrypoint_profilers | std::views::values) thread_total += t;

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

					tbb::concurrent_vector<std::pair<std::string, double>> entries = data.entrypoint_profilers;

					if (auto specs = ImGui::TableGetSortSpecs(); specs && specs->SpecsCount > 0) {
						auto& spec = specs->Specs[0];
						bool asc = spec.SortDirection == ImGuiSortDirection_Ascending;
						std::ranges::sort(entries, [&](auto& a, auto& b) {
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

					tbb::concurrent_vector<std::pair<std::string, double>> entries = data.profilers;

					if (auto specs = ImGui::TableGetSortSpecs(); specs && specs->SpecsCount > 0) {
						auto& spec = specs->Specs[0];
						bool asc = spec.SortDirection == ImGuiSortDirection_Ascending;
						std::ranges::sort(entries, [&](auto& a, auto& b) {
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

	void Profilers::DisplayReport() {

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
		new_snapshot.total_time = TotalTime.GetElapsed();

		for (auto& [tid, data] : thread_data) {
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
			CleanupExpiredThreads();

			std::lock_guard lock(snapshot_mutex);
			display_snapshot = std::move(new_snapshot);
		}

		// Always reset every frame
		for (auto& data : thread_data | std::views::values) {
			for (auto& kv : data.profilers | std::views::values) kv.Reset();
			for (auto& kv : data.entrypoint_profilers | std::views::values) kv.Reset();
		}
		TotalTime.Reset();

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
		ImGui::SameLine(); ImGui::Text("Loaded Actors: %d", GTSManager::LoadedActorCount);
		ImGui::SameLine(); ImGui::Text("Threads: %zu", current_snapshot.threads.size());

		if (ImGui::Button("Settings")) {
			ImGui::OpenPopup("ProfilerSettings");
		}

		if (ImGui::BeginPopup("ProfilerSettings")) {
			float expiration = static_cast<float>(thread_expiration_time);
			if (ImGui::SliderFloat("Thread Expiration (s)", &expiration, 5.0f, 300.0f, "%.1f")) {
				thread_expiration_time = expiration;
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
			thread_data.clear();
			thread_names.clear();
			TotalTime.Reset();
		}

		if (!current_snapshot.threads.empty()) {
			ImGui::Separator();

			std::vector<std::pair<std::thread::id, const DisplaySnapshot::ThreadSnapshot*>> sorted_threads;
			for (auto& [tid, data] : current_snapshot.threads) {
				sorted_threads.emplace_back(tid, &data);
			}

			std::ranges::sort(sorted_threads, [&](auto const& a, auto const& b) {
				return GetThreadName(a.first) < GetThreadName(b.first);
			});

			for (auto& [tid, data] : sorted_threads) {
				std::string name = GetThreadName(tid);
				DisplayThreadTableFromSnapshot(name, tid, *data, current_snapshot.total_time);
			}
		}

		ImGui::End();
		ImFontManager::Pop();
	}

	//---------------
	// Thread Expire
	//---------------

	void Profilers::SetThreadExpirationTime(double seconds) {
		thread_expiration_time = seconds;
	}

	double Profilers::GetThreadExpirationTime() {
		return thread_expiration_time;
	}


	//-----------
	// Running
	//-----------

	bool Profilers::AnyRunning() {
		for (const auto& data : thread_data | std::views::values) {
			for (auto& profiler : data.profilers | std::views::values) {
				if (profiler.IsRunning()) {
					return true;
				}
			}
		}
		return false;
	}

	bool Profilers::AnyEntrypointRunning() {
		for (const auto& data : thread_data | std::views::values) {
			for (auto& profiler : data.entrypoint_profilers | std::views::values) {
				if (profiler.IsRunning()) {
					return true;
				}
			}
		}
		return false;
	}

	//------------
	// Other
	//------------

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
			// safe because no iterator involved
			thread_data.unsafe_erase(tid);
			thread_names.unsafe_erase(tid);
		}
	}
}

#endif //GTS_PROFILER_ENABLED