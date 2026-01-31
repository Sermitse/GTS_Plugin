#pragma once


namespace GTS {
	std::vector<Actor*> StompKickSwipeAI_FilterList(Actor* a_Pred, const std::vector<Actor*>& a_PotentialPrey);
	void StompAI_Start(Actor* a_Performer, Actor* a_Prey);
	void KickSwipeAI_Start(Actor* a_Performer);
}