// Mikhail Efremov. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "BonusParent.h"
#include "BonusBallPower.generated.h"

UCLASS()
class ARKANOID_API ABonusBallPower : public ABonusParent
{
	GENERATED_BODY()

public:
	ABonusBallPower();

protected:
	virtual void BonusAction(APaddle* Paddle) override;
	
};
