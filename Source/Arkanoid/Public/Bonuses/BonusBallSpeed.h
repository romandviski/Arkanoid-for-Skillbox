// Mikhail Efremov. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "BonusParent.h"
#include "BonusBallSpeed.generated.h"

UCLASS()
class ARKANOID_API ABonusBallSpeed : public ABonusParent
{
	GENERATED_BODY()

public:
	ABonusBallSpeed();

protected:
	virtual void BonusAction(APaddle* Paddle) override;
	
};
