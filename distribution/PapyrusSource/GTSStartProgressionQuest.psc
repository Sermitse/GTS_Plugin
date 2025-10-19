Scriptname GTSStartProgressionQuest extends ObjectReference  

GTSProgressionQuest Property ProgressionQuest Auto

Event OnContainerChanged(ObjectReference newContainer, ObjectReference oldContainer)
    ;Do nothing if the quest is already running or completed
    if ProgressionQuest.GetStage() >= 10
        return
    endif
    
    ;If the player picked up the item, start the quest
    if (newContainer == Game.GetPlayer())   
        ProgressionQuest.SetStage(10)
    endif
EndEvent

Event OnRead()
    if ProgressionQuest.GetStage() >= 10
        return
    else
        ProgressionQuest.SetStage(10)
        Debug.MessageBox("You can't figure out what is written in this book. After examining it closer, you've noticed that it's missing a lot of pages.")
    endif
EndEvent


