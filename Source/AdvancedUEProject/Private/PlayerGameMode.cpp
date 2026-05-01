#include "PlayerGameMode.h"

#include "WaveGameState.h"
#include "Enemy/EnemyWaveSpawner.h"
#include "Kismet/GameplayStatics.h"

void APlayerGameMode::BeginPlay()
{
	Super::BeginPlay();
	bUseSeamlessTravel = true;
	AEnemyWaveSpawner* Spawner = Cast<AEnemyWaveSpawner>(
		UGameplayStatics::GetActorOfClass(GetWorld(), AEnemyWaveSpawner::StaticClass()));

	if (AWaveGameState* GS = GetGameState<AWaveGameState>())
	{
		GS->RegisterSpawner(Spawner);
		GS->SetGameplayMapPath(TEXT("/Game/FirstPerson/Lvl_FirstPerson"));
	}

}

void APlayerGameMode::HandleStartingNewPlayer_Implementation(APlayerController* NewPlayer)
{
	Super::HandleStartingNewPlayer_Implementation(NewPlayer);
	if (AWaveGameState* GS = GetGameState<AWaveGameState>())
		GS->NotifyPlayerSpawned();
}
