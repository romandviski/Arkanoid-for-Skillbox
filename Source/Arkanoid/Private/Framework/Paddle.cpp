// Mikhail Efremov. All rights reserved.


#include "Framework/Paddle.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Components/ArrowComponent.h"
#include "Components/BoxComponent.h"
#include "Framework/ArkanoidGameMode.h"
#include "Framework/ArkanoidPC.h"
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

	for (const auto BallLive : BallLives)
	{
		BallLive->DestroyComponent();
	}
	BallLives.Empty();

	for (int8 i = 0; i < Lives - 1; ++i)
	{
		auto NewMeshComponent = NewObject<UStaticMeshComponent>(this, *FString::Printf(TEXT("Lives %d"), i + 1));
		if (NewMeshComponent)
		{
			NewMeshComponent->SetStaticMesh(Mesh);
			NewMeshComponent->SetMaterial(0, Material);
			NewMeshComponent->SetAbsolute(false, false,true);
			NewMeshComponent->SetWorldScale3D(FVector(0.3f * DefaultScale.X * 2));
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
	const float BallSpacing = 30.0f * DefaultScale.X * 2; // Расстояние между шариками
	const int8 NumBalls = BallLives.Num(); // Количество шариков

	// Учитываем масштаб каретки по оси Y
	const float PaddleScaleY = GetActorScale().Y;
	
	// Рассчитываем начальное смещение для симметричного расположения
	const float TotalWidth = (NumBalls - 1) * BallSpacing; // Общая ширина расположения шариков
	const float StartOffset = -TotalWidth / 2; // Начальное смещение для первого шарика

	// Расставляем шарики симметрично относительно центра
	for (int8 i = 0; i < NumBalls; ++i)
	{
		const float Offset = StartOffset + i * BallSpacing;
		if(IsValid(BallLives[i]))
		{
			BallLives[i]->SetRelativeLocation(FVector(-100, Offset / PaddleScaleY, 0.0f));
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

void APaddle::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);

	// Код для обработки изменения свойства
	if (PropertyChangedEvent.Property != nullptr)
	{
		const FName PropertyName = PropertyChangedEvent.Property->GetFName();
		if (PropertyName == GET_MEMBER_NAME_CHECKED(APaddle, DefaultScale))
		{
			// Реагируем на изменение только DefaultScale
			//Arrow->SetRelativeLocation(FVector(150.0f / DefaultScale.X, 0.0f, 0.0f));
		}
	}
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

void APaddle::NotifyHit(class UPrimitiveComponent* MyComp, AActor* Other, class UPrimitiveComponent* OtherComp, bool bSelfMoved, FVector HitLocation, FVector HitNormal, FVector NormalImpulse, const FHitResult& Hit)
{
	if (bIsSticky && Other)
	{
		if (CurrentBall == Cast<ABall>(Other))
		{
			CurrentBall->SetBallState(EState::Idle);
			CurrentBall->SetActorLocation(Arrow->GetComponentLocation());
			CurrentBall->AttachToComponent(Arrow, FAttachmentTransformRules::SnapToTargetNotIncludingScale);
		}
		
		bIsSticky = false;
	}
}


void APaddle::ExitGame()
{
	//UGameplayStatics::OpenLevel(GetWorld(),"Menu", true);
	if (const auto Pc = Cast<AArkanoidPC>(Controller))
	{
		Pc->ExitButtonPressed();	
	}
}

void APaddle::StartGame()
{
	if (CurrentBall)
	{
		CurrentBall->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
		CurrentBall->SetBallState(EState::Moving);
		StopBonusStickyEffect();
		bIsSticky = false;
	}
}

void APaddle::Move(const FInputActionValue& Value)
{
	const FVector2D AxisVector = Value.Get<FVector2D>();

	if (Controller)
	{
		DirectionAxis = AxisVector.X;
		
		const float CurrentSpeed = DirectionAxis * Speed * UGameplayStatics::GetWorldDeltaSeconds(GetWorld());
		AddActorWorldOffset(FVector(0.0f, CurrentSpeed, 0.0f), true);
	}
}

void APaddle::SpawnBall()
{
	if (BallClass && !CurrentBall)
	{
		FTransform SpawnTransform = Arrow->GetComponentTransform();
		CurrentBall = GetWorld()->SpawnActor<ABall>(BallClass, SpawnTransform);

		if (CurrentBall)
		{
			CurrentBall->SetOwner(this);
			CurrentBall->SetBallState(EState::Idle);
			CurrentBall->OnDeadEvent.AddDynamic(this, &APaddle::BallIsDead);
			//CurrentBall->AttachToActor(this, FAttachmentTransformRules::KeepWorldTransform);
			CurrentBall->AttachToComponent(Arrow, FAttachmentTransformRules::SnapToTargetNotIncludingScale);
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
	else
	{
		if (const auto Gm = Cast<AArkanoidGameMode>(GetWorld()->GetAuthGameMode()))
			Gm->GameEnded(false);
	}
}

void APaddle::SetDefaultSize()
{
	SetActorScale3D(DefaultScale);
	BoxCollider->SetBoxExtent(FVector(25.0f, 50.0f + 20.0f / DefaultScale.Y, 25.0f));
	// Обновляем позиции шариков жизни
	UpdateBallLivesLocation();
}

void APaddle::BonusChangeSize(const float AdditionalSize, const float BonusTime)
{
	if (AdditionalSize && BonusTime)
	{
		if (!GetWorld()->GetTimerManager().IsTimerActive(TimerForBonusSize))
		{
			FVector TempScale = GetActorScale3D();
			TempScale.Y = TempScale.Y + TempScale.Y * AdditionalSize;
			SetActorScale3D(TempScale);
			BoxCollider->SetBoxExtent(FVector(25.0f, 50.0f + 20.0f / TempScale.Y, 25.0f));
			// Обновляем позиции шариков жизни
			UpdateBallLivesLocation();
		}

		GetWorld()->GetTimerManager().SetTimer(
			TimerForBonusSize,
			this,
			&APaddle::SetDefaultSize,
			BonusTime,
			false);
	}
}

void APaddle::BonusChangeLife(const int32 Amount)
{
	Lives += Amount;
	SpawnBallLives();
}

void APaddle::BonusChangeBallSpeed(const float Amount)
{
	if (IsValid(CurrentBall))
	{
		CurrentBall->ChangeSpeed(Amount);
	}
}

void APaddle::BonusChangeBallPower(const int32 Amount, const float BonusTime)
{
	if (IsValid(CurrentBall))
	{
		CurrentBall->ChangeBallPower(Amount, BonusTime);
	}
}

void APaddle::BonusBall(const float BallLifeTime) const
{
	if (BallClass)
	{
		FVector SpawnLocation = Arrow->GetComponentLocation();
		const FRotator SpawnRotation = Arrow->GetComponentRotation();

		if (IsValid(CurrentBall))
			SpawnLocation.X = SpawnLocation.X + 50.0f;
		
		const auto BonusBall = GetWorld()->SpawnActor<ABall>(BallClass, SpawnLocation, SpawnRotation);
		if (BonusBall)
			BonusBall->SetBallBonus(BallLifeTime);
	}
}

void APaddle::BonusManyBalls(const float BallsLifeTime) const
{
	if (BallClass && CurrentBall)
	{
		// Количество акторов для спавна
		constexpr int8 NumBalls = 10;
		// Радиус круга
		constexpr float Radius = 100.0f;

		const FVector SpawnerLocation = CurrentBall->GetActorLocation();

		for (int8 i = 0; i < NumBalls; ++i)
		{
			// Случайный угол для направления шарика
			const float Angle = FMath::FRandRange(0.0f, 2 * PI);
			// Вычисляем позицию по кругу
			FVector Offset(Radius * FMath::Cos(Angle), Radius * FMath::Sin(Angle), 0.0f);
			FVector SpawnLocation = SpawnerLocation + Offset;

			// Ротируем актор в направлении от центра круга
			FRotator SpawnRotation = (SpawnLocation - SpawnerLocation).Rotation();

			// Спавним актор
			const auto SpawnedBall = GetWorld()->SpawnActor<ABall>(BallClass, SpawnLocation, SpawnRotation);
			if (SpawnedBall)
				SpawnedBall->SetBallBonus(BallsLifeTime);
		}
	}
}

void APaddle::BonusSticky()
{
	bIsSticky = true;
	PlayBonusStickyEffect();
}
