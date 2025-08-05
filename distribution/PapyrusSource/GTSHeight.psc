scriptName GTSHeight hidden

; Target Height
;
; These will be the akTarget height which is achieved on the main loop over the
; next few frames gradually
; Use this to alter the current height
;
; It works using the GtsScale.SetScale method
;   You can adjust which scale is adjusted using GtsScale.SetScaleMethod
;
; Target height is saved into the COSAVE and will persist

Function SetTargetHeight(Actor akTarget, Float afHeight) global native
Float Function GetTargetHeight(Actor akTarget) global native
Function ModTargetHeight(Actor akTarget, Float afAmount) global native

; Max Height
;
; These will set the max height
;
; Max height is saved into the COSAVE and will persist

Function SetMaxHeight(Actor akTarget, Float afHeight) global native
Float Function GetMaxHeight(Actor akTarget) global native
Function ModMaxHeight(Actor akTarget, Float afAmount) global native

; Visual Height
;
; This is the current actual height of the actor. While the akTarget height
; is what this value aims for, before it gets to the akTarget height
; then this represents the actual height
;
; Use this for any size effects
;
; Plan is to have growth be stop by obstacles to the akTarget height
; like ceilings, and this will be the actual height that is achieved.
;
; Visual height is saved into the COSAVE and will persist

Float Function GetVisualHeight(Actor akTarget) global native
