#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MP_HealthUI.generated.h"

class UProgressBar;
class ABaseCharacter;

UCLASS()
class ADVANCEDUEPROJECT_API UMP_HealthUI : public UUserWidget
{
	GENERATED_BODY()
public:
	UFUNCTION()
	void SetHealthPercent(float CurrentHealth, float MaxHealth);

	void InitializeWithCharacter(ABaseCharacter* InCharacter);

protected:
	virtual void NativeConstruct() override;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UProgressBar> PB_HealthBar;
};