;BEGIN FRAGMENT CODE - Do not edit anything between this and the end comment
;NEXT FRAGMENT INDEX 28
Scriptname GTSUtilTalkToActorFragment Extends Perk Hidden

;BEGIN FRAGMENT Fragment_18
Function Fragment_18(ObjectReference akTargetRef, Actor akActor)
;BEGIN CODE
ProgressScript.TalkTo(akTargetRef as actor, playerref)
;END CODE
EndFunction
;END FRAGMENT

;END FRAGMENT CODE - Do not edit anything between this and the begin comment


Actor Property PlayerRef  Auto  
GTSProgressionQuest Property ProgressScript Auto
