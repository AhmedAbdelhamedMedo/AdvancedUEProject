#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "HealthPickup.generated.h"

class UStaticMeshComponent;
class USphereComponent;
class UParticleSystem;
class USoundBase;

UCLASS()
class ADVANCEDUEPROJECT_API AHealthPickup : public AActor
{
	GENERATED_BODY()
	
public:	
	AHealthPickup();

protected:
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UStaticMeshComponent> PickupStaticMesh;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<USphereComponent> SphereCollision;

	UPROPERTY(EditAnywhere)
	TObjectPtr<UParticleSystem> PickupParticle;

	UPROPERTY(EditAnywhere)
	TObjectPtr<USoundBase> PickupSound;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float HealAmount = 10.0f;

	UFUNCTION(NetMulticast, Unreliable)
	void MULTICAST_PlayPickupEffects();
	
public:	
	virtual void NotifyActorBeginOverlap(AActor* OtherActor) override;

	virtual void Destroyed() override;
};