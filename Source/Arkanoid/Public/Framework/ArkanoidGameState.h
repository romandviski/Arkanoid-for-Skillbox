// Mikhail Efremov. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "ArkanoidGameState.generated.h"

UCLASS()
class ARKANOID_API AArkanoidGameState : public AGameStateBase
{
	GENERATED_BODY()

private:
	float GameTime = 0.0f;
	bool bGameStarted = false;

protected:
	virtual void Tick(float DeltaTime) override;

public:
	AArkanoidGameState();

	void StartGame();
	void StopGame();

	UFUNCTION(BlueprintPure, Category = "GameState")
	void GetGameTime(int32& Minutes, int32& Seconds, int32& Milliseconds) const;
};
