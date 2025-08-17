[![CI](https://github.com/Sermitse/GTS_Plugin/actions/workflows/build.yml/badge.svg)](https://github.com/Sermitse/GTS_Plugin/actions/workflows/build.yml)

# Size Matters SKSE64 Plugin

This is the source for the [Gianttess Mod - Size matters NG](https://www.nexusmods.com/skyrimspecialedition/mods/37823?tab=description) SKSE plugin.

Monke coding with new features/expansions by Sermit and Arial.
Very complex parts (98% of DLL code) done by Andy.

Still WIP

## Building Requirements
- [git](https://gitforwindows.org/) and add git to `PATH`
- [cmake](https://cmake.org/download/) and add cmake to `PATH`
- [vcpkg](https://github.com/microsoft/vcpkg) with the `VCPKG_ROOT` enviorment variable set to the vcpkg folder
- [Visual studio 2022](https://visualstudio.microsoft.com/vs/) with Desktop development for C++
- [Ninja](https://ninja-build.org/) which can be installed with `winget install --id=Ninja-build.Ninja -e`

## Building
Launch `x64 Native Tools Command Promt For VS 2022` from the start menu first
```
git clone https://github.com/sermitse/GTS_Plugin/
cd GTS_Plugin
cmake --preset build-release
cmake --build --preset preset-release
```

## Feature Wish List

- [X] [1] Auto scale height to room
- [X] [2] Correct bumper for height (Done through Side-Mod)
- [X] [3] Talk to actor when crouching
- [X] [4] Accurate Body Collision that inflicts size effects on contact instead of using cloak magiceffects
- [X] [5] Fixed Player and NPC headtracking
- [X] [6] Ability to edit HitBoxes so it'll be possible to have Giant Enemies/Have more accurate hitbox (Done through Side-Mod)
  - AABB was shared between all actors of the same skeleton
  - Found a way to clone the AABB and make them unique to each actor
- [X] [7] Ability to spawn dust effect explosions literally under the feet, not under center of character. Ideally even scale them.
- [X] [8] Maybe proper High-Heel module that dynamically adjusts high heel size based on Root scale?
- [X] [9] In the perfect scenario, repair Animation Speed based on size
- [X] [10] Transfer from SkyrimPlatform to Pure DLL, so SP won't be needed
- [X] [11] Disable swimming when huge enough: make player sink instead of swimming when huge enough or based on size
- [X] [12] Make DLL track Giantess PC/NPC footsteps to shake the screen/place dust without lags.
- [X] [13] Vore on button press
- [ ] [14] Blood on the feet/hand after crushing
- [ ] [15] Quest progression ui/current size ui
- [X] [16] Vore anim and other animations
- [ ] [17] Vore belly
- [X] [18] Integrate new Potions into the world
- [X] [19] .dll optimization so it will eat less CPU and have better performance (Partially done)
- [X] [20] Affect stealth by size (Partially done)
- [X] [21] Make feet deal damage and effects based on it's speed instead of being able to kill everyone if standing still
- [ ] [22] Improve dll coding/code readability in sections written by me (Sermit) so it doesn't look ugly in some files
- [ ] [23] +-TF2 Style size-related kill feed 
- [ ] [24] Buff Icons above/near GTS scale ui for following mechanics:
- - - - - - Life Absorption Stacks (Perk);
- - - - - - Damage Reduction (passive);
- - - - - - Size Reserve Stacks (buff);
- - - - - - Aspect Of Giantess (buff);
- - - - - - Vore Stacks (passive);
- - - - - - On The Edge (Perk);



## Easier Things
- [X] Scale
  - [x] Change scale in papyrus
  - [X] Get height in meters of any actor
  - [X] Get volume of any actor
- [x] Mechanics  
  - [x] Apply size effects to nearby actor
  - [x] Move away from favor active
  - [x] Animation speed adjustment
- [X] Walk/Jump events
  - [x] Camera distance to event
  - [X] Camera shake
  - [X] Feet sounds
