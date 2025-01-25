// Mikhail Efremov. All rights reserved.

#include "Framework/ArkanoidGameMode.h"
#include "EngineUtils.h"
#include "Framework/ArkanoidGameState.h"
#include "Framework/ArkanoidPC.h"
#include "Framework/ArkanoidPlayerState.h"
#include "World/PlayingBoard.h"

AArkanoidGameMode::AArkanoidGameMode()
{
	GameStateClass = AArkanoidGameState::StaticClass();
	PlayerStateClass = AArkanoidPlayerState::StaticClass();
}

void AArkanoidGameMode::BeginPlay()
{
	Super::BeginPlay();

	CalculatePlayingBoard();
	GameStarted();
}

void AArkanoidGameMode::CalculatePlayingBoard()
{
	for (TActorIterator<APlayingBoard> It(GetWorld()); It; ++It)
		if (const auto CurrentBoard = *It)
			AmountPlayingBoards++; 
}

void AArkanoidGameMode::GameStarted()
{
	//auto Temp = GetWorld()->GetGameState();

	auto ArkanoidGameState = Cast<AArkanoidGameState>(GameState);
	if (ArkanoidGameState)
		ArkanoidGameState->StartGame();
}

void AArkanoidGameMode::GameEnded(const bool bIsWinner)
{
	AmountPlayingBoards = bIsWinner ? --AmountPlayingBoards : 0;
	
	if (!AmountPlayingBoards)
	{
		if (const auto ArkanoidGameState = Cast<AArkanoidGameState>(GameState))
			ArkanoidGameState->StopGame();
		
		for (APlayerState* PlayerState : GameState->PlayerArray) // Уведомляем всех игроков о завершении игры
		{
			if (PlayerState)
			{
				const auto Player = Cast<AArkanoidPC>(PlayerState->GetPlayerController());
				if (Player)
					Player->ShowGameEndMenu(bIsWinner); // Показываем меню поражения
			}
		}
	}
}
