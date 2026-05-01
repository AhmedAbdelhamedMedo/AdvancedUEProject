#include "Enemy/EnemyWaveSpawner.h"

#include "NavigationSystem.h"
#include "Net/UnrealNetwork.h"
#include "AI/NavigationSystemBase.h"
#include "Enemy/EQSEnemyAICharacter.h"

AEnemyWaveSpawner::AEnemyWaveSpawner()
{
	PrimaryActorTick.bCanEverTick = false;
	bReplicates = true;
	
	CurrentWaveNumber = 0;
}

void AEnemyWaveSpawner::BeginPlay()
{
	Super::BeginPlay();
	
	TotalWavesNumber = TotalWaves;
	
	if (HasAuthority())
		StartWave();
}

void AEnemyWaveSpawner::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(AEnemyWaveSpawner, TotalWavesNumber);
	DOREPLIFETIME(AEnemyWaveSpawner, CurrentWaveNumber);
}

int AEnemyWaveSpawner::GetTotalWavesNumber() const
{
	return TotalWavesNumber;
}

int AEnemyWaveSpawner::GetCurrentWaveNumber() const
{
	return CurrentWaveNumber;
}

void AEnemyWaveSpawner::OnRep_CurrentWaveNumber()
{
	OnWaveUpdated.Broadcast(CurrentWaveNumber);
}

void AEnemyWaveSpawner::StartWave()
{
	if (CurrentWaveNumber >= TotalWavesNumber)
		return;

	CurrentWaveNumber++;
	OnRep_CurrentWaveNumber();
	
	SpawnedEnemiesThisWave = 0;
	AliveEnemiesNumber = 0;
	
	GetWorldTimerManager().SetTimer(
		SpawnTimerHandle,
		this,
		&AEnemyWaveSpawner::SpawnEnemy,
		SpawnInterval,
		true
	);
}

void AEnemyWaveSpawner::SpawnEnemy()
{
	if (!EnemyClass)
		return;

	if (SpawnedEnemiesThisWave >= EnemiesPerWave)
	{
		GetWorldTimerManager().ClearTimer(SpawnTimerHandle);
		return;
	}

	const FVector SpawnLocation = GetSpawnLocation();

	FActorSpawnParameters Params;

	AEQSEnemyAICharacter* Enemy =
		GetWorld()->SpawnActor<AEQSEnemyAICharacter>(
			EnemyClass,
			SpawnLocation,
			FRotator::ZeroRotator,
			Params
		);

	if (Enemy)
	{
		AliveEnemiesNumber++;
		SpawnedEnemiesThisWave++;
		Enemy->SetWaveSpawner(this);
		OnEnemyCountUpdated.Broadcast(AliveEnemiesNumber, EnemiesPerWave);

	}

	SpawnedEnemiesThisWave++;
}

void AEnemyWaveSpawner::NotifyEnemyKilled()
{
	AliveEnemiesNumber = FMath::Max(0, AliveEnemiesNumber - 1);
 
	OnEnemyCountUpdated.Broadcast(AliveEnemiesNumber, EnemiesPerWave);
 
	CheckWaveCompleted();
}

void AEnemyWaveSpawner::CheckWaveCompleted()
{
	const bool bAllSpawned = (SpawnedEnemiesThisWave >= EnemiesPerWave);
	const bool bAllDead    = (AliveEnemiesNumber <= 0);
 
	if (!bAllSpawned || !bAllDead)
		return;
	
	if (CurrentWaveNumber >= TotalWavesNumber)
		return;
 
	GetWorldTimerManager().SetTimer(
		NextWaveTimerHandle,
		this,
		&AEnemyWaveSpawner::StartWave,
		TimeBetweenWaves,
		false
	);
}

FVector AEnemyWaveSpawner::GetSpawnLocation() const
{
	UNavigationSystemV1* NavSys =
		FNavigationSystem::GetCurrent<UNavigationSystemV1>(GetWorld());

	if (!NavSys)
		return GetActorLocation();

	FNavLocation NavLocation;

	if (NavSys->GetRandomReachablePointInRadius(
		GetActorLocation(),
		SpawnRadius,
		NavLocation))
		return NavLocation.Location;

	return GetActorLocation();
}