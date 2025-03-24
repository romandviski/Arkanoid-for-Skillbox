// Mikhail Efremov. All rights reserved.

#include "Bonuses/BonusBall.h"
#include "Framework/Paddle.h"


ABonusBall::ABonusBall()
{
	Duration = 30.0f;
}

void ABonusBall::BonusAction(APaddle* Paddle)
{
	Paddle->BonusBall(Duration);
	
	Super::BonusAction(Paddle);
}
