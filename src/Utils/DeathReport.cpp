#include "Utils/DeathReport.hpp"
#include "Managers/HighHeel.hpp"
#include "Utils/KillDataUtils.hpp"

using namespace GTS;

namespace {

	constexpr std::string_view lFoot  = "NPC L Foot [Lft ]";
	constexpr std::string_view rFoot  = "NPC R Foot [Rft ]";
	constexpr std::string_view rCalf  = "NPC R Calf [RClf]";
	constexpr std::string_view lCalf  = "NPC L Calf [LClf]";
	constexpr std::string_view rHand  = "NPC R Finger20 [RF20]";
	constexpr std::string_view lHand  = "NPC L Finger20 [LF20]";
	constexpr std::string_view rThigh = "NPC R FrontThigh";
	constexpr std::string_view breast = "NPC Spine2 [Spn2]";
	constexpr std::string_view booty  = "NPC Spine [Spn0]";
	constexpr std::string_view none   = "None";

	void ShockwaveMessage(std::string_view GiantName, std::string_view TinyName, int random) {
		switch (random) {
			case 0: Cprint("{} exploded into pieces from the pressure of {}'s stride", TinyName, GiantName);				break;
			case 1: Cprint("{} has been obliterated by the blast wave radiating from {}'s step", TinyName, GiantName);		break;
			case 2: Cprint("{} evaporated under the devastating force ripple of {}'s stomp", TinyName, GiantName);			break;
			case 3: Cprint("{} was shredded to pieces by the concussive blast from {}", TinyName, GiantName);				break;
			case 4: Cprint("{} brought down a quake with a single tread - {} was erased", GiantName, TinyName);				break;
			case 5: Cprint("{} met their end in the aftermath of {}'s motion", TinyName, GiantName);						break;
			case 6: Cprint("{} ceased to exist as {}'s pressure wave ruptured the atmosphere", TinyName, GiantName);		break;
			case 7: Cprint("{} faced annihilation in the wake of {}'s expanding shockfront", TinyName, GiantName);			break;
			case 8: Cprint("{} got pulverized by sheer proximity to {}", TinyName, GiantName); 								break;
			default: break;
		}
	}

	void VoreMessage_SwallowedAbsorbing(Actor* pred, Actor* prey) {
		if (pred) {
			int random = RandomInt(0, 3);
			if (!prey->IsDead() && !Runtime::HasPerk(pred, Runtime::PERK.GTSPerkFullAssimilation) || random <= 1) {
				Cprint("{} was Swallowed and is now being slowly absorbed by {}", prey->GetDisplayFullName(), pred->GetDisplayFullName());
			} else if (random == 2) {
				Cprint("{} is now absorbing {}", pred->GetDisplayFullName(), prey->GetDisplayFullName());
			} else if (random >= 3) {
				Cprint("{} will soon be completely absorbed by {}", prey->GetDisplayFullName(), pred->GetDisplayFullName());
			}
		}
	}

	void VoreMessage_Absorbed(Actor* pred, std::string_view prey) {
		if (pred) {
			int random = RandomInt(0, 3);
			if (!Runtime::HasPerk(pred, Runtime::PERK.GTSPerkFullAssimilation) || random == 0) {
				Cprint("{} was absorbed by {}", prey, pred->GetDisplayFullName());
			} else if (Runtime::HasPerk(pred, Runtime::PERK.GTSPerkFullAssimilation) && random == 1) {
				Cprint("{} became one with {}", prey, pred->GetDisplayFullName());
			} else if (Runtime::HasPerk(pred, Runtime::PERK.GTSPerkFullAssimilation) && random >= 2) {
				Cprint("{} was greedily devoured by {}", prey, pred->GetDisplayFullName());
			} else {
				Cprint("{} was absorbed by {}", prey, pred->GetDisplayFullName());
			}
		}
	}

