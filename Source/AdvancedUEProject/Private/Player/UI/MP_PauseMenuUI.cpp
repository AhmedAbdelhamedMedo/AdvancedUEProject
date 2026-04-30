#include "Player/UI/MP_PauseMenuUI.h"

#include "Components/Button.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/PlayerController.h"

bool UMP_PauseMenuUI::Initialize()
{
	if (!Super::Initialize())
		return false;

	if (B_Resume)
		B_Resume->OnClicked.AddDynamic(this, &UMP_PauseMenuUI::OnResumeClicked);

	if (B_MainMenu)
		B_MainMenu->OnClicked.AddDynamic(this, &UMP_PauseMenuUI::OnMainMenuClicked);
	
	if (B_Exit)
		B_Exit->OnClicked.AddDynamic(this, &UMP_PauseMenuUI::OnExitClicked);

	PlayerController = GetOwningPlayer();
	
	return true;
}

void UMP_PauseMenuUI::OnResumeClicked()
{
	if (!PlayerController)
		return;

	RemoveFromParent();

	PlayerController->SetInputMode(FInputModeGameOnly());
	PlayerController->bShowMouseCursor = false;
	PlayerController->SetIgnoreMoveInput(false);
	PlayerController->SetIgnoreLookInput(false);
}

void UMP_PauseMenuUI::OnMainMenuClicked()
{
	if (!PlayerController)
		return;

	UGameplayStatics::OpenLevel(this, FName("MainMenu"));
}

void UMP_PauseMenuUI::OnExitClicked()
{
	UKismetSystemLibrary::QuitGame(
		this,
		GetOwningPlayer(),
		EQuitPreference::Quit,
		false
	);
}