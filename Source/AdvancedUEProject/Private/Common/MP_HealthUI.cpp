#include "Common/MP_HealthUI.h"

#include "Common/BaseCharacter.h"
#include <Components/ProgressBar.h>

void UMP_HealthUI::NativeConstruct()
{
	Super::NativeConstruct();
	PB_HealthBar->SetPercent(1.0f);
}

void UMP_HealthUI::InitializeWithCharacter(ABaseCharacter* InCharacter)
{
	if (InCharacter)
		InCharacter->OnHealthChanged.AddDynamic(this, &UMP_HealthUI::SetHealthPercent);
}

void UMP_HealthUI::SetHealthPercent(const float CurrentHealth, const float MaxHealth)
{
	PB_HealthBar->SetPercent(CurrentHealth/MaxHealth);
}