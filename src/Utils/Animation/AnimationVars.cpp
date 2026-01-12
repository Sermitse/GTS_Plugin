#include "AnimationVars.hpp"

namespace GraphVars {


	//------------------ // NON GTS
	constexpr PSString _EnableSpineRotationDuringHeadTrack = "bHeadTrackSpine";
	constexpr PSString _IsSneaking                         = "iIsInSneak";
	constexpr PSString _StaggerMagintute                   = "staggerMagnitude";
	constexpr PSString _CollisionInstalled                 = "Collision_Installed"; //Detects 'Precision' mod
	constexpr PSString _CollisionPitchMult                 = "Collision_PitchMult";
	constexpr PSString _CurrentDefaulltState               = "currentDefaultState";
	constexpr PSString _IsPandoraGenerated                 = "bIsPandoraGenerated";
	constexpr PSString _IsNemesisGenerated                 = "bIsNemesisGenerated";


	//------------------ // GENERIC
	// Bools
	constexpr PSString _IsTransitioning        = "GTS_Transitioning";          // Enabled When transitioning between either standing, sneaking or crawling
	constexpr PSString _IsInstalled            = "GTS_Installed";              // Enabled by default, to check if the behaviors have been installed correctly
	constexpr PSString _IsBusy                 = "GTS_Busy";                   // Enabled when performing a GTS action of some kind
	constexpr PSString _IsSwimming             = "GTS_Swimming";               // Enabled when we're swimming
	constexpr PSString _DisableHH              = "GTS_DisableHH";              // Enabling this disables any High Heal Offset
	constexpr PSString _IsFollower             = "GTS_IsFollower";             // Used to check if the Actor is Follower or not Used By Hugs
	constexpr PSString _IsInDialogue           = "GTSIsInDialogue";            // Enabled on Followers, Checking if their in dialogue
	constexpr PSString _EnableAlternativeStomp = "GTS_EnableAlternativeStomp"; // When Enabled it makes the GTS play different light stomp Animations
	constexpr PSString _IsAlternativeGrind     = "GTS_IsAlternativeGrind";     // When Enabled it makes the GTS play different light grind Animations
	constexpr PSString _Gender                 = "GTS_Gender";                 // Used to grab the gender of the actor
	constexpr PSString _DisableSneakTrans      = "GTS_DisableSneakTrans";      // Used to disable the Sneak and crawl transition animations

	// Floats
	constexpr PSString _GiantessVelocity       = "GiantessVelocity";           // Fall Velocity of an actor
	constexpr PSString _GiantessScale          = "GiantessScale";              // The Scale of an actor
	constexpr PSString _SizeDifference         = "GTS_SizeDifference";         // Used by the Heal hug to blend the anim between tiny sizes gets the size difference between a gts and her tiny
	constexpr PSString _PitchOverride          = "GTSPitchOverride";           // Used to adjust spines when in dialogue
	constexpr PSString _HHOffset               = "GTS_HHoffset";


	// Ints
	constexpr PSString _HasGrabbedTiny         = "GTS_GrabbedTiny";            // 1 when holding a tiny, 0 when not holding a tiny
	constexpr PSString _IsStoringTiny          = "GTS_Storing_Tiny";           // 1 when we have a tiny between our boobs, 0 when we don't have a tiny
	constexpr PSString _DefState               = "GTS_Def_State";

	//------------------ // HUG RELATED
	// Bools
	constexpr PSString _IsHugging              = "GTS_Hugging";                // Enabled when we're in the hugging state
	constexpr PSString _IsBeingHugged          = "GTS_BeingHugged";            // Enabled on an actor if they're being hugged
	constexpr PSString _IsHugCrushing          = "IsHugCrushing";              // Enabled If we're hug CRUSHING someone
	constexpr PSString _IsHugHealing           = "GTS_IsHugHealing";           // Enabled when we're hug Healing someone
	constexpr PSString _IsHugAbsorbing         = "GTS_IsHugAbsorbing";         // Enabled when we're hug Absorbing someone
	constexpr PSString _IsHuggingTeammate      = "GTS_HuggingTeammate";        // Enabled when we're hugging a follower
	constexpr PSString _IsHugStarting          = "GTS_IsHugStarting";          // Enabled when crawl hugging intro, used to make the tiny copy the GTS's z pos
	constexpr PSString _TinyAbsorbed           = "GTS_TinyAbsorbed";           // Finished absorbing tiny during hug crush

	//------------------ // ACTION RELATED
	// Bools
	constexpr PSString _IsThighCrushing        = "GTS_IsThighCrushing";        // Enabled when thigh crushing(sitting on the floor with tinys between out legs)
	constexpr PSString _IsThighSandwiching     = "GTS_IsThighSandwiching";     // Enabled when thigh sandwiching(when we're sitting on a rune)
	constexpr PSString _IsSitting              = "GTS_Sitting";                // Enabled when doing either of the thigh sandwich or crush(May add more to this is we add more sitting anims)
	constexpr PSString _IsStomping             = "GTS_IsStomping";             // Enabled when we do any of the Stomps
	constexpr PSString _IsFootGrinding         = "GTS_IsFootGrinding";         // Enabled when we do the foot grind after the light stomp
	constexpr PSString _IsVoring               = "GTS_IsVoring";               // Enabled when we vore a target
	constexpr PSString _IsKicking              = "GTS_IsKicking";              // Enabled when we do a Kick
	constexpr PSString _IsBoobing              = "GTS_IsBoobing";              // Enabled when we go into the Mess with Tiny while in Cleavage State
	constexpr PSString _OverrideZ              = "GTS_OverrideZ";              // Does something to the GTS Z position(height) when doing a crawl hug
	constexpr PSString _IsInGrabPlayState      = "GTS_IsInGrabPlayState";      // Enabled when we are in the Hand state
	constexpr PSString _IsKissing              = "GTS_IsKissing";              // Enabled when we Kiss a Tiny during grab-play
	constexpr PSString _IsPlaying              = "GTS_IsPlaying";              // Enabled when we Play with a Tiny, mostly used in the hand state
	constexpr PSString _AllowInterupt          = "GTS_AllowInterupt";          // Only used in one place as of April 2025(Hand State kiss vore) but allows some actions to be cancelled into other anims
	constexpr PSString _IsButtState            = "GTS_IsButtState";            // Enabled when we're in the butt state while sitting on a rune
	constexpr PSString _IsButtGrinding         = "GTS_IsButtGrinding";         // Enabled when we're grinding a tiny to death while sitting on a rune
	constexpr PSString _IsUnderGrinding        = "GTS_IsUnderGrinding";        // Enabled when performing a grinding action that resulted from an understomp
	constexpr PSString _IsUnderTrampling       = "GTS_IsUnderTrampling";       // True when peforming a trample animation that resulted from an understomp


