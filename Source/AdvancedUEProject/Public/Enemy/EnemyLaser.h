#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "EnemyLaser.generated.h"

class USphereComponent;
class UProjectileMovementComponent;

UCLASS()
class ADVANCEDUEPROJECT_API AEnemyLaser : public AActor
{
	GENERATED_BODY()

public:
	AEnemyLaser();

	UFUNCTION()
	void InitVelocity(const FVector& Direction) const;

protected:
	virtual void BeginPlay() override;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Laser|Components")
	USphereComponent* CollisionComp;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Laser|Components")
	UProjectileMovementComponent* ProjectileMovement;

	UPROPERTY(EditDefaultsOnly, Category = "Laser|Combat")
	float DamageAmount = 20.f;

	UFUNCTION()
	void OnOverlapBegin(
		UPrimitiveComponent* OverlappedComp,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex,
		bool bFromSweep,
		const FHitResult& SweepResult);
};