// Mikhail Efremov. All rights reserved.


#include "Components/LifeComponent.h"


ULifeComponent::ULifeComponent()
{
	PrimaryComponentTick.bCanEverTick = false;

}

void ULifeComponent::SetLife(const int32 NewLife)
{
	Life = FMath::Abs(NewLife);
}

void ULifeComponent::TakeDamage(const int32 Damage)
{
	Life = FMath::Max(Life - Damage, 0);
}

bool ULifeComponent::IsAlive() const
{
	return Life > 0;
}

