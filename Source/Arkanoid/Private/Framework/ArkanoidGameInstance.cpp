// Mikhail Efremov. All rights reserved.

#include "Framework/ArkanoidGameInstance.h"
#include "GameFramework/GameUserSettings.h"
#include "Kismet/GameplayStatics.h"
#include "SaveFiles/RecordsSaveGame.h"

void UArkanoidGameInstance::Init()
{
	Super::Init();

	SetGameSettings();
	LoadRecords();
}

void UArkanoidGameInstance::SetGameSettings() const
{
	if (!GEngine)
		return;

	GEngine->GameUserSettings->SetVSyncEnabled(true);
	GEngine->GameUserSettings->ApplySettings(true);
	GEngine->GameUserSettings->SaveSettings();
	GEngine->Exec(GetWorld(), TEXT("t.MaxFPS 60"));
}

void UArkanoidGameInstance::SaveRecords() const
{
	auto SaveGameFile = Cast<URecordsSaveGame>(UGameplayStatics::LoadGameFromSlot(NameSaveFile, 0));

	if (!SaveGameFile)
		SaveGameFile = Cast<URecordsSaveGame>(UGameplayStatics::CreateSaveGameObject(URecordsSaveGame::StaticClass()));

	if (SaveGameFile)
	{
		SaveGameFile->SavedRecords = LevelRecords;
		UGameplayStatics::SaveGameToSlot(SaveGameFile, NameSaveFile, 0);
	}
}

void UArkanoidGameInstance::LoadRecords()
{
	const auto LoadedSaveGame = UGameplayStatics::LoadGameFromSlot(NameSaveFile, 0);

	if (const auto SaveGameFile = Cast<URecordsSaveGame>(LoadedSaveGame))
		LevelRecords = SaveGameFile->SavedRecords;
}

void UArkanoidGameInstance::DeleteRecords() const
{
	if (UGameplayStatics::DoesSaveGameExist(NameSaveFile, 0))
	{
		UGameplayStatics::DeleteGameInSlot(NameSaveFile, 0);
		if (GEngine)
			GEngine->AddOnScreenDebugMessage(-1, 30.f, FColor::Yellow, ("Deleted Records %s", *NameSaveFile));
	}
}

void UArkanoidGameInstance::SetLevelRecord(const FString& LevelName, const int32 NewRecord)
{
	if (NewRecord > GetLevelRecord(LevelName))
	{
		LevelRecords.Add(LevelName, NewRecord);
		SaveRecords();
	}
}

int32 UArkanoidGameInstance::GetLevelRecord(const FString& LevelName) const
{
	return LevelRecords.FindRef(LevelName);
}
