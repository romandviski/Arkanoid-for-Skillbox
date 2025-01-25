// Mikhail Efremov. All rights reserved.

#include "Framework/ArkanoidPlayerState.h"

void AArkanoidPlayerState::ChangePlayerScore(const int32 Amount)
{
	PlayerScore = FMath::Max(PlayerScore + Amount, 0);
	OnPlayerScoreChanged.Broadcast(PlayerScore);

	UE_LOG(LogTemp, Log, TEXT("PlayerScore = %d"), PlayerScore);
}
