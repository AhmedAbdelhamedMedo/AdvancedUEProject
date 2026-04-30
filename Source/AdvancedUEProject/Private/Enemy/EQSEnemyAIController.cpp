#include "Enemy/EQSEnemyAIController.h"

#include <BehaviorTree/BehaviorTree.h>
#include <BehaviorTree/BlackboardComponent.h>
#include <Perception/AIPerceptionComponent.h>
#include <Perception/AISenseConfig_Sight.h>

AEQSEnemyAIController::AEQSEnemyAIController()
{
	PerceptionComponent = CreateDefaultSubobject<UAIPerceptionComponent>(TEXT("Perception"));
	SightConfig = CreateDefaultSubobject<UAISenseConfig_Sight>(TEXT("SightConfig"));

	SightConfig->SightRadius = 3000.0f;
	SightConfig->LoseSightRadius = 3500.0f;
	SightConfig->PeripheralVisionAngleDegrees = 90.0f;
	SightConfig->DetectionByAffiliation.bDetectEnemies = true;
	SightConfig->DetectionByAffiliation.bDetectNeutrals = true;
	SightConfig->DetectionByAffiliation.bDetectFriendlies = false;

	PerceptionComponent->ConfigureSense(*SightConfig);
	PerceptionComponent->SetDominantSense(SightConfig->GetSenseImplementation());

	PerceptionComponent->OnTargetPerceptionUpdated.AddDynamic(this, &AEQSEnemyAIController::OnTargetPerceptionUpdated);
}

void AEQSEnemyAIController::OnPossess(class APawn* InPawn)
{
	Super::OnPossess(InPawn);
	
	if (BehaviorTreeAsset && BehaviorTreeAsset->BlackboardAsset)
		UseBlackboard(BehaviorTreeAsset->BlackboardAsset, BlackboardComponent);

	RunBehaviorTree(BehaviorTreeAsset);
}

void AEQSEnemyAIController::OnTargetPerceptionUpdated(AActor* InActor, FAIStimulus Stims)
{
	if (InActor && BlackboardComponent && Stims.WasSuccessfullySensed())
	{
		GetWorldTimerManager().ClearTimer(LostSightTimerHandle);
		UpdateSightKey(true);
		UpdateTargetKey(InActor);
	}
	else
	{
		UpdateSightKey(false);
		GetWorldTimerManager().SetTimer(LostSightTimerHandle, this, &AEQSEnemyAIController::LostSight, 8.0f, false);
	}
}

void AEQSEnemyAIController::LostSight()
{
	UpdateTargetKey(nullptr);
}

void AEQSEnemyAIController::UpdateSightKey(bool bHasLineOfSight)
{
	BlackboardComponent->SetValueAsBool(TEXT("HasLineOfSight"), bHasLineOfSight);
}

void AEQSEnemyAIController::UpdateTargetKey(AActor* TargetActor)
{
	BlackboardComponent->SetValueAsObject(TEXT("TargetActor"), TargetActor);
}