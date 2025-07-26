scriptName GTSControl hidden

; Grow TeamMate
Function GrowTeammate(Float afPower) global native

; Shink TeamMate
Function ShrinkTeammate(Float afPower) global native

; Grow / Shrink the Player similar to 2 Functions above (but the target is player in this case)
Function GrowPlayer(Float afPower) global native
Function ShrinkPlayer(Float afPower) global native
    
;Rapid player growth
Function CallRapidGrowth(float afAmount, float afHalfLife) global native 
 
;Rapid player shrink
Function CallRapidShrink(float afAmount, float afHalfLife) global native    
