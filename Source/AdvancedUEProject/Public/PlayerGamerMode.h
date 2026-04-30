#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "PlayerGamerMode.generated.h"

UCLASS()
class ADVANCEDUEPROJECT_API APlayerGamerMode : public AGameModeBase
{
	GENERATED_BODY()
	
protected:
	virtual void BeginPlay() override;
};
