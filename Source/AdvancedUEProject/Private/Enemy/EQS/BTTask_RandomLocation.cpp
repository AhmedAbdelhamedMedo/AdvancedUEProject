#include "Enemy/EQS/BTTask_RandomLocation.h"

#include <AIController.h>
#include <NavigationSystem.h>
#include <BehaviorTree/BlackboardComponent.h>

UBTTask_RandomLocation::UBTTask_RandomLocation()
{
	NodeName = TEXT("GetRandomLocation");
}

EBTNodeResult::Type UBTTask_RandomLocation::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	AAIController* AiCtrl = OwnerComp.GetAIOwner();
	if (!AiCtrl || !AiCtrl->GetPawn())
		return EBTNodeResult::Failed;

	UBlackboardComponent* BlackboardComponent = OwnerComp.GetBlackboardComponent();
	if (!BlackboardComponent)
		return EBTNodeResult::Failed;

	UNavigationSystemV1* NavSys = UNavigationSystemV1::GetCurrent(AiCtrl->GetWorld());
	if (!NavSys)
		return EBTNodeResult::Failed;

	FVector OriginPoint = AiCtrl->GetPawn()->GetActorLocation();

	FNavLocation NavResult;
	if (NavSys->GetRandomReachablePointInRadius(OriginPoint, SearchRadius, NavResult))
	{
		BlackboardComponent->SetValueAsVector(MoveToLocationKey.SelectedKeyName, NavResult.Location);
		return EBTNodeResult::Succeeded;
	}
	else
	{
		BlackboardComponent->SetValueAsVector(MoveToLocationKey.SelectedKeyName, OriginPoint);
		return EBTNodeResult::Succeeded;
	}
}