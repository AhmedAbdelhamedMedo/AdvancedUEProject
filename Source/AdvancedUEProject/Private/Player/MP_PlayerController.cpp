#include "Player/MP_PlayerController.h"

#include "EnhancedInputSubsystems.h"
#include "Player/MP_PlayerCharacter.h"
#include "Common/MP_HealthUI.h"

void AMP_PlayerController::BeginPlay()
{
    Super::BeginPlay();

    bShowMouseCursor = true;

    FInputModeGameAndUI Mode;
    Mode.SetHideCursorDuringCapture(false);
    SetInputMode(Mode);

	if (IsLocalController())
	{
		HealthWidget = CreateWidget<UMP_HealthUI>(this, HealthWidgetClass);

		if (HealthWidget)
		{
			HealthWidget->AddToViewport();
			AMP_PlayerCharacter* Char = Cast<AMP_PlayerCharacter>(GetPawn());
			HealthWidget->InitializeWithCharacter(Char);
		}
	}
	
    UE_LOG(LogTemp, Warning, TEXT("PC BeginPlay: %s"), *GetName());
}

void AMP_PlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();
	if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer()))
		Subsystem->AddMappingContext(DefaultMappingContext, 0);
}