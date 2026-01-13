#include "Utils/Animation/AnimationVars.hpp"
#include "Utils/Animation/GraphHelpers.hpp"

using namespace AnimationVars::Helpers;

namespace GraphVars {

	//------------------ // NON GTS
	//Bools
	constexpr PSString _EnableSpineRotationDuringHeadTrack = "bHeadTrackSpine";
	constexpr PSString _CollisionInstalled                 = "Collision_Installed"; //Detects 'Precision' mod
	constexpr PSString _IsPandoraGenerated                 = "bIsPandoraGenerated";
	constexpr PSString _IsNemesisGenerated                 = "bIsNemesisGenerated";
	constexpr PSString _IsInJumpState                      = "bInJumpState";
	constexpr PSString _IsSynched                          = "bIsSynced";

	//Ints
	constexpr PSString _IsSneaking                         = "iIsInSneak";
	constexpr PSString _CurrentDefaulltState               = "currentDefaultState";

	//Floats
	constexpr PSString _StaggerMagintute                   = "staggerMagnitude";
	constexpr PSString _CollisionPitchMult                 = "Collision_PitchMult";


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
	constexpr PSString _CanDoPaired            = "GTS_CanDoPaired";
	constexpr PSString _CanCombo               = "GTS_CanCombo";

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
	// constexpr PSString _Unk = "GTS_IsAbsorbing";
	// constexpr PSString _Unk = "GTS_IsCasting";
	// constexpr PSString _Unk = "GTS_IsCrawlGrinding";
	// constexpr PSString _Unk = "GTS_IsCrawlSandwich";
	// constexpr PSString _Unk = "GTS_IsCrawlStomping";
	// constexpr PSString _Unk = "GTS_IsCrawlStomping_Strong";
	// constexpr PSString _Unk = "GTS_IsCrawlThighCrush";
	// constexpr PSString _Unk = "GTS_isCrawling"; //Yeah this one has a lowercase i in "is".
	// constexpr PSString _Unk = "GTS_Pitch_Enable";
	// constexpr PSString _Unk = "GTS_ProneDiving";
	// constexpr PSString _Unk = "GTS_Ready";
	// constexpr PSString _Unk = "GTS_VoreCamera";
	// constexpr PSString _Unk = "GTS_WantBlock_Invert";
	// constexpr PSString _Unk = "GTS_WeapOut";
	// constexpr PSString _Unk = "GTS_isGrowing";

	//Ints
	// constexpr PSString _Unk = "iState_GTSProne";
	// constexpr PSString _Unk = "GTS_WantBlock_Float";

	//Floats
	// constexpr PSString _Unk = "fCur_Def_State";

	// Unknown Var type
	// constexpr PSString _Unk = "GTS_MagicOut";
	// constexpr PSString _Unk = "GTS_Cur_Def_St";
	// constexpr PSString _Unk = "GTS_OffsetPlayback";
	// constexpr PSString _Unk = "GTS_Mag_Crawl_Idle";

}

namespace GTS::AnimationVars {

	namespace Other {

		//---- GETTERS
		//Bool
		bool IsSpineRotationEnabled(RE::Actor* a_actor) { return GetBool(a_actor, GraphVars::_EnableSpineRotationDuringHeadTrack); }
		bool IsVanillaSneaking(RE::Actor* a_actor)      { return GetInt(a_actor, GraphVars::_IsSneaking) > 0; }
		bool IsCollisionInstalled(RE::Actor* a_actor)   { return GetBool(a_actor, GraphVars::_CollisionInstalled); }
		bool IsPandoraGenerated(RE::Actor* a_actor)     { return GetBool(a_actor, GraphVars::_IsPandoraGenerated); }
		bool IsNemesisGenerated(RE::Actor* a_actor)     { return GetBool(a_actor, GraphVars::_IsNemesisGenerated); }
		bool IsJumping(RE::Actor* a_actor)              { return GetBool(a_actor, GraphVars::_IsInJumpState); }
		bool IsSynched(RE::Actor* a_actor)              { return GetBool(a_actor, GraphVars::_IsSynched); }

		//Int
		int CurrentDefaultState(RE::Actor* a_actor)     { return GetInt(a_actor, GraphVars::_CurrentDefaulltState); }

