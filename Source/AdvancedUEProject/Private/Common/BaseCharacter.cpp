#include "Common/BaseCharacter.h"

#include "GameFramework/CharacterMovementComponent.h"
#include "Components/CapsuleComponent.h"
#include "Net/UnrealNetwork.h"
#include "Player/MP_PlayerCharacter.h"
#include "WaveGameState.h"

ABaseCharacter::ABaseCharacter()
{
	PrimaryActorTick.bCanEverTick = false;

	bReplicates = true;
	bAlwaysRelevant = false;

	MaxHealth = 100.0f;
	CurrentHealth = MaxHealth;
}

void ABaseCharacter::BeginPlay()
{
	Super::BeginPlay();

	if (HasAuthority())
	{
		CurrentHealth = MaxHealth;
		BroadcastHealthChanged();
	}
}

void ABaseCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ABaseCharacter, CurrentHealth);
	DOREPLIFETIME(ABaseCharacter, MaxHealth);
}

void ABaseCharacter::RestoreHealth_Implementation(float HealAmount)
{
	if (!IsAlive()) return;

	if (HasAuthority())
		ApplyHealthDelta(FMath::Abs(HealAmount));
	else
		SERVER_RestoreHealth(HealAmount);
}

void ABaseCharacter::ApplyDamage_Implementation(float DamageAmount)
{
	if (!IsAlive()) return;

	if (HasAuthority())
		ApplyHealthDelta(-FMath::Abs(DamageAmount));
	else
		SERVER_ApplyDamage(DamageAmount);
}

float ABaseCharacter::GetHealthPercent() const
{
	if (MaxHealth <= 0.0f) return 0.0f;
	return FMath::Clamp(CurrentHealth / MaxHealth, 0.0f, 1.0f);
}

void ABaseCharacter::OnRep_CurrentHealth(float OldHealth)
{
	BroadcastHealthChanged();

	if (OldHealth > 0.f && CurrentHealth <= 0.f && !bDeathTriggered)
	{
		bDeathTriggered = true;
		OnDeath();
	}
}

void ABaseCharacter::SERVER_RestoreHealth_Implementation(float HealAmount)
{
	RestoreHealth_Implementation(HealAmount);
}

void ABaseCharacter::SERVER_ApplyDamage_Implementation(float DamageAmount)
{
	ApplyDamage_Implementation(DamageAmount);
}

void ABaseCharacter::ApplyHealthDelta(float Delta)
{
	check(HasAuthority());

	const float OldHealth = CurrentHealth;
	CurrentHealth = FMath::Clamp(CurrentHealth + Delta, 0.0f, MaxHealth);

	BroadcastHealthChanged();

	if (OldHealth > 0.0f && CurrentHealth <= 0.0f && !bDeathTriggered)
	{
		bDeathTriggered = true;
		OnDeath();
	}
}

void ABaseCharacter::BroadcastHealthChanged()
{
	OnHealthChanged.Broadcast(CurrentHealth, MaxHealth);

	if (IsLocallyControlled())
		OnHealthChangedLocal.Broadcast(CurrentHealth, MaxHealth);
}

void ABaseCharacter::OnDeath()
{
	NetMulticast_OnDeath();
	
	if (HasAuthority() && Cast<AMP_PlayerCharacter>(this))
		if (AWaveGameState* GS = GetWorld()->GetGameState<AWaveGameState>())
			GS->NotifyPlayerDied();
}

void ABaseCharacter::NetMulticast_OnDeath_Implementation()
{
	UCapsuleComponent* Capsule = GetCapsuleComponent();

	Capsule->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	Capsule->SetCollisionResponseToAllChannels(ECR_Ignore);
	Capsule->SetCollisionResponseToChannel(ECC_WorldStatic, ECR_Block);
	Capsule->SetCollisionResponseToChannel(ECC_WorldDynamic, ECR_Block);
	
	if (UCharacterMovementComponent* MovementComponent = GetCharacterMovement())
	{
		MovementComponent->StopMovementImmediately();
		MovementComponent->DisableMovement();
	}
}