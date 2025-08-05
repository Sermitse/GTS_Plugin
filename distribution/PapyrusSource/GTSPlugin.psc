scriptName GTSPlugin hidden

; Internal methods for papyrus -> dll interop.

; Quest
Function ResetQuestProgression() global native
Float function Quest_GetProgression(int aiStage) global native
Bool function WasDragonEaten() global native

; Devourment Compatibility
Function CallDevourmentCompatibility(Actor akPred, Actor akPrey, bool Digested) global native