	void BreastAbsorbedMessage(std::string_view GiantName, std::string_view TinyName) {
        int random = RandomInt(0, 4);
        if (random == 0) {
            Cprint("Breasts of {} lovingly ate {}", GiantName, TinyName);
        } else if (random == 1) {
            Cprint("{} suddenly disappeared between the breasts of {}", TinyName, GiantName);
        } else if (random == 2) {
            Cprint("Mountains of {} greedily absorbed {}", GiantName, TinyName);
        } else if (random == 3) {
            Cprint("{} became one with the breasts of {}", TinyName, GiantName);
        } else {
            Cprint("{} was gently devoured by the milkers of {}", TinyName, GiantName);
        }
    }

	void ThighSuffocatedMessage(std::string_view GiantName, std::string_view TinyName) {
		int random = RandomInt(0, 5);
		if (random <= 1) {
			Cprint("{} was slowly smothered between {} thighs", TinyName, GiantName);
		} else if (random == 2) {
			Cprint("{} was suffocated by the thighs of {}", TinyName, GiantName);
		} else if (random == 3) {
			Cprint("Thighs of {} suffocated {} to death", GiantName, TinyName);
		} else if (random == 4) {
			Cprint("{} got smothered between the thighs of {}", TinyName, GiantName);
		} else if (random >= 5) {
			Cprint("{} lost life to the thighs of {}", TinyName, GiantName);
		}
	}
	void CrushedMessage(std::string_view GiantName, std::string_view TinyName, int random, bool hh) {
		if (!hh) {
			if (random < 2) {
				Cprint("{} became a bloody stain under {} foot.", TinyName, GiantName);
			} else if (random == 2) {
				Cprint("{} was crushed by the feet of {}", TinyName, GiantName);
			} else if (random == 3) {
				Cprint("Feet of {} crushed {} into nothing", GiantName, TinyName);
			} else if (random == 4) {
				Cprint("{} stept on {} too hard", GiantName, TinyName);
			} else if (random == 5) {
				Cprint("{} got crushed by {}", TinyName, GiantName);
			} else if (random == 6) {
				Cprint("{} ended up being crushed by the {}", TinyName, GiantName);
			} else if (random >= 7) {
				Cprint("{} relentlessly crushed {}", GiantName, TinyName);
			}
			return;
		} else {
			if (random < 2) {
				Cprint("{} became a bloody stain under {} heel.", TinyName, GiantName);
			} else if (random == 2) {
				Cprint("{} was crushed under the heel of {}", TinyName, GiantName);
			} else if (random == 3) {
				Cprint("High Heels of {} obliterated {}", GiantName, TinyName);
			} else if (random == 4) {
				Cprint("{} stept on {} too hard", GiantName, TinyName);
			} else if (random == 5) {
				Cprint("{} got crushed under the heels of {}", TinyName, GiantName);
			} else if (random == 6) {
				Cprint("{} ended up being crushed by heels of {}", TinyName, GiantName);
			} else if (random >= 7) {
				Cprint("{} turned {} into bloody mist", GiantName, TinyName);
			}
		}
	}

	void HandGrabCrushedMessage(std::string_view GiantName, std::string_view TinyName, int random) {
		if (random == 1) {
			Cprint("{} life was squeezed out in {} grip", TinyName, GiantName);
		} else if (random == 2) {
			Cprint("{} Squeezed her palm, unexpectedly crushing {}", GiantName, TinyName);
		} else if (random == 3) {
			Cprint("{} was transformed into bloody mist by the tight grip of {}", TinyName, GiantName);
		} else if (random == 4) {
			Cprint("{} has been crushed inside the hand of {}", TinyName, GiantName);
		} else if (random >= 6) {
			Cprint("{} applied too much pressure to her hand, crushing {}", GiantName, TinyName);
		} else if (random >= 7) {
			Cprint("{} was turned into pulp by the palm of {}", TinyName, GiantName);
		}
	}

