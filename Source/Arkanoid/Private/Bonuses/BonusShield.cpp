// Mikhail Efremov. All rights reserved.

#include "Bonuses/BonusShield.h"
#include "Framework/Paddle.h"
#include "Kismet/GameplayStatics.h"
#include "World/Shield.h"


ABonusShield::ABonusShield()
{
	Duration = 0.0f;
	Value = 2;
}

void ABonusShield::BonusAction(APaddle* Paddle)
{
	if (ShieldClass)
	{
		const auto PreviousShield = UGameplayStatics::GetActorOfClass(GetWorld(), ShieldClass);
		if (PreviousShield)
		{
			PreviousShield->Destroy();
		}
		
		auto SpawnLocation = FVector(0.0f);
		SpawnLocation.X = Paddle->GetActorLocation().X - 70.0f;
		SpawnLocation.Z = Paddle->GetActorLocation().Z;
		
		const auto SpawnedShield = GetWorld()->SpawnActor<AShield>(ShieldClass, SpawnLocation, FRotator(0, 0, 0));
		if (SpawnedShield)
		{
			SpawnedShield->Init(Value, Duration);
		}
	}
	
	Super::BonusAction(Paddle);
}


