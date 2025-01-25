// Mikhail Efremov. All rights reserved.

#include "Widgets/HudWidget.h"

#include "Components/TextBlock.h"
#include "Framework/ArkanoidGameState.h"
#include "Framework/ArkanoidPlayerState.h"

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
}

void UHudWidget::UpdateTime()
{
	if (GameTime && GameState)
	{
		int32 Minutes = 0;
		int32 Seconds = 0;
		int32 Milliseconds = 0;
		GameState->GetGameTime(Minutes, Seconds, Milliseconds);

		const FString TimeString = FString::Printf(TEXT("%02d : %02d : %02d"), Minutes, Seconds, Milliseconds);
		CurrentScore->SetText(FText::FromString(TimeString));
	}
}
