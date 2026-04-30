#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTask_RandomLocation.generated.h"

UCLASS()
class ADVANCEDUEPROJECT_API UBTTask_RandomLocation : public UBTTaskNode
{
	GENERATED_BODY()
	
public:
	UBTTask_RandomLocation();

	UPROPERTY(EditAnywhere, Category = "Blackboard")
	FBlackboardKeySelector MoveToLocationKey;

	UPROPERTY(EditAnywhere, Category = "Blackboard")
	float SearchRadius = 1000.0f;

	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
};