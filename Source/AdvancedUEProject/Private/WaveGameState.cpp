#include "WaveGameState.h"

#include "Enemy/EnemyWaveSpawner.h"
#include "Net/UnrealNetwork.h"

AWaveGameState::AWaveGameState()
{
	SetNetUpdateFrequency(10.0f);
}

int AWaveGameState::GetTotalWaves() const
{
	return TotalWaves;
}

int AWaveGameState::GetAliveEnemies() const
{
	return AliveEnemies;
}

int AWaveGameState::GetTotalEnemies() const
{
	return TotalEnemies;
}

int AWaveGameState::GetAlivePlayers() const
{
	return AlivePlayers;
}

int AWaveGameState::GetRestartVotes() const
{
	return RestartVotes;
}

EMatchPhase AWaveGameState::GetMatchPhase() const
{
	return MatchPhase;
}

bool AWaveGameState::IsMatchInProgress() const
{
	return MatchPhase == EMatchPhase::InProgress;
}

int AWaveGameState::GetCurrentWave() const
{
	return CurrentWave;
}

void AWaveGameState::BeginPlay()
{
	Super::BeginPlay();

	if (HasAuthority())
	{
		AlivePlayers = PlayerArray.Num();
	}
}

void AWaveGameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AWaveGameState, MatchPhase);
	DOREPLIFETIME(AWaveGameState, CurrentWave);
	DOREPLIFETIME(AWaveGameState, TotalWaves);
	DOREPLIFETIME(AWaveGameState, AliveEnemies);
	DOREPLIFETIME(AWaveGameState, TotalEnemies);
	DOREPLIFETIME(AWaveGameState, AlivePlayers);
	DOREPLIFETIME(AWaveGameState, RestartVotes);
}

void AWaveGameState::RegisterSpawner(AEnemyWaveSpawner* InSpawner)
{
	if (!HasAuthority() || !InSpawner)
	{
		return;
	}

	Spawner = InSpawner;

	TotalWaves = Spawner->GetTotalWavesNumber();
	CurrentWave = Spawner->GetCurrentWaveNumber();

	Spawner->OnWaveUpdated.AddDynamic(this, &AWaveGameState::OnSpawnerWaveUpdated);
	Spawner->OnEnemyCountUpdated.AddDynamic(this, &AWaveGameState::OnSpawnerEnemyCountUpdated);

	SetMatchPhase(EMatchPhase::InProgress);
}

void AWaveGameState::OnSpawnerWaveUpdated(int NewWave)
{
	if (!HasAuthority())
		return;

	CurrentWave = NewWave;
	TotalWaves = Spawner ? Spawner->GetTotalWavesNumber() : TotalWaves;

	OnWaveProgressChanged.Broadcast(CurrentWave, TotalWaves);

	if ((CurrentWave >= TotalWaves - 1) && (AliveEnemies == 0))
		SetMatchPhase(EMatchPhase::Victory);
}

void AWaveGameState::OnSpawnerEnemyCountUpdated(int Alive, int Total)
{
	if (!HasAuthority())
		return;

	AliveEnemies = Alive;
	TotalEnemies = Total;

	OnEnemyCountChanged.Broadcast(AliveEnemies, TotalEnemies);

	if (Spawner
		&& (CurrentWave >= Spawner->GetTotalWavesNumber() - 1)
		&& (AliveEnemies <= 0)
		&& MatchPhase == EMatchPhase::InProgress)
		SetMatchPhase(EMatchPhase::Victory);
}

void AWaveGameState::NotifyPlayerDied()
{
	if (!HasAuthority())
		return;

	AlivePlayers = FMath::Max(0, AlivePlayers - 1);

	OnAlivePlayersChanged.Broadcast(AlivePlayers);

	CheckDefeatCondition();
}

void AWaveGameState::NotifyPlayerSpawned()
{
	if (!HasAuthority()) return;

	AlivePlayers++;
	OnAlivePlayersChanged.Broadcast(AlivePlayers);
}

void AWaveGameState::SetMatchPhase(EMatchPhase NewPhase)
{
	if (!HasAuthority() || MatchPhase == NewPhase)
		return;

	MatchPhase = NewPhase;

	OnMatchPhaseChanged.Broadcast(MatchPhase);
}

void AWaveGameState::CheckDefeatCondition()
{
	if (MatchPhase == EMatchPhase::InProgress && AlivePlayers <= 0)
		SetMatchPhase(EMatchPhase::Defeat);
}

void AWaveGameState::OnRep_MatchPhase()
{
	OnMatchPhaseChanged.Broadcast(MatchPhase);
}

void AWaveGameState::OnRep_WaveProgress()
{
	OnWaveProgressChanged.Broadcast(CurrentWave, TotalWaves);
}

void AWaveGameState::OnRep_EnemyCount()
{
	OnEnemyCountChanged.Broadcast(AliveEnemies, TotalEnemies);
}

void AWaveGameState::OnRep_AlivePlayers()
{
	OnAlivePlayersChanged.Broadcast(AlivePlayers);
}

void AWaveGameState::OnRep_RestartVotes()
{
	OnRestartVoteChanged.Broadcast(RestartVotes, PlayerArray.Num());
}

void AWaveGameState::RegisterRestartVote(APlayerController* Voter)
{
	if (!HasAuthority() || !Voter)
		return;
 
	if (MatchPhase != EMatchPhase::Victory && MatchPhase != EMatchPhase::Defeat)
		return;
 
	if (RestartVoters.Contains(Voter))
		return;
 
	RestartVoters.Add(Voter);
	RestartVotes = RestartVoters.Num();
	
	const int VotesNeeded = PlayerArray.Num();
	OnRestartVoteChanged.Broadcast(RestartVotes, VotesNeeded);
 
	if (RestartVotes >= VotesNeeded)
		TriggerRestart();
}
 
void AWaveGameState::TriggerRestart()
{
	if (!HasAuthority())
		return;
	
	const FString CurrentMap = GetWorld()->GetMapName();
 
	FString CleanMap = CurrentMap;
	CleanMap.RemoveFromStart(GetWorld()->StreamingLevelsPrefix);
 
	FTimerHandle RestartTimerHandle;
	GetWorldTimerManager().SetTimer(
		RestartTimerHandle,
		[this, CleanMap]()
		{GetWorld()->ServerTravel(CleanMap + TEXT("?listen"), true);},
		1.5f, 
		false);
}