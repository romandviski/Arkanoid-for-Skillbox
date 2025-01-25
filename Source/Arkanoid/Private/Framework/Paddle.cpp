// Mikhail Efremov. All rights reserved.


#include "Framework/Paddle.h"

#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Components/ArrowComponent.h"
#include "Components/BoxComponent.h"
#include "Kismet/GameplayStatics.h"


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