		//Float
		float StaggerMagnitude(RE::Actor* a_actor)      { return GetFloat(a_actor, GraphVars::_StaggerMagintute); }
		float CollisionPitchMult(RE::Actor* a_actor)    { return GetFloat(a_actor, GraphVars::_CollisionPitchMult); }


		//---- SETTERS
		//Bool
		bool SetSpineRotationEnabled(RE::Actor* a_actor, bool a_value) { return SetBool(a_actor, GraphVars::_EnableSpineRotationDuringHeadTrack, a_value); }
		bool SetVanillaSneaking(RE::Actor* a_actor, bool a_value)      { return SetInt(a_actor, GraphVars::_IsSneaking, a_value ? 1 : 0); }
		bool SetCurrentDefaultState(RE::Actor* a_actor, int a_value)   { return SetInt(a_actor, GraphVars::_CurrentDefaulltState, a_value); }

		//Float
		bool SetStaggerMagnitude(RE::Actor* a_actor, float a_value)    { return SetFloat(a_actor, GraphVars::_StaggerMagintute, a_value); }
		bool SetCollisionPitchMult(RE::Actor* a_actor, float a_value)  { return SetFloat(a_actor, GraphVars::_CollisionPitchMult, a_value); }

	}

	namespace Utility {

		//---- GETTERS
		//Bool
		bool BehaviorsInstalled(RE::Actor* a_actor) { return GetBool(a_actor, GraphVars::_IsInstalled); }

	}

	namespace Stomp {

		//---- GETTERS
		//Bool
		bool IsAlternativeStompEnabled(RE::Actor* a_actor) { return GetBool(a_actor, GraphVars::_EnableAlternativeStomp); }
		bool IsAlternativeGrindEnabled(RE::Actor* a_actor) { return GetBool(a_actor, GraphVars::_IsAlternativeGrind); }
		bool IsTrampling(RE::Actor* a_actor)               { return GetBool(a_actor, GraphVars::_IsTrampling); }

		//Float
		float UnderStompBlend(RE::Actor* a_actor) { return GetFloat(a_actor, GraphVars::_StompBlend); }

		//---- SETTERS
		//Bool
		bool SetAlternativeStompEnabled(RE::Actor* a_actor, bool a_value) { return SetBool(a_actor, GraphVars::_EnableAlternativeStomp, a_value); }
		bool SetIsAlternativeGrindEnabled(RE::Actor* a_actor, bool a_value) { return SetBool(a_actor, GraphVars::_IsAlternativeGrind, a_value); }
		bool SetIsTrampling(RE::Actor* a_actor, bool a_value)             { return SetBool(a_actor, GraphVars::_IsTrampling, a_value); }

		//Float
		bool SetUnderStompBlend(RE::Actor* a_actor, float a_value) { return SetFloat(a_actor, GraphVars::_StompBlend, a_value); }

	}

	namespace General {

		//---- GETTERS
		//Bool
		bool IsTransitioning(RE::Actor* a_actor) { return GetBool(a_actor, GraphVars::_IsTransitioning); }
		bool IsBusy(RE::Actor* a_actor)          { return GetBool(a_actor, GraphVars::_IsBusy); }
		bool IsGTSBusy(Actor* a_actor)           { return IsBusy(a_actor) && !CanCombo(a_actor); }
		bool IsSwimming(RE::Actor* a_actor)      { return GetBool(a_actor, GraphVars::_IsSwimming); }
		bool IsHHDisabled(RE::Actor* a_actor)    { return GetBool(a_actor, GraphVars::_DisableHH); }
		bool IsFollower(RE::Actor* a_actor)      { return GetBool(a_actor, GraphVars::_IsFollower); }
		bool IsInDialogue(RE::Actor* a_actor)    { return GetBool(a_actor, GraphVars::_IsInDialogue); }

		bool Gender(RE::Actor* a_actor)                   { return GetBool(a_actor, GraphVars::_Gender); }
		bool SneakTransitionsDisabled(RE::Actor* a_actor) { return GetBool(a_actor, GraphVars::_DisableSneakTrans); }
		bool CanDoPaired(RE::Actor* a_actor)              { return GetBool(a_actor, GraphVars::_CanDoPaired); }
		bool CanCombo(RE::Actor* a_actor)                 { return GetBool(a_actor, GraphVars::_CanCombo); }


