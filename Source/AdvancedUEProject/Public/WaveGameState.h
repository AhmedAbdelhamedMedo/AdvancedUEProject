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

UCLASS()
class ADVANCEDUEPROJECT_API AWaveGameState : public AGameStateBase
{
	GENERATED_BODY()

public:
	AWaveGameState();

	UFUNCTION(BlueprintCallable, Category = "Wave")
	void RegisterSpawner(AEnemyWaveSpawner* InSpawner);

	void NotifyPlayerDied();

	void NotifyPlayerSpawned();

	UFUNCTION(BlueprintPure, Category = "Wave")
	int GetCurrentWave() const;

	UFUNCTION(BlueprintPure, Category = "Wave")
	int GetTotalWaves() const;

	UFUNCTION(BlueprintPure, Category = "Wave")
	int GetAliveEnemies() const;

	UFUNCTION(BlueprintPure, Category = "Wave")
	int GetTotalEnemies() const;

	UFUNCTION(BlueprintPure, Category = "Match")
	int GetAlivePlayers() const;

	UFUNCTION(BlueprintPure, Category = "Match")
	EMatchPhase GetMatchPhase() const;

	UFUNCTION(BlueprintPure, Category = "Match")
	bool IsMatchInProgress() const;


	UPROPERTY(BlueprintAssignable, Category = "Match|Events")
	FOnMatchPhaseChanged OnMatchPhaseChanged;

	UPROPERTY(BlueprintAssignable, Category = "Wave|Events")
	FOnWaveProgressChanged OnWaveProgressChanged;

	UPROPERTY(BlueprintAssignable, Category = "Wave|Events")
	FOnEnemyCountChanged OnEnemyCountChanged;

	UPROPERTY(BlueprintAssignable, Category = "Match|Events")
	FOnAlivePlayersChanged OnAlivePlayersChanged;

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

	UFUNCTION()
	void OnRep_MatchPhase();

	UFUNCTION()
	void OnRep_WaveProgress();

	UFUNCTION()
	void OnRep_EnemyCount();

	UFUNCTION()
	void OnRep_AlivePlayers();

	UFUNCTION()
	void OnSpawnerWaveUpdated(int NewWave);

	UFUNCTION()
	void OnSpawnerEnemyCountUpdated(int Alive, int Total);

	void SetMatchPhase(EMatchPhase NewPhase);

	void CheckDefeatCondition();

	UPROPERTY()
	TObjectPtr<AEnemyWaveSpawner> Spawner;
};