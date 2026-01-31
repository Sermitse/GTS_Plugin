;BEGIN FRAGMENT CODE - Do not edit anything between this and the end comment
;NEXT FRAGMENT INDEX 33
Scriptname QF_GTSProgressionQuest Extends Quest Hidden

;BEGIN ALIAS PROPERTY gtsBookAlias
;ALIAS PROPERTY TYPE ReferenceAlias
ReferenceAlias Property Alias_gtsBookAlias Auto
;END ALIAS PROPERTY

;BEGIN FRAGMENT Fragment_8
Function Fragment_8()
;BEGIN AUTOCAST TYPE GTSProgressionQuest
Quest __temp = self as Quest
GTSProgressionQuest kmyQuest = __temp as GTSProgressionQuest
;END AUTOCAST
;BEGIN CODE
;Update Quest info
SetObjectiveCompleted(70)
SetObjectiveDisplayed(80)
kmyQuest.currentStage = 80

kmyQuest.UpdateSpells()
;END CODE
EndFunction
;END FRAGMENT

;BEGIN FRAGMENT Fragment_21
Function Fragment_21()
;BEGIN AUTOCAST TYPE GTSProgressionQuest
Quest __temp = self as Quest
GTSProgressionQuest kmyQuest = __temp as GTSProgressionQuest
;END AUTOCAST
;BEGIN CODE
;Update Quest info
SetObjectiveCompleted(20)
SetObjectiveDisplayed(25)
SetObjectiveDisplayed(26)
SetObjectiveDisplayed(30)
kmyQuest.currentStage = 30
;END CODE
EndFunction
;END FRAGMENT

;BEGIN FRAGMENT Fragment_30
Function Fragment_30()
;BEGIN AUTOCAST TYPE GTSProgressionQuest
Quest __temp = self as Quest
GTSProgressionQuest kmyQuest = __temp as GTSProgressionQuest
;END AUTOCAST
;BEGIN CODE
;Update Quest info
SetObjectiveCompleted(40)
SetObjectiveDisplayed(55)
SetObjectiveDisplayed(56)
SetObjectiveDisplayed(50)
kmyQuest.currentStage = 50
;END CODE
EndFunction
;END FRAGMENT

;BEGIN FRAGMENT Fragment_20
Function Fragment_20()
;BEGIN AUTOCAST TYPE GTSProgressionQuest
Quest __temp = self as Quest
GTSProgressionQuest kmyQuest = __temp as GTSProgressionQuest
;END AUTOCAST
;BEGIN CODE
SetObjectiveDisplayed(5)
SetObjectiveDisplayed(10)

kmyQuest.BeginQuest()
kmyQuest.UpdateSpells()
kmyQuest.currentStage = 10
;END CODE
EndFunction
;END FRAGMENT

;BEGIN FRAGMENT Fragment_7
Function Fragment_7()
;BEGIN AUTOCAST TYPE GTSProgressionQuest
Quest __temp = self as Quest
GTSProgressionQuest kmyQuest = __temp as GTSProgressionQuest
;END AUTOCAST
;BEGIN CODE
;Update Quest info
SetObjectiveCompleted(50)
SetObjectiveCompleted(55)
SetObjectiveCompleted(56)
SetObjectiveDisplayed(65)
SetObjectiveDisplayed(60)
kmyQuest.currentStage = 60
;END CODE
EndFunction
;END FRAGMENT

;BEGIN FRAGMENT Fragment_0
Function Fragment_0()
;BEGIN AUTOCAST TYPE GTSProgressionQuest
Quest __temp = self as Quest
GTSProgressionQuest kmyQuest = __temp as GTSProgressionQuest
;END AUTOCAST
;BEGIN CODE
;Update Quest info
SetObjectiveCompleted(10)
SetObjectiveCompleted(5)
SetObjectiveDisplayed(20)
kmyQuest.currentStage = 20
;END CODE
EndFunction
;END FRAGMENT

;BEGIN FRAGMENT Fragment_6
Function Fragment_6()
;BEGIN AUTOCAST TYPE GTSProgressionQuest
Quest __temp = self as Quest
GTSProgressionQuest kmyQuest = __temp as GTSProgressionQuest
;END AUTOCAST
;BEGIN CODE
;Update Quest info
SetObjectiveCompleted(30)
SetObjectiveCompleted(25)
SetObjectiveCompleted(26)
SetObjectiveDisplayed(40)
kmyQuest.currentStage = 40
;END CODE
EndFunction
;END FRAGMENT

;BEGIN FRAGMENT Fragment_13
Function Fragment_13()
;BEGIN AUTOCAST TYPE GTSProgressionQuest
Quest __temp = self as Quest
GTSProgressionQuest kmyQuest = __temp as GTSProgressionQuest
;END AUTOCAST
;BEGIN CODE
;Update Quest info
SetObjectiveDisplayed(70)
SetObjectiveCompleted(60)
SetObjectiveCompleted(65)
kmyQuest.currentStage = 70
;END CODE
EndFunction
;END FRAGMENT

;BEGIN FRAGMENT Fragment_12
Function Fragment_12()
;BEGIN AUTOCAST TYPE GTSProgressionQuest
Quest __temp = self as Quest
GTSProgressionQuest kmyQuest = __temp as GTSProgressionQuest
;END AUTOCAST
;BEGIN CODE
;Update Quest info
SetObjectiveCompleted(70)
SetObjectiveCompleted(80)

kmyQuest.currentStage = 100
kmyQuest.UpdateSpells()

SetStage(200)
;END CODE
EndFunction
;END FRAGMENT

;END FRAGMENT CODE - Do not edit anything between this and the begin comment
