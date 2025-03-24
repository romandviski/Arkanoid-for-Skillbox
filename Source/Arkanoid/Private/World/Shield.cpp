// Mikhail Efremov. All rights reserved.

#include "World/Shield.h"
#include "Components/LifeComponent.h"
#include "World/Ball.h"


AShield::AShield()
{
	PrimaryActorTick.bCanEverTick = false;

	SceneRoot = CreateDefaultSubobject<USceneComponent>(TEXT("SceneRoot"));
	SetRootComponent(SceneRoot);
	
	StaticMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Static Mesh"));
	StaticMesh->SetCollisionProfileName(TEXT("BlockAll"));
	StaticMesh->SetCollisionResponseToChannel(ECC_Pawn, ECR_Ignore);
	StaticMesh->SetupAttachment(SceneRoot);

	LifeComponent = CreateDefaultSubobject<ULifeComponent>(TEXT("Life Component"));
}

void AShield::Init(const int32 Life, const float LifeTime)
{
	UpdateLife(Life);
	
	if (LifeTime > 1)
		SetLifeSpan(LifeTime);
}

void AShield::BeginPlay()
{
	Super::BeginPlay();
	
	UpdateLife(LifeMaterials.Num());
}

void AShield::NotifyHit(class UPrimitiveComponent* MyComp, AActor* Other, class UPrimitiveComponent* OtherComp,
					   bool bSelfMoved, FVector HitLocation, FVector HitNormal, FVector NormalImpulse, const FHitResult& Hit)
{
	Super::NotifyHit(MyComp, Other, OtherComp, bSelfMoved, HitLocation, HitNormal, NormalImpulse, Hit);

	if (const auto Ball = Cast<ABall>(Other))
	{
		if (IsValid(LifeComponent))
		{
			LifeComponent->TakeDamage();
			
			if (LifeComponent->IsAlive())
			{
				UpdateMaterial();
			}
			else
			{
				Destroy();
			}
		}
	}
}

void AShield::UpdateMaterial()
{
	if (LifeComponent)
		if (LifeMaterials.IsValidIndex(LifeComponent->GetLife() - 1))
			StaticMesh->SetMaterial(0, LifeMaterials[LifeComponent->GetLife() - 1]);
}

void AShield::UpdateLife(const int32 NewLife)
{
	if (LifeComponent)
		LifeComponent->SetLife(NewLife > LifeMaterials.Num() ? LifeMaterials.Num() : NewLife);

	UpdateMaterial();
}
