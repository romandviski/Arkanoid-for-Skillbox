// Mikhail Efremov. All rights reserved.

#include "Bonuses/BonusBallPower.h"
#include "Framework/Paddle.h"

ABonusBallPower::ABonusBallPower()
{
	Value = 1.0f;
	Duration = 10.0f;
}

void ABonusBallPower::BonusAction(APaddle* Paddle)
{
	Paddle->BonusChangeBallPower(Value, Duration);
	
	Super::BonusAction(Paddle);
}
