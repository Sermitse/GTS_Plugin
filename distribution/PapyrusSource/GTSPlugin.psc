scriptName GTSPlugin hidden

; Internal methods for papyrus -> dll interop.

; Quest
Function ResetQuestProgression() global native
Float function Quest_GetProgression(int aiStage) global native
Bool function WasDragonEaten() global native

; Devourment Compatibility
Function CallDevourmentCompatibility(Actor akPred, Actor akPrey, bool Digested) global native

; Force-Start (emphasis on START) size interactions between two actors, further AI actions will rely on your AI settings.
; int Type explanation:
; 0 = Vore
; 1 = Devourment (Only with Devourment)
; 2 = Stomp
; 3 = Kicks
; 4 = Thigh Sandwich
; 5 = Thigh Crush
; 6 = Butt Crush
; 7 = Hugs
; 8 = Grab
Function ForceStartSizeInteraction(Actor Pred, Actor Prey, int Type) global native