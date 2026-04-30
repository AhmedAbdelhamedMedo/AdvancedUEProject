#include "Enemy/EQS/EQC_PlayerContext.h"

#include "EnvironmentQuery/Items/EnvQueryItemType_Actor.h"
#include "EnvironmentQuery/EnvQueryTypes.h"
#include "Player/MP_PlayerCharacter.h"

void UEQC_PlayerContext::ProvideContext(FEnvQueryInstance& QueryInstance, FEnvQueryContextData& ContextData) const
{
	UObject* QueryOwner = QueryInstance.Owner.Get();
	if (!QueryOwner) return;

	UWorld* World = QueryOwner->GetWorld();
	if (!World) return;

	TArray<AActor*> PlayerActors;

	for (FConstPlayerControllerIterator It = World->GetPlayerControllerIterator(); It; ++It)
	{
		APlayerController* PC = It->Get();
		if (PC)
		{
			APawn* Pawn = PC->GetPawn();
            
			AMP_PlayerCharacter* MyPlayerCharacter = Cast<AMP_PlayerCharacter>(Pawn);
            
			if (MyPlayerCharacter)
				PlayerActors.Add(Pawn);
		}
	}

	UEnvQueryItemType_Actor::SetContextHelper(ContextData, PlayerActors);
}