#pragma once

#include "CoreMinimal.h"
#include "Common/BaseCharacter.h"
#include "EQSEnemyAICharacter.generated.h"

class UWidgetComponent;
class AEnemyLaser;
class AEnemyWaveSpawner;

UCLASS()
class ADVANCEDUEPROJECT_API AEQSEnemyAICharacter : public ABaseCharacter
{
	GENERATED_BODY()

public:
	AEQSEnemyAICharacter();
	
	UFUNCTION()
	void Fire();

	UFUNCTION()
	void Death();

	UFUNCTION()
	void SetWaveSpawner(AEnemyWaveSpawner* InSpawner);
	
private:
	float LastFireTime = -100.f;
	bool bIsDead = false;
	
protected:
	virtual void BeginPlay() override;
	
	virtual void NetMulticast_OnDeath_Implementation() override;
	
	void DestroyEnemy();
	
	UPROPERTY()
	TObjectPtr<AEnemyWaveSpawner> WaveSpawner;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="UI")
	TObjectPtr<UWidgetComponent> HealthWidgetUI;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Enemy|Attack")
	TSubclassOf<class AEnemyLaser> LaserClass;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Enemy|Attack")
	float FireRate = 0.2f;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Enemy|Patrol")
	float PatrolSpeed = 300.0f;
};