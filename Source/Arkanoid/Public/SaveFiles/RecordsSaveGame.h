// Mikhail Efremov. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "RecordsSaveGame.generated.h"

UCLASS()
class ARKANOID_API URecordsSaveGame : public USaveGame
{
	GENERATED_BODY()

public:
	UPROPERTY()
	TMap<FString, int32> SavedRecords;
	
};
