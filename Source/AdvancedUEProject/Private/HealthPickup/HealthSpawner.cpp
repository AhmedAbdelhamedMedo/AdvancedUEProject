#include "HealthPickup/HealthSpawner.h"

#include <NavigationSystem.h>
#include <GameFramework/Actor.h>
#include "HealthPickup/HealthPickup.h"
#include "NavMesh/NavMeshBoundsVolume.h"

AHealthSpawner::AHealthSpawner()
{
	PrimaryActorTick.bCanEverTick = false;
}

void AHealthSpawner::BeginPlay()
{
	Super::BeginPlay();
	
	UNavigationSystemV1* NavSys = FNavigationSystem::GetCurrent<UNavigationSystemV1>(GetWorld());

	if (!NavSys || !NavMesh)
		return;

	FVector Origin;
	FVector Extent;

	NavMesh->GetActorBounds(false, Origin, Extent);
	
	const float Radius = FMath::Max(Extent.X, Extent.Y);
	
	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = this;
	SpawnParams.Instigator = GetInstigator();
	
	FNavLocation NavResult;
	for (int i=0; i<HealthPackCount; i++)
	{
		if (NavSys->GetRandomReachablePointInRadius(GetActorLocation(), Radius, NavResult))
		{
			GetWorld()->SpawnActor<AHealthPickup>(
				HealthPickupClass,
				NavResult.Location,
				FRotator::ZeroRotator,
				SpawnParams
			);
		}
	}
}