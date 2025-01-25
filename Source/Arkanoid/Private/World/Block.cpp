// Mikhail Efremov. All rights reserved.


#include "World/Block.h"
#include "World/Ball.h"

// Sets default values
ABlock::ABlock()
{
	PrimaryActorTick.bCanEverTick = false;

	StaticMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Static Mesh"));
	SetRootComponent(StaticMesh);
}

void ABlock::NotifyHit(class UPrimitiveComponent* MyComp, AActor* Other, class UPrimitiveComponent* OtherComp,
	bool bSelfMoved, FVector HitLocation, FVector HitNormal, FVector NormalImpulse, const FHitResult& Hit)
{
	Super::NotifyHit(MyComp, Other, OtherComp, bSelfMoved, HitLocation, HitNormal, NormalImpulse, Hit);

	if (Cast<ABall>(Other))
	{
		if (BonusClass && GetWorld())
		{
			GetWorld()->SpawnActor<AActor>(BonusClass, GetActorLocation(), GetActorRotation());
		}

		Destroy();
	}
}

void ABlock::Init(const FVector NewScale, const int32 LifeAmoun, const TSubclassOf<AActor> NewBonusClass)
{
	SetActorScale3D(NewScale);
	BonusClass = NewBonusClass;
}
