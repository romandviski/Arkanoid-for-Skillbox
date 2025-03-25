// Mikhail Efremov. All rights reserved.


#include "World/Block.h"

#include "Bonuses/BonusParent.h"
#include "Components/LifeComponent.h"
#include "Framework/ArkanoidPlayerState.h"
#include "World/Ball.h"

// Sets default values
ABlock::ABlock()
{
	PrimaryActorTick.bCanEverTick = false;

	StaticMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Static Mesh"));
	StaticMesh->SetCollisionProfileName(TEXT("BlockAll"));
	StaticMesh->SetCollisionResponseToChannel(ECC_Pawn, ECR_Ignore); 
	SetRootComponent(StaticMesh);

	LifeComponent = CreateDefaultSubobject<ULifeComponent>(TEXT("Life Component"));
}

void ABlock::BeginPlay()
{
	Super::BeginPlay();

	if (LifeMaterials.IsValidIndex(LifeComponent->GetLife() - 1))
		StaticMesh->SetMaterial(0, LifeMaterials[LifeComponent->GetLife() - 1]);
}

void ABlock::NotifyHit(class UPrimitiveComponent* MyComp, AActor* Other, class UPrimitiveComponent* OtherComp,
                       bool bSelfMoved, FVector HitLocation, FVector HitNormal, FVector NormalImpulse, const FHitResult& Hit)
{
	Super::NotifyHit(MyComp, Other, OtherComp, bSelfMoved, HitLocation, HitNormal, NormalImpulse, Hit);

	if (const auto Ball = Cast<ABall>(Other))
	{
		if (IsValid(LifeComponent))
		{
			LifeComponent->TakeDamage(Ball->GetPower());

			if (!LifeComponent->IsAlive())
			{
				if (BonusClass && GetWorld())
				{
					const auto CurrentBonus = GetWorld()->SpawnActor<ABonusParent>(BonusClass, GetActorLocation(), FRotator::ZeroRotator);
					//CurrentBonus->InitScale(GetActorScale3D());
				}

				if (const auto Pawn = Cast<APawn>(Other->GetOwner()))
				{
					if (auto PlayerState = Cast<AArkanoidPlayerState>(Pawn->GetPlayerState()))
						PlayerState->ChangePlayerScore(ScoreByLife * MaxLife);
				}
				
				Destroy();
			}
			else
			{
				if (LifeMaterials.IsValidIndex(LifeComponent->GetLife() - 1))
					StaticMesh->SetMaterial(0, LifeMaterials[LifeComponent->GetLife() - 1]);
			}
		}
	}
}

void ABlock::Init(const FVector NewScale, const int32 LifeAmount, const TSubclassOf<ABonusParent> NewBonusClass)
{
	SetActorScale3D(NewScale);
	BonusClass = NewBonusClass;
	LifeComponent->SetLife(LifeAmount);
	MaxLife = LifeAmount;
	
	if (LifeMaterials.IsValidIndex(LifeComponent->GetLife() - 1))
		StaticMesh->SetMaterial(0, LifeMaterials[LifeComponent->GetLife() - 1]);
}
