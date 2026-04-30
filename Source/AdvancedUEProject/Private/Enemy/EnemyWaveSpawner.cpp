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
		Enemy->SetWaveSpawner(this);
	}

	SpawnedEnemiesThisWave++;
}

void AEnemyWaveSpawner::NotifyEnemyKilled()
{
	AliveEnemiesNumber--;
	CheckWaveCompleted();
}

void AEnemyWaveSpawner::CheckWaveCompleted()
{
	if (AliveEnemiesNumber > 0)
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