		// Float
		float GiantessVelocity(RE::Actor* a_actor) { return GetFloat(a_actor, GraphVars::_GiantessVelocity); }
		float GiantessScale(RE::Actor* a_actor)    { return GetFloat(a_actor, GraphVars::_GiantessScale); }
		float SizeDifference(RE::Actor* a_actor)   { return GetFloat(a_actor, GraphVars::_SizeDifference); }
		float PitchOverride(RE::Actor* a_actor)    { return GetFloat(a_actor, GraphVars::_PitchOverride); }
		float HHOffset(RE::Actor* a_actor)         { return GetFloat(a_actor, GraphVars::_HHOffset); }

		// Int
		int DefState(RE::Actor* a_actor) { return GetInt(a_actor, GraphVars::_DefState); }

		//---- SETTERS
		//Bool
		bool SetIsTransitioning(RE::Actor* a_actor, bool a_value)          { return SetBool(a_actor, GraphVars::_IsTransitioning, a_value); }
		bool SetIsBusy(RE::Actor* a_actor, bool a_value)                   { return SetBool(a_actor, GraphVars::_IsBusy, a_value); }
		bool SetIsSwimming(RE::Actor* a_actor, bool a_value)               { return SetBool(a_actor, GraphVars::_IsSwimming, a_value); }
		bool SetHHDisabled(RE::Actor* a_actor, bool a_value)               { return SetBool(a_actor, GraphVars::_DisableHH, a_value); }
		bool SetIsFollower(RE::Actor* a_actor, bool a_value)               { return SetBool(a_actor, GraphVars::_IsFollower, a_value); }
		bool SetIsInDialogue(RE::Actor* a_actor, bool a_value)             { return SetBool(a_actor, GraphVars::_IsInDialogue, a_value); }
		bool SetGender(RE::Actor* a_actor, bool a_value)                   { return SetBool(a_actor, GraphVars::_Gender, a_value); }
		bool SetSneakTransitionsDisabled(RE::Actor* a_actor, bool a_value) { return SetBool(a_actor, GraphVars::_DisableSneakTrans, a_value); }

		//Int
		bool SetDefState(RE::Actor* a_actor, int a_value) { return SetInt(a_actor, GraphVars::_DefState, a_value); }

		//Float
		bool SetGiantessVelocity(RE::Actor* a_actor, float a_value) { return SetFloat(a_actor, GraphVars::_GiantessVelocity, a_value); }
		bool SetGiantessScale(RE::Actor* a_actor, float a_value)    { return SetFloat(a_actor, GraphVars::_GiantessScale, a_value); }
		bool SetSizeDifference(RE::Actor* a_actor, float a_value)   { return SetFloat(a_actor, GraphVars::_SizeDifference, a_value); }
		bool SetPitchOverride(RE::Actor* a_actor, float a_value)    { return SetFloat(a_actor, GraphVars::_PitchOverride, a_value); }
		bool SetHHOffset(RE::Actor* a_actor, float a_value)         { return SetFloat(a_actor, GraphVars::_HHOffset, a_value); }
	}

	
	namespace Hug {

		//---- GETTERS
		//Bool
		bool IsHugging(RE::Actor* a_actor)         { return GetBool(a_actor, GraphVars::_IsHugging); }
		bool IsHugCrushing(RE::Actor* a_actor)     { return GetBool(a_actor, GraphVars::_IsHugCrushing); }
		bool IsHugHealing(RE::Actor* a_actor)      { return GetBool(a_actor, GraphVars::_IsHugHealing); }
		bool IsHugAbsorbing(RE::Actor* a_actor)    { return GetBool(a_actor, GraphVars::_IsHugAbsorbing); }
		bool IsHuggingTeammate(RE::Actor* a_actor) { return GetBool(a_actor, GraphVars::_IsHuggingTeammate); }
		bool IsHugStarting(RE::Actor* a_actor)     { return GetBool(a_actor, GraphVars::_IsHugStarting); }
		bool IsHasAbsorbedTiny(RE::Actor* a_actor) { return GetBool(a_actor, GraphVars::_TinyAbsorbed); }

