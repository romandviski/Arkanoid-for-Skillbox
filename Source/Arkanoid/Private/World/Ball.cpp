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

void ABall::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);

	SetActorScale3D(FVector(InitParameters.Scale));
	Power = InitParameters.Power;
	Speed = InitParameters.Speed;
}

// Called when the game starts or when spawned
void ABall::BeginPlay()
{
	Super::BeginPlay();
	
	Direction = GetActorForwardVector().GetSafeNormal();
	SetBallState(EState::Moving);
}

// Called every frame
void ABall::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	switch (State)
	{
		case EState::Idle:
			break;
		case EState::Moving:
			Move(DeltaTime);
			break;
		default:
			UE_LOG(LogTemp, Warning, TEXT("Unknown Ball State"));
			break;
	}
}

void ABall::Destroyed()
{
	OnDeadEvent.Broadcast();
	
	Super::Destroyed();
}

void ABall::Move(const float DeltaTime)
{
	const FVector Offset = Direction * Speed * DeltaTime;
	FHitResult HitResult;
	AddActorWorldOffset(Offset, true, &HitResult);

	if (HitResult.bBlockingHit)
	{
		/*
		Формула для вычисления отраженного вектора выглядит так:
		ReflectedDirection = Direction − 2 * (Direction ⋅ Normal) * Normal
		(в выражении (Direction ⋅ Normal) используется скалярное произведение!)
		*/
		Direction = Direction - 2 * (FVector::DotProduct(Direction, HitResult.Normal)) * HitResult.Normal;
		Direction.Z = 0.0f;
		Direction = Direction.GetSafeNormal();

		if (Speed < InitParameters.MaxSpeed)
		{
			Speed += InitParameters.Speed * 0.1f;
			Speed = FMath::Min(Speed, InitParameters.MaxSpeed);
		}
		
		// выведем скорость для дебага
		// https://www.chrismccole.com/blog/logging-in-ue4-cpp
		// https://unrealcommunity.wiki/logging-lgpidy6i
		UE_LOG(LogTemp, Warning, TEXT("Ball name %s is speed %f"), *GetName(), Speed);
	}
}

void ABall::SetBallState(const EState NewState)
{
	State = NewState;
}

