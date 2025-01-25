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

	TMap<FString, int32> LevelRecords;
	FString NameSaveFile = "RecordsSaveGame";

	void SetGameSettings() const;
	void SaveRecords() const;
	void LoadRecords();
	void DeleteRecords() const;

public:
	UFUNCTION(BlueprintCallable, Category = "SaveRecord")
	void SetLevelRecord(const FString& LevelName, const int32 NewRecord);
	UFUNCTION(BlueprintPure, Category = "SaveRecord")
	int32 GetLevelRecord(const FString& LevelName) const;
	
};
