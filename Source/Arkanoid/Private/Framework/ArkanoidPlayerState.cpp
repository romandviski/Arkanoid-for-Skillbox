// Mikhail Efremov. All rights reserved.

#include "Framework/ArkanoidPlayerState.h"
#include "Framework/ArkanoidGameInstance.h"
#include "Kismet/GameplayStatics.h"

void AArkanoidPlayerState::ChangePlayerScore(const int32 Amount)
{
	PlayerScore = FMath::Max(PlayerScore + Amount, 0);

	if (const auto Gi = Cast<UArkanoidGameInstance>(GetGameInstance()))
		Gi->SetLevelRecord(UGameplayStatics::GetCurrentLevelName(this), PlayerScore);
	
	OnPlayerScoreChanged.Broadcast(PlayerScore);

	UE_LOG(LogTemp, Log, TEXT("PlayerScore = %d"), PlayerScore);
}
