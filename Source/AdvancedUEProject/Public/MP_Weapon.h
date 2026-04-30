#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MP_Weapon.generated.h"

class UNiagaraSystem;

UCLASS()
class ADVANCEDUEPROJECT_API AMP_Weapon : public AActor
{
	GENERATED_BODY()
	
public:
	AMP_Weapon();

protected:
	UPROPERTY(VisibleAnywhere, Category = "Weapon|Mesh")
	TObjectPtr<USkeletalMeshComponent> WeaponMesh;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon|Effects")
	TObjectPtr<UNiagaraSystem> ImpactVFX;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon|Damage")
	float BaseDamage = 25.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon|Firing")
	float TraceRange = 500.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon|Firing")
	float SpreadAngle = 1.5f;

public:
	UFUNCTION()
	void StartFire();
	
	UFUNCTION(NetMulticast, Unreliable)
	void MULTICAST_PlayImpactEffect(const FVector ImpactPoint, const FVector ImpactNormal);
	
	UFUNCTION(Server, Reliable)
	void SERVER_Fire(const FVector TraceStart, const FVector RandomizedDirection);
};