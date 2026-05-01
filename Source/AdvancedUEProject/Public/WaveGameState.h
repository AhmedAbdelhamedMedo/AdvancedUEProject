#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "WaveGameState.generated.h"

class AEnemyWaveSpawner;

UENUM(BlueprintType)
enum class EMatchPhase : uint8
{
	WaitingToStart UMETA(DisplayName = "Waiting To Start"),
	InProgress UMETA(DisplayName = "In Progress"),
	Victory UMETA(DisplayName = "Victory"),
	Defeat UMETA(DisplayName = "Defeat")
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnMatchPhaseChanged, EMatchPhase, NewPhase);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnWaveProgressChanged, int, CurrentWave, int, TotalWaves);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnEnemyCountChanged, int, Alive, int, Total);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnAlivePlayersChanged, int, AlivePlayers);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnRestartVoteChanged, int, VotesCount, int, VotesNeeded);

UCLASS()
class ADVANCEDUEPROJECT_API AWaveGameState : public AGameStateBase
{
	GENERATED_BODY()

public:
	AWaveGameState();

	UFUNCTION()
	void RegisterSpawner(AEnemyWaveSpawner* InSpawner);

	void NotifyPlayerDied();

	void NotifyPlayerSpawned();
	
	void SetGameplayMapPath(const FString& MapPath) { GameplayMapPath = MapPath; }

	void RegisterRestartVote(APlayerController* Voter);

	UFUNCTION()
	int GetCurrentWave() const;

	UFUNCTION()
	int GetTotalWaves() const;

	UFUNCTION()
	int GetAliveEnemies() const;

	UFUNCTION()
	int GetTotalEnemies() const;

	UFUNCTION()
	int GetAlivePlayers() const;

	UFUNCTION()
	int GetRestartVotes()   const;

	UFUNCTION()
	EMatchPhase GetMatchPhase() const;

	UFUNCTION()
	bool IsMatchInProgress() const;


	UPROPERTY()
	FOnMatchPhaseChanged OnMatchPhaseChanged;

	UPROPERTY()
	FOnWaveProgressChanged OnWaveProgressChanged;

	UPROPERTY()
	FOnEnemyCountChanged OnEnemyCountChanged;

	UPROPERTY()
	FOnAlivePlayersChanged OnAlivePlayersChanged;

	UPROPERTY()
	FOnRestartVoteChanged OnRestartVoteChanged;
	bool bFinalWaveStarted;

protected:
	virtual void BeginPlay() override;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

private:
	UPROPERTY(ReplicatedUsing = OnRep_MatchPhase, BlueprintReadOnly, Category = "Match", meta = (AllowPrivateAccess))
	EMatchPhase MatchPhase = EMatchPhase::WaitingToStart;

	UPROPERTY(ReplicatedUsing = OnRep_WaveProgress, BlueprintReadOnly, Category = "Wave", meta = (AllowPrivateAccess))
	int CurrentWave = 0;

	UPROPERTY(Replicated, BlueprintReadOnly, Category = "Wave", meta = (AllowPrivateAccess))
	int TotalWaves = 0;

	UPROPERTY(ReplicatedUsing = OnRep_EnemyCount, BlueprintReadOnly, Category = "Wave", meta = (AllowPrivateAccess))
	int AliveEnemies = 0;

	UPROPERTY(Replicated, BlueprintReadOnly, Category = "Wave", meta = (AllowPrivateAccess))
	int TotalEnemies = 0;

	UPROPERTY(ReplicatedUsing = OnRep_AlivePlayers, BlueprintReadOnly, Category = "Match", meta = (AllowPrivateAccess))
	int AlivePlayers = 0;

	UPROPERTY(ReplicatedUsing = OnRep_RestartVotes, BlueprintReadOnly, Category = "Match", meta = (AllowPrivateAccess))
	int RestartVotes = 0;
	
	UFUNCTION()
	void OnRep_MatchPhase();

	UFUNCTION()
	void OnRep_WaveProgress();

	UFUNCTION()
	void OnRep_EnemyCount();

	UFUNCTION()
	void OnRep_AlivePlayers();

	UFUNCTION()
	void OnRep_RestartVotes();
	
	UFUNCTION()
	void OnSpawnerWaveUpdated(int NewWave);

	UFUNCTION()
	void OnSpawnerEnemyCountUpdated(int Alive, int Total);

	void SetMatchPhase(EMatchPhase NewPhase);

	void CheckDefeatCondition();

	void TriggerRestart();
	
	UPROPERTY()
	TObjectPtr<AEnemyWaveSpawner> Spawner;
	
	UPROPERTY()
	TArray<TObjectPtr<APlayerController>> RestartVoters;
	
	FString GameplayMapPath;
};