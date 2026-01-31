Scriptname GTSPileCleaner extends ObjectReference
Actor Property PlayerRef Auto

bool MarkForDeletion = false
bool WasDeleted = false
float timer = 12.0 ; 12 hours

Event OnReset()
    Self.Disable()
EndEvent

Event OnLoad()
    if (MarkForDeletion == false) 
        RegisterForSingleUpdateGameTime(timer) ; start delete task after 12 hours
        MarkForDeletion = true ; mark for deletion
    endif
EndEvent

Event OnUpdateGameTime() ; delete it
    if (WasDeleted == false)
        Self.Disable()
        Self.Delete()
    endif
endevent
 
Event OnItemRemoved(Form akBaseItem, int aiItemCount, ObjectReference akItemReference, ObjectReference akDestContainer)
    if Self.GetNumItems() <= 0
        WasDeleted = true
        Self.Disable()
        Self.Delete()
    endif
endEvent

Event OnOpen(ObjectReference akActionRef)
    if Self.GetNumItems() <= 0
        WasDeleted = true
        Self.Disable()
        Self.Delete()
    endif
endEvent    