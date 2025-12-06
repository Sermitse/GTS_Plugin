#include "Utils/KillDataUtils.hpp"

#include "UI/Windows/Other/KillFeedWindow.hpp"

namespace GTS {


    void RecordKill(Actor* a_giant, Actor* a_victim, DeathType a_type) {

		if (!a_giant) {
			return;
		}

		if (a_giant->formID == 0x14 || IsTeammate(a_giant)) {

			PersistentKillCountData* KillData = Persistent::GetKillCountData(a_giant);

			if (KillData) {

				KillFeedWindow::AddKillEntry(a_giant, a_victim, a_type);

				KillData->iTotalKills += 1; // Always increment total kills
				switch (a_type) {
					case DeathType::kShrunkToNothing: 	KillData->iShrunkToNothing += 1;		break;
					case DeathType::kBreastAbsorbed:	KillData->iBreastAbsorbed += 1;			break;
					case DeathType::kBreastCrushed:		KillData->iBreastCrushed += 1;			break;
					case DeathType::kBreastSuffocated:  KillData->iBreastSuffocated +=1;    	break;
					case DeathType::kHugCrushed:		KillData->iHugCrushed += 1;				break;
					case DeathType::kGrabCrushed:		KillData->iGrabCrushed += 1;			break;
					case DeathType::kButtCrushed:		KillData->iButtCrushed += 1;			break;
					case DeathType::kThighCrushed:		KillData->iThighCrushed += 1;			break;
					case DeathType::kThighSuffocated:	KillData->iThighSuffocated += 1;		break;
					case DeathType::kThighSandwiched:	KillData->iThighSandwiched += 1;		break;
					case DeathType::kThighGrinded:		KillData->iThighGrinded += 1;			break;
					case DeathType::kFingerCrushed:		KillData->iFingerCrushed += 1;			break;
					case DeathType::kErasedFromExistence:KillData->iErasedFromExistence += 1;	break;
					case DeathType::kAbsorbed:			KillData->iAbsorbed += 1;				break;
					case DeathType::kCrushed:			KillData->iCrushed += 1;				break;
					case DeathType::kEaten:				KillData->iEaten += 1;					break;
					case DeathType::kKicked:			KillData->iKicked += 1;					break;
					case DeathType::kGrinded:			KillData->iGrinded += 1;				break;
					
					default:
					case DeathType::kOtherSources:		KillData->iOtherSources += 1;			break;
					
				}
			}
		}
	}

	uint32_t GetKillCount(Actor* giant, DeathType Type) {


		if (!giant) {
			return 0;
		}

		if (giant->formID == 0x14 || IsTeammate(giant)) {

			PersistentKillCountData* KillData = Persistent::GetKillCountData(giant);

			if (KillData) {

				switch (Type) {
					case DeathType::kTotalKills:		 return KillData->iTotalKills;
					case DeathType::kShrunkToNothing: 	 return KillData->iShrunkToNothing;
					case DeathType::kBreastAbsorbed:	 return KillData->iBreastAbsorbed;
					case DeathType::kBreastCrushed:		 return KillData->iBreastCrushed;
					case DeathType::kBreastSuffocated:   return KillData->iBreastSuffocated;
					case DeathType::kHugCrushed:		 return KillData->iHugCrushed;
					case DeathType::kGrabCrushed:		 return KillData->iGrabCrushed;
					case DeathType::kButtCrushed:		 return KillData->iButtCrushed;
					case DeathType::kThighCrushed:		 return KillData->iThighCrushed;
					case DeathType::kThighSuffocated:	 return KillData->iThighSuffocated;
					case DeathType::kThighSandwiched:	 return KillData->iThighSandwiched;
					case DeathType::kThighGrinded:		 return KillData->iThighGrinded;
					case DeathType::kFingerCrushed:		 return KillData->iFingerCrushed;
					case DeathType::kErasedFromExistence:return KillData->iErasedFromExistence;
					case DeathType::kAbsorbed:			 return KillData->iAbsorbed;
					case DeathType::kCrushed:			 return KillData->iCrushed;
					case DeathType::kEaten:				 return KillData->iEaten;
					case DeathType::kKicked:			 return KillData->iKicked;
					case DeathType::kGrinded:			 return KillData->iGrinded;

					default:
					case DeathType::kOtherSources:		 return KillData->iOtherSources;
				}
			}
		}
		return 0;
	}
}
