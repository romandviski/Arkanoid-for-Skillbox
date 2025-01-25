// Mikhail Efremov. All rights reserved.

#include "Framework/ArkanoidGameInstance.h"

#include "AssetRegistry/AssetRegistryModule.h"
#include "GameFramework/GameUserSettings.h"
#include "Kismet/GameplayStatics.h"
#include "SaveFiles/RecordsSaveGame.h"

void UArkanoidGameInstance::Init()
{
	Super::Init();

	SetGameSettings();
	LoadRecords();
	CollectGameLevels();
}

void UArkanoidGameInstance::CollectGameLevels()
{
	// Получаем модуль реестра ассетов
	const FAssetRegistryModule& AssetRegistryModule =
		FModuleManager::LoadModuleChecked<FAssetRegistryModule>("AssetRegistry");
	
	FARFilter Filter; 	// Формируем фильтр для поиска ассетов в указанной папке
	Filter.PackagePaths.Add(*LevelsPath);
	Filter.bRecursivePaths = false; // Если нужно искать ассеты во всех подпапках
	
	TArray<FAssetData> AssetDataList; // Получаем список ассетов, соответствующих фильтру
	AssetRegistryModule.Get().GetAssets(Filter, AssetDataList);
	
	for (const FAssetData& AssetData : AssetDataList) 	// Собираем имена ассетов в массив
	{
		FString CleanLevelName = AssetData.AssetName.ToString();
		CleanLevelName.RemoveFromStart("UEDPIE_0_"); // Очищаем от префиксов
		LevelNames.Add(AssetData.AssetName.ToString());
	}

	LevelNames.Sort(); // Сортируем итоговый массив по алфавиту
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

void UArkanoidGameInstance::OpenNextLevel(const FString& CurrentLevelName)
{
	const auto CurrentLevelIndex = LevelNames.Find(CurrentLevelName);
	if (CurrentLevelIndex != INDEX_NONE && LevelNames.IsValidIndex(CurrentLevelIndex + 1))
	{
		const FString NextLevelName = LevelNames[CurrentLevelIndex + 1];
		if (FPackageName::DoesPackageExist(FString::Printf(TEXT("%s/%s"), *LevelsPath, *NextLevelName)))
		{
			UGameplayStatics::OpenLevel(this, FName(*NextLevelName));
			return; // Выходим из функции, чтобы не выполнять код ниже
		}
	}
	
	UGameplayStatics::OpenLevel(this, FName("Menu"));
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
