#pragma once

namespace GTS {
    KillMoveParameters GetStandingKillMoveParams(AimOutcome outcome, bool strongAttack, bool trample, KillMoveParameters params = {});
    KillMoveParameters GetSneakKillMoveParams(AimOutcome outcome, bool strongAttack, KillMoveParameters params = {});
    KillMoveParameters GetCrawlKillMoveParams(AimOutcome outcome, KillMoveParameters params = {});
    KillMoveParameters GetKickKillMoveParams(bool left, bool strong, KillMoveParameters params = {});
}