	void CollisionMessage(std::string_view GiantName, std::string_view TinyName, int random) {
		if (random <= 3) {
			Cprint("{} exploded after colliding with {}", TinyName, GiantName);
		} else {
			Cprint("{} was instantly turned into mush by the body of {}", TinyName, GiantName);
		}
	}
	void ShrinkToNothingMessage(std::string_view GiantName, std::string_view TinyName, int random) {
		if (random <= 2) {
			Cprint("{} greedily absorbed {}", GiantName, TinyName);
		} else if (random == 3) {
			Cprint("{} completely absorbed {}", GiantName, TinyName);
		} else if (random == 4) {
			Cprint("{} was absorbed by {}", TinyName, GiantName);
		} else if (random == 5) {
			Cprint("{} was shrunken to nothing by {}", TinyName, GiantName);
		} else if (random == 6) {
			Cprint("{} size was completely drained by {}", TinyName, GiantName);
		} else if (random >= 7) {
			Cprint("{} stole all the size from {}, exploding {}", GiantName, TinyName);
		}
	}
	void ThighCrushedMessage(std::string_view GiantName, std::string_view TinyName, int random) {
		if (random == 1) {
			Cprint("{} was crushed to death between {} thighs.", TinyName, GiantName);
		} else if (random <= 3) {
			Cprint("{} crushed {} during leg stretch", GiantName, TinyName);
		} else if (random == 4) {
			Cprint("{} ended life of {} between legs", GiantName, TinyName);
		} else if (random >= 5) {
			Cprint("Thighs of {} took the life of {}", GiantName, TinyName);
		} 
	}
	void BodyCrushedMessage(std::string_view GiantName, std::string_view TinyName, int random) {
		if (random == 1) {
			Cprint("{} was crushed by the body of {}", TinyName, GiantName);
		} else if (random <= 3) {
			Cprint("{} body obliterated {}", GiantName, TinyName);
		} else if (random == 4) {
			Cprint("{} let her body do the job", GiantName);
		} else if (random == 5) {
			Cprint("{} dropped her body onto {}", GiantName, TinyName);
		} else if (random >= 6) {
			Cprint("{} was turned into mush by the body of {}", TinyName, GiantName);
		}
	}
	void ThighSandwichedMessage(std::string_view GiantName, std::string_view TinyName, int random) {
		if (random <= 3) {
			Cprint("{} was crushed by the thighs of {}", TinyName, GiantName);
		} else if (random == 4) {
			Cprint("Thighs of {} gently crushed {}", GiantName, TinyName);
		} else if (random == 5) {
			Cprint("{} has disappeared between the thighs of {}", TinyName, GiantName);
		} else if (random == 6) {
			Cprint("{} was smothered to nothing between the thighs of {}", TinyName, GiantName);
		} else if (random >= 7) {
			Cprint("Thighs of {} sandwiched {} to nothing", GiantName, TinyName);
		}
	}

	void OverkillMessage(std::string_view GiantName, std::string_view TinyName, int random) {
		if (random == 1) {
			Cprint("{} body exploded because of massive size difference with {}", TinyName, GiantName);
		} else if (random == 2) {
			Cprint("{} hit {} with so much force that {} exploded", GiantName, TinyName, TinyName);
		} else if (random == 3) {
			Cprint("{} was pulverized into nothing by {}", TinyName, GiantName);
		} else if (random == 4) {
			Cprint("All that's left from {} after being hit by {} is a bloody pulp", TinyName, GiantName);
		} else if (random >= 6) {
			Cprint("{} couldn't handle enormous hit from {}", TinyName, GiantName);
		} else if (random >= 7) {
			Cprint("{} put so much force into attack that {} turned into red mush", GiantName, TinyName);
		}
	}

