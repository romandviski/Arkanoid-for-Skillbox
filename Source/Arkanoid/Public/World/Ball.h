// Mikhail Efremov. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Ball.generated.h"

class UArrowComponent;

UCLASS()
class ARKANOID_API ABall : public AActor
{
	GENERATED_BODY()
	
private:
	// Переменные указатели под части нашего актора
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Components, meta = (AllowPrivateAccess = "true"))
	UStaticMeshComponent* StaticMesh = nullptr;
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Components, meta = (AllowPrivateAccess = "true"))
    UArrowComponent* ForwardArrow = nullptr;
	
public:	
	ABall();

protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

public:
	UFUNCTION(BlueprintCallable, Category = Ball)
	void Move();
};
