#include "Enemy/EnemyLaser.h"

#include "Components/SphereComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "DamageableInterface.h"

AEnemyLaser::AEnemyLaser()
{
	PrimaryActorTick.bCanEverTick = true;
	
	bReplicates = true;
	AActor::SetReplicateMovement(true);
	
	CollisionComp = CreateDefaultSubobject<USphereComponent>("CollisionComp");
	SetRootComponent(CollisionComp);
	
	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>("ProjectileMovement");
	ProjectileMovement->InitialSpeed = 2000.0f;
	ProjectileMovement->MaxSpeed = 2000.0f;
}

void AEnemyLaser::BeginPlay()
{
	Super::BeginPlay();

	CollisionComp->OnComponentBeginOverlap.AddDynamic(this, &AEnemyLaser::OnOverlapBegin);
}

void AEnemyLaser::InitVelocity(const FVector& Direction) const
{
	if (ProjectileMovement)
		ProjectileMovement->Velocity = Direction * ProjectileMovement->InitialSpeed;
}

void AEnemyLaser::OnOverlapBegin(
		UPrimitiveComponent* OverlappedComp,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex,
		bool bFromSweep,
		const FHitResult& SweepResult)
{	
	if (HasAuthority() && OtherActor && OtherActor->GetClass()->ImplementsInterface(UDamageableInterface::StaticClass()))
		IDamageableInterface::Execute_ApplyDamage(OtherActor, DamageAmount);

	Destroy();
}