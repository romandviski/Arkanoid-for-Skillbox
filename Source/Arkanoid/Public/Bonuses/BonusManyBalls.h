// Mikhail Efremov. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "BonusParent.h"
#include "BonusManyBalls.generated.h"

UCLASS()
class ARKANOID_API ABonusManyBalls : public ABonusParent
{
	GENERATED_BODY()

public:
	ABonusManyBalls();

protected:
	virtual void BeginPlay() override;
	
};
