#include "Player/UI/MainMenuPlayerController.h"

#include "Menu.h"

void AMainMenuPlayerController::BeginPlay()
{
	Super::BeginPlay();

	if (!IsLocalController())
		return;

	if (MainMenuWidgetClass)
	{
		MainMenuWidget = CreateWidget<UMenu>(
			this,
			MainMenuWidgetClass
		);

		if (MainMenuWidget)
		{
			MainMenuWidget->AddToViewport();
			MainMenuWidget->MenuSetup(4, "FreeForAll", "/Game/FirstPerson/Lvl_FirstPerson");
			bShowMouseCursor = true;

			FInputModeUIOnly InputMode;
			InputMode.SetWidgetToFocus(MainMenuWidget->TakeWidget());

			SetInputMode(InputMode);
		}
	}
}
