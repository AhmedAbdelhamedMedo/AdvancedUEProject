#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "MainMenuPlayerController.generated.h"

class UMenu;

UCLASS()
class ADVANCEDUEPROJECT_API AMainMenuPlayerController : public APlayerController
{
	GENERATED_BODY()

protected:
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "UI")
	TSubclassOf<UMenu> MainMenuWidgetClass;

	UPROPERTY()
	TObjectPtr<UMenu> MainMenuWidget;
};