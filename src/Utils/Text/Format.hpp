#pragma once

namespace GTS {
	std::string GetRawName(const void* obj_c);
	std::string Vector2Str(const hkVector4& vector);
	std::string Vector2Str(const hkVector4* vector);
	std::string Vector2Str(const NiPoint3& vector);
	std::string Vector2Str(const NiPoint3* vector);
	std::string Vector2Str(const NiQuaternion& vector);
	std::string Vector2Str(const NiQuaternion* vector);
}


template <> struct fmt::formatter<RE::BSFixedString> : formatter<std::string_view> {
	auto format(const RE::BSFixedString& v, format_context& ctx) const {
		string_view name = "<empty>";
		if (v.c_str()) {
			name = v.c_str();
		}
		return formatter<string_view>::format(name, ctx);
	}
};

template <> struct fmt::formatter<RE::ACTOR_VALUE_MODIFIER>: formatter<string_view> {
	auto format(RE::ACTOR_VALUE_MODIFIER v, format_context& ctx) const {
		string_view name = "unknown";
		switch (v) {
			case RE::ACTOR_VALUE_MODIFIER::kPermanent: name = "kPermanent"; break;
			case RE::ACTOR_VALUE_MODIFIER::kTemporary: name = "kTemporary"; break;
			case RE::ACTOR_VALUE_MODIFIER::kDamage: name = "kDamage"; break;
		}
		return formatter<string_view>::format(name, ctx);
	}
};

template <> struct fmt::formatter<RE::COL_LAYER>: formatter<string_view> {
	auto format(RE::COL_LAYER v, format_context& ctx) const {
		string_view name = "unknown";
		switch (v) {
			case RE::COL_LAYER::kUnidentified: name = "kUnidentified"; break;
			case RE::COL_LAYER::kStatic: name = "kStatic"; break;
			case RE::COL_LAYER::kAnimStatic: name = "kAnimStatic"; break;
			case RE::COL_LAYER::kTransparent: name = "kTransparent"; break;
			case RE::COL_LAYER::kClutter: name = "kClutter"; break;
			case RE::COL_LAYER::kWeapon: name = "kWeapon"; break;
			case RE::COL_LAYER::kProjectile: name = "kProjectile"; break;
			case RE::COL_LAYER::kSpell: name = "kSpell"; break;
			case RE::COL_LAYER::kBiped: name = "kBiped"; break;
			case RE::COL_LAYER::kTrees: name = "kTrees"; break;
			case RE::COL_LAYER::kProps: name = "kProps"; break;
			case RE::COL_LAYER::kWater: name = "kWater"; break;
			case RE::COL_LAYER::kTrigger: name = "kTrigger"; break;
			case RE::COL_LAYER::kTerrain: name = "kTerrain"; break;
			case RE::COL_LAYER::kTrap: name = "kTrap"; break;
			case RE::COL_LAYER::kNonCollidable: name = "kNonCollidable"; break;
			case RE::COL_LAYER::kCloudTrap: name = "kCloudTrap"; break;
			case RE::COL_LAYER::kGround: name = "kGround"; break;
			case RE::COL_LAYER::kPortal: name = "kPortal"; break;
			case RE::COL_LAYER::kDebrisSmall: name = "kDebrisSmall"; break;
			case RE::COL_LAYER::kDebrisLarge: name = "kDebrisLarge"; break;
			case RE::COL_LAYER::kAcousticSpace: name = "kAcousticSpace"; break;
			case RE::COL_LAYER::kActorZone: name = "kActorZone"; break;
			case RE::COL_LAYER::kProjectileZone: name = "kProjectileZone"; break;
			case RE::COL_LAYER::kGasTrap: name = "kGasTrap"; break;
			case RE::COL_LAYER::kShellCasting: name = "kShellCasting"; break;
			case RE::COL_LAYER::kTransparentWall: name = "kTransparentWall"; break;
			case RE::COL_LAYER::kInvisibleWall: name = "kInvisibleWall"; break;
			case RE::COL_LAYER::kTransparentSmallAnim: name = "kTransparentSmallAnim"; break;
			case RE::COL_LAYER::kClutterLarge: name = "kClutterLarge"; break;
			case RE::COL_LAYER::kCharController: name = "kCharController"; break;
			case RE::COL_LAYER::kStairHelper: name = "kStairHelper"; break;
			case RE::COL_LAYER::kDeadBip: name = "kDeadBip"; break;
			case RE::COL_LAYER::kBipedNoCC: name = "kBipedNoCC"; break;
			case RE::COL_LAYER::kAvoidBox: name = "kAvoidBox"; break;
			case RE::COL_LAYER::kCollisionBox: name = "kCollisionBox"; break;
			case RE::COL_LAYER::kCameraSphere: name = "kCameraSphere"; break;
			case RE::COL_LAYER::kDoorDetection: name = "kDoorDetection"; break;
			case RE::COL_LAYER::kConeProjectile: name = "kConeProjectile"; break;
			case RE::COL_LAYER::kCamera: name = "kCamera"; break;
			case RE::COL_LAYER::kItemPicker: name = "kItemPicker"; break;
			case RE::COL_LAYER::kLOS: name = "kLOS"; break;
			case RE::COL_LAYER::kPathingPick: name = "kPathingPick"; break;
			case RE::COL_LAYER::kUnused0: name = "kUnused0"; break;
			case RE::COL_LAYER::kUnused1: name = "kUnused1"; break;
			case RE::COL_LAYER::kSpellExplosion: name = "kSpellExplosion"; break;
			case RE::COL_LAYER::kDroppingPick: name = "kDroppingPick"; break;
		}
		return formatter<string_view>::format(name, ctx);
	}
};

