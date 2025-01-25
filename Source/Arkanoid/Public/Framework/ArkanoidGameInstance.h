// Mikhail Efremov. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "ArkanoidGameInstance.generated.h"

UCLASS()
class ARKANOID_API UArkanoidGameInstance : public UGameInstance
{
	GENERATED_BODY()

protected:
	virtual void Init() override;

	TArray<FString> LevelNames;
	void CollectGameLevels();
	
	TMap<FString, int32> LevelRecords;
	FString NameSaveFile = "RecordsSaveGame";

	void SetGameSettings() const;
	void SaveRecords() const;
	void LoadRecords();
	void DeleteRecords() const;

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings")
	FString LevelsPath = TEXT("/Game/Arkanoid/Maps/Game");
	UFUNCTION(BlueprintCallable, Category = "ArkanoidGameInstance")
	void OpenNextLevel(const FString &CurrentLevelName);
	UFUNCTION(BlueprintPure, Category = "ArkanoidGameInstance")
	FORCEINLINE TArray<FString> GetLevelNames() const {return LevelNames;}
	
	UFUNCTION(BlueprintCallable, Category = "SaveRecord")
	void SetLevelRecord(const FString& LevelName, const int32 NewRecord);
	UFUNCTION(BlueprintPure, Category = "SaveRecord")
	int32 GetLevelRecord(const FString& LevelName) const;
	
};