	void HitStealMessage(std::string_view GiantName, std::string_view TinyName, int random) {
		if (random <= 2) {
			Cprint("{} body exploded after trying to hit {}", TinyName, GiantName);
		} else if (random == 3) {
			Cprint("Protective magic of {} made {} absorb {}", GiantName, GiantName, TinyName);
		} else if (random > 3) {
			Cprint("{} Tried to kill {}, but ended up being absorbed by the size magic of {}", TinyName, GiantName, GiantName);
		}
	}
	void PoisonOfShrinkMessage(std::string_view GiantName, std::string_view TinyName, int random) {
		if (random <= 2) {
			Cprint("{} exploded into bloody dust", TinyName);
		} else if (random == 3) {
			Cprint("{} suddenly exploded", TinyName);
		} else if (random > 3) {
			Cprint("{} was turned into nothing", TinyName);
		}
	}
	void DamageShareMessage(std::string_view GiantName, std::string_view TinyName, int random) {
		if (random == 1) {
			Cprint("{} received too much damage and was automatically crushed in the hands of {}", TinyName, GiantName);
		} else if (random == 2) {
			Cprint("{} was crushed from receiving too much damage {}", TinyName, GiantName);
		} else if (random == 3) {
			Cprint("{} stopped to be useful, so he was turned into bloody mist in the hands of {}", TinyName, GiantName);
		} else if (random == 4) {
			Cprint("{} took extreme amounts of damage and exploded inside the hands of {}", TinyName, GiantName);
		} else if (random >= 6) {
			Cprint("{} took a little more damage than intended, so her fingers ended up squeezing {} into nothing", GiantName, TinyName);
		} else if (random >= 7) {
			Cprint("{} blocked too much damage and was squeezed into bloody stain by {}", TinyName, GiantName);
		}
	}
	void FootGrindedMessage(std::string_view GiantName, std::string_view TinyName, int random) {
		if (random < 2) {
			Cprint("{} became a bloody stain under {} foot.", TinyName, GiantName);
		} else if (random == 2) {
			Cprint("{} was crushed by the feet of {}", TinyName, GiantName);
		} else if (random == 3 || random == 4) {
			Cprint("Feet of {} crushed {} into nothing", GiantName, TinyName);
		} else if (random == 5 || random == 6) {
			Cprint("{} got crushed by {}", TinyName, GiantName);
		} else if (random >= 7) {
			Cprint("{} relentlessly crushed {}", GiantName, TinyName);
		}
	}
	void MeltedMessage(std::string_view GiantName, std::string_view TinyName, int random) {
		if (random < 2) {
			Cprint("{} was melted by the tongue of {}.", TinyName, GiantName);
		} else if (random == 2) {
			Cprint("{} got absorbed by the tongue of {}", TinyName, GiantName);
		} else if (random == 3 || random == 4) {
			Cprint("Hot tongue of {} melted {} like a candy", GiantName, TinyName);
		} else if (random == 5 || random == 6) {
			Cprint("{} was (un)forunate enough to be melted by the tongue of {} ", TinyName, GiantName);
		} else if (random >= 7) {
			Cprint("Tongue of {} sucked all life out of {}", GiantName, TinyName);
		}
	}
	void BreastGrabMessage(std::string_view GiantName, std::string_view TinyName, int random) {
		if (random < 2) {
			Cprint("{} was weakened and got accidentally crushed by {} breasts", TinyName, GiantName);
		} else if (random == 2) {
			Cprint("{} got unintentionally crushed by the breasts of {}", TinyName, GiantName);
		} else if (random == 3) {
			Cprint("{} left this world by being crushed between the cleavage of {}", TinyName, GiantName);
		} else if (random == 4) {
			Cprint("Breasts of {} squeezed all life out of {}", GiantName, TinyName);
		} else if (random >= 6) {
			Cprint("{} took some damage and ended up crushing {} between her breasts", GiantName, TinyName);
		} else if (random >= 7) {
			Cprint("{} got smothered by soft breasts of {}", TinyName, GiantName);
		}
	}
	void BreastCrushMessage(std::string_view GiantName, std::string_view TinyName, int random) {
		if (random < 2) {
			Cprint("{} was crushed under the soft breasts of {}", TinyName, GiantName);
		} else if (random == 2) {
			Cprint("{} ended up being crushed under the cleavage of {}", TinyName, GiantName);
		} else if (random == 3) {
			Cprint("{} was murdered beneath the breasts of {}", TinyName, GiantName);
		} else if (random == 4) {
			Cprint("Breasts of {} squeezed all life out of {}", GiantName, TinyName);
		} else if (random >= 6) {
			Cprint("Cleavage of {} annihilated {}", GiantName, TinyName);
		} else if (random >= 7) {
			Cprint("{} got smothered under the soft breasts of {}", TinyName, GiantName);
		}
	}
	void ButtCrushMessage(std::string_view GiantName, std::string_view TinyName, int random) {
		if (random < 2) {
			Cprint("{} got crushed by the butt of {}.", TinyName, GiantName);
		} else if (random == 2) {
			Cprint("{} was overwhelmed by the booty of {}", TinyName, GiantName);
		} else if (random == 3) {
			Cprint("Bootie of {} completely pulverized {}", GiantName, TinyName);
		} else if (random == 4) {
			Cprint("Booty of {} completely pulverized {}", GiantName, TinyName);
		} else if (random == 5) {
			Cprint("{} has been squashed by butt attack of {}", TinyName, GiantName);
		} else if (random == 6) {
			Cprint("{} cheeks dropped on {}, turning {} into red paste", GiantName, TinyName);
		} else if (random >= 7) {
			Cprint("{} relentlessly crushed {} with butt attack", GiantName, TinyName);
		}
	}
	void HugCrushMessage(std::string_view GiantName, std::string_view TinyName, int random) {
		if (random < 2) {
			Cprint("{} was hugged to death by {}", TinyName, GiantName);
		} else if (random == 2) {
			Cprint("{} got too carried away hugging {}, crushing {} as a result", GiantName, TinyName);
		} else if (random == 3) {
			Cprint("{} applied too much force to the hugs, killing {}", GiantName, TinyName);
		} else if (random == 4) {
			Cprint("{} couldn't resist hug crushing {}", GiantName, TinyName);
		} else if (random == 5) {
			Cprint("{} failed to escape hugs of death from {}", TinyName, GiantName);
		} else if (random == 6) {
			Cprint("{} got greedy and stole all size from {} through hugs", GiantName, TinyName);
		} else if (random >= 7) {
			Cprint("{} gently hug crushed {}", GiantName, TinyName);
		}
	}
	void KneeCrushMessage(std::string_view GiantName, std::string_view TinyName, int random) {
		if (random < 2) {
			Cprint("{} got crushed under the knee of {}", TinyName, GiantName);
		} else if (random == 2) {
			Cprint("Knee of {} turned {} into nothing", GiantName, TinyName);
		} else if (random == 3) {
			Cprint("{} was unlucky to be under {} knee", TinyName, GiantName);
		} else if (random == 4) {
			Cprint("{} couldn't survive being under the knee of {}", TinyName, GiantName);
		} else if (random == 5) {
			Cprint("For some reason something has crunched under {} knee", GiantName);
		} else if (random == 6) {
			Cprint("{} was obliterated by {} knee", TinyName, GiantName);
		} else if (random >= 7) {
			Cprint("{} didn't realize that it's extremely dangerous to be under {} knees", TinyName, GiantName);
		}
	}

