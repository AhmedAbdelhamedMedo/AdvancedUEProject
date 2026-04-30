#include "Player/MP_PlayerUI.h"

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
			Spawner->OnEnemyCountUpdated.AddDynamic(this, &UMP_PlayerUI::HandleEnemyCountUpdated);
		}
		break;
	}
}

void UMP_PlayerUI::HandleWaveUpdated(int CurrentWave)
{
	TB_Wave->SetText(FText::AsNumber(CurrentWave));
}

void UMP_PlayerUI::HandleEnemyCountUpdated(int Alive, int Total)
{
	TB_EnemyCount->SetText(FText::FromString(FString::Printf(TEXT("%i / %i"), Alive, Total)));
}