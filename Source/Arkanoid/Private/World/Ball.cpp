// Mikhail Efremov. All rights reserved.


#include "World/Ball.h"
#include "Components/ArrowComponent.h"


// Sets default values
ABall::ABall()
{
	PrimaryActorTick.bCanEverTick = true;

	StaticMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Static Mesh"));
	SetRootComponent(StaticMesh);

	ForwardArrow = CreateDefaultSubobject<UArrowComponent>(TEXT("Forward Arrow"));
	ForwardArrow->SetupAttachment(StaticMesh);

	// Загружаем статичный меш сферы (заполнять лучше в блюпринтах)
	static ConstructorHelpers::FObjectFinder<UStaticMesh> SphereMeshAsset(TEXT("/Engine/BasicShapes/Sphere.Sphere"));
	if(SphereMeshAsset.Succeeded())
	{
		StaticMesh->SetStaticMesh(SphereMeshAsset.Object);
	}
}

// Called when the game starts or when spawned
void ABall::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ABall::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

