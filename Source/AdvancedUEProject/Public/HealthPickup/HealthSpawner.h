#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "HealthSpawner.generated.h"

class AHealthPickup;
class UNavigationSystemV1;
class ANavMeshBoundsVolume;

UCLASS()
class ADVANCEDUEPROJECT_API AHealthSpawner : public AActor
{
	GENERATED_BODY()
	
public:	
	AHealthSpawner();

protected:
	virtual void BeginPlay() override;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "HealthPickup")
	int HealthPackCount;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "HealthPickup")
	TObjectPtr<ANavMeshBoundsVolume> NavMesh;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "HealthPickup")
	TSubclassOf<AHealthPickup> HealthPickupClass;
};