// Mikhail Efremov. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "BonusParent.h"
#include "BonusShield.generated.h"

class AShield;

UCLASS()
class ARKANOID_API ABonusShield : public ABonusParent
{
	GENERATED_BODY()

public:
	ABonusShield();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Setting")
	TSubclassOf<AShield> ShieldClass;

protected:
	virtual void BonusAction(APaddle* Paddle) override;
	
};
