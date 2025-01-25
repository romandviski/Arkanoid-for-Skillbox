// Mikhail Efremov. All rights reserved.

#include "Widgets/HudWidget.h"
#include "Components/TextBlock.h"
#include "Framework/ArkanoidGameInstance.h"
#include "Framework/ArkanoidGameState.h"
#include "Framework/ArkanoidPlayerState.h"
#include "Kismet/GameplayStatics.h"

void UHudWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (GetOwningPlayer())
	{
		PlayerState = GetOwningPlayer()->GetPlayerState<AArkanoidPlayerState>();
		if (PlayerState)
			PlayerState->OnPlayerScoreChanged.AddDynamic(this, &UHudWidget::UpdateScore);
	}

	if (GetWorld())
		GameState = Cast<AArkanoidGameState>(GetWorld()->GetGameState());

	UpdateRecordOnScreen();
}

void UHudWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	UpdateTime();
}

void UHudWidget::UpdateScore(const int32 NewScore)
{
	if (CurrentScore)
	{
		const FString ScoreString = FString::Printf(TEXT("СЧЁТ: %03d"), NewScore);
		CurrentScore->SetText(FText::FromString(ScoreString));

		if (ShakeAnimation)
		{
			PlayAnimation(ShakeAnimation, 0.0f, 3,
				EUMGSequencePlayMode::Forward, 1.0f);
		}
	}

	UpdateRecordOnScreen();
}

void UHudWidget::UpdateTime()
{
	if (GameTime && GameState)
	{
		int32 Minutes = 0;
		int32 Seconds = 0;
		int32 Milliseconds = 0;
		GameState->GetGameTime(Minutes, Seconds, Milliseconds);

		const FString TimeString = FString::Printf(TEXT("%02d : %02d : %03d"), Minutes, Seconds, Milliseconds);
		GameTime->SetText(FText::FromString(TimeString));
	}
}

void UHudWidget::UpdateRecordOnScreen()
{
	if (LevelRecord)
	{
		if (const auto Gi = Cast<UArkanoidGameInstance>(GetGameInstance()))
		{
			const int32 CurrentRecord = Gi->GetLevelRecord(UGameplayStatics::GetCurrentLevelName(this));
			const FString ScoreText = FString::Printf(TEXT("РЕКОРД: %03d"), CurrentRecord);
			LevelRecord->SetText(FText::FromString(*ScoreText));
		}
	}
}
