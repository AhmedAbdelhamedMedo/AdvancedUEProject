#include "Player/UI/MP_PlayerUI.h"

#include "WaveGameState.h"
#include "Components/TextBlock.h"

void UMP_PlayerUI::NativeConstruct()
{
	Super::NativeConstruct();

	AWaveGameState* GameState = GetWorld()->GetGameState<AWaveGameState>();
	if (!GameState)
		return;

	GameState->OnWaveProgressChanged.AddDynamic(this, &UMP_PlayerUI::HandleWaveUpdated);
	GameState->OnAlivePlayersChanged.AddDynamic(this, &UMP_PlayerUI::HandlePlayersUpdated);

	HandleWaveUpdated(GameState->GetCurrentWave(), GameState->GetTotalWaves());
	HandlePlayersUpdated(GameState->GetAlivePlayers());
}

void UMP_PlayerUI::HandlePlayersUpdated(int AlivePlayers)
{
	TB_PlayersNumber->SetText(FText::AsNumber(AlivePlayers));
}

void UMP_PlayerUI::HandleWaveUpdated(int CurrentWave, int TotalWaves)
{
	TB_CurrentWave->SetText(FText::AsNumber(CurrentWave));
	TB_TotalWaveCount->SetText(FText::AsNumber(TotalWaves));
}