		//---- SETTERS
		//Bool
		bool SetIsHugging(RE::Actor* a_actor, bool a_value)         { return SetBool(a_actor, GraphVars::_IsHugging, a_value); }
		bool SetIsHugCrushing(RE::Actor* a_actor, bool a_value)     { return SetBool(a_actor, GraphVars::_IsHugCrushing, a_value); }
		bool SetIsHugHealing(RE::Actor* a_actor, bool a_value)      { return SetBool(a_actor, GraphVars::_IsHugHealing, a_value); }
		bool SetIsHugAbsorbing(RE::Actor* a_actor, bool a_value)    { return SetBool(a_actor, GraphVars::_IsHugAbsorbing, a_value); }
		bool SetIsHuggingTeammate(RE::Actor* a_actor, bool a_value) { return SetBool(a_actor, GraphVars::_IsHuggingTeammate, a_value); }
		bool SetIsHugStarting(RE::Actor* a_actor, bool a_value)     { return SetBool(a_actor, GraphVars::_IsHugStarting, a_value); }
		bool SetIsHasAbsorbedTiny(RE::Actor* a_actor, bool a_value) { return SetBool(a_actor, GraphVars::_TinyAbsorbed, a_value); }
	}

	
	namespace Action {

		//---- GETTERS
		//Bool
		bool IsThighCrushing(RE::Actor* a_actor)            { return GetBool(a_actor, GraphVars::_IsThighCrushing); }
		bool IsThighSandwiching(RE::Actor* a_actor)         { return GetBool(a_actor, GraphVars::_IsThighSandwiching); }
		bool IsSitting(RE::Actor* a_actor)                  { return GetBool(a_actor, GraphVars::_IsSitting); }
		bool IsStomping(RE::Actor* a_actor)                 { return GetBool(a_actor, GraphVars::_IsStomping); }
		bool IsFootGrinding(RE::Actor* a_actor)             { return GetBool(a_actor, GraphVars::_IsFootGrinding); }
		bool IsVoring(RE::Actor* a_actor)                   { return GetBool(a_actor, GraphVars::_IsVoring); }
		bool IsKicking(RE::Actor* a_actor)                  { return GetBool(a_actor, GraphVars::_IsKicking); }
		bool IsInCleavageState(RE::Actor* a_actor)          { return GetBool(a_actor, GraphVars::_IsBoobing); }
		bool IsCleavageZOverrideEnabled(RE::Actor* a_actor) { return GetBool(a_actor, GraphVars::_OverrideZ); }
		bool IsInGrabPlayState(RE::Actor* a_actor)          { return GetBool(a_actor, GraphVars::_IsInGrabPlayState); }
		bool IsKissing(RE::Actor* a_actor)                  { return GetBool(a_actor, GraphVars::_IsKissing); }
		bool IsGrabPlaying(RE::Actor* a_actor)              { return GetBool(a_actor, GraphVars::_IsPlaying); }
		bool IsButtState(RE::Actor* a_actor)                { return GetBool(a_actor, GraphVars::_IsButtState); }
		bool IsButtGrinding(RE::Actor* a_actor)             { return GetBool(a_actor, GraphVars::_IsButtGrinding); }
		bool IsUnderGrinding(RE::Actor* a_actor)            { return GetBool(a_actor, GraphVars::_IsUnderGrinding); }
		bool IsUnderTrampling(RE::Actor* a_actor)           { return GetBool(a_actor, GraphVars::_IsUnderTrampling); }
		bool IsStoringTiny(RE::Actor* a_actor)              { return GetInt(a_actor, GraphVars::_IsStoringTiny) > 0; }

		bool AllowInterupt(RE::Actor* a_actor) { return GetBool(a_actor, GraphVars::_AllowInterupt); }