	//------------------ // GRAB RELATED
	//Bools
	constexpr PSString _IsGrabAttacking        = "GTS_IsGrabAttacking";        // Are we doing a GTS Grab attack
	constexpr PSString _GrabState              = "GTS_Grab_State";

	//------------------ // CRAWL RELATED
	//Bools
	constexpr PSString _IsCrawling             = "GTS_IsCrawling";             // This is enabled when we are currently Crawling
	constexpr PSString _IsCrawlEnabled         = "GTS_CrawlEnabled";           // This checks if Crawling is Enabled
	constexpr PSString _IsCrawlVoring          = "GTS_IsCrawlVoring";          // Enabled when we do the Crawl vore animations
	constexpr PSString _IsHandStomping         = "GTS_IsHandStomping";         // Enabled when we do either the right or left LIGHT hand slam(Sneak / crawl Stomp)
	constexpr PSString _IsHandStompingStrong   = "GTS_IsHandStomping_Strong";  // Enabled when we do either the right or left HEAVY hand slam(Sneak / crawl Stomp)
	constexpr PSString _IsCrawlButtCrush       = "GTS_IsCrawlButtCrush";       // Enabled when we do the boob crush(Intro, Loop, Quick Boob crush and Outro)
	constexpr PSString _IsNotCrawling          = "GTS_Is_NOT_Crawling";        // Enabled when we're not Crawling
	constexpr PSString _IsHandAttacking        = "GTS_IsHandAttacking";        // Enabled When we do the swipes(kicks) while crawling

	//------------------ // PRONE RELATED
	//Bools
	constexpr PSString _IsProne                = "GTS_IsProne";                // Enabled when we're in all aspects of Prone (Idle, Move, Rolls and The Dive)
	constexpr PSString _IsSBECrawling          = "IsCrawling";                 // Same as Above but it's from SBE
	constexpr PSString _IsProneRolling         = "GTS_IsProneRolling";         // Enabled when rolling while Prone
	constexpr PSString _IsProneDiving          = "GTS_IsProneDiving";          // Enabled when we're doing a Dive to Prone anim (all 3)

	//------------------ // BUTT CRUSH RELATED
	//Bools
	constexpr PSString _IsButtCrushing         = "GTS_IsButtCrushing";         // Enabled when we're in the butt crush animations


	//------------------ // TRAMPLE/STOMP RELATED
	//Bools
	constexpr PSString _IsTrampling            = "GTS_IsTrampling";            // Enabled when we Start Tramplings(intro and Main anim)

	//Floats
	constexpr PSString _StompBlend             = "GTS_StompBlend";

	//------------------ // TINY RELATED
	//Bools
	constexpr PSString _IsBeingGrinded         = "GTS_BeingGrinded";           // Enabled when a Tiny is being grinded by either the stomp, finger grind, or butt grind
	constexpr PSString _IsinBoobs              = "GTS_IsinBoobs";              // Enabled when a Tiny is between a GTS's Cleavage
	constexpr PSString _IsBeingSneakHugged     = "GTS_Hug_Sneak_Tny";          // Enabled for the Tiny when the GTS Sneaks
	constexpr PSString _IsBeingCrawlHugged     = "GTS_Hug_Crawl_Tny";          // Enabled for the Tiny when the GTS crawls
	constexpr PSString _IsBeingShrunk          = "GTS_Being_Shrunk";           // Enabled when a Tiny is receiving Shrink magic
	constexpr PSString _IsUnderButt            = "GTS_IsUnderButt";            // Enabled when a Tiny is being sat on by the GTS while the GTS is sitting on a rune
	constexpr PSString _IsInThighs             = "GTS_IsinThighs";             // Enabled when a Tiny is being Thigh Sandwiched, disabled when we enter the butt state

	//------------------ // GROWTH RELATED
	//Bools
	constexpr PSString _IsGrowing              = "GTS_IsGrowing";              // Enabled when we're Growing disables input spam
	constexpr PSString _IsShrinking            = "GTS_IsShrinking";            // Enabled when we're Shrinking disables input spam
	constexpr PSString _GrowthRoll             = "GTS_Growth_Roll";            // Used to pick a random growth anim to play

	//------------------ // SHRINK SPELL RELATED
	//Bools
	constexpr PSString _IsCastingShrink        = "GTS_isCastingShrink";        // Enabled when we're casting a shrink spell on a Tiny

	//------------------ // BOOBING RELATED
	//Bools
	constexpr PSString _IsBoobsDoting          = "GTS_Isboobs_doting";         // Enabled when we do the boobing state damage over time
	constexpr PSString _IsSuffocating          = "GTS_IsSuffocating";          // Enabled when we're suffocating a tiny bin the Cleavage state
	constexpr PSString _IsBoobsLeaving         = "GTS_Isboobs_leaving";        // Enabled when GTS leaves DOT state through another anim


	//------------------ // HAND RELATED
	//Bools
	constexpr PSString _IsHandCrushing         = "GTS_IsHandCrushing";         // Enabled when we do the fist crush and hand sandwich in the HandState
	constexpr PSString _CanAim                 = "GTS_CanAim";                 // A test to see if we can aim the flicks