	void HandCrushedMessage(std::string_view GiantName, std::string_view TinyName, int random) {
		if (random < 2) {
			Cprint("{} was accidentally crushed by the hand of {}", TinyName, GiantName);
		} else if (random == 2) {
			Cprint("{} accidentally transformed {} into red jam with under her hands", GiantName, TinyName);
		} else if (random == 3) {
			Cprint("{} died under the hand of {}", TinyName, GiantName);
		} else if (random == 4) {
			Cprint("{} death wasn't even noticed by {}", TinyName, GiantName);
		} else if (random == 5) {
			Cprint("For some reason something has crunched under {} hands", GiantName);
		} else if (random == 6) {
			Cprint("{} wonders why her hands feel warm and wet in some places", GiantName);
		} else if (random >= 7) {
			Cprint("{} heard some crunching around her hands", GiantName, TinyName);
		}
	}
	void HandSlammedMessage(std::string_view GiantName, std::string_view TinyName, int random) {
		if (random < 2) {
			Cprint("Hand of {} aggressively crushed {}", GiantName, TinyName);
		} else if (random == 2) {
			Cprint("{} killed {} with her own hands", GiantName, TinyName);
		} else if (random == 3) {
			Cprint("{} was turned into red jam by the hand of {}", TinyName, GiantName);
		} else if (random == 4) {
			Cprint("Last thing {} saw is a huge hand coming from above", TinyName);
		} else if (random == 5) {
			Cprint("{} slammed {} into bloody mist", GiantName, TinyName);
		} else if (random == 6) {
			Cprint("{} decided to test endurance of {}", GiantName, TinyName);
		} else if (random >= 7) {
			Cprint("{} smashed {} with too much force", GiantName, TinyName);
		}
	}
	void FingerGrindedMessage(std::string_view GiantName, std::string_view TinyName, int random) {
		if (random < 2) {
			Cprint("{} got crushed by the finger of {}", TinyName, GiantName);
		} else {
			Cprint("{} obliteated {} with her index finger", GiantName, TinyName);
		} 
	}
	void HandSwipeMessage(std::string_view GiantName, std::string_view TinyName, int random) {
		if (random < 2) {
			Cprint("{} was sent flying by the {}", TinyName, GiantName);
		} else if (random == 2) {
			Cprint("{} turned {} into bloody mush with her hand", GiantName, TinyName);
		} else if (random == 3) {
			Cprint("{} annoyed {}", TinyName, GiantName);
		} else if (random == 4) {
			Cprint("{} death wasn't even noticed by {}", TinyName, GiantName);
		} else if (random == 5) {
			Cprint("{} applied too much force, crushing {} with her palm", GiantName, TinyName);
		} else if (random == 6) {
			Cprint("{} hit {} with her hand so hard that {} exploded", GiantName, TinyName, TinyName);
		} else if (random >= 7) {
			Cprint("{} wanted to push {} away, but ended up crushing {} instead", GiantName, TinyName, TinyName);
		}
	}
	void KickedMessage(std::string_view GiantName, std::string_view TinyName, int random) {
		if (random < 2) {
			Cprint("{} was sent flying by the kick of {}", TinyName, GiantName);
		} else if (random == 2) {
			Cprint("{} attempted to launch {} into the sky", GiantName, TinyName);
		} else if (random == 3) {
			Cprint("{} tried to learn how to fly from {}", TinyName, GiantName);
		} else if (random == 4) {
			Cprint("{} met the mighty kick of {}", TinyName, GiantName);
		} else if (random == 5) {
			Cprint("{} kicked {} to death", GiantName, TinyName);
		} else if (random == 6) {
			Cprint("{} kick was too strong for {} to handle", GiantName, TinyName);
		} else if (random >= 7) {
			Cprint("{} obliterated {} with a kick", GiantName, TinyName);
		}
	}
}


