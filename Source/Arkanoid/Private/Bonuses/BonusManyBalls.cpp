// Mikhail Efremov. All rights reserved.

#include "Bonuses/BonusManyBalls.h"

#include "EngineUtils.h"
#include "Framework/Paddle.h"


ABonusManyBalls::ABonusManyBalls()
{
	Duration = 10.0f;
}

void ABonusManyBalls::BeginPlay()
{
	Super::BeginPlay();

	// После спавна сразу срабатывает бонус, перебирая все каретки
	for (const auto Paddle : TActorRange<APaddle>(GetWorld()))
	{
		if (Paddle)
		{
			Paddle->BonusManyBalls(Duration);
			Destroy();
		}
	}
}
