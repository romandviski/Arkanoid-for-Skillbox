// Mikhail Efremov. All rights reserved.


#include "World/MyKillZone.h"

#include "Components/ArrowComponent.h"
#include "Components/BoxComponent.h"

// Sets default values
AMyKillZone::AMyKillZone()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	SceneRoot = CreateDefaultSubobject<USceneComponent>(TEXT("Scene Root"));
	SetRootComponent(SceneRoot);

	ForwardArrow = CreateDefaultSubobject<UArrowComponent>(TEXT("Forward Arrow"));
	ForwardArrow->SetupAttachment(SceneRoot);
	ForwardArrow->SetAbsolute(false,false,true);

	KillBox = CreateDefaultSubobject<UBoxComponent>(TEXT("Kill Box"));
	KillBox->SetupAttachment(SceneRoot);
	KillBox->SetBoxExtent(FVector(50.0f,500.0f,50.0f));
}

void AMyKillZone::NotifyActorBeginOverlap(AActor* OtherActor)
{
	Super::NotifyActorBeginOverlap(OtherActor);

	if (OtherActor && OtherActor != this)
	{
		OtherActor->Destroy();
	}
}
