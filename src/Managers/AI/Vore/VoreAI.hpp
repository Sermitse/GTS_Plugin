#pragma once

namespace GTS {

	std::vector<Actor*> VoreAI_FilterList(Actor* a_Pred, const std::vector<Actor*>& a_PotentialPrey);
	void VoreAI_StartVore(Actor* a_Predator, const std::vector<Actor*>& a_PotentialPrey);
}