template <> struct fmt::formatter<RE::DEFAULT_OBJECT>: formatter<string_view> {
	auto format(RE::DEFAULT_OBJECT v, format_context& ctx) const {
		string_view name = "unknown";
		switch (v) {
			case RE::DEFAULT_OBJECT::kWerewolfSpell: name = "kWerewolfSpell"; break;
			case RE::DEFAULT_OBJECT::kSittingAngleLimit: name = "kSittingAngleLimit"; break;
			case RE::DEFAULT_OBJECT::kAllowPlayerShout: name = "kAllowPlayerShout"; break;
			case RE::DEFAULT_OBJECT::kGold: name = "kGold"; break;
			case RE::DEFAULT_OBJECT::kLockpick: name = "kLockpick"; break;
			case RE::DEFAULT_OBJECT::kSkeletonKey: name = "kSkeletonKey"; break;
			case RE::DEFAULT_OBJECT::kPlayerFaction: name = "kPlayerFaction"; break;
			case RE::DEFAULT_OBJECT::kGuardFaction: name = "kGuardFaction"; break;
			case RE::DEFAULT_OBJECT::kDefaultMusic: name = "kDefaultMusic"; break;
			case RE::DEFAULT_OBJECT::kBattleMusic: name = "kBattleMusic"; break;
			case RE::DEFAULT_OBJECT::kDeathMusic: name = "kDeathMusic"; break;
			case RE::DEFAULT_OBJECT::kSuccessMusic: name = "kSuccessMusic"; break;
			case RE::DEFAULT_OBJECT::kLevelUpMusic: name = "kLevelUpMusic"; break;
			case RE::DEFAULT_OBJECT::kDungeonClearedMusic: name = "kDungeonClearedMusic"; break;
			case RE::DEFAULT_OBJECT::kPlayerVoiceMale: name = "kPlayerVoiceMale"; break;
			case RE::DEFAULT_OBJECT::kPlayerVoiceMaleChild: name = "kPlayerVoiceMaleChild"; break;
			case RE::DEFAULT_OBJECT::kPlayerVoiceFemale: name = "kPlayerVoiceFemale"; break;
			case RE::DEFAULT_OBJECT::kPlayerVoiceFemaleChild: name = "kPlayerVoiceFemaleChild"; break;
			case RE::DEFAULT_OBJECT::kEatPackageDefaultFood: name = "kEatPackageDefaultFood"; break;
			case RE::DEFAULT_OBJECT::kLeftHandEquip: name = "kLeftHandEquip"; break;
			case RE::DEFAULT_OBJECT::kRightHandEquip: name = "kRightHandEquip"; break;
			case RE::DEFAULT_OBJECT::kEitherHandEquip: name = "kEitherHandEquip"; break;
			case RE::DEFAULT_OBJECT::kVoiceEquip: name = "kVoiceEquip"; break;
			case RE::DEFAULT_OBJECT::kPotionEquip: name = "kPotionEquip"; break;
			case RE::DEFAULT_OBJECT::kEveryActorAbility: name = "kEveryActorAbility"; break;
			case RE::DEFAULT_OBJECT::kCommandedActorAbility: name = "kCommandedActorAbility"; break;
			case RE::DEFAULT_OBJECT::kDrugWearsOffImageSpace: name = "kDrugWearsOffImageSpace"; break;
			case RE::DEFAULT_OBJECT::kFootstepSet: name = "kFootstepSet"; break;
			case RE::DEFAULT_OBJECT::kLandscapeMaterial: name = "kLandscapeMaterial"; break;
			case RE::DEFAULT_OBJECT::kDragonLandZoneMarker: name = "kDragonLandZoneMarker"; break;
			case RE::DEFAULT_OBJECT::kDragonCrashZoneMarker: name = "kDragonCrashZoneMarker"; break;
			case RE::DEFAULT_OBJECT::kCombatStyle: name = "kCombatStyle"; break;
			case RE::DEFAULT_OBJECT::kDefaultPackList: name = "kDefaultPackList"; break;
			case RE::DEFAULT_OBJECT::kWaitForDialoguePackage: name = "kWaitForDialoguePackage"; break;
			case RE::DEFAULT_OBJECT::kLocRefTypeBoss: name = "kLocRefTypeBoss"; break;
			case RE::DEFAULT_OBJECT::kVirtualLocation: name = "kVirtualLocation"; break;
			case RE::DEFAULT_OBJECT::kPersistAllLocation: name = "kPersistAllLocation"; break;
			case RE::DEFAULT_OBJECT::kInventoryPlayer: name = "kInventoryPlayer"; break;
			case RE::DEFAULT_OBJECT::kPathingTestNPC: name = "kPathingTestNPC"; break;
			case RE::DEFAULT_OBJECT::kFavorCostSmall: name = "kFavorCostSmall"; break;
			case RE::DEFAULT_OBJECT::kFavorCostMedium: name = "kFavorCostMedium"; break;
			case RE::DEFAULT_OBJECT::kFavorCostLarge: name = "kFavorCostLarge"; break;
			case RE::DEFAULT_OBJECT::kFavorGiftsPerDay: name = "kFavorGiftsPerDay"; break;
			case RE::DEFAULT_OBJECT::kActionSwimStateChange: name = "kActionSwimStateChange"; break;
			case RE::DEFAULT_OBJECT::kActionLook: name = "kActionLook"; break;
			case RE::DEFAULT_OBJECT::kActionLeftAttack: name = "kActionLeftAttack"; break;
			case RE::DEFAULT_OBJECT::kActionLeftReady: name = "kActionLeftReady"; break;
			case RE::DEFAULT_OBJECT::kActionLeftRelease: name = "kActionLeftRelease"; break;
			case RE::DEFAULT_OBJECT::kActionLeftInterrupt: name = "kActionLeftInterrupt"; break;
			case RE::DEFAULT_OBJECT::kActionRightAttack: name = "kActionRightAttack"; break;
			case RE::DEFAULT_OBJECT::kActionRightReady: name = "kActionRightReady"; break;
			case RE::DEFAULT_OBJECT::kActionRightRelease: name = "kActionRightRelease"; break;
			case RE::DEFAULT_OBJECT::kActionRightInterrupt: name = "kActionRightInterrupt"; break;
			case RE::DEFAULT_OBJECT::kActionDualAttack: name = "kActionDualAttack"; break;
			case RE::DEFAULT_OBJECT::kActionDualRelease: name = "kActionDualRelease"; break;
			case RE::DEFAULT_OBJECT::kActionActivate: name = "kActionActivate"; break;
			case RE::DEFAULT_OBJECT::kActionJump: name = "kActionJump"; break;
			case RE::DEFAULT_OBJECT::kActionFall: name = "kActionFall"; break;
			case RE::DEFAULT_OBJECT::kActionLand: name = "kActionLand"; break;
			case RE::DEFAULT_OBJECT::kActionSneak: name = "kActionSneak"; break;
			case RE::DEFAULT_OBJECT::kActionVoice: name = "kActionVoice"; break;
			case RE::DEFAULT_OBJECT::kActionVoiceReady: name = "kActionVoiceReady"; break;
			case RE::DEFAULT_OBJECT::kActionVoiceRelease: name = "kActionVoiceRelease"; break;
			case RE::DEFAULT_OBJECT::kActionVoiceInterrupt: name = "kActionVoiceInterrupt"; break;
			case RE::DEFAULT_OBJECT::kActionIdle: name = "kActionIdle"; break;
			case RE::DEFAULT_OBJECT::kActionSprintStart: name = "kActionSprintStart"; break;
			case RE::DEFAULT_OBJECT::kActionSprintStop: name = "kActionSprintStop"; break;
			case RE::DEFAULT_OBJECT::kActionDraw: name = "kActionDraw"; break;
			case RE::DEFAULT_OBJECT::kActionSheath: name = "kActionSheath"; break;
			case RE::DEFAULT_OBJECT::kActionLeftPowerAttack: name = "kActionLeftPowerAttack"; break;
			case RE::DEFAULT_OBJECT::kActionRightPowerAttack: name = "kActionRightPowerAttack"; break;
			case RE::DEFAULT_OBJECT::kActionDualPowerAttack: name = "kActionDualPowerAttack"; break;
			case RE::DEFAULT_OBJECT::kActionStaggerStart: name = "kActionStaggerStart"; break;
			case RE::DEFAULT_OBJECT::kActionBlockHit: name = "kActionBlockHit"; break;
			case RE::DEFAULT_OBJECT::kActionBlockAnticipate: name = "kActionBlockAnticipate"; break;
			case RE::DEFAULT_OBJECT::kActionRecoil: name = "kActionRecoil"; break;
			case RE::DEFAULT_OBJECT::kActionLargeRecoil: name = "kActionLargeRecoil"; break;
			case RE::DEFAULT_OBJECT::kActionBleedoutStart: name = "kActionBleedoutStart"; break;
			case RE::DEFAULT_OBJECT::kActionBleedoutStop: name = "kActionBleedoutStop"; break;
			case RE::DEFAULT_OBJECT::kActionIdleStop: name = "kActionIdleStop"; break;
			case RE::DEFAULT_OBJECT::kActionWardHit: name = "kActionWardHit"; break;
			case RE::DEFAULT_OBJECT::kActionForceEquip: name = "kActionForceEquip"; break;
			case RE::DEFAULT_OBJECT::kActionShieldChange: name = "kActionShieldChange"; break;
			case RE::DEFAULT_OBJECT::kActionPathStart: name = "kActionPathStart"; break;
			case RE::DEFAULT_OBJECT::kActionPathEnd: name = "kActionPathEnd"; break;
			case RE::DEFAULT_OBJECT::kActionLargeMovementDelta: name = "kActionLargeMovementDelta"; break;
			case RE::DEFAULT_OBJECT::kActionFlyStart: name = "kActionFlyStart"; break;
			case RE::DEFAULT_OBJECT::kActionFlyStop: name = "kActionFlyStop"; break;
			case RE::DEFAULT_OBJECT::kActionHoverStart: name = "kActionHoverStart"; break;
			case RE::DEFAULT_OBJECT::kActionHoverStop: name = "kActionHoverStop"; break;
			case RE::DEFAULT_OBJECT::kActionBumpedInto: name = "kActionBumpedInto"; break;
			case RE::DEFAULT_OBJECT::kActionSummonedStart: name = "kActionSummonedStart"; break;
			case RE::DEFAULT_OBJECT::kActionTalkingIdle: name = "kActionTalkingIdle"; break;
			case RE::DEFAULT_OBJECT::kActionListenIdle: name = "kActionListenIdle"; break;
			case RE::DEFAULT_OBJECT::kActionDeath: name = "kActionDeath"; break;
			case RE::DEFAULT_OBJECT::kActionDeathWait: name = "kActionDeathWait"; break;
			case RE::DEFAULT_OBJECT::kActionIdleWarn: name = "kActionIdleWarn"; break;
			case RE::DEFAULT_OBJECT::kActionMoveStart: name = "kActionMoveStart"; break;
			case RE::DEFAULT_OBJECT::kActionMoveStop: name = "kActionMoveStop"; break;
			case RE::DEFAULT_OBJECT::kActionTurnRight: name = "kActionTurnRight"; break;
			case RE::DEFAULT_OBJECT::kActionTurnLeft: name = "kActionTurnLeft"; break;
			case RE::DEFAULT_OBJECT::kActionTurnStop: name = "kActionTurnStop"; break;
			case RE::DEFAULT_OBJECT::kActionMoveForward: name = "kActionMoveForward"; break;
			case RE::DEFAULT_OBJECT::kActionMoveBackward: name = "kActionMoveBackward"; break;
			case RE::DEFAULT_OBJECT::kActionMoveLeft: name = "kActionMoveLeft"; break;
			case RE::DEFAULT_OBJECT::kActionMoveRight: name = "kActionMoveRight"; break;
			case RE::DEFAULT_OBJECT::kActionResetAnimationGraph: name = "kActionResetAnimationGraph"; break;
			case RE::DEFAULT_OBJECT::kActionKnockdown: name = "kActionKnockdown"; break;
			case RE::DEFAULT_OBJECT::kActionGetUp: name = "kActionGetUp"; break;
			case RE::DEFAULT_OBJECT::kActionIdleStopInstant: name = "kActionIdleStopInstant"; break;
			case RE::DEFAULT_OBJECT::kActionRagdollInstant: name = "kActionRagdollInstant"; break;
			case RE::DEFAULT_OBJECT::kActionWaterwalkStart: name = "kActionWaterwalkStart"; break;
			case RE::DEFAULT_OBJECT::kActionReload: name = "kActionReload"; break;
			case RE::DEFAULT_OBJECT::kPickupSoundGeneric: name = "kPickupSoundGeneric"; break;
			case RE::DEFAULT_OBJECT::kPutdownSoundGeneric: name = "kPutdownSoundGeneric"; break;
			case RE::DEFAULT_OBJECT::kPickupSoundWeapon: name = "kPickupSoundWeapon"; break;
			case RE::DEFAULT_OBJECT::kPutdownSoundWeapon: name = "kPutdownSoundWeapon"; break;
			case RE::DEFAULT_OBJECT::kPickupSoundArmor: name = "kPickupSoundArmor"; break;
			case RE::DEFAULT_OBJECT::kPutdownSoundArmor: name = "kPutdownSoundArmor"; break;
			case RE::DEFAULT_OBJECT::kPickupSoundBook: name = "kPickupSoundBook"; break;
			case RE::DEFAULT_OBJECT::kPutdownSoundBook: name = "kPutdownSoundBook"; break;
			case RE::DEFAULT_OBJECT::kPickupSoundIngredient: name = "kPickupSoundIngredient"; break;
			case RE::DEFAULT_OBJECT::kPutdownSoundIngredient: name = "kPutdownSoundIngredient"; break;
			case RE::DEFAULT_OBJECT::kHarvestSound: name = "kHarvestSound"; break;
			case RE::DEFAULT_OBJECT::kHarvestFailedSound: name = "kHarvestFailedSound"; break;
			case RE::DEFAULT_OBJECT::kWardBreakSound: name = "kWardBreakSound"; break;
			case RE::DEFAULT_OBJECT::kWardAbsorbSound: name = "kWardAbsorbSound"; break;
			case RE::DEFAULT_OBJECT::kWardDeflectSound: name = "kWardDeflectSound"; break;
			case RE::DEFAULT_OBJECT::kMagicFailSound: name = "kMagicFailSound"; break;
			case RE::DEFAULT_OBJECT::kShoutFailSound: name = "kShoutFailSound"; break;
			case RE::DEFAULT_OBJECT::kHeartbeatSoundFast: name = "kHeartbeatSoundFast"; break;
			case RE::DEFAULT_OBJECT::kHeartbeatSoundSlow: name = "kHeartbeatSoundSlow"; break;
			case RE::DEFAULT_OBJECT::kImagespaceLowHealth: name = "kImagespaceLowHealth"; break;
			case RE::DEFAULT_OBJECT::kSoulCapturedSound: name = "kSoulCapturedSound"; break;
			case RE::DEFAULT_OBJECT::kNoActivationSound: name = "kNoActivationSound"; break;
			case RE::DEFAULT_OBJECT::kMapMenuLoopingSound: name = "kMapMenuLoopingSound"; break;
			case RE::DEFAULT_OBJECT::kDialogueVoiceCategory: name = "kDialogueVoiceCategory"; break;
			case RE::DEFAULT_OBJECT::kNonDialogueVoiceCategory: name = "kNonDialogueVoiceCategory"; break;
			case RE::DEFAULT_OBJECT::kSFXToFadeInDialogueCategory: name = "kSFXToFadeInDialogueCategory"; break;
			case RE::DEFAULT_OBJECT::kPauseDuringMenuCategoryFade: name = "kPauseDuringMenuCategoryFade"; break;
			case RE::DEFAULT_OBJECT::kPauseDuringMenuCategoryImmediate: name = "kPauseDuringMenuCategoryImmediate"; break;
			case RE::DEFAULT_OBJECT::kPauseDuringLoadingMenuCategory: name = "kPauseDuringLoadingMenuCategory"; break;
			case RE::DEFAULT_OBJECT::kMusicSoundCategory: name = "kMusicSoundCategory"; break;
			case RE::DEFAULT_OBJECT::kStatsMuteCategory: name = "kStatsMuteCategory"; break;
			case RE::DEFAULT_OBJECT::kStatsMusic: name = "kStatsMusic"; break;
			case RE::DEFAULT_OBJECT::kMasterSoundCategory: name = "kMasterSoundCategory"; break;
			case RE::DEFAULT_OBJECT::kTimeSensitiveSoundCategory: name = "kTimeSensitiveSoundCategory"; break;
			case RE::DEFAULT_OBJECT::kDialogueOutputModel3D: name = "kDialogueOutputModel3D"; break;
			case RE::DEFAULT_OBJECT::kDialogueOutputModel2D: name = "kDialogueOutputModel2D"; break;
			case RE::DEFAULT_OBJECT::kPlayersOutputModel1stPerson: name = "kPlayersOutputModel1stPerson"; break;
			case RE::DEFAULT_OBJECT::kPlayersOutputModel3rdPerson: name = "kPlayersOutputModel3rdPerson"; break;
			case RE::DEFAULT_OBJECT::kInterfaceOutputModel: name = "kInterfaceOutputModel"; break;
			case RE::DEFAULT_OBJECT::kReverbType: name = "kReverbType"; break;
			case RE::DEFAULT_OBJECT::kUnderwaterLoopSound: name = "kUnderwaterLoopSound"; break;
			case RE::DEFAULT_OBJECT::kUnderwaterReverbType: name = "kUnderwaterReverbType"; break;
			case RE::DEFAULT_OBJECT::kKeywordHorse: name = "kKeywordHorse"; break;
			case RE::DEFAULT_OBJECT::kKeywordUndead: name = "kKeywordUndead"; break;
			case RE::DEFAULT_OBJECT::kKeywordNPC: name = "kKeywordNPC"; break;
			case RE::DEFAULT_OBJECT::kKeywordBeastRace: name = "kKeywordBeastRace"; break;
			case RE::DEFAULT_OBJECT::kKeywordDummyObject: name = "kKeywordDummyObject"; break;
			case RE::DEFAULT_OBJECT::kKeywordUseGeometryEmitter: name = "kKeywordUseGeometryEmitter"; break;
			case RE::DEFAULT_OBJECT::kKeywordMustStop: name = "kKeywordMustStop"; break;
			case RE::DEFAULT_OBJECT::kKeywordUpdateDuringArchery: name = "kKeywordUpdateDuringArchery"; break;
			case RE::DEFAULT_OBJECT::kKeywordSkipOutfitItems: name = "kKeywordSkipOutfitItems"; break;
			case RE::DEFAULT_OBJECT::kMaleFaceTextureSetHead: name = "kMaleFaceTextureSetHead"; break;
			case RE::DEFAULT_OBJECT::kMaleFaceTextureSetMouth: name = "kMaleFaceTextureSetMouth"; break;
			case RE::DEFAULT_OBJECT::kMaleFaceTextureSetEyes: name = "kMaleFaceTextureSetEyes"; break;
			case RE::DEFAULT_OBJECT::kFemaleFaceTextureSetHead: name = "kFemaleFaceTextureSetHead"; break;
			case RE::DEFAULT_OBJECT::kFemaleFaceTextureSetMouth: name = "kFemaleFaceTextureSetMouth"; break;
			case RE::DEFAULT_OBJECT::kFemaleFaceTextureSetEyes: name = "kFemaleFaceTextureSetEyes"; break;
			case RE::DEFAULT_OBJECT::kImageSpaceModifierforinventorymenu: name = "kImageSpaceModifierforinventorymenu"; break;
			case RE::DEFAULT_OBJECT::kPackagetemplate: name = "kPackagetemplate"; break;
			case RE::DEFAULT_OBJECT::kMainMenuCell: name = "kMainMenuCell"; break;
			case RE::DEFAULT_OBJECT::kDefaultMovementTypeWalk: name = "kDefaultMovementTypeWalk"; break;
			case RE::DEFAULT_OBJECT::kDefaultMovementTypeRun: name = "kDefaultMovementTypeRun"; break;
			case RE::DEFAULT_OBJECT::kDefaultMovementTypeSwim: name = "kDefaultMovementTypeSwim"; break;
			case RE::DEFAULT_OBJECT::kDefaultMovementTypeFly: name = "kDefaultMovementTypeFly"; break;
			case RE::DEFAULT_OBJECT::kDefaultMovementTypeSneak: name = "kDefaultMovementTypeSneak"; break;
			case RE::DEFAULT_OBJECT::kDefaultMovementTypeSprint: name = "kDefaultMovementTypeSprint"; break;
			case RE::DEFAULT_OBJECT::kKeywordSpecialFurniture: name = "kKeywordSpecialFurniture"; break;
			case RE::DEFAULT_OBJECT::kKeywordFurnitureForces1stPerson: name = "kKeywordFurnitureForces1stPerson"; break;
			case RE::DEFAULT_OBJECT::kKeywordFurnitureForces3rdPerson: name = "kKeywordFurnitureForces3rdPerson"; break;
			case RE::DEFAULT_OBJECT::kKeywordActivatorFurnitureNoPlayer: name = "kKeywordActivatorFurnitureNoPlayer"; break;
			default: name = fmt::format("Other: {}", std::to_underlying(v)); break;
		}
		return formatter<string_view>::format(name, ctx);
	}
};