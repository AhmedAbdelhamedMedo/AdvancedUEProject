#pragma once

#include "CoreMinimal.h"
#include "Common/MP_HealthUI.h"
#include "MP_PlayerUI.generated.h"

class UTextBlock;

UCLASS()
class ADVANCEDUEPROJECT_API UMP_PlayerUI : public UMP_HealthUI
{
	GENERATED_BODY()

protected:
	virtual void NativeConstruct() override;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> TB_PlayersNumber;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> TB_Wave;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> TB_EnemyCount;

	UFUNCTION()
	void HandleWaveUpdated(int CurrentWave, int TotalWaves);

	UFUNCTION()
	void HandlePlayersUpdated(int AlivePlayers);
};