Scriptname GTSProxyPlayerAlias extends ReferenceAlias

;Register Devourment mod events and call the DLL when they happen.

Event OnInit()
    
    RegisterForModEvent("Devourment_DeadDigested", "DeadDigested") 
    RegisterForModEvent("Devourment_onPreyDeath", "PreyDeath") 
	
EndEvent

Event OnPlayerLoadGame()
    
    RegisterForModEvent("Devourment_DeadDigested", "DeadDigested") 
    RegisterForModEvent("Devourment_onPreyDeath", "PreyDeath") 
	
EndEvent

; When target is fully digested.
Event DeadDigested(Form akFormPred, Form akFormPrey, int preyData)
    Actor Pred = akFormPred as Actor
    Actor Prey = akFormPrey as Actor
    GTSPlugin.CallDevourmentCompatibility(Pred, Prey, True)
EndEvent

; When target dies, not digested yet.
Event PreyDeath(Form akFormPred, Form akFormPrey)
    Actor Pred = akFormPred as Actor
    Actor Prey = akFormPrey as Actor
    GTSPlugin.CallDevourmentCompatibility(Pred, Prey, False)
EndEvent