	//------------------ // UNKNOWN
	//Bools

	// constexpr PSString _Unk = "GTSBeh_DontExit";
	// constexpr PSString _Unk = "GTS_CanCombo";
	// constexpr PSString _Unk = "GTS_CanDoPaired";
	// constexpr PSString _Unk = "GTS_Cur_Def_St";
	// constexpr PSString _Unk = "GTS_IsAbsorbing";
	// constexpr PSString _Unk = "GTS_IsCasting";
	// constexpr PSString _Unk = "GTS_IsCrawlGrinding";
	// constexpr PSString _Unk = "GTS_IsCrawlSandwich";
	// constexpr PSString _Unk = "GTS_IsCrawlStomping";
	// constexpr PSString _Unk = "GTS_IsCrawlStomping_Strong";
	// constexpr PSString _Unk = "GTS_IsCrawlThighCrush";
	// constexpr PSString _Unk = "GTS_isCrawling"; //Yeah this one has a lowercase i in "is".
	// constexpr PSString _Unk = "GTS_Mag_Crawl_Idle";
	// constexpr PSString _Unk = "GTS_MagicOut";
	// constexpr PSString _Unk = "GTS_OffsetPlayback";
	// constexpr PSString _Unk = "GTS_Pitch_Enable";
	// constexpr PSString _Unk = "GTS_ProneDiving";
	// constexpr PSString _Unk = "GTS_Ready";
	// constexpr PSString _Unk = "GTS_VoreCamera";
	// constexpr PSString _Unk = "GTS_WantBlock_Float";
	// constexpr PSString _Unk = "GTS_WantBlock_Invert";
	// constexpr PSString _Unk = "GTS_WeapOut";
	// constexpr PSString _Unk = "GTS_isGrowing";
	// constexpr PSString _Unk = "fCur_Def_State";
	// constexpr PSString _Unk = "iState_GTSProne";

}

namespace {

	[[nodiscard]] 
	constexpr __forceinline bool GetBool(const RE::Actor* a_actor, const RE::BSFixedString& a_var) {
		bool result = false;

		if (a_actor) {
			if (a_actor->GetGraphVariableBool(a_var, result)) {
				return result;
			}
		}

		return false;
	}

	[[nodiscard]]
	constexpr __forceinline bool SetBool(RE::Actor* a_actor, const RE::BSFixedString& a_var, bool a_value) {
		return a_actor && a_actor->SetGraphVariableBool(a_var, a_value);
	}

	[[nodiscard]]
	constexpr __forceinline float GetFloat(const RE::Actor* a_actor, const RE::BSFixedString& a_var) {
		float result = 0.0f;

		if (a_actor) {
			if (a_actor->GetGraphVariableFloat(a_var, result)) {
				return result;
			}
		}
		return 0.0f;
	}

	[[nodiscard]]
	constexpr __forceinline bool SetFloat(RE::Actor* a_actor, const RE::BSFixedString& a_var, float a_value) {
		return a_actor && a_actor->SetGraphVariableFloat(a_var, a_value);
	}

	[[nodiscard]]
	constexpr __forceinline float GetInt(const RE::Actor* a_actor, const RE::BSFixedString& a_var) {
		int result = 0;

		if (a_actor) {
			if (a_actor->GetGraphVariableInt(a_var, result)) {
				return result;
			}
		}
		return 0;
	}

	[[nodiscard]]
	constexpr __forceinline bool SetInt(RE::Actor* a_actor, const RE::BSFixedString& a_var, int a_value) {
		return a_actor && a_actor->SetGraphVariableInt(a_var, a_value);
	}

}

namespace GTS::AnimationVars {

	namespace Other {

		bool GetSpineRotationEnabled(RE::Actor* a_actor)               { return GetBool(a_actor, GraphVars::_EnableSpineRotationDuringHeadTrack); }
		bool SetSpineRotationEnabled(RE::Actor* a_actor, bool a_value) { return SetBool(a_actor, GraphVars::_EnableSpineRotationDuringHeadTrack, a_value); }

		bool GetIsVanillaSneaking(RE::Actor* a_actor)                  { return GetInt(a_actor, GraphVars::_IsSneaking) > 0; }
		bool SetIsVanillaSneaking(RE::Actor* a_actor, bool a_value)    { return SetInt(a_actor, GraphVars::_IsSneaking, a_value ? 1 : 0); }

		float GetStaggerMagnitude(RE::Actor* a_actor)                  { return GetFloat(a_actor, GraphVars::_StaggerMagintute); }
		bool SetStaggerMagnitude(RE::Actor* a_actor, float a_value)    { return SetFloat(a_actor, GraphVars::_StaggerMagintute, a_value); }

		bool GetIsCollisionInstalled(RE::Actor* a_actor)               { return GetBool(a_actor, GraphVars::_CollisionInstalled); }
		bool SetIsCollisionInstalled(RE::Actor* a_actor, bool a_value) { return SetBool(a_actor, GraphVars::_CollisionInstalled, a_value); }

		float GetCollisionPitchMult(RE::Actor* a_actor)                { return GetFloat(a_actor, GraphVars::_CollisionPitchMult); }
		bool SetCollisionPitchMult(RE::Actor* a_actor, float a_value)  { return SetFloat(a_actor, GraphVars::_CollisionPitchMult, a_value); }

		int GetCurrentDefaultState(RE::Actor* a_actor)                 { return GetInt(a_actor, GraphVars::_CurrentDefaulltState); }
		bool SetCurrentDefaultState(RE::Actor* a_actor, int a_value)   { return SetInt(a_actor, GraphVars::_CurrentDefaulltState, a_value); }

		bool GetIsPandoraGenerated(RE::Actor* a_actor)                 { return GetBool(a_actor, GraphVars::_IsPandoraGenerated); }
		bool GetIsNemesisGenerated(RE::Actor* a_actor)                 { return GetBool(a_actor, GraphVars::_IsNemesisGenerated); }

	}

