// Mikhail Efremov. All rights reserved.


#include "World/PlayingBoard.h"
#include "Bonuses/BonusParent.h"
#include "Framework/ArkanoidGameMode.h"
#include "Framework/Paddle.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "World/Block.h"

void APlayingBoard::CreatePreviewComponents()
{
	if (GridSizeX <= 0 || GridSizeY <= 0)
	{
		UE_LOG(LogTemp, Error, TEXT("The grid size must be positive"));
		return;
	}
	
	// Вычисляем размер блока в юнитах
	const float BlockWidth = BlockScale.X * 100.0f;
	const float BlockHeight = BlockScale.Y * 100.0f;
	const float BlockDepth = BlockScale.Z * 100.0f;

	// Вычисляем общий размер сетки с учетом отступов
	const float TotalWidth = GridSizeX * BlockWidth + (GridSizeX - 1) * SpacingX;
	const float TotalHeight = GridSizeY * BlockHeight + (GridSizeY - 1) * SpacingY;

	// Вычисляем смещение для центровки сетки
	const FVector CenterOffset = FVector((TotalWidth - BlockWidth) / 2, (TotalHeight - BlockHeight) / 2, 0.0f);
	// Получаем текущую трансформацию объекта (позиция и поворот)
	const FTransform ActorTransform = GetActorTransform();

	// Создаем превью-компоненты
	for (int32 x = 0; x < GridSizeX; x++)
	{
		for (int32 y = 0; y < GridSizeY; y++)
		{
			if (PreviewMesh)
			{
				// Вычисляем позицию блока с учетом отступов от края
				const float XOffset = x * (BlockWidth + SpacingX);
				const float YOffset = y * (BlockHeight + SpacingY);
				const FVector PreviewLocation = FVector(XOffset, YOffset, 0.0f) - CenterOffset;
				// Преобразуем локальную позицию в мировую с учетом поворота объекта
				const FVector WorldPreviewLocation = ActorTransform.TransformPosition(PreviewLocation);

				// Проверяем, есть ли что-то на месте превью-компонента
				FCollisionQueryParams CollisionParams;
				CollisionParams.AddIgnoredActor(this);

				// Используем размер блока в юнитах для проверки столкновений
				const FVector BoxExtents = FVector(BlockWidth * 0.5f, BlockHeight * 0.5f, BlockDepth * 0.5f);

				const bool bIsBlocked = GetWorld()->OverlapBlockingTestByChannel(
					WorldPreviewLocation, // Место проверки
					ActorTransform.GetRotation(), // Вращение объекта
					ECC_Visibility, // Канал проверки столкновений
					FCollisionShape::MakeBox(BoxExtents), // Размер области проверки
					CollisionParams
					);

				if (bIsBlocked)
				{
					// Визуализация области проверки
					DrawDebugBox(GetWorld(), WorldPreviewLocation, BoxExtents, ActorTransform.GetRotation(),
						FColor::Red, false, 10.0f);
					continue;
				}
				else
				{
					// Создаём превью меш компонент и настраиваем его
					UStaticMeshComponent* PreviewMeshComponent = NewObject<UStaticMeshComponent>(this);
					PreviewMeshComponent->SetStaticMesh(PreviewMesh);
					PreviewMeshComponent->AttachToComponent(GetRootComponent(), FAttachmentTransformRules::KeepRelativeTransform);
					PreviewMeshComponent->SetRelativeScale3D(BlockScale);
					PreviewMeshComponent->SetRelativeLocation(PreviewLocation);
					PreviewMeshComponent->RegisterComponent();
					PreviewComponents.Add(PreviewMeshComponent);
				}
			}
		}
	}
}

void APlayingBoard::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);

	ClearPreviewComponents();
	CreatePreviewComponents();
}

void APlayingBoard::ClearPreviewComponents()
{
	for(UStaticMeshComponent* PreviewComponent : PreviewComponents)
	{
		if (PreviewComponent)
		{
			PreviewComponent->DestroyComponent();
		}
	}
	PreviewComponents.Empty();
}

void APlayingBoard::SpawnBlockActors()
{
	for(UStaticMeshComponent* PreviewComponent : PreviewComponents)
	{
		if (PreviewComponent)
		{
			const FTransform SpawnTransform = PreviewComponent->GetComponentTransform();
			if (auto CurrentBlock = GetWorld()->SpawnActor<ABlock>(BlockClassForSpawn, SpawnTransform))
			{
				// Определяем количество жизней для куба на основе GameDifficulty
				// тернарная операция (условие ? выражение_если_истина : выражение_если_ложь;)
				const int32 Life = UKismetMathLibrary::RandomBoolWithWeight(GameDifficulty) ? 2 : 1;

				// Определяем, будет ли куб содержать бонус на основе BonusChance
				const auto BonusClass = UKismetMathLibrary::RandomBoolWithWeight(BonusChance) ? GetBonusClass() : nullptr;
				CurrentBlock->Init(BlockScale, Life, BonusClass);

				CurrentBlock->AttachToComponent(SceneRoot, FAttachmentTransformRules::KeepWorldTransform);
				CurrentBlock->OnDestroyed.AddDynamic(this, &APlayingBoard::OnBlockDestroyed);
				BlockActors.Add(CurrentBlock);
			}
		}
	}
}

