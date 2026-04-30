#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "EQSEnemyAIController.generated.h"

class UBehaviorTree;
class UBlackboardComponent;
class UAISenseConfig_Sight;
struct FAIStimulus;

UCLASS()
class ADVANCEDUEPROJECT_API AEQSEnemyAIController : public AAIController
{
	GENERATED_BODY()
public:
	AEQSEnemyAIController();
	
	FGenericTeamId TeamID = FGenericTeamId(1);
	
	virtual FGenericTeamId GetGenericTeamId() const override { return TeamID; }

protected:
	virtual void OnPossess(class APawn* InPawn) override;
	
	UPROPERTY(EditDefaultsOnly, Category = "AI")
	UBehaviorTree* BehaviorTreeAsset;

	UPROPERTY(EditDefaultsOnly, Category = "AI")
	UBlackboardComponent* BlackboardComponent;

	UPROPERTY(EditDefaultsOnly, Category = "AI")
	UAISenseConfig_Sight* SightConfig;

	UFUNCTION()
	void OnTargetPerceptionUpdated(AActor* InActor, FAIStimulus Stims);

	UFUNCTION()
	void LostSight();
	
	FTimerHandle LostSightTimerHandle;
	
	void UpdateSightKey(bool bHasLineOfSight);
	
	void UpdateTargetKey(AActor* TargetActor);
};
