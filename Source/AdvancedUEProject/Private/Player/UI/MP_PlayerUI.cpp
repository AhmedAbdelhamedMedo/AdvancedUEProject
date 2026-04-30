#include "Player/UI/MP_PlayerUI.h"

#include "EngineUtils.h"
#include "Components/TextBlock.h"
#include "Enemy/EnemyWaveSpawner.h"

void UMP_PlayerUI::NativeConstruct()
{
	Super::NativeConstruct();
	
	for (TActorIterator<AEnemyWaveSpawner> It(GetWorld()); It; ++It)
	{
		if (AEnemyWaveSpawner* Spawner = *It)
		{
			Spawner->OnWaveUpdated.AddDynamic(this, &UMP_PlayerUI::HandleWaveUpdated);
			TB_EnemyCount->SetText(FText::AsNumber(Spawner->GetTotalWavesNumber()));
			TB_Wave->SetText(FText::AsNumber(Spawner->GetCurrentWaveNumber()));
		}
		break;
	}
}

void UMP_PlayerUI::HandleWaveUpdated(int CurrentWave)
{
	TB_Wave->SetText(FText::AsNumber(CurrentWave));
}