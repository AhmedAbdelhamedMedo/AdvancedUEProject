#include "HealthPickup/HealthPickup.h"

#include <Components/StaticMeshComponent.h>
#include <Components/SphereComponent.h>
#include <Kismet/GameplayStatics.h>
#include "DamageableInterface.h"

AHealthPickup::AHealthPickup()
{
	PrimaryActorTick.bCanEverTick = false;

	bNetLoadOnClient = true;
	bReplicates = true;

	PickupStaticMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("PickupStaticMesh"));
	SetRootComponent(PickupStaticMesh);
	PickupStaticMesh->SetIsReplicated(true);

	SphereCollision = CreateDefaultSubobject<USphereComponent>(TEXT("SphereCollision"));
	SphereCollision->SetupAttachment(RootComponent);
	SphereCollision->SetIsReplicated(true);
}

void AHealthPickup::BeginPlay()
{
	Super::BeginPlay();
	
	if (PickupStaticMesh && SphereCollision)
		SphereCollision->AttachToComponent(PickupStaticMesh, FAttachmentTransformRules::SnapToTargetNotIncludingScale);
}

void AHealthPickup::NotifyActorBeginOverlap(AActor* OtherActor)
{
	Super::NotifyActorBeginOverlap(OtherActor);
	
	if (HasAuthority() && OtherActor && OtherActor->GetClass()->ImplementsInterface(UDamageableInterface::StaticClass()))
	{
		IDamageableInterface::Execute_RestoreHealth(OtherActor, HealAmount);
		Destroy();
	}
}

void AHealthPickup::Destroyed()
{
	Super::Destroyed();
	MULTICAST_PlayPickupEffects();
}

void AHealthPickup::MULTICAST_PlayPickupEffects_Implementation()
{
	if (PickupParticle)
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), PickupParticle, GetActorLocation());

	if (PickupSound)
		UGameplayStatics::PlaySoundAtLocation(this, PickupSound, GetActorLocation());
}