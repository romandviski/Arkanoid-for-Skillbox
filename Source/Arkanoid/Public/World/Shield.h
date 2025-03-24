// Mikhail Efremov. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Shield.generated.h"

class ULifeComponent;

UCLASS()
class ARKANOID_API AShield : public AActor
{
	GENERATED_BODY()

private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Components,
	meta = (AllowPrivateAccess = "true"))
	USceneComponent* SceneRoot = nullptr;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Components,
		meta = (AllowPrivateAccess = "true"))
	UStaticMeshComponent* StaticMesh = nullptr;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Components,
		meta = (AllowPrivateAccess = "true"))
	ULifeComponent* LifeComponent = nullptr;
	
public:
	AShield();

	// От количества материалов зависит количество жизней
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings")
	TArray<UMaterialInterface*> LifeMaterials;

	void Init(const int32 Life, const float LifeTime);
	
protected:
	virtual void BeginPlay() override;
	virtual void NotifyHit(class UPrimitiveComponent* MyComp, AActor* Other,
		class UPrimitiveComponent* OtherComp, bool bSelfMoved, FVector HitLocation,
		FVector HitNormal, FVector NormalImpulse, const FHitResult& Hit) override;

	void UpdateMaterial();
	void UpdateLife(const int32 NewLife);
	
};
