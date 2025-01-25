// Mikhail Efremov. All rights reserved.


#include "Framework/Paddle.h"

#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Components/ArrowComponent.h"
#include "Components/BoxComponent.h"
#include "Kismet/GameplayStatics.h"
#include "World/Ball.h"


void APaddle::SpawnBallLives()
{
	UStaticMesh* Mesh = LoadObject<UStaticMesh>(nullptr, TEXT("/Engine/BasicShapes/Sphere.Sphere"));
	UMaterialInterface* Material = LoadObject<UMaterialInterface>(nullptr, TEXT("/Engine/BasicShapes/BasicShapeMaterial.BasicShapeMaterial"));

	if (!Mesh || !Material)
	{
		return;
	}

	for (int8 i = 0; i < Lives - 1; ++i)
	{
		auto NewMeshComponent = NewObject<UStaticMeshComponent>(this, *FString::Printf(TEXT("Lives %d"), i + 1));
		if (NewMeshComponent)
		{
			NewMeshComponent->SetStaticMesh(Mesh);
			NewMeshComponent->SetMaterial(0, Material);
			NewMeshComponent->SetAbsolute(false, false,true);
			NewMeshComponent->SetWorldScale3D(FVector(0.5f));
			NewMeshComponent->SetupAttachment(StaticMesh);
			NewMeshComponent->RegisterComponent();

			BallLives.Add(NewMeshComponent);
		}
	}
	
	UpdateBallLivesLocation();
}

void APaddle::UpdateBallLivesLocation()
{
	// Симметричное расположение шариков, constexpr - вычисляется на этапе компиляции
	constexpr float BallSpacing = 30.0f; // Расстояние между шариками
	const int8 NumBalls = BallLives.Num(); // Количество шариков
	// Рассчитываем начальное смещение для симметричного расположения
	const float TotalWidth = (NumBalls - 1) * BallSpacing; // Общая ширина расположения шариков
	const float StartOffset = -TotalWidth / 2; // Начальное смещение для первого шарика

	// Расставляем шарики симметрично относительно центра
	for (int8 i = 0; i < NumBalls; ++i)
	{
		const float Offset = StartOffset + i * BallSpacing;
		if(IsValid(BallLives[i]))
		{
			BallLives[i]->SetRelativeLocation(FVector(-100, Offset, 0.0f));
		}
	}
}

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

	if (APlayerController* PlayerController = Cast<APlayerController>(Controller))
	{
		const auto Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer());
		if (Subsystem)
		{
			Subsystem->AddMappingContext(DefaultMappingContext, 0);
		}
	}

	SpawnBall();
	SpawnBallLives();
}

// Called to bind functionality to input
void APaddle::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	const auto EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent);
	if (EnhancedInputComponent)
	{
		EnhancedInputComponent->BindAction(EscapeAction, ETriggerEvent::Started, this, &APaddle::ExitGame);
		EnhancedInputComponent->BindAction(SpawnBallAction, ETriggerEvent::Started, this, &APaddle::StartGame);
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &APaddle::Move);
	}
}

void APaddle::ExitGame()
{
	UGameplayStatics::OpenLevel(GetWorld(),"Menu", true);
}

void APaddle::StartGame()
{
	if (CurrentBall)
	{
		CurrentBall->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
		CurrentBall->SetBallState(EState::Moving);
	}
}

void APaddle::Move(const FInputActionValue& Value)
{
	const FVector2D AxisVector = Value.Get<FVector2D>();

	if (Controller)
	{
		const float CurrentSpeed = AxisVector.X * Speed * UGameplayStatics::GetWorldDeltaSeconds(GetWorld());
		AddActorWorldOffset(FVector(0.0f, CurrentSpeed, 0.0f), true);
	}
}

void APaddle::SpawnBall()
{
	if (BallClass && !CurrentBall)
	{
		const FVector SpawnLocation = Arrow->GetComponentLocation();
		const FRotator SpawnRotation = Arrow->GetComponentRotation();

		CurrentBall = GetWorld()->SpawnActor<ABall>(BallClass, SpawnLocation, SpawnRotation);

		if (CurrentBall)
		{
			CurrentBall->SetOwner(this);
			CurrentBall->SetBallState(EState::Idle);
			CurrentBall->OnDeadEvent.AddDynamic(this, &APaddle::BallIsDead);
			CurrentBall->AttachToActor(this, FAttachmentTransformRules::KeepWorldTransform);
		}
	}
}

void APaddle::BallIsDead()
{
	CurrentBall = nullptr;
	Lives = FMath::Max(Lives - 1, 0);

	if (Lives)
	{
		SpawnBall();
		BallLives[Lives - 1]->DestroyComponent();
		BallLives.RemoveAt(Lives - 1);
		UpdateBallLivesLocation();
	}
}
