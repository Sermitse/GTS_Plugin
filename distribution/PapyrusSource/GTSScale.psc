scriptName GTSScale hidden

; Directly set the actor's scale

Bool Function SetScale(Actor akTarget, Float afScale) global native
Float Function GetScale(Actor akTarget) global native
Bool Function ModScale(Actor akTarget, Float afAmount) global native

; Target Scale
;
; These will be the akTarget scale which is achieved on the main loop over the
; next few frames gradually
; Use this to alter the current scale
;
; It works using the SetScale method
; You can adjust which scale is adjusted using SetScaleMethod
;
; Target scale is saved into the COSAVE and will persist

Function SetTargetScale(Actor akTarget, Float afScale) global native
Float Function GetTargetScale(Actor akTarget) global native
Function ModTargetScale(Actor akTarget, Float afAmount) global native

; Max Scale
;
; These will set the max scales
;
; Max scale is saved into the COSAVE and will persist

Function SetMaxScale(Actor akTarget, Float afScale) global native
Float Function GetMaxScale(Actor akTarget) global native
Function ModMaxScale(Actor akTarget, Float afAmount) global native

; Visual Scale
;
; This is the current actual scale of the actor. While the Target scale
; is what this value aims for, before it gets to the Target scale
; then this represents the actual scale
;
; Use this for any size effects
;
; Plan is to have growth be stop by obstacles to the Target scale
; like ceilings, and this will be the actual scale that is achieved.
;
; Visual scale is saved into the COSAVE and will persist

Float Function GetVisualScale(Actor akTarget) global native


; Report visuals scale without any adjustments, pure scale value
float Function GetGiantessScale(Actor akTarget) global native

; Reports natural scale of actor
float Function GetNaturalScale(Actor akTarget) global native
