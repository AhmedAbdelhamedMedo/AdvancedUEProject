#include "PlayerGamerMode.h"

#include "WaveGameState.h"
#include "Enemy/EnemyWaveSpawner.h"
#include "Kismet/GameplayStatics.h"

void APlayerGamerMode::BeginPlay()
{
	Super::BeginPlay();

	AEnemyWaveSpawner* Spawner = Cast<AEnemyWaveSpawner>(
		UGameplayStatics::GetActorOfClass(GetWorld(), AEnemyWaveSpawner::StaticClass()));

	if (AWaveGameState* GS = GetGameState<AWaveGameState>())
		GS->RegisterSpawner(Spawner);
}