	namespace Utility {

		bool GetIsInstalled(RE::Actor* a_actor)               { return GetBool(a_actor, GraphVars::_IsInstalled); }
		bool SetIsInstalled(RE::Actor* a_actor, bool a_value) { return SetBool(a_actor, GraphVars::_IsInstalled, a_value); }

	}

	namespace Stomp {

		bool GetEnableAlternativeStomp(RE::Actor* a_actor) { return GetBool(a_actor, GraphVars::_EnableAlternativeStomp); }
		bool SetEnableAlternativeStomp(RE::Actor* a_actor, bool a_value) { return SetBool(a_actor, GraphVars::_EnableAlternativeStomp, a_value); }

		bool GetIsAlternativeGrind(RE::Actor* a_actor) { return GetBool(a_actor, GraphVars::_IsAlternativeGrind); }
		bool SetIsAlternativeGrind(RE::Actor* a_actor, bool a_value) { return SetBool(a_actor, GraphVars::_IsAlternativeGrind, a_value); }

		bool GetIsTrampling(RE::Actor* a_actor) { return GetBool(a_actor, GraphVars::_IsTrampling); }
		bool SetIsTrampling(RE::Actor* a_actor, bool a_value) { return SetBool(a_actor, GraphVars::_IsTrampling, a_value); }

		bool GetStompBlend(RE::Actor* a_actor) { return GetFloat(a_actor, GraphVars::_IsTrampling); }
		bool SetStompBlend(RE::Actor* a_actor, float a_value) { return SetFloat(a_actor, GraphVars::_IsTrampling, a_value); }

	}

	
	namespace General {

		// Bools
		bool GetIsTransitioning(RE::Actor* a_actor)					       { return GetBool(a_actor, GraphVars::_IsTransitioning); }
		bool SetIsTransitioning(RE::Actor* a_actor, bool a_value)          { return SetBool(a_actor, GraphVars::_IsTransitioning, a_value); }

		bool GetIsBusy(RE::Actor* a_actor)                                 { return GetBool(a_actor, GraphVars::_IsBusy); }
		bool SetIsBusy(RE::Actor* a_actor, bool a_value)                   { return SetBool(a_actor, GraphVars::_IsBusy, a_value); }

		bool GetIsSwimming(RE::Actor* a_actor)                             { return GetBool(a_actor, GraphVars::_IsSwimming); }
		bool SetIsSwimming(RE::Actor* a_actor, bool a_value)               { return SetBool(a_actor, GraphVars::_IsSwimming, a_value); }

		bool GetHHDisabled(RE::Actor* a_actor)                             { return GetBool(a_actor, GraphVars::_DisableHH); }
		bool SetHHDisabled(RE::Actor* a_actor, bool a_value)               { return SetBool(a_actor, GraphVars::_DisableHH, a_value); }

		bool GetIsFollower(RE::Actor* a_actor)                             { return GetBool(a_actor, GraphVars::_IsFollower); }
		bool SetIsFollower(RE::Actor* a_actor, bool a_value)               { return SetBool(a_actor, GraphVars::_IsFollower, a_value); }

		bool GetIsInDialogue(RE::Actor* a_actor)                           { return GetBool(a_actor, GraphVars::_IsInDialogue); }
		bool SetIsInDialogue(RE::Actor* a_actor, bool a_value)             { return SetBool(a_actor, GraphVars::_IsInDialogue, a_value); }

		bool GetGender(RE::Actor* a_actor)                                 { return GetBool(a_actor, GraphVars::_Gender); }
		bool SetGender(RE::Actor* a_actor, bool a_value)                   { return SetBool(a_actor, GraphVars::_Gender, a_value); }

		bool GetDisableSneakTransition(RE::Actor* a_actor)                 { return GetBool(a_actor, GraphVars::_DisableSneakTrans); }
		bool SetDisableSneakTransition(RE::Actor* a_actor, bool a_value)   { return SetBool(a_actor, GraphVars::_DisableSneakTrans, a_value); }


		// Floats

		float GetGiantessVelocity(RE::Actor* a_actor)                      { return GetFloat(a_actor, GraphVars::_GiantessVelocity); }
		bool SetGiantessVelocity(RE::Actor* a_actor, float a_value)        { return SetFloat(a_actor, GraphVars::_GiantessVelocity, a_value); }

		float GetGiantessScale(RE::Actor* a_actor)                         { return GetFloat(a_actor, GraphVars::_GiantessScale); }
		bool SetGiantessScale(RE::Actor* a_actor, float a_value)           { return SetFloat(a_actor, GraphVars::_GiantessScale, a_value); }

		float GetSizeDifference(RE::Actor* a_actor)                        { return GetFloat(a_actor, GraphVars::_SizeDifference); }
		bool SetSizeDifference(RE::Actor* a_actor, float a_value)          { return SetFloat(a_actor, GraphVars::_SizeDifference, a_value); }

		float GetPitchOverride(RE::Actor* a_actor)                         { return GetFloat(a_actor, GraphVars::_PitchOverride); }
		bool SetPitchOverride(RE::Actor* a_actor, float a_value)           { return SetFloat(a_actor, GraphVars::_PitchOverride, a_value); }

		float GetHHOffset(RE::Actor* a_actor)                              { return GetFloat(a_actor, GraphVars::_HHOffset); }
		bool SetHHOffset(RE::Actor* a_actor, float a_value)                { return SetFloat(a_actor, GraphVars::_HHOffset, a_value); }

		int GetDefState(RE::Actor* a_actor) { return GetInt(a_actor, GraphVars::_DefState); }
		bool SetDefState(RE::Actor* a_actor, int a_value) { return SetInt(a_actor, GraphVars::_DefState, a_value); }
	}

	
	namespace Hug {

		// Bools

