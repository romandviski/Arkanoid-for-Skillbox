// Mikhail Efremov. All rights reserved.

#include "Framework/ArkanoidGameState.h"

AArkanoidGameState::AArkanoidGameState()
{
	PrimaryActorTick.bCanEverTick = true;
}

void AArkanoidGameState::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (bGameStarted)
	{
		GameTime += DeltaTime;
	}
}

void AArkanoidGameState::StartGame()
{
	bGameStarted = true;
	//StartTime = GetWorld()->GetTimeSeconds()
}

void AArkanoidGameState::StopGame()
{
	bGameStarted = false;
}

void AArkanoidGameState::GetGameTime(int32& Minutes, int32& Seconds, int32& Milliseconds) const
{
	//float ElapsedTime = GetWorld()->GetTimeSeconds() - StartTime;
	Minutes = static_cast<int32>(GameTime) / 60;
	Seconds = static_cast<int32>(GameTime) % 60;
	Milliseconds = static_cast<int32>(GameTime - FMath::Floor(GameTime)) * 1000;
}
