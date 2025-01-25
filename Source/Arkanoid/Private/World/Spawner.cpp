// Mikhail Efremov. All rights reserved.


#include "World/Spawner.h"
#include "Components/ArrowComponent.h"

// Sets default values
ASpawner::ASpawner()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	SceneRoot = CreateDefaultSubobject<USceneComponent>(TEXT("Scene Root"));
	SetRootComponent(SceneRoot);

	ForwardArrow = CreateDefaultSubobject<UArrowComponent>(TEXT("Forward Arrow"));
	ForwardArrow->SetupAttachment(SceneRoot);
	ForwardArrow->SetAbsolute(false,false,true);
}

// Called when the game starts or when spawned
void ASpawner::BeginPlay()
{
	Super::BeginPlay();

	SpawnActor();
}

void ASpawner::SpawnActor()
{
	if (SpawnedClass)
	{
		if (const auto World = GetWorld())
		{
			World->SpawnActor<AActor>(SpawnedClass, ForwardArrow->GetComponentLocation(), ForwardArrow->GetComponentRotation());
		}
	}
}
