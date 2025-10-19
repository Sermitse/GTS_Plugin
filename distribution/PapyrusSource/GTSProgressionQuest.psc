Scriptname GTSProgressionQuest extends Quest

Actor Property PlayerRef Auto

GlobalVariable Property GTSSkillPerkPoints Auto

Shout property GTSShoutGrowth Auto
WordOfPower Property GTSWordGro1 Auto
WordOfPower Property GTSWordGro2 Auto
WordOfPower Property GTSWordGro3 Auto

Shout property GTSShoutAbsorb auto
WordOfPower Property GTSWordAb1 Auto

;Gained During The Quest
Spell Property GTSSpellShrinkEnemy auto
Spell Property GTSSpellGrowth auto
Spell Property GTSSpellShrink auto
Spell Property GTSSpellRestoreSize auto
Spell Property GTSSpellRestoreSizeOther Auto
Spell Property GTSSpellShrinkEnemyAOE auto

;Gained At the end of the quest.
Spell Property GTSSPellGrowAlly auto
Spell Property GTSSpellShrinkAlly auto
Spell Property GTSSpellShrinkEnemyAOEMastery auto
Spell Property GTSSpellShrinkToNothing auto
Spell Property GTSSpellSlowGrowth auto

Perk property GTSPerkMightOfGiants Auto

int property CurrentStage auto

bool DragonWasDevoured = false
float TickRate = 1.0

Function BeginQuest()
    CurrentStage = 10
    RegisterForSingleUpdate(1.0)
EndFunction

;Quest Update
Event OnUpdate()

        float StolenSize_Hugs = GTSPlugin.Quest_GetProgression(0)
        float StolenSize_Shrink = GTSPlugin.Quest_GetProgression(1)
        float TrampleCount = GTSPlugin.Quest_GetProgression(2)
        float TrampleAndSTNCount = GTSPlugin.Quest_GetProgression(3)
        float GrabKillCount = GTSPlugin.Quest_GetProgression(4)
        float VoreCount_Normal = GTSPlugin.Quest_GetProgression(5)
        float AbsorbedGiants = GTSPlugin.Quest_GetProgression(6)
		
        if CurrentStage == 10
            if StolenSize_Hugs >= 1.0 
                SetStage(20)
                PlayerRef.AddSpell(GTSSpellShrinkEnemy)
            endif
        endif
		
        if CurrentStage == 20 ; Steal size through Size Steal Spells

            if StolenSize_Shrink >= 2.0 ; Shrink enemies with steal size stage
                PlayerRef.AddSpell(GTSSpellShrink) ; Add Shrink Self spell
                SetStage(30)
           endif 
		   
        elseif CurrentStage == 30 ; Crush LIVING targets (not dead/killed)
		
            if TrampleCount >= 3.0
                PlayerRef.AddSpell(GTSSpellRestoreSize)
                SetStage(40)
            endif
			
        elseif CurrentStage == 40 ; Crush / Shrink to nothing enemies 
		
            if TrampleAndSTNCount >= 6.0
                PlayerRef.AddSpell(GTSSpellRestoreSizeOther)
                SetStage(50)
            endif
			
        elseif CurrentStage == 50 ; Crush with hands
		
            if GrabKillCount >= 3.0
                PlayerRef.AddSpell(GTSSpellShrinkEnemyAOE)
                SetStage(60)
            endif
			
        elseif CurrentStage == 60 ; Just eat others
		
            if VoreCount_Normal >= 6.0 ; no rewards so far, only bonus size
                SetStage(70)
            endif
			
        elseif CurrentStage == 70 ; Stage with Giants
		
            if AbsorbedGiants >= 1.0
			
                GTSSkillPerkPoints.Mod(1.0)
                Debug.Notification("You've learned a bonus perk point")

                PlayerRef.AddPerk(GTSPerkMightOfGiants)
                PlayerRef.AddSpell(GTSSpellGrowth)
				
				;Add Growth Spurt shout
                Game.GetPlayer().AddShout(GTSShoutGrowth)
                Game.TeachWord(GtsWordGro1)
                Game.UnlockWord(GtsWordGro1) 
				
                SetStage(80)
				
            endif
			
        elseif CurrentStage == 80 ; Dragon Stage
		
            if GTSPlugin.WasDragonEaten() == true
				DragonWasDevoured = true
			endif
			
            if DragonWasDevoured == true
			
                GTSSkillPerkPoints.Mod(4.0)
                Debug.Notification("You've learned 4 bonus perk points")
				
                PlayerRef.AddShout(GTSShoutAbsorb)
                Game.TeachWord(GTSWordAb1)
                Game.UnlockWord(GTSWordAb1)
				
				Game.TeachWord(GTSWordGro2)
				Game.UnlockWord(GTSWordGro2) 
				Game.TeachWord(GTSWordGro3)
				Game.UnlockWord(GTSWordGro3) 
				
				Debug.messageBox("By Absorbing a dragon, you've learned a new shout and additional spells. You also feel lots of hidden potential inside your body.")
                SetStage(100)
            endif   
			
        elseif CurrentStage == 100
			   UpdateSpells()
			   SetStage(200)
               Utility.wait(0.50)
               GTSPlugin.ResetQuestProgression() ; reset everything just in case
               UnregisterForUpdate()
			   
        endif
		
        if(CurrentStage < 200)
            RegisterForSingleUpdate(TickRate)
        endif
		
