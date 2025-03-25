// Mikhail Efremov. All rights reserved.


#include "World/Ball.h"
#include "Components/ArrowComponent.h"
#include "Components/AudioComponent.h"
#include "Framework/Paddle.h"


// Sets default values
ABall::ABall()
{
	PrimaryActorTick.bCanEverTick = true;

	StaticMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Static Mesh"));
	SetRootComponent(StaticMesh);
	
	//StaticMesh->SetCollisionProfileName(TEXT("BlockAllDynamic"));
	StaticMesh->SetCollisionObjectType(ECC_WorldDynamic);
	StaticMesh->SetCollisionResponseToAllChannels(ECR_Overlap);
	// Если нужно изменить конкретные каналы:
	StaticMesh->SetCollisionResponseToChannel(ECC_WorldStatic, ECR_Block); // Блокировать статические объекты
	StaticMesh->SetCollisionResponseToChannel(ECC_Pawn, ECR_Block); 
	
	ForwardArrow = CreateDefaultSubobject<UArrowComponent>(TEXT("Forward Arrow"));
	ForwardArrow->SetupAttachment(StaticMesh);

	AudioComponent = CreateDefaultSubobject<UAudioComponent>(TEXT("AudioComponent"));
	AudioComponent->SetupAttachment(StaticMesh);
	AudioComponent->SetAutoActivate(false);

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

	if (StaticMesh)
	{
		DefaultMaterial = StaticMesh->GetMaterial(0);
	}
	
	UpdateBallMaterial();
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

	// Вращение меша шара
	if (StaticMesh)
	{
		// Вычисляем угловую скорость (вращение) в зависимости от скорости и радиуса шара
		const float BallRadius = StaticMesh->Bounds.SphereRadius; // Получаем радиус шара
		const float AngularVelocity = (Speed / BallRadius) * 0.5f; // Угловая скорость с коэффициентом

		// Вектор оси вращения (перпендикулярно направлению движения)
		const FVector RotationAxis = FVector::CrossProduct(FVector::UpVector, Direction).GetSafeNormal();

		// Вычисляем угол вращения в радианах
		const float RotationAngle = AngularVelocity * DeltaTime;

		// Создаем кватернион вращения
		const FQuat DeltaRotation = FQuat(RotationAxis, RotationAngle);

		// Применяем вращение к мешу в мировых координатах
		StaticMesh->AddWorldRotation(DeltaRotation);
	}
	
	if (HitResult.bBlockingHit)
	{
		AudioComponent->Play();

		/*
		*	Формула для вычисления отраженного вектора выглядит так:
		*	ReflectedDirection = Direction − 2 * (Direction ⋅ Normal) * Normal
		*	(в выражении (Direction ⋅ Normal) используется скалярное произведение!)
		*/
		Direction = Direction - 2 * (FVector::DotProduct(Direction, HitResult.Normal)) * HitResult.Normal;
		
		const auto GamePaddle = Cast<APaddle>(HitResult.GetActor());
		if (GamePaddle)
		{
			Direction.Y += GamePaddle->GetDirectionAxis();
		}
		else
		{
			if (Speed < InitParameters.MaxSpeed)
			{
				Speed += InitParameters.Speed * 0.01f;
				Speed = FMath::Min(Speed, InitParameters.MaxSpeed);
			}
		}
		
		Direction.Z = 0.0f;
		Direction = Direction.GetSafeNormal();
		
		// выведем скорость для дебага
		// https://www.chrismccole.com/blog/logging-in-ue4-cpp
		// https://unrealcommunity.wiki/logging-lgpidy6i
		//UE_LOG(LogTemp, Warning, TEXT("Ball name %s is speed %f"), *GetName(), Speed);
		// Добавляем случайность, если угол близок к 90 градусам
		AddRandomnessToDirection();
	}
}

void ABall::AddRandomnessToDirection()
{
	// Определяем угол между текущим направлением и вертикальной осью
	const FVector VerticalAxis = FVector::UpVector; // Вертикальная ось (Y-ось)
	const float DotProduct = FVector::DotProduct(Direction, VerticalAxis);
	const float AngleRadians = FMath::Acos(DotProduct); // Угол в радианах
	const float AngleDegrees = FMath::RadiansToDegrees(AngleRadians); // Угол в градусах

	// Если угол близок к 90 градусам (например, ±10 градусов), добавляем случайность
	const float AngleThreshold = 10.0f; // Пороговое значение в градусах
	if (FMath::Abs(AngleDegrees - 90.0f) <= AngleThreshold)
	{
		// Генерируем случайное отклонение
		const float RandomDeviation = FMath::RandRange(-0.2f, 0.2f); // Случайное число от -0.2 до 0.2
		Direction.X += RandomDeviation;
		Direction.Y += RandomDeviation;

		// Нормализуем направление, чтобы сохранить единичную длину
		Direction = Direction.GetSafeNormal();
	}
}

void ABall::UpdateBallMaterial()
{
	if (!StaticMesh)
		return;

	if (Power > 1)
	{
		if (PowerMaterial)
		{
			StaticMesh->SetMaterial(0, PowerMaterial);
		}
	}
	else
	{
		StaticMesh->SetMaterial(0, DefaultMaterial);
	}
}

void ABall::ResetBallPower()
{
	Power = InitParameters.Power;
	UpdateBallMaterial();
}

void ABall::SetBallState(const EState NewState)
{
	State = NewState;
}

void ABall::ChangeSpeed(const float Amount)
{
	if (Amount < 0)
	{
		Speed = FMath::Min(Speed - Speed * Amount, InitParameters.Speed);
	}
	else if (Amount > 0)
	{
		Speed = FMath::Max(Speed + Speed * Amount, InitParameters.MaxSpeed);
	}
}

void ABall::ChangeBallPower(const int32 Amount, const float BonusTime)
{
	if (Amount != 0 && BonusTime > 0)
	{
		if (!GetWorld()->GetTimerManager().IsTimerActive(TimerBallPower))
		{
			Power = FMath::Max(Power + Amount, 1);
			UpdateBallMaterial();
		}

		GetWorld()->GetTimerManager().SetTimer(
			TimerBallPower,
			this,
			&ABall::ResetBallPower,
			BonusTime,
			true);
	}
}

void ABall::SetBallBonus(const float BallLifeTime)
{
	if (StaticMesh)
	{
		StaticMesh->SetVectorParameterValueOnMaterials(
			"Emissive color 01", FVector(40.0f, 40.0f, 0.03f));
		StaticMesh->SetVectorParameterValueOnMaterials(
			"Emissive color 02", FVector(40.0f, 40.0f, 0.03f));
	}

	SetActorScale3D(FVector(0.3f));
	SetLifeSpan(BallLifeTime);
}
