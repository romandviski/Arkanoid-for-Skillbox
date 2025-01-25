// Mikhail Efremov. All rights reserved.

#include "Framework/ArkanoidHUD.h"
#include "Blueprint/UserWidget.h"
#include "Widgets/HudWidget.h"
#include "Widgets/PauseWidget.h"

void AArkanoidHUD::ChangeInputMode(UWidget* Widget) const
{
	if (Widget)
	{
		FInputModeGameAndUI InputMode;
		InputMode.SetWidgetToFocus(Widget->TakeWidget());
		PlayerOwner->SetInputMode(InputMode);
		PlayerOwner->SetShowMouseCursor(true);
	}
	else
	{
		const FInputModeGameOnly InputMode;
		PlayerOwner->SetInputMode(InputMode);
		PlayerOwner->SetShowMouseCursor(false);
		PlayerOwner->SetPause(false);
	}
}

void AArkanoidHUD::BeginPlay()
{
	Super::BeginPlay();

	if (HudWidgetClass)
	{
		HudWidget = CreateWidget<UHudWidget>(PlayerOwner, HudWidgetClass);
		if (HudWidget)
			HudWidget->AddToViewport();
	}

	ChangeInputMode();
}

void AArkanoidHUD::ShowPauseWidget()
{
	if (!PauseWidget)
	{
		PauseWidget = CreateWidget<UPauseWidget>(PlayerOwner, PauseWidgetClass);
		if (PauseWidget)
		{
			PauseWidget->AddToViewport(99);
			ChangeInputMode(PauseWidget);
			PauseWidget->OnGameResume.AddDynamic(this, &AArkanoidHUD::HidePauseWidget);
		}
	}
	else
	{
		PauseWidget->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
		ChangeInputMode(PauseWidget);
	}
}

void AArkanoidHUD::HidePauseWidget()
{
	if (PauseWidget)
	{
		PauseWidget->SetVisibility(ESlateVisibility::Collapsed);
		ChangeInputMode();
	}
}

void AArkanoidHUD::ShowGameEndWidget(const bool bWinStatus)
{
	ShowPauseWidget();
	if (PauseWidget)
		PauseWidget->SetWinStatus(bWinStatus);
}