namespace GTS {

	std::string_view GetDeathNodeName(DamageSource cause) {
		switch (cause) {
			case DamageSource::HandIdleR:
			case DamageSource::HandCrawlRight:
			case DamageSource::HandSwipeRight:
			case DamageSource::HandSlamRight:
			case DamageSource::HandDropRight:
				return rHand;

			case DamageSource::HandIdleL:
			case DamageSource::HandCrawlLeft:
			case DamageSource::HandSwipeLeft:
			case DamageSource::HandSlamLeft:
			case DamageSource::HandDropLeft:
			case DamageSource::HandCrushed: // When killing through grab attack, left hand
				return lHand;

			case DamageSource::KickedRight:
			case DamageSource::CrushedRight:
			case DamageSource::WalkRight:
			case DamageSource::FootIdleR:
			case DamageSource::FootGrindedRight:
			case DamageSource::FootGrindedRight_Impact:
				return rFoot;

			case DamageSource::KickedLeft:
			case DamageSource::CrushedLeft:
			case DamageSource::FootIdleL:
			case DamageSource::WalkLeft:
			case DamageSource::FootGrindedLeft:
			case DamageSource::FootGrindedLeft_Impact:
				return lFoot;

			case DamageSource::KneeIdleR:
			case DamageSource::KneeRight:
			case DamageSource::KneeDropRight:
				return rCalf;

			case DamageSource::KneeIdleL:
			case DamageSource::KneeLeft:
			case DamageSource::KneeDropLeft:
				return lCalf;

			case DamageSource::BodyCrush:
			case DamageSource::Hugs:
			case DamageSource::Breast:
			case DamageSource::BreastImpact:
				return breast;

			case DamageSource::Booty:
				return booty;

			case DamageSource::ThighSandwiched:
			case DamageSource::ThighCrushed:
				return rThigh;

			default:
				return none;
		}
	}

