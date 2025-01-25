// Mikhail Efremov. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "LifeComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class ARKANOID_API ULifeComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	ULifeComponent();

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Settings")
	int32 Life = 1;

public:
	/**
	 * Устанавливает количество жизней
	 * @param NewLife новое количество жизней
	 */
	UFUNCTION(BlueprintCallable)
	void SetLife(const int32 NewLife);
	UFUNCTION(BlueprintPure)
	FORCEINLINE int32 GetLife() const { return Life; }
	UFUNCTION(BlueprintCallable)
	void TakeDamage(const int32 Damage = 1);
	UFUNCTION(BlueprintPure)
	bool IsAlive() const;
		
};
