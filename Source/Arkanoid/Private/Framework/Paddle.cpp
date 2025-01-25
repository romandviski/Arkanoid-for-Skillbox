// Mikhail Efremov. All rights reserved.


#include "Framework/Paddle.h"
#include "Components/ArrowComponent.h"
#include "Components/BoxComponent.h"


APaddle::APaddle()
{
	PrimaryActorTick.bCanEverTick = false;

	BoxCollider = CreateDefaultSubobject<UBoxComponent>(TEXT("Box Collider"));
	BoxCollider->SetBoxExtent(FVector(25.0f, 50.0f, 25.0f));
	BoxCollider->SetCollisionResponseToAllChannels(ECR_Block);
	SetRootComponent(BoxCollider);

	StaticMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Static Mesh"));
	StaticMesh->SetupAttachment(BoxCollider);

	LeftStaticMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Left Static Mesh"));
	LeftStaticMesh->SetupAttachment(StaticMesh);
	LeftStaticMesh->AddRelativeLocation(FVector(0.0f, -50.0f, 0.0f));
	LeftStaticMesh->SetAbsolute(false, false, true);

	RightStaticMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Right Static Mesh"));
	RightStaticMesh->SetupAttachment(StaticMesh);
	RightStaticMesh->AddRelativeLocation(FVector(0.0f, 50.0f, 0.0f));
	RightStaticMesh->SetAbsolute(false, false, true);

	Arrow = CreateDefaultSubobject<UArrowComponent>(TEXT("Arrow"));
	Arrow->SetupAttachment(StaticMesh);
	Arrow->AddRelativeLocation(FVector(150.0f, 0.0f, 0.0f));
	Arrow->SetAbsolute(false, false, true);
}

void APaddle::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);

	SetActorScale3D(DefaultScale);
	BoxCollider->SetBoxExtent(FVector(25.0f, 50.0f + 20.0f / DefaultScale.Y, 25.0f));
	const FVector TempScale = FVector(GetActorScale().X, GetActorScale().X, GetActorScale().Z);
	LeftStaticMesh->SetWorldScale3D(TempScale);
	RightStaticMesh->SetWorldScale3D(TempScale);
}

void APaddle::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called to bind functionality to input
void APaddle::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