		bool GetIsHugging(RE::Actor* a_actor)                       { return GetBool(a_actor, GraphVars::_IsHugging); }
		bool SetIsHugging(RE::Actor* a_actor, bool a_value)         { return SetBool(a_actor, GraphVars::_IsHugging, a_value); }

		bool GetIsHugCrushing(RE::Actor* a_actor)                   { return GetBool(a_actor, GraphVars::_IsHugCrushing); }
		bool SetIsHugCrushing(RE::Actor* a_actor, bool a_value)     { return SetBool(a_actor, GraphVars::_IsHugCrushing, a_value); }

		bool GetIsHugHealing(RE::Actor* a_actor)                    { return GetBool(a_actor, GraphVars::_IsHugHealing); }
		bool SetIsHugHealing(RE::Actor* a_actor, bool a_value)      { return SetBool(a_actor, GraphVars::_IsHugHealing, a_value); }

		bool GetIsHugAbsorbing(RE::Actor* a_actor)                  { return GetBool(a_actor, GraphVars::_IsHugAbsorbing); }
		bool SetIsHugAbsorbing(RE::Actor* a_actor, bool a_value)    { return SetBool(a_actor, GraphVars::_IsHugAbsorbing, a_value); }

		bool GetIsHuggingTeammate(RE::Actor* a_actor)               { return GetBool(a_actor, GraphVars::_IsHuggingTeammate); }
		bool SetIsHuggingTeammate(RE::Actor* a_actor, bool a_value) { return SetBool(a_actor, GraphVars::_IsHuggingTeammate, a_value); }

		bool GetIsHugStarting(RE::Actor* a_actor)                   { return GetBool(a_actor, GraphVars::_IsHugStarting); }
		bool SetIsHugStarting(RE::Actor* a_actor, bool a_value)     { return SetBool(a_actor, GraphVars::_IsHugStarting, a_value); }

		bool GetIsHasAbsorbedTiny(RE::Actor* a_actor)               { return GetBool(a_actor, GraphVars::_TinyAbsorbed); }
		bool SetIsHasAbsorbedTiny(RE::Actor* a_actor, bool a_value) { return SetBool(a_actor, GraphVars::_TinyAbsorbed, a_value); }

	}

	
	namespace Action {

		bool GetIsThighCrushing(RE::Actor* a_actor)                  { return GetBool(a_actor, GraphVars::_IsThighCrushing); }
		bool SetIsThighCrushing(RE::Actor* a_actor, bool a_value)    { return SetBool(a_actor, GraphVars::_IsThighCrushing, a_value); }

		bool GetIsThighSandwiching(RE::Actor* a_actor)               { return GetBool(a_actor, GraphVars::_IsThighSandwiching); }
		bool SetIsThighSandwiching(RE::Actor* a_actor, bool a_value) { return SetBool(a_actor, GraphVars::_IsThighSandwiching, a_value); }

		bool GetIsSitting(RE::Actor* a_actor)                        { return GetBool(a_actor, GraphVars::_IsSitting); }
		bool SetIsSitting(RE::Actor* a_actor, bool a_value)          { return SetBool(a_actor, GraphVars::_IsSitting, a_value); }

		bool GetIsStomping(RE::Actor* a_actor)                       { return GetBool(a_actor, GraphVars::_IsStomping); }
		bool SetIsStomping(RE::Actor* a_actor, bool a_value)         { return SetBool(a_actor, GraphVars::_IsStomping, a_value); }

		bool GetIsFootGrinding(RE::Actor* a_actor)                   { return GetBool(a_actor, GraphVars::_IsFootGrinding); }
		bool SetIsFootGrinding(RE::Actor* a_actor, bool a_value)     { return SetBool(a_actor, GraphVars::_IsFootGrinding, a_value); }

		bool GetIsVoring(RE::Actor* a_actor)                         { return GetBool(a_actor, GraphVars::_IsVoring); }
		bool SetIsVoring(RE::Actor* a_actor, bool a_value)           { return SetBool(a_actor, GraphVars::_IsVoring, a_value); }
		
		bool GetIsKicking(RE::Actor* a_actor)                        { return GetBool(a_actor, GraphVars::_IsKicking); }
		bool SetIsKicking(RE::Actor* a_actor, bool a_value)          { return SetBool(a_actor, GraphVars::_IsKicking, a_value); }
		
		bool GetIsBoobing(RE::Actor* a_actor)                        { return GetBool(a_actor, GraphVars::_IsBoobing); }
		bool SetIsBoobing(RE::Actor* a_actor, bool a_value)          { return SetBool(a_actor, GraphVars::_IsBoobing, a_value); }
		
		bool GetCleavageOverrideZ(RE::Actor* a_actor)                { return GetBool(a_actor, GraphVars::_OverrideZ); }
		bool SetCleavageOverrideZ(RE::Actor* a_actor, bool a_value)  { return SetBool(a_actor, GraphVars::_OverrideZ, a_value); }

		bool GetIsInGrabPlayState(RE::Actor* a_actor)                { return GetBool(a_actor, GraphVars::_IsInGrabPlayState); }
		bool SetIsInGrabPlayState(RE::Actor* a_actor, bool a_value)  { return SetBool(a_actor, GraphVars::_IsInGrabPlayState, a_value); }

		bool GetIsKissing(RE::Actor* a_actor)                        { return GetBool(a_actor, GraphVars::_IsKissing); }
		bool SetIsKissing(RE::Actor* a_actor, bool a_value)          { return SetBool(a_actor, GraphVars::_IsKissing, a_value); }

		bool GetIsGrabPlaying(RE::Actor* a_actor)                    { return GetBool(a_actor, GraphVars::_IsPlaying); }
		bool SetIsGrabPlaying(RE::Actor* a_actor, bool a_value)      { return SetBool(a_actor, GraphVars::_IsPlaying, a_value); }

		bool GetAllowInterupt(RE::Actor* a_actor)                    { return GetBool(a_actor, GraphVars::_AllowInterupt); }
		bool SetAllowInterupt(RE::Actor* a_actor, bool a_value)      { return SetBool(a_actor, GraphVars::_AllowInterupt, a_value); }

