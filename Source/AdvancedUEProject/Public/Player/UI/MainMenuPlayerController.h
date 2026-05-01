#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Controller.h"
#include "MainMenuPlayerController.generated.h"

class UMainMenuWidget;

UCLASS()
class ADVANCEDUEPROJECT_API AMainMenuPlayerController : public AController
{
	GENERATED_BODY()

protected:
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "UI")
	TSubclassOf<UMainMenuWidget> MainMenuWidgetClass;

	UPROPERTY()
	TObjectPtr<UMainMenuWidget> MainMenuWidget;
};
