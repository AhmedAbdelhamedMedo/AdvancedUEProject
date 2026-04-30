#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "EnemyWaveSpawner.generated.h"

class AEQSEnemyAICharacter;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnWaveUpdated, int, CurrentWave);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnEnemyCountUpdated, int, Alive, int, Total);

UCLASS()
class ADVANCEDUEPROJECT_API AEnemyWaveSpawner : public AActor
{
	GENERATED_BODY()

public:
	AEnemyWaveSpawner();

	void NotifyEnemyKilled();
	
	int GetTotalWavesNumber() const;
	int GetCurrentWaveNumber() const;

	UPROPERTY(BlueprintAssignable, Category="Wave")
	FOnWaveUpdated OnWaveUpdated;

	UPROPERTY(BlueprintAssignable, Category="Wave")
	FOnEnemyCountUpdated OnEnemyCountUpdated;
	
protected:
	virtual void BeginPlay() override;
	
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UPROPERTY(EditAnywhere, Category = "Wave")
	TSubclassOf<AEQSEnemyAICharacter> EnemyClass;

	UPROPERTY(EditAnywhere, Category = "Wave")
	int EnemiesPerWave = 5;

	UPROPERTY(EditAnywhere, Category = "Wave")
	float SpawnInterval = 1.0f;

	UPROPERTY(EditAnywhere, Category = "Wave")
	float TimeBetweenWaves = 5.0f;

	UPROPERTY(EditAnywhere, Category = "Wave")
	float SpawnRadius = 1000.0f;

	UPROPERTY(ReplicatedUsing = OnRep_CurrentWaveNumber, BlueprintReadOnly)
	int CurrentWaveNumber;

	UPROPERTY(Replicated, BlueprintReadOnly, Category="Wave")
	int TotalWavesNumber;

	UPROPERTY()
	int AliveEnemiesNumber;

	UPROPERTY(EditAnywhere, Category="Wave")
	int TotalWaves;
	
	UFUNCTION()
	void OnRep_CurrentWaveNumber();

private:
	FTimerHandle SpawnTimerHandle;
	
	FTimerHandle NextWaveTimerHandle;
	
	void StartWave();

	void SpawnEnemy();

	void CheckWaveCompleted();

	FVector GetSpawnLocation() const;
	
	int SpawnedEnemiesThisWave;
};