void APlayingBoard::OnBlockDestroyed(AActor* DestroyedBlock)
{
	ABlock* Block = Cast<ABlock>(DestroyedBlock);
	if (Block)
	{
		BlockActors.Remove(Block);
		InitialBlockLocations.Remove(Block); // Очищаем уничтоженный блок
	}

	if (!BlockActors.Num())
	{
		if (const auto GM = Cast<AArkanoidGameMode>(GetWorld()->GetAuthGameMode()))
		{
			GM->GameEnded(true);
		}	
	}
	else if (BlockActors.Num() == BlocksToSlide  && !bIsDroppingBlocks) // Проверяем количество блоков
	{
		// Запускаем таймер
		GetWorld()->GetTimerManager().SetTimer(
			DropBlocksTimerHandle,
			this,
			&APlayingBoard::StartDroppingBlocks,
			DropTime, // Проверка каждые 10 секунд
			true // Повторяющийся таймер
		);

		StartDroppingBlocks();
	}
}

void APlayingBoard::StartDroppingBlocks()
{
	if (!bIsDroppingBlocks) // Проверяем, что опускание ещё не началось
	{
		bIsDroppingBlocks = true;
		DropElapsedTime = 0.0f; // Сбрасываем время

		// Сохраняем начальные позиции блоков
		InitialBlockLocations.Empty();
		for (ABlock* Block : BlockActors)
		{
			if (Block)
			{
				InitialBlockLocations.Add(Block, Block->GetActorLocation());
			}
		}

		// Вычисляем целевую позицию для опускания
		TargetDropLocation = FVector(-OneBlockHeight, 0.0f, 0.0f);
	}
}

APlayingBoard::APlayingBoard()
{
	PrimaryActorTick.bCanEverTick = true;

	SceneRoot = CreateDefaultSubobject<USceneComponent>(TEXT("Scene Root"));
	SetRootComponent(SceneRoot);
	
}

void APlayingBoard::BeginPlay()
{
	Super::BeginPlay();

	SpawnBlockActors();
	ClearPreviewComponents();

	const auto Paddle = UGameplayStatics::GetActorOfClass(GetWorld(), APaddle::StaticClass());
	if (Paddle)
		KillX = Paddle->GetActorLocation().X + 100.0f;

	OneBlockHeight = BlockScale.Y * 50.0f; // Высота одного блока

	// таймер для старта автоспуска
	/* 
	GetWorld()->GetTimerManager().SetTimer(
		DropBlocksTimerHandle,
		this,
		&APlayingBoard::StartDroppingBlocks,
		1.0f, // Проверка каждые 10 секунд
		true // Повторяющийся таймер
	);
	*/
}

void APlayingBoard::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (bIsDroppingBlocks)
	{
		DropElapsedTime += DeltaTime;

		// Вычисляем прогресс опускания (от 0 до 1)
		float Alpha = FMath::Clamp(DropElapsedTime / DropDuration, 0.0f, 1.0f);

		// Плавно опускаем блоки
		for (auto& BlockPair : InitialBlockLocations)
		{
			const auto Block = BlockPair.Key;
			if (Block)
			{
				FVector NewLocation = FMath::Lerp(BlockPair.Value, BlockPair.Value + TargetDropLocation, Alpha);
				if (NewLocation.X <= KillX)
				{
					Block->Destroy();
				}
				else				
				{
					Block->SetActorLocation(NewLocation);
				}
			}
		}

		// Если опускание завершено
		if (Alpha >= 1.0f)
		{
			bIsDroppingBlocks = false;
			InitialBlockLocations.Empty();
		}
	}
}

TSubclassOf<ABonusParent> APlayingBoard::GetBonusClass()
{
	if (BonusTypeByChance.Num() == 0 || !BonusTypeByChance[0].BonusClass)
	{
		return nullptr;
	}

	// Получаем случайное значение из массива с учётом веса
	int32 TotalWeight = 0;
	for(const auto& CurrentBonus : BonusTypeByChance)
	{
		TotalWeight += CurrentBonus.DropChance * 100;
	}
	
	int32 RandomWeight = FMath::RandHelper(TotalWeight);
	
	for(const auto& CurrentBonus : BonusTypeByChance)
	{
		if (RandomWeight > CurrentBonus.DropChance * 100)
		{
			RandomWeight -= CurrentBonus.DropChance * 100;
		}
		else
		{
			return CurrentBonus.BonusClass;
		}
	}

	return nullptr;
}

void APlayingBoard::BonusDestroyCubes(const int32 Amount)
{
	if (Amount <= 0)
	{
		return;
	}

	const int32 NumToDestroy = FMath::Min(Amount, BlockActors.Num());

	for (int32 i = 0; i < NumToDestroy; i++)
	{
		const int32 RandomIndex = FMath::RandHelper(BlockActors.Num());

		if (BlockActors.IsValidIndex(RandomIndex))
		{
			BlockActors[RandomIndex]->Destroy();
		}
	}
}
