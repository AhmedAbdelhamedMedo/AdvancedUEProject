#include "Enemy/EQS/BTTask_EnemyAttack.h"

#include "Enemy/EQSEnemyAICharacter.h"
#include "AIController.h"

UBTTask_EnemyAttack::UBTTask_EnemyAttack()
{
	NodeName = TEXT("AttackPlayer");
}

EBTNodeResult::Type UBTTask_EnemyAttack::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	AAIController* AI = OwnerComp.GetAIOwner();
	
	if (!AI)
		return EBTNodeResult::Failed;

	AEQSEnemyAICharacter* Enemy = Cast<AEQSEnemyAICharacter>(AI->GetPawn());
	
	if (!Enemy)
		return EBTNodeResult::Failed;

	Enemy->Fire();

	return EBTNodeResult::Succeeded;
}