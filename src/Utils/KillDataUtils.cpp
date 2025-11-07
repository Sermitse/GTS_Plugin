#include "Utils/KillDataUtils.hpp"

namespace GTS {

    void IncrementKillCount(Actor* giant, SizeKillType Type) {

		if (!giant) {
			return;
		}

		if (giant->formID == 0x14 || IsTeammate(giant)) {

			PersistentKillCountData* KillData = Persistent::GetKillCountData(giant);

			if (KillData) {

				KillData->iTotalKills += 1; // Always increment total kills
				switch (Type) {
					case SizeKillType::kShrunkToNothing: 	KillData->iShrunkToNothing += 1;		break;
					case SizeKillType::kOtherSources:		KillData->iOtherSources += 1;			break;
					case SizeKillType::kBreastAbsorbed:		KillData->iBreastAbsorbed += 1;			break;
					case SizeKillType::kBreastCrushed:		KillData->iBreastCrushed += 1;			break;
					case SizeKillType::kBreastSuffocated:   KillData->iBreastSuffocated +=1;    	break;
					case SizeKillType::kHugCrushed:			KillData->iHugCrushed += 1;				break;
					case SizeKillType::kGrabCrushed:		KillData->iGrabCrushed += 1;			break;
					case SizeKillType::kButtCrushed:		KillData->iButtCrushed += 1;			break;
					case SizeKillType::kThighCrushed:		KillData->iThighCrushed += 1;			break;
					case SizeKillType::kThighSuffocated:	KillData->iThighSuffocated += 1;		break;
					case SizeKillType::kThighSandwiched:	KillData->iThighSandwiched += 1;		break;
					case SizeKillType::kThighGrinded:		KillData->iThighGrinded += 1;			break;
					case SizeKillType::kFingerCrushed:		KillData->iFingerCrushed += 1;			break;
					case SizeKillType::kErasedFromExistence:KillData->iErasedFromExistence += 1;	break;
					case SizeKillType::kAbsorbed:			KillData->iAbsorbed += 1;				break;
					case SizeKillType::kCrushed:			KillData->iCrushed += 1;				break;
					case SizeKillType::kEaten:				KillData->iEaten += 1;					break;
					case SizeKillType::kKicked:				KillData->iKicked += 1;					break;
					case SizeKillType::kGrinded:			KillData->iGrinded += 1;				break;
				}
			}
		}
	}

	uint32_t GetKillCount(Actor* giant, SizeKillType Type) {


		if (!giant) {
			return 0;
		}

		if (giant->formID == 0x14 || IsTeammate(giant)) {

			PersistentKillCountData* KillData = Persistent::GetKillCountData(giant);

			if (KillData) {

				switch (Type) {
					case SizeKillType::kTotalKills:			return KillData->iTotalKills;
					case SizeKillType::kShrunkToNothing: 	return KillData->iShrunkToNothing;
					case SizeKillType::kOtherSources:		return KillData->iOtherSources;
					case SizeKillType::kBreastAbsorbed:		return KillData->iBreastAbsorbed;
					case SizeKillType::kBreastCrushed:		return KillData->iBreastCrushed;
					case SizeKillType::kBreastSuffocated:   return KillData->iBreastSuffocated;
					case SizeKillType::kHugCrushed:			return KillData->iHugCrushed;
					case SizeKillType::kGrabCrushed:		return KillData->iGrabCrushed;
					case SizeKillType::kButtCrushed:		return KillData->iButtCrushed;
					case SizeKillType::kThighCrushed:		return KillData->iThighCrushed;
					case SizeKillType::kThighSuffocated:	return KillData->iThighSuffocated;
					case SizeKillType::kThighSandwiched:	return KillData->iThighSandwiched;
					case SizeKillType::kThighGrinded:		return KillData->iThighGrinded;
					case SizeKillType::kFingerCrushed:		return KillData->iFingerCrushed;
					case SizeKillType::kErasedFromExistence:return KillData->iErasedFromExistence;
					case SizeKillType::kAbsorbed:			return KillData->iAbsorbed;
					case SizeKillType::kCrushed:			return KillData->iCrushed;
					case SizeKillType::kEaten:				return KillData->iEaten;
					case SizeKillType::kKicked:				return KillData->iKicked;
					case SizeKillType::kGrinded:			return KillData->iGrinded;
				}
			}
		}
		return 0;
	}
}