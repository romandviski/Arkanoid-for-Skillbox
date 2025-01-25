// Mikhail Efremov. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Block.generated.h"

class ABonusParent;
class ULifeComponent;

UCLASS()
class ARKANOID_API ABlock : public AActor
{
	GENERATED_BODY()
	
private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Components,
		meta = (AllowPrivateAccess = "true"))
	UStaticMeshComponent* StaticMesh = nullptr;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Components,
		meta = (AllowPrivateAccess = "true"))
	ULifeComponent* LifeComponent = nullptr;

	TSubclassOf<ABonusParent> BonusClass = nullptr;
	int32 MaxLife = 1;
	
public:	
	// Sets default values for this actor's properties
	ABlock();

protected:
	virtual void BeginPlay() override;
	virtual void NotifyHit(class UPrimitiveComponent* MyComp, AActor* Other,
		class UPrimitiveComponent* OtherComp, bool bSelfMoved, FVector HitLocation,
		FVector HitNormal, FVector NormalImpulse, const FHitResult& Hit) override;

public:
	void Init(const FVector NewScale, const int32 LifeAmount,
		const TSubclassOf<ABonusParent> NewBonusClass = nullptr);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings")
	int32 ScoreByLife = 25;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings")
	TArray<UMaterialInterface*> LifeMaterials;
};