		bool GetIsButtState(RE::Actor* a_actor)                      { return GetBool(a_actor, GraphVars::_IsButtState); }
		bool SetIsButtState(RE::Actor* a_actor, bool a_value)        { return SetBool(a_actor, GraphVars::_IsButtState, a_value); }

		bool GetIsButtGrinding(RE::Actor* a_actor)                   { return GetBool(a_actor, GraphVars::_IsButtGrinding); }
		bool SetIsButtGrinding(RE::Actor* a_actor, bool a_value)     { return SetBool(a_actor, GraphVars::_IsButtGrinding, a_value); }

		bool GetIsUnderGrinding(RE::Actor* a_actor)                  { return GetBool(a_actor, GraphVars::_IsUnderGrinding); }
		bool SetIsUnderGrinding(RE::Actor* a_actor, bool a_value)    { return SetBool(a_actor, GraphVars::_IsUnderGrinding, a_value); }

		bool GetIsUnderTrampling(RE::Actor* a_actor)                 { return GetBool(a_actor, GraphVars::_IsUnderTrampling); }
		bool SetIsUnderTrampling(RE::Actor* a_actor, bool a_value)   { return SetBool(a_actor, GraphVars::_IsUnderTrampling, a_value); }

		bool GetIsStoringTiny(RE::Actor* a_actor)                    { return GetInt(a_actor, GraphVars::_IsStoringTiny) > 0; }
		bool SetIsStoringTiny(RE::Actor* a_actor, bool a_value)      { return SetInt(a_actor, GraphVars::_IsStoringTiny, a_value ? 1 : 0); }

	}

	namespace Grab {

		bool GetIsGrabAttacking(RE::Actor* a_actor)               { return GetBool(a_actor, GraphVars::_IsGrabAttacking); }
		bool SetIsGrabAttacking(RE::Actor* a_actor, bool a_value) { return SetBool(a_actor, GraphVars::_IsGrabAttacking, a_value); }

		bool GetHasGrabbedTiny(RE::Actor* a_actor)                { return GetInt(a_actor, GraphVars::_HasGrabbedTiny) > 0; }
		bool SetHasGrabbedTiny(RE::Actor* a_actor, bool a_value)  { return SetInt(a_actor, GraphVars::_HasGrabbedTiny, a_value ? 1 : 0); }

		bool GetGrabState(RE::Actor* a_actor)                     { return GetInt(a_actor, GraphVars::_GrabState) > 0; }
		bool SetGrabState(RE::Actor* a_actor, bool a_value)       { return SetInt(a_actor, GraphVars::_GrabState, a_value ? 1 : 0); }

	}


	namespace Crawl {

		bool IsCrawling(RE::Actor* a_actor) {

			if (a_actor) {
				if (TransientActorData* data = Transient::GetActorData(a_actor)) {
					if (a_actor->IsPlayerRef() && a_actor->IsSneaking() && IsFirstPerson()) {
						return data->FPCrawling; // Because we have no FP behaviors
					}
				}
			}
			return GetBool(a_actor, GraphVars::_IsCrawling) && a_actor->IsSneaking();
		}

		bool GetIsCrawlEnabled(RE::Actor* a_actor)                     { return GetBool(a_actor, GraphVars::_IsCrawlEnabled); }
		bool SetIsCrawlEnabled(RE::Actor* a_actor, bool a_value)       { return SetBool(a_actor, GraphVars::_IsCrawlEnabled, a_value); }

		bool GetIsCrawlVoring(RE::Actor* a_actor)                      { return GetBool(a_actor, GraphVars::_IsCrawlVoring); }
		bool SetIsCrawlVoring(RE::Actor* a_actor, bool a_value)        { return SetBool(a_actor, GraphVars::_IsCrawlVoring, a_value); }

		bool GetIsHandStomping(RE::Actor* a_actor)                     { return GetBool(a_actor, GraphVars::_IsHandStomping); }
		bool SetIsHandStomping(RE::Actor* a_actor, bool a_value)       { return SetBool(a_actor, GraphVars::_IsHandStomping, a_value); }

		bool GetIsHandStompingStrong(RE::Actor* a_actor)               { return GetBool(a_actor, GraphVars::_IsHandStompingStrong); }
		bool SetIsHandStompingStrong(RE::Actor* a_actor, bool a_value) { return SetBool(a_actor, GraphVars::_IsHandStompingStrong, a_value); }

		bool GetIsCrawlButtCrush(RE::Actor* a_actor)                   { return GetBool(a_actor, GraphVars::_IsCrawlButtCrush); }
		bool SetIsCrawlButtCrush(RE::Actor* a_actor, bool a_value)     { return SetBool(a_actor, GraphVars::_IsCrawlButtCrush, a_value); }
		
		bool GetIsNotCrawling(RE::Actor* a_actor)                      { return GetBool(a_actor, GraphVars::_IsNotCrawling); }
		bool SetIsNotCrawling(RE::Actor* a_actor, bool a_value)        { return SetBool(a_actor, GraphVars::_IsNotCrawling, a_value); }
		
		bool GetIsHandAttacking(RE::Actor* a_actor)                    { return GetBool(a_actor, GraphVars::_IsHandAttacking); }
		bool SetIsHandAttacking(RE::Actor* a_actor, bool a_value)      { return SetBool(a_actor, GraphVars::_IsHandAttacking, a_value); }

	}

	namespace Prone {

		bool IsProne(RE::Actor* a_actor) {
			if (a_actor) {
				if (TransientActorData* data = Transient::GetActorData(a_actor)) {
					if (a_actor->IsPlayerRef() && a_actor->IsSneaking() && IsFirstPerson()) {
						return data->FPProning; // Because we have no FP behaviors, 
						// ^ it is Needed to fix proning being applied to FP even when Prone is off
					}
				}
			}
			return GetBool(a_actor, GraphVars::_IsProne);;
		}
		
		 
		bool GetIsSBECrawling(RE::Actor* a_actor)                { return GetBool(a_actor, GraphVars::_IsSBECrawling); }
		bool SetIsSBECrawling(RE::Actor* a_actor, bool a_value)  { return SetBool(a_actor, GraphVars::_IsSBECrawling, a_value); }

