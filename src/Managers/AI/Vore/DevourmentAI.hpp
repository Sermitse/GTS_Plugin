#pragma once

namespace GTS {
	std::vector<Actor*> DevourmentAI_FilterList(Actor* a_Pred, const std::vector<Actor*>& a_PotentialPrey);
	void DevourmentAI_Start(Actor* a_Predator, const std::vector<Actor*>& a_PotentialPrey);
}