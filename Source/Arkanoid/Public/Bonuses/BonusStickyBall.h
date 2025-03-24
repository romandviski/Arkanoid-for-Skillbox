// Mikhail Efremov. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "BonusParent.h"
#include "BonusStickyBall.generated.h"

UCLASS()
class ARKANOID_API ABonusStickyBall : public ABonusParent
{
	GENERATED_BODY()

protected:
	virtual void BonusAction(APaddle* Paddle) override;
};
