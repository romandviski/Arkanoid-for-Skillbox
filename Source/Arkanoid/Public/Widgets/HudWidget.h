// Mikhail Efremov. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "HudWidget.generated.h"

class AArkanoidGameState;
class AArkanoidPlayerState;
class UTextBlock;

UCLASS()
class ARKANOID_API UHudWidget : public UUserWidget
{
	GENERATED_BODY()

private:
	UPROPERTY(meta = (BindWidget, AllowPrivateAccess = true))
	UTextBlock* CurrentScore = nullptr;
	UPROPERTY(meta = (BindWidget, AllowPrivateAccess = true))
	UTextBlock* GameTime = nullptr;
	UPROPERTY(meta = (BindWidget, AllowPrivateAccess = true))
	UTextBlock* LevelRecord = nullptr;
	UPROPERTY(Transient, meta = (BindWidgetAnim, AllowPrivateAccess = true))
	UWidgetAnimation* ShakeAnimation = nullptr;

	UPROPERTY()
	AArkanoidPlayerState* PlayerState = nullptr;
	UPROPERTY()
	AArkanoidGameState* GameState = nullptr;

protected:
	virtual void NativeConstruct() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

	UFUNCTION()
	void UpdateScore(const int32 NewScore);
	void UpdateTime();

	void UpdateRecordOnScreen();
};
