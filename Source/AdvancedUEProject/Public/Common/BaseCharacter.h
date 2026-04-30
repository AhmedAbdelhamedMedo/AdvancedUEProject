#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "DamageableInterface.h"
#include "BaseCharacter.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(
	FOnHealthChangedLocal,
	float, CurrentHealth,
	float, MaxHealth
);

UCLASS(Abstract, Blueprintable, BlueprintType)
class ADVANCEDUEPROJECT_API ABaseCharacter : public ACharacter, public IDamageableInterface
{
	GENERATED_BODY()

public:
	ABaseCharacter();

	// Health
	virtual void RestoreHealth_Implementation(float HealAmount) override;

	virtual void ApplyDamage_Implementation(float DamageAmount) override;


	float GetCurrentHealth() const { return CurrentHealth; }

	float GetMaxHealth() const { return MaxHealth; }

	float GetHealthPercent() const;

	bool IsAlive() const { return CurrentHealth > 0.f; }

	// Delegates
	UPROPERTY(BlueprintAssignable, Category = "Health|Events")
	FOnHealthChangedLocal OnHealthChanged;

	UPROPERTY(BlueprintAssignable, Category = "Health|Events")
	FOnHealthChangedLocal OnHealthChangedLocal;

protected:
	virtual void BeginPlay() override;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UPROPERTY(EditDefaultsOnly, Replicated, BlueprintReadOnly, Category = "Health")
	float MaxHealth;

	UPROPERTY(VisibleAnywhere, ReplicatedUsing = OnRep_CurrentHealth, Category = "Health")
	float CurrentHealth = MaxHealth;


	UFUNCTION()
	void OnRep_CurrentHealth(float OldHealth);

	UFUNCTION(Server, Reliable)
	void SERVER_RestoreHealth(float HealAmount);
	//void SERVER_RestoreHealth_Implementation(float HealAmount);

	UFUNCTION(Server, Reliable)
	void SERVER_ApplyDamage(float DamageAmount);
	//void SERVER_ApplyDamage_Implementation(float DamageAmount);

	virtual void OnDeath();

	UFUNCTION(NetMulticast, Reliable)
	void NetMulticast_OnDeath();

	virtual void NetMulticast_OnDeath_Implementation();

private:
	bool bDeathTriggered = false;
	void ApplyHealthDelta(float Delta);
	void BroadcastHealthChanged();
};