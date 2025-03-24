// Mikhail Efremov. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "BonusParent.h"
#include "BonusBall.generated.h"

UCLASS()
class ARKANOID_API ABonusBall : public ABonusParent
{
	GENERATED_BODY()
public:
	ABonusBall();
	
protected:
	virtual void BonusAction(APaddle* Paddle) override;
	
};
