#include "ShooterGameModeBase.h"

AShooterGameModeBase::AShooterGameModeBase()
{
	PlayerControllerClass = AShooterPlayerController::StaticClass();
	DefaultPawnClass = AShooterPlayer::StaticClass();
}