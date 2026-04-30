#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MP_PauseMenuUI.generated.h"

class UButton;
class APlayerController;

UCLASS()
class ADVANCEDUEPROJECT_API UMP_PauseMenuUI : public UUserWidget
{
	GENERATED_BODY()
	
protected:
	virtual bool Initialize() override;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> B_Resume;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> B_MainMenu;
	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> B_Exit;

	UPROPERTY()
	TObjectPtr<APlayerController> PlayerController;
	
	UFUNCTION()
	void OnResumeClicked();
	
	UFUNCTION()
	void OnMainMenuClicked();

	UFUNCTION()
	void OnExitClicked();
};