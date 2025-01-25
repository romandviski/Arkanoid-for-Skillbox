// Mikhail Efremov. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MyKillZone.generated.h"

class UBoxComponent;
class UArrowComponent;

UCLASS()
class ARKANOID_API AMyKillZone : public AActor
{
	GENERATED_BODY()

private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Components,
		meta = (AllowPrivateAccess = "true"))
	USceneComponent* SceneRoot = nullptr;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Components,
		meta = (AllowPrivateAccess = "true"))
	UArrowComponent* ForwardArrow = nullptr;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Components,
		meta = (AllowPrivateAccess = "true"))
	UBoxComponent* KillBox = nullptr;
	
public:	
	// Sets default values for this actor's properties
	AMyKillZone();

protected:
	virtual void NotifyActorBeginOverlap(AActor* OtherActor) override;

};