		//---- SETTERS
		//Bool
		bool SetIsThighCrushing(RE::Actor* a_actor, bool a_value)             { return SetBool(a_actor, GraphVars::_IsThighCrushing, a_value); }
		bool SetIsThighSandwiching(RE::Actor* a_actor, bool a_value)          { return SetBool(a_actor, GraphVars::_IsThighSandwiching, a_value); }
		bool SetIsSitting(RE::Actor* a_actor, bool a_value)                   { return SetBool(a_actor, GraphVars::_IsSitting, a_value); }
		bool SetIsStomping(RE::Actor* a_actor, bool a_value)                  { return SetBool(a_actor, GraphVars::_IsStomping, a_value); }
		bool SetIsFootGrinding(RE::Actor* a_actor, bool a_value)              { return SetBool(a_actor, GraphVars::_IsFootGrinding, a_value); }
		bool SetIsVoring(RE::Actor* a_actor, bool a_value)                    { return SetBool(a_actor, GraphVars::_IsVoring, a_value); }
		bool SetIsKicking(RE::Actor* a_actor, bool a_value)                   { return SetBool(a_actor, GraphVars::_IsKicking, a_value); }
		bool SetIsBoobing(RE::Actor* a_actor, bool a_value)                   { return SetBool(a_actor, GraphVars::_IsBoobing, a_value); }
		bool SetIsCleavageZOverrideEnabled(RE::Actor* a_actor, bool a_value)  { return SetBool(a_actor, GraphVars::_OverrideZ, a_value); }
		bool SetIsInGrabPlayState(RE::Actor* a_actor, bool a_value)           { return SetBool(a_actor, GraphVars::_IsInGrabPlayState, a_value); }
		bool SetIsKissing(RE::Actor* a_actor, bool a_value)                   { return SetBool(a_actor, GraphVars::_IsKissing, a_value); }
		bool SetIsGrabPlaying(RE::Actor* a_actor, bool a_value)               { return SetBool(a_actor, GraphVars::_IsPlaying, a_value); }
		bool SetIsButtState(RE::Actor* a_actor, bool a_value)                 { return SetBool(a_actor, GraphVars::_IsButtState, a_value); }
		bool SetIsButtGrinding(RE::Actor* a_actor, bool a_value)              { return SetBool(a_actor, GraphVars::_IsButtGrinding, a_value); }
		bool SetIsUnderGrinding(RE::Actor* a_actor, bool a_value)             { return SetBool(a_actor, GraphVars::_IsUnderGrinding, a_value); }
		bool SetIsUnderTrampling(RE::Actor* a_actor, bool a_value)            { return SetBool(a_actor, GraphVars::_IsUnderTrampling, a_value); }
		bool SetIsStoringTiny(RE::Actor* a_actor, bool a_value)               { return SetInt(a_actor, GraphVars::_IsStoringTiny, a_value ? 1 : 0); }

		bool SetAllowInterupt(RE::Actor* a_actor, bool a_value) { return SetBool(a_actor, GraphVars::_AllowInterupt, a_value); }

	}

	namespace Grab {


		//---- GETTERS
		//Bool
		bool IsGrabAttacking(RE::Actor* a_actor) { return GetBool(a_actor, GraphVars::_IsGrabAttacking); }
		bool HasGrabbedTiny(RE::Actor* a_actor)  { return GetInt(a_actor, GraphVars::_HasGrabbedTiny) > 0; }
		bool GrabState(RE::Actor* a_actor)       { return GetInt(a_actor, GraphVars::_GrabState) > 0; }

		//---- SETTERS
		//Bool
		bool SetIsGrabAttacking(RE::Actor* a_actor, bool a_value) { return SetBool(a_actor, GraphVars::_IsGrabAttacking, a_value); }
		bool SetHasGrabbedTiny(RE::Actor* a_actor, bool a_value)  { return SetInt(a_actor, GraphVars::_HasGrabbedTiny, a_value ? 1 : 0); }
		bool SetGrabState(RE::Actor* a_actor, bool a_value)       { return SetInt(a_actor, GraphVars::_GrabState, a_value ? 1 : 0); }

	}


	namespace Crawl {

		//---- GETTERS
		//Bool
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

		bool IsCrawlEnabled(RE::Actor* a_actor)       { return GetBool(a_actor, GraphVars::_IsCrawlEnabled); }
		bool IsCrawlVoring(RE::Actor* a_actor)        { return GetBool(a_actor, GraphVars::_IsCrawlVoring); }
		bool IsHandStomping(RE::Actor* a_actor)       { return GetBool(a_actor, GraphVars::_IsHandStomping); }
		bool IsHandStompingStrong(RE::Actor* a_actor) { return GetBool(a_actor, GraphVars::_IsHandStompingStrong); }
		bool IsCrawlButtCrush(RE::Actor* a_actor)     { return GetBool(a_actor, GraphVars::_IsCrawlButtCrush); }
		bool IsNotCrawling(RE::Actor* a_actor)        { return GetBool(a_actor, GraphVars::_IsNotCrawling); }
		bool IsHandAttacking(RE::Actor* a_actor)      { return GetBool(a_actor, GraphVars::_IsHandAttacking); }

