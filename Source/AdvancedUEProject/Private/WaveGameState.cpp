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


	if (TotalWaves > 0 && CurrentWave >= TotalWaves)
	{
		bFinalWaveStarted = true;
	}
	OnWaveProgressChanged.Broadcast(CurrentWave, TotalWaves);
}

void AWaveGameState::OnSpawnerEnemyCountUpdated(int Alive, int Total)
{
	if (!HasAuthority())
		return;

	AliveEnemies = Alive;
	TotalEnemies = Total;

	OnEnemyCountChanged.Broadcast(AliveEnemies, TotalEnemies);
	
	if (bFinalWaveStarted && AliveEnemies <= 0 && MatchPhase == EMatchPhase::InProgress)
	{
		SetMatchPhase(EMatchPhase::Victory);
	}
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
	
	FString MapToLoad = GameplayMapPath;
	if (MapToLoad.IsEmpty())
	{
		UE_LOG(LogTemp, Warning, TEXT("WaveGameState: GameplayMapPath is not set! "
			"Call SetGameplayMapPath() from your GameMode BeginPlay."));
		return;
	}
 
	bFinalWaveStarted = false;
	FTimerHandle RestartTimerHandle;
	GetWorldTimerManager().SetTimer(RestartTimerHandle, [this, MapToLoad]()
	{
		if (UWorld* World = GetWorld())
		{

			World->ServerTravel(MapToLoad + TEXT("?listen"), true);
		}
	}, 1.5f, false);
}