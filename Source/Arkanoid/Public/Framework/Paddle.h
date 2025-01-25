// Mikhail Efremov. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "Paddle.generated.h"

class ABall;
class UArrowComponent;
class UBoxComponent;

UCLASS()
class ARKANOID_API APaddle : public APawn
{
	GENERATED_BODY()

private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta=(AllowPrivateAccess=true), Category="Components")
	UBoxComponent* BoxCollider = nullptr;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta=(AllowPrivateAccess=true), Category="Components")
	UStaticMeshComponent* StaticMesh = nullptr;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta=(AllowPrivateAccess=true), Category="Components")
	UStaticMeshComponent* LeftStaticMesh = nullptr;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta=(AllowPrivateAccess=true), Category="Components")
	UStaticMeshComponent* RightStaticMesh = nullptr;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta=(AllowPrivateAccess=true), Category="Components")
	UArrowComponent* Arrow = nullptr;

	UPROPERTY()
	ABall* CurrentBall = nullptr;
	
public:
	APaddle();

protected:
	virtual void OnConstruction(const FTransform& Transform) override;
	virtual void BeginPlay() override;
	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

public:
	UPROPERTY(EditDefaultsOnly, Category = "Settings | Game", meta=(tooltip="Класс мяча для спавна"))
	TSubclassOf<ABall> BallClass = nullptr;
	UPROPERTY(EditDefaultsOnly, Category = "Settings | Game")
	int32 Lives = 3;
	UPROPERTY(EditDefaultsOnly, Category = "Settings | Game")
	FVector DefaultScale = FVector(0.4f, 2.4f, 0.5);
	UPROPERTY(EditDefaultsOnly, Category = "Settings | Game")
	float Speed = 2000.0f;
};
