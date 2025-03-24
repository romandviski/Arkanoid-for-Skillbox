// Mikhail Efremov. All rights reserved.

#include "Bonuses/BonusStickyBall.h"
#include "Framework/Paddle.h"


void ABonusStickyBall::BonusAction(APaddle* Paddle)
{
	Paddle->BonusSticky();
	
	Super::BonusAction(Paddle);
}
