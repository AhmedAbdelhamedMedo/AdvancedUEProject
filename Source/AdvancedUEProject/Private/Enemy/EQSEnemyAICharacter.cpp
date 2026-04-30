#include "Enemy/EQSEnemyAICharacter.h"

#include <GameFramework/CharacterMovementComponent.h>
#include "Components/WidgetComponent.h"
#include "Common/MP_HealthUI.h"
#include "Enemy/EQSEnemyAIController.h"
#include "Enemy/EnemyLaser.h"
#include "Enemy/EnemyWaveSpawner.h"

AEQSEnemyAICharacter::AEQSEnemyAICharacter()
{
	PrimaryActorTick.bCanEverTick = false;

	AIControllerClass = AEQSEnemyAIController::StaticClass();
	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;

	UCharacterMovementComponent* MovementComponent = GetCharacterMovement();
	MovementComponent->MaxWalkSpeed = PatrolSpeed;
	MovementComponent->bOrientRotationToMovement = true;

	bUseControllerRotationPitch = false;
	bUseControllerRotationRoll = false;
	bUseControllerRotationYaw = false;
	
	HealthWidgetUI = CreateDefaultSubobject<UWidgetComponent>(TEXT("HealthWidget"));
	HealthWidgetUI->SetupAttachment(GetRootComponent());
	HealthWidgetUI->SetWidgetSpace(EWidgetSpace::Screen);
	HealthWidgetUI->SetDrawSize(FVector2D(100.f, 16.f));
	HealthWidgetUI->SetRelativeLocation(FVector(0.f, 0.f, 120.f));
}

void AEQSEnemyAICharacter::BeginPlay()
{
	Super::BeginPlay();
	
	if (!HealthWidgetUI)
		return;

	UMP_HealthUI* Widget = Cast<UMP_HealthUI>(HealthWidgetUI->GetUserWidgetObject());

	if (!Widget)
		return;

	Widget->InitializeWithCharacter(this);
}

void AEQSEnemyAICharacter::Fire()
{
	if (!HasAuthority() || bIsDead)
		return;

	const float Time = GetWorld()->GetTimeSeconds();
	
	if (Time - LastFireTime < FireRate)
		return;
	
	LastFireTime = Time;
	
	if (!LaserClass)
		return;
	
	const FVector MuzzleLocation = GetMesh()->GetSocketLocation("Muzzle");
	const FRotator MuzzleRotation = GetControlRotation();

	FActorSpawnParameters Params;
	Params.Owner = this;
	Params.Instigator = GetInstigator();

	AEnemyLaser* Bullet = GetWorld()->SpawnActor<AEnemyLaser>(
		LaserClass,
		MuzzleLocation,
		MuzzleRotation,
		Params
	);
}

void AEQSEnemyAICharacter::NetMulticast_OnDeath_Implementation()
{
	Super::NetMulticast_OnDeath_Implementation();
	Death();
}

void AEQSEnemyAICharacter::Death()
{
	bIsDead = true;
	
	if (HealthWidgetUI)
		HealthWidgetUI->SetVisibility(false);

	if (WaveSpawner)
		WaveSpawner->NotifyEnemyKilled();
	
	if (GetMesh())
	{
		GetMesh()->SetCollisionProfileName(TEXT("Ragdoll"));
		GetMesh()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
        GetMesh()->SetCollisionObjectType(ECC_PhysicsBody);
		GetMesh()->SetSimulatePhysics(true);
		GetMesh()->SetEnableGravity(true);
        GetMesh()->WakeAllRigidBodies();
	}
	
	FTimerHandle DestroyTimerHandle;

	GetWorld()->GetTimerManager().SetTimer(
		DestroyTimerHandle,
		this,
		&AEQSEnemyAICharacter::DestroyEnemy,
		2.0f,
		false
	);
}

void AEQSEnemyAICharacter::DestroyEnemy()
{
	Destroy();
}

void AEQSEnemyAICharacter::SetWaveSpawner(AEnemyWaveSpawner* InSpawner)
{
	WaveSpawner = InSpawner;
}