		bool GetIsProneRolling(RE::Actor* a_actor)               { return GetBool(a_actor, GraphVars::_IsProneRolling); }
		bool SetIsProneRolling(RE::Actor* a_actor, bool a_value) { return SetBool(a_actor, GraphVars::_IsProneRolling, a_value); }

		bool GetIsProneDiving(RE::Actor* a_actor)                { return GetBool(a_actor, GraphVars::_IsProneDiving); }
		bool SetIsProneDiving(RE::Actor* a_actor, bool a_value)  { return SetBool(a_actor, GraphVars::_IsProneDiving, a_value); }

	}

	namespace ButtCrush {

		bool GetIsButtCrushing(RE::Actor* a_actor)               { return GetBool(a_actor, GraphVars::_IsButtCrushing); }
		bool SetIsButtCrushing(RE::Actor* a_actor, bool a_value) { return SetBool(a_actor, GraphVars::_IsButtCrushing, a_value); }

	}

	namespace Tiny {

		bool GetIsBeingHugged(RE::Actor* a_actor)                    { return GetBool(a_actor, GraphVars::_IsBeingHugged); }
		bool SetIsBeingHugged(RE::Actor* a_actor, bool a_value)      { return SetBool(a_actor, GraphVars::_IsBeingHugged, a_value); }

		bool GetIsBeingGrinded(RE::Actor* a_actor)                   { return GetBool(a_actor, GraphVars::_IsBeingGrinded); }
		bool SetIsBeingGrinded(RE::Actor* a_actor, bool a_value)     { return SetBool(a_actor, GraphVars::_IsBeingGrinded, a_value); }

		bool GetIsinBoobs(RE::Actor* a_actor)                        { return GetBool(a_actor, GraphVars::_IsinBoobs); }
		bool SetIsinBoobs(RE::Actor* a_actor, bool a_value)          { return SetBool(a_actor, GraphVars::_IsinBoobs, a_value); }

		bool GetIsBeingSneakHugged(RE::Actor* a_actor)               { return GetBool(a_actor, GraphVars::_IsBeingSneakHugged); }
		bool SetIsBeingSneakHugged(RE::Actor* a_actor, bool a_value) { return SetBool(a_actor, GraphVars::_IsBeingSneakHugged, a_value); }

		bool GetIsBeingCrawlHugged(RE::Actor* a_actor)               { return GetBool(a_actor, GraphVars::_IsBeingCrawlHugged); }
		bool SetIsBeingCrawlHugged(RE::Actor* a_actor, bool a_value) { return SetBool(a_actor, GraphVars::_IsBeingCrawlHugged, a_value); }

		// Returns true when performing Wrathful Calamity animation on someone
		bool GetIsBeingShrunk(RE::Actor* a_actor)                    { return GetBool(a_actor, GraphVars::_IsBeingShrunk); }
		bool SetIsBeingShrunk(RE::Actor* a_actor, bool a_value)      { return SetBool(a_actor, GraphVars::_IsBeingShrunk, a_value); }

		bool GetIsUnderButt(RE::Actor* a_actor)                      { return GetBool(a_actor, GraphVars::_IsUnderButt); }
		bool SetIsUnderButt(RE::Actor* a_actor, bool a_value)        { return SetBool(a_actor, GraphVars::_IsUnderButt, a_value); }

		bool GetIsInThighs(RE::Actor* a_actor)                       { return GetBool(a_actor, GraphVars::_IsInThighs); }
		bool SetIsInThighs(RE::Actor* a_actor, bool a_value)         { return SetBool(a_actor, GraphVars::_IsInThighs, a_value); }

	}

	namespace Growth {

		bool GetIsGrowing(RE::Actor* a_actor)                 { return GetBool(a_actor, GraphVars::_IsGrowing); }
		bool SetIsGrowing(RE::Actor* a_actor, bool a_value)   { return SetBool(a_actor, GraphVars::_IsGrowing, a_value); }

		bool GetIsShrinking(RE::Actor* a_actor)               { return GetBool(a_actor, GraphVars::_IsShrinking); }
		bool SetIsShrinking(RE::Actor* a_actor, bool a_value) { return SetBool(a_actor, GraphVars::_IsShrinking, a_value); }

		int GetGrowthRoll(RE::Actor* a_actor)                { return GetInt(a_actor, GraphVars::_GrowthRoll); }
		bool SetGrowthRoll(RE::Actor* a_actor, int a_value)  { return SetInt(a_actor, GraphVars::_GrowthRoll, a_value); }

		// Used to disallow growth/shrink during specific animations
		bool IsChangingSize(RE::Actor* a_actor) {
			return GetIsGrowing(a_actor) || GetIsShrinking(a_actor);
		}

	}

	namespace Spell {

		bool GetIsCastingShrink(RE::Actor* a_actor)               { return GetBool(a_actor, GraphVars::_IsCastingShrink); }
		bool SetIsCastingShrink(RE::Actor* a_actor, bool a_value) { return SetBool(a_actor, GraphVars::_IsCastingShrink, a_value); }

	}

	namespace Cleavage {
		
		bool GetIsBoobsDoting(RE::Actor* a_actor)                   { return GetBool(a_actor, GraphVars::_IsBoobsDoting); }
		bool SetIsBoobsDoting(RE::Actor* a_actor, bool a_value)     { return SetBool(a_actor, GraphVars::_IsBoobsDoting, a_value); }

		bool GetIsSuffocating(RE::Actor* a_actor)                   { return GetBool(a_actor, GraphVars::_IsSuffocating); }
		bool SetIsSuffocating(RE::Actor* a_actor, bool a_value)     { return SetBool(a_actor, GraphVars::_IsSuffocating, a_value); }