		//---- SETTERS
		//Bool
		bool SetIsCrawlEnabled(RE::Actor* a_actor, bool a_value)       { return SetBool(a_actor, GraphVars::_IsCrawlEnabled, a_value); }
		bool SetIsCrawlVoring(RE::Actor* a_actor, bool a_value)        { return SetBool(a_actor, GraphVars::_IsCrawlVoring, a_value); }
		bool SetIsHandStomping(RE::Actor* a_actor, bool a_value)       { return SetBool(a_actor, GraphVars::_IsHandStomping, a_value); }
		bool SetIsHandStompingStrong(RE::Actor* a_actor, bool a_value) { return SetBool(a_actor, GraphVars::_IsHandStompingStrong, a_value); }
		bool SetIsCrawlButtCrush(RE::Actor* a_actor, bool a_value)     { return SetBool(a_actor, GraphVars::_IsCrawlButtCrush, a_value); }
		bool SetIsNotCrawling(RE::Actor* a_actor, bool a_value)        { return SetBool(a_actor, GraphVars::_IsNotCrawling, a_value); }
		bool SetIsHandAttacking(RE::Actor* a_actor, bool a_value)      { return SetBool(a_actor, GraphVars::_IsHandAttacking, a_value); }

	}

	namespace Prone {

		//---- GETTERS
		//Bool
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

		bool IsSBECrawling(RE::Actor* a_actor)  { return GetBool(a_actor, GraphVars::_IsSBECrawling); }
		bool IsProneRolling(RE::Actor* a_actor) { return GetBool(a_actor, GraphVars::_IsProneRolling); }
		bool IsProneDiving(RE::Actor* a_actor)  { return GetBool(a_actor, GraphVars::_IsProneDiving); }

		//---- SETTERS
		//Bool
		bool SetIsSBECrawling(RE::Actor* a_actor, bool a_value)  { return SetBool(a_actor, GraphVars::_IsSBECrawling, a_value); }
		bool SetIsProneRolling(RE::Actor* a_actor, bool a_value) { return SetBool(a_actor, GraphVars::_IsProneRolling, a_value); }
		bool SetIsProneDiving(RE::Actor* a_actor, bool a_value)  { return SetBool(a_actor, GraphVars::_IsProneDiving, a_value); }

	}

	namespace ButtCrush {

		//---- GETTERS
		//Bool
		bool IsButtCrushing(RE::Actor* a_actor) { return GetBool(a_actor, GraphVars::_IsButtCrushing); }

		//---- SETTERS
		//Bool
		bool SetIsButtCrushing(RE::Actor* a_actor, bool a_value) { return SetBool(a_actor, GraphVars::_IsButtCrushing, a_value); }

	}

	namespace Tiny {

		//---- GETTERS
		//Bool
		bool IsBeingGrinded(RE::Actor* a_actor) {

			if (const auto& data = Transient::GetActorData(a_actor)) {
				return data->BeingFootGrinded;
			}

			return GetBool(a_actor, GraphVars::_IsBeingGrinded);
		}

		bool IsBeingHugged(RE::Actor* a_actor)                  { return GetBool(a_actor, GraphVars::_IsBeingHugged); }
		bool IsInBoobs(RE::Actor* a_actor)                      { return !IsHuman(a_actor) ? true : GetBool(a_actor, GraphVars::_IsinBoobs); }  // Bypass incase someone uses creatures...
		bool IsBeingSneakHugged(RE::Actor* a_actor)             { return GetBool(a_actor, GraphVars::_IsBeingSneakHugged); }
		bool IsBeingCrawlHugged(RE::Actor* a_actor)             { return GetBool(a_actor, GraphVars::_IsBeingCrawlHugged); }
		bool IsBeingShrunk(RE::Actor* a_actor)                  { return GetBool(a_actor, GraphVars::_IsBeingShrunk); } // Returns true when performing Wrathful Calamity animation on someone
		bool IsUnderButt(RE::Actor* a_actor)                    { return GetBool(a_actor, GraphVars::_IsUnderButt); }
		bool IsInThighs(RE::Actor* a_actor)                     { return GetBool(a_actor, GraphVars::_IsInThighs); }