EndEvent

Function UpdateSpells() 

    ;If the quest is over give all the remaining spells.
	
    if(CurrentStage >= 90)
	
		PlayerRef.AddSpell(GTSSpellShrinkEnemyAOE)
		PlayerRef.AddSpell(GTSSPellGrowAlly)
		PlayerRef.AddSpell(GTSSpellShrinkAlly)
		PlayerRef.AddSpell(GTSSpellShrinkEnemyAOEMastery)
		PlayerRef.AddSpell(GTSSpellShrinkToNothing)
		
    endif
	
EndFunction

; Proxy Functions. Its easier to have them here due to all the spell propperties

Function Proxy_SkipQuest() 

    SetStage(100)
	
    PlayerRef.AddShout(GTSShoutGrowth)
    Game.TeachWord(GTSWordGro1)
    Game.UnlockWord(GTSWordGro1)
	Game.TeachWord(GTSWordGro2)
	Game.UnlockWord(GTSWordGro2) 
	Game.TeachWord(GTSWordGro3)
	Game.UnlockWord(GTSWordGro3) 
    
    PlayerRef.AddShout(GTSShoutAbsorb)
    Game.TeachWord(GTSWordAb1)
    Game.UnlockWord(GTSWordAb1)
    
    PlayerRef.AddSpell(GTSSpellShrinkEnemy)
    PlayerRef.AddSpell(GTSSpellShrink)
    PlayerRef.AddSpell(GTSSpellRestoreSize)
    PlayerRef.AddSpell(GTSSpellRestoreSizeOther)
    PlayerRef.AddSpell(GTSSpellGrowth)
	PlayerRef.AddSpell(GTSSpellShrinkEnemyAOE)
    PlayerRef.AddSpell(GTSSPellGrowAlly)
	PlayerRef.AddSpell(GTSSpellShrinkAlly)
	PlayerRef.AddSpell(GTSSpellShrinkEnemyAOEMastery)
	PlayerRef.AddSpell(GTSSpellShrinkToNothing)
	PlayerRef.AddSpell(GTSSpellSlowGrowth)

    PlayerRef.AddPerk(GTSPerkMightOfGiants)
	
    GtsSkillPerkPoints.Mod(5.0)   
    Debug.Notification("You've learned 5 bonus perk points")
	
	UnregisterForUpdate()
	
EndFunction

;Shouts are a pain in the ass to add through the DLL,
;So we add them through papyrus instead.
Function Proxy_GetAllShouts() 

    PlayerRef.AddShout(GTSShoutGrowth)
    Game.TeachWord(GTSWordGro1)
    Game.UnlockWord(GTSWordGro1)
	Game.TeachWord(GTSWordGro2)
	Game.UnlockWord(GTSWordGro2) 
	Game.TeachWord(GTSWordGro3)
	Game.UnlockWord(GTSWordGro3) 
    
    PlayerRef.AddShout(GTSShoutAbsorb)
    Game.TeachWord(GTSWordAb1)
    Game.UnlockWord(GTSWordAb1)
	
	PlayerRef.AddShout(GTSShoutAbsorb)
    Game.TeachWord(GTSWordAb1)
    Game.UnlockWord(GTSWordAb1)
    
EndFunction

;Called by the Util perk. Allows us to talk to npc's when sneaking
Function TalkTo(Actor akTiny, Actor akGiant) 

    if akTiny.IsPlayerTeammate()
        akTiny.activate(akGiant)
    else
        akTiny.SetPlayerTeammate(true) 
        akTiny.activate(akGiant)
        akTiny.SetPlayerTeammate(false)
    endif
	
endfunction