		bool GetIsExitingStrangle(RE::Actor* a_actor)               { return GetBool(a_actor, GraphVars::_IsBoobsLeaving); }
		bool SetIsExitingStrangle(RE::Actor* a_actor, bool a_value) { return SetBool(a_actor, GraphVars::_IsBoobsLeaving, a_value); }

	}

	namespace Hands {

		bool GetIsHandCrushing(RE::Actor* a_actor)               { return GetBool(a_actor, GraphVars::_IsHandCrushing); }
		bool SetIsHandCrushing(RE::Actor* a_actor, bool a_value) { return SetBool(a_actor, GraphVars::_IsHandCrushing, a_value); }

		bool GetCanAim(RE::Actor* a_actor)                       { return GetBool(a_actor, GraphVars::_CanAim); }
		bool SetCanAim(RE::Actor* a_actor, bool a_value)         { return SetBool(a_actor, GraphVars::_CanAim, a_value); }

	}
}

namespace GTS {

	bool IsFootGrinding(Actor* actor) {
		bool grind = false;
		actor->GetGraphVariableBool("GTS_IsFootGrinding", grind);
		return grind;
	}

	bool IsJumping(Actor* actor) {
		bool jumping = false;
		actor->GetGraphVariableBool("bInJumpState", jumping);
		return jumping;
	}

	bool IsTransferingTiny(Actor* actor) { // Reports 'Do we have someone grabed?'
		int grabbed = 0;
		actor->GetGraphVariableInt("GTS_GrabbedTiny", grabbed);
		return grabbed > 0;
	}

	bool IsUsingThighAnimations(Actor* actor) { // Do we currently use Thigh Crush / Thigh Sandwich?
		int sitting = false;
		actor->GetGraphVariableInt("GTS_Sitting", sitting);
		return sitting > 0;
	}

	bool IsSynced(Actor* actor) {
		bool sync = false;
		actor->GetGraphVariableBool("bIsSynced", sync);
		return sync;
	}

	bool CanDoPaired(Actor* actor) {
		bool paired = false;
		actor->GetGraphVariableBool("GTS_CanDoPaired", paired);
		return paired;
	}

	bool IsThighCrushing(Actor* actor) { // Are we currently doing Thigh Crush?
		int crushing = 0;
		actor->GetGraphVariableInt("GTS_IsThighCrushing", crushing);
		return crushing > 0;
	}

	bool IsThighSandwiching(Actor* actor) { // Are we currently Thigh Sandwiching?
		int sandwiching = 0;
		actor->GetGraphVariableInt("GTS_IsThighSandwiching", sandwiching);
		return sandwiching > 0;
	}

	bool IsGtsBusy(Actor* actor) {
		GTS_PROFILE_SCOPE("ActorUtils: IsGtsBusy");
		bool GTSBusy = false;
		actor->GetGraphVariableBool("GTS_Busy", GTSBusy);

		bool Busy = GTSBusy && !CanDoCombo(actor);
		return Busy;
	}

	bool IsStomping(Actor* actor) {
		bool Stomping = false;
		actor->GetGraphVariableBool("GTS_IsStomping", Stomping);

		return Stomping;
	}

	bool IsInCleavageState(Actor* actor) { // For GTS 
		bool Cleavage = false;

		actor->GetGraphVariableBool("GTS_IsBoobing", Cleavage);

		return Cleavage;
	}

	bool IsCleavageZIgnored(Actor* actor) {
		bool ignored = false;

		actor->GetGraphVariableBool("GTS_OverrideZ", ignored);

		return ignored;
	}

	bool IsInsideCleavage(Actor* actor) { // For tinies
		bool InCleavage = false;
		if (!IsHuman(actor)) return true; // Bypass incase someone uses creatures...
		actor->GetGraphVariableBool("GTS_IsinBoobs", InCleavage);

		return InCleavage;
	}

	bool IsKicking(Actor* actor) {
		bool Kicking = false;
		actor->GetGraphVariableBool("GTS_IsKicking", Kicking);

		return Kicking;
	}

	bool IsTrampling(Actor* actor) {
		bool Trampling = false;
		actor->GetGraphVariableBool("GTS_IsTrampling", Trampling);

		return Trampling;
	}

	bool CanDoCombo(Actor* actor) {
		bool Combo = false;
		actor->GetGraphVariableBool("GTS_CanCombo", Combo);
		return Combo;
	}

	bool IsCameraEnabled(Actor* actor) {
		bool Camera = false;
		actor->GetGraphVariableBool("GTS_VoreCamera", Camera);
		return Camera;
	}

	bool IsCrawlVoring(Actor* actor) {
		bool Voring = false;
		actor->GetGraphVariableBool("GTS_IsCrawlVoring", Voring);
		return Voring;//Voring;
	}

	bool IsButtCrushing(Actor* actor) {
		bool ButtCrushing = false;
		actor->GetGraphVariableBool("GTS_IsButtCrushing", ButtCrushing);
		return ButtCrushing;
	}

	bool IsBeingGrinded(Actor* actor) {
		auto transient = Transient::GetActorData(actor);
		bool grinded = false;
		actor->GetGraphVariableBool("GTS_BeingGrinded", grinded);
		if (transient) {
			return transient->BeingFootGrinded;
		}
		return grinded;
	}

	bool IsHugging(Actor* actor) {
		bool hugging = false;
		actor->GetGraphVariableBool("GTS_Hugging", hugging);
		return hugging;
	}

	bool IsBeingHugged(Actor* actor) {
		bool hugged = false;
		actor->GetGraphVariableBool("GTS_BeingHugged", hugged);
		return hugged;
	}

	bool IsUsingAlternativeStomp(Actor* giant) { // Used for alternative grind
		bool alternative = false;
		giant->GetGraphVariableBool("GTS_IsAlternativeGrind", alternative);
		return alternative;
	}

}