		//---- SETTERS
		//Bool
		bool SetIsBeingHugged(RE::Actor* a_actor, bool a_value)      { return SetBool(a_actor, GraphVars::_IsBeingHugged, a_value); }
		bool SetIsBeingSneakHugged(RE::Actor* a_actor, bool a_value) { return SetBool(a_actor, GraphVars::_IsBeingSneakHugged, a_value); }
		bool SetIsBeingCrawlHugged(RE::Actor* a_actor, bool a_value) { return SetBool(a_actor, GraphVars::_IsBeingCrawlHugged, a_value); }
		bool SetIsBeingShrunk(RE::Actor* a_actor, bool a_value)      { return SetBool(a_actor, GraphVars::_IsBeingShrunk, a_value); }
		bool SetIsUnderButt(RE::Actor* a_actor, bool a_value)        { return SetBool(a_actor, GraphVars::_IsUnderButt, a_value); }
		bool SetIsInThighs(RE::Actor* a_actor, bool a_value)         { return SetBool(a_actor, GraphVars::_IsInThighs, a_value); }

	}

	namespace Growth {

		//---- GETTERS
		//Bool
		bool IsGrowing(RE::Actor* a_actor)      { return GetBool(a_actor, GraphVars::_IsGrowing); }
		bool IsShrinking(RE::Actor* a_actor)    { return GetBool(a_actor, GraphVars::_IsShrinking); }
		bool IsChangingSize(RE::Actor* a_actor) { return IsGrowing(a_actor) || IsShrinking(a_actor); } // Used to disallow growth/shrink during specific animations

		//Int
		int GrowthRoll(RE::Actor* a_actor) { return GetInt(a_actor, GraphVars::_GrowthRoll); } 

		//---- SETTERS
		//Bool
		bool SetIsGrowing(RE::Actor* a_actor, bool a_value)   { return SetBool(a_actor, GraphVars::_IsGrowing, a_value); }
		bool SetIsShrinking(RE::Actor* a_actor, bool a_value) { return SetBool(a_actor, GraphVars::_IsShrinking, a_value); }

		//Int
		bool SetGrowthRoll(RE::Actor* a_actor, int a_value) { return SetInt(a_actor, GraphVars::_GrowthRoll, a_value); }

	}

	namespace Spell {

		//---- GETTERS
		//Bool
		bool IsCastingShrink(RE::Actor* a_actor) { return GetBool(a_actor, GraphVars::_IsCastingShrink); }

		//---- SETTERS
		//Bool
		bool SetIsCastingShrink(RE::Actor* a_actor, bool a_value) { return SetBool(a_actor, GraphVars::_IsCastingShrink, a_value); }

	}

	namespace Cleavage {

		//---- GETTERS
		//Bool
		bool IsBoobsDoting(RE::Actor* a_actor)     { return GetBool(a_actor, GraphVars::_IsBoobsDoting); }
		bool IsSuffocating(RE::Actor* a_actor)     { return GetBool(a_actor, GraphVars::_IsSuffocating); }
		bool IsExitingStrangle(RE::Actor* a_actor) { return GetBool(a_actor, GraphVars::_IsBoobsLeaving); }

		//---- SETTERS
		//Bool
		bool SetIsBoobsDoting(RE::Actor* a_actor, bool a_value)     { return SetBool(a_actor, GraphVars::_IsBoobsDoting, a_value); }
		bool SetIsSuffocating(RE::Actor* a_actor, bool a_value)     { return SetBool(a_actor, GraphVars::_IsSuffocating, a_value); }
		bool SetIsExitingStrangle(RE::Actor* a_actor, bool a_value) { return SetBool(a_actor, GraphVars::_IsBoobsLeaving, a_value); }

	}

	namespace Hands {

		//---- GETTERS
		//Bool
		bool IsHandCrushing(RE::Actor* a_actor) { return GetBool(a_actor, GraphVars::_IsHandCrushing); }
		bool CanAim(RE::Actor* a_actor)         { return GetBool(a_actor, GraphVars::_CanAim); }

		//---- SETTERS
		//Bool
		bool SetIsHandCrushing(RE::Actor* a_actor, bool a_value) { return SetBool(a_actor, GraphVars::_IsHandCrushing, a_value); }
		bool SetCanAim(RE::Actor* a_actor, bool a_value)         { return SetBool(a_actor, GraphVars::_CanAim, a_value); }

	}
}
