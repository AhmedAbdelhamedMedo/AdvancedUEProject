#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "GenericTeamAgentInterface.h"
#include "MP_PlayerController.generated.h"

class UInputMappingContext;
class UUserWidget;
class UMP_HealthUI;
class UMP_PauseMenuUI;

UCLASS()
class ADVANCEDUEPROJECT_API AMP_PlayerController : public APlayerController, public IGenericTeamAgentInterface
{
	GENERATED_BODY()

public:
	FGenericTeamId TeamID = FGenericTeamId(5);
	
	virtual FGenericTeamId GetGenericTeamId() const override { return TeamID; }
	
	void TogglePauseMenu();
	
protected:
	UPROPERTY(EditAnywhere, Category = "Input|Input Mapping")
	UInputMappingContext* DefaultMappingContext;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI")
	TSubclassOf<UMP_HealthUI> HealthWidgetClass;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI")
	TSubclassOf<UMP_PauseMenuUI> PauseMenuClass;

	UPROPERTY()
	TObjectPtr<UMP_HealthUI> HealthWidget;
	
	UPROPERTY()
	TObjectPtr<UMP_PauseMenuUI> PauseMenuWidget;

	virtual void BeginPlay() override;

	virtual void SetupInputComponent() override;
};