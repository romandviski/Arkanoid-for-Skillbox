// Mikhail Efremov. All rights reserved.

#include "Bonuses/BonusLife.h"
#include "Framework/Paddle.h"

ABonusLife::ABonusLife()
{
	Value = 1.0f;
}

void ABonusLife::BonusAction(APaddle* Paddle)
{
	Paddle->BonusChangeLife(Value);
	
	Super::BonusAction(Paddle);
}
