#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "DamageableInterface.generated.h"

UINTERFACE(MinimalAPI)
class UDamageableInterface : public UInterface
{
	GENERATED_BODY()
};

class ADVANCEDUEPROJECT_API IDamageableInterface
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void RestoreHealth(float HealAmount);

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void ApplyDamage(float DamageAmount);
};
