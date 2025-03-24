// Mikhail Efremov. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PlayingBoard.generated.h"

class ABonusParent;
class ABlock;

USTRUCT(BlueprintType)
struct FBonusTypeChance
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<ABonusParent> BonusClass = nullptr;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(ClampMin="0.0", ClampMax="1"))
	float DropChance = 0.2f;
};

UCLASS()
class ARKANOID_API APlayingBoard : public AActor
{
	GENERATED_BODY()

private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta=(AllowPrivateAccess=true), Category="Components")
	USceneComponent* SceneRoot = nullptr;

	UPROPERTY()
	TArray<UStaticMeshComponent*> PreviewComponents;
	UPROPERTY()
	TArray<ABlock*> BlockActors;

	void CreatePreviewComponents();
	void ClearPreviewComponents();
	void SpawnBlockActors();
	UFUNCTION()
	void OnBlockDestroyed(AActor* DestroyedBlock);
	
	// Флаг, указывающий, что блоки опускаются
	bool bIsDroppingBlocks = false;
	// Таймер для опускания блоков =)
	FTimerHandle DropBlocksTimerHandle;
	// Время, за которое блоки опускаются
	float DropDuration = 1.0f;
	// Блоки опускаються с промежутком
	float DropTime = 1.0f;
	float DropElapsedTime = 0.0f;
	// Целевая позиция для опускания
	FVector TargetDropLocation;
	// Высота каретки
	float KillX = 0.0f;
	// Высота одного блока
	float OneBlockHeight;
	// Начальная позиция блоков перед опусканием
	TMap<ABlock*, FVector> InitialBlockLocations;
	UFUNCTION()
	void StartDroppingBlocks();
	
public:	
	APlayingBoard();

protected:
	virtual void OnConstruction(const FTransform& Transform) override;
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
	
	TSubclassOf<ABonusParent> GetBonusClass();

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings | Base", meta = (ToolTip = "Блюпринт кубика"))
	TSubclassOf<ABlock> BlockClassForSpawn;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings | Base", meta = (ToolTip = "Меш для превью"))
	UStaticMesh* PreviewMesh = nullptr;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings | Base")
	FVector BlockScale = FVector(0.5f, 0.5f, 0.5f);
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings | Base")
	int32 GridSizeX = 5;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings | Base")
	int32 GridSizeY = 5;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings | Base")
	int32 SpacingX = 60;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings | Base")
	int32 SpacingY = 60;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings | Game")
	int32 BlocksToSlide = 5;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings | Game", meta=(ClampMin="0.0", ClampMax="1"))
	float GameDifficulty = 0.2f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings | Game", meta=(ClampMin="0.0", ClampMax="1"))
	float BonusChance = 0.2f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings | Game")
	TArray<FBonusTypeChance> BonusTypeByChance;

	void BonusDestroyCubes(const int32 Amount);
};
