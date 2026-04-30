#include "MP_Weapon.h"

#include "DamageableInterface.h"
#include "DrawDebugHelpers.h"
#include "NiagaraFunctionLibrary.h"

AMP_Weapon::AMP_Weapon()
{
    bReplicates = true;
    WeaponMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Weapon"));
    SetRootComponent(WeaponMesh);
    WeaponMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void AMP_Weapon::MULTICAST_PlayImpactEffect_Implementation(const FVector ImpactPoint, const FVector ImpactNormal)
{
    if (ImpactVFX)
    {
        UNiagaraFunctionLibrary::SpawnSystemAtLocation(
            GetWorld(), 
            ImpactVFX, 
            ImpactPoint, 
            ImpactNormal.Rotation()
        );
    }
}

void AMP_Weapon::StartFire()
{
    if (!GetOwner()) return;

    const FVector TraceStart = WeaponMesh->GetSocketLocation("Muzzle");
    const FRotator SocketRotation = WeaponMesh->GetSocketRotation("Muzzle");
    
    const float HalfRad = FMath::DegreesToRadians(SpreadAngle);
    const FVector TraceDirection = FMath::VRandCone(SocketRotation.Vector(), HalfRad, HalfRad).GetSafeNormal();

    SERVER_Fire(TraceStart, TraceDirection);
}

void AMP_Weapon::SERVER_Fire_Implementation(const FVector TraceStart, const FVector RandomizedDirection)
{
    const FVector TraceEnd = TraceStart + (RandomizedDirection * TraceRange);

    FHitResult Hit;
    FCollisionQueryParams Params;
    Params.AddIgnoredActor(GetOwner());

    const bool bHit = GetWorld()->LineTraceSingleByChannel(
        Hit,
        TraceStart,
        TraceEnd,
        ECC_Visibility,
        Params
    );

    if (bHit)
    {
        if (Hit.GetActor()->GetClass()->ImplementsInterface(UDamageableInterface::StaticClass()))
            IDamageableInterface::Execute_ApplyDamage(Hit.GetActor(), BaseDamage);
        
        MULTICAST_PlayImpactEffect(Hit.ImpactPoint, Hit.ImpactNormal);
        
        DrawDebugLine(GetWorld(), TraceStart, Hit.ImpactPoint, FColor::Red, false, 1.0f);
    }
    else
    {
        DrawDebugLine(GetWorld(), TraceStart, TraceEnd, FColor::Green, false, 1.0f);
    }
}