	void ReportDeath(Actor* giant, Actor* tiny, DamageSource cause, bool vore_absorbed) {
		int random = RandomInt(0, 8);

		std::string_view TinyName = tiny->GetDisplayFullName();
		std::string_view GiantName = giant->GetDisplayFullName();

		switch (cause) {

			case DamageSource::Shockwave:
			{

				if (!tiny->IsDead()) {
					RecordKill(giant, tiny, DeathType::kOtherSources);
				}

				ShockwaveMessage(GiantName, TinyName, random);
				break;
			}

			case DamageSource::CrushedLeft:
			case DamageSource::CrushedRight:
			case DamageSource::FootIdleR:
			case DamageSource::FootIdleL:
			case DamageSource::WalkRight:
			case DamageSource::WalkLeft:
			{

				if (!tiny->IsDead()) {
					RecordKill(giant, tiny, DeathType::kCrushed);
				}

				CrushedMessage(GiantName, TinyName, random, HighHeelManager::IsWearingHH(giant));
				break;
			}

			case DamageSource::HandCrushed:
			{
				RecordKill(giant, tiny, DeathType::kGrabCrushed);
				HandGrabCrushedMessage(GiantName, TinyName, random);
				break;
			}

			case DamageSource::Collision:
			{
				RecordKill(giant, tiny, DeathType::kOtherSources);
				CollisionMessage(GiantName, TinyName, random);
				break;
			}

			case DamageSource::ShrinkToNothing:
			{
				if (!tiny->IsDead()) { RecordKill(giant, tiny, DeathType::kShrunkToNothing); }
				ShrinkToNothingMessage(GiantName, TinyName, random);
				break;
			}

			case DamageSource::Vored: 
			{
				vore_absorbed ? VoreMessage_Absorbed(giant, TinyName) : VoreMessage_SwallowedAbsorbing(giant, tiny);
				RecordKill(giant, tiny, !vore_absorbed ? DeathType::kEaten : DeathType::kVoreAbsorbed); //Don't record twice
				break;
			}

			case DamageSource::ThighCrushed: 
			{
				RecordKill(giant, tiny, DeathType::kThighCrushed);
				ThighCrushedMessage(GiantName, TinyName, random);
				break;
			}

			case DamageSource::ThighSandwiched:
			{
				RecordKill(giant, tiny, DeathType::kThighSandwiched);
				ThighSandwichedMessage(GiantName, TinyName, random);
				break;
			}

			case DamageSource::ThighSuffocated:
			{
				RecordKill(giant, tiny, DeathType::kThighSuffocated);
				ThighSuffocatedMessage(GiantName, TinyName);
				break;
			}

			case DamageSource::BodyCrush:
			{
				RecordKill(giant, tiny, DeathType::kCrushed);
				BodyCrushedMessage(GiantName, TinyName, random);
				break;
			}

			case DamageSource::Overkill:
			{
				RecordKill(giant, tiny, DeathType::kOtherSources);
				OverkillMessage(GiantName, TinyName, random);
				break;
			}

			case DamageSource::HitSteal:
			{
				RecordKill(giant, tiny, DeathType::kOtherSources);
				HitStealMessage(GiantName, TinyName, random);
				break;
			}

			case DamageSource::Explode:
			{
				RecordKill(giant, tiny, DeathType::kOtherSources);
				PoisonOfShrinkMessage(GiantName, TinyName, random);
				break;
			}

			case DamageSource::BlockDamage:
			{
				RecordKill(giant, tiny, DeathType::kOtherSources);
				DamageShareMessage(GiantName, TinyName, random);
				break;
			}

			case DamageSource::FootGrindedLeft:
			case DamageSource::FootGrindedRight:
			case DamageSource::FootGrindedLeft_Impact:
			case DamageSource::FootGrindedRight_Impact:
			{
				RecordKill(giant, tiny, DeathType::kGrinded);
				FootGrindedMessage(GiantName, TinyName, random);
				break;
			}

			case DamageSource::Melted:
			{
				RecordKill(giant, tiny, DeathType::kOtherSources);
				MeltedMessage(GiantName, TinyName, random);
				break;
			}

			case DamageSource::Breast:
			{
				RecordKill(giant, tiny, DeathType::kBreastCrushed);
				BreastGrabMessage(GiantName, TinyName, random);
				break;
			}

			case DamageSource::BreastImpact:
			{
				RecordKill(giant, tiny, DeathType::kBreastCrushed);
				BreastCrushMessage(GiantName, TinyName, random);
				break;
			}

			case DamageSource::BreastAbsorb:
			{
				RecordKill(giant, tiny, DeathType::kBreastAbsorbed);
				BreastAbsorbedMessage(GiantName, TinyName);
				break;
			}

			case DamageSource::Booty:
			{
				RecordKill(giant, tiny, DeathType::kButtCrushed);
				ButtCrushMessage(GiantName, TinyName, random);
				break;
			}
			case DamageSource::Crushed:
			{
				RecordKill(giant, tiny, DeathType::kCrushed);
				CrushedMessage(GiantName, TinyName, random, HighHeelManager::IsWearingHH(giant));
				break;
			}
			case DamageSource::Hugs:
			{
				RecordKill(giant, tiny, DeathType::kHugCrushed);
				HugCrushMessage(GiantName, TinyName, random);
				break;
			}

			case DamageSource::KneeLeft:
			case DamageSource::KneeRight:
			case DamageSource::KneeIdleL:
			case DamageSource::KneeIdleR:
			case DamageSource::KneeDropLeft:
			case DamageSource::KneeDropRight:
			{
				RecordKill(giant, tiny, DeathType::kCrushed);
				KneeCrushMessage(GiantName, TinyName, random);
				break;
			}

			case DamageSource::HandCrawlLeft:
			case DamageSource::HandCrawlRight:
			case DamageSource::HandDropRight:
			case DamageSource::HandDropLeft:
			case DamageSource::HandIdleL:
			case DamageSource::HandIdleR:
			{
				RecordKill(giant, tiny, DeathType::kCrushed);
				HandCrushedMessage(GiantName, TinyName, random);
				break;
			}

			case DamageSource::HandSlamLeft:
			case DamageSource::HandSlamRight:
			{
				RecordKill(giant, tiny, DeathType::kCrushed);
				HandSlammedMessage(GiantName, TinyName, random);
				break;
			}

			case DamageSource::RightFinger:
			case DamageSource::LeftFinger:
			case DamageSource::RightFinger_Impact:
			case DamageSource::LeftFinger_Impact:
			{
				RecordKill(giant, tiny, DeathType::kFingerCrushed);
				FingerGrindedMessage(GiantName, TinyName, random);
				break;
			}

			case DamageSource::HandSwipeLeft:
			case DamageSource::HandSwipeRight:
			{
				RecordKill(giant, tiny, DeathType::kKicked);
				HandSwipeMessage(GiantName, TinyName, random);
				break;
			}

			case DamageSource::KickedLeft:
			case DamageSource::KickedRight:
			{
				RecordKill(giant, tiny, DeathType::kKicked);
				KickedMessage(GiantName, TinyName, random);
				break;
			}

			case DamageSource::EraseFromExistence:
			{
				Cprint("{} erased {} from the world", GiantName, TinyName);
				RecordKill(giant, tiny, DeathType::kErasedFromExistence);
				break;
			}

			default: 
			{
				break;
			}
		}		
	}
}
