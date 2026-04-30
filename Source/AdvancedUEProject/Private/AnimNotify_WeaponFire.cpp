#include "AnimNotify_WeaponFire.h"

#include "Player/MP_PlayerCharacter.h"

void UAnimNotify_WeaponFire::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
	const FAnimNotifyEventReference& EventReference)
{
	Super::Notify(MeshComp, Animation, EventReference);
	
	if (MeshComp && MeshComp->GetOwner())
	{
		if (AMP_PlayerCharacter* PlayerChar = Cast<AMP_PlayerCharacter>(MeshComp->GetOwner()))
			PlayerChar->FireWeaponNotify();
	}
}