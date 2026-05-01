#include "Player/UI/MP_MatchResultUI.h"

#include "WaveGameState.h"
#include "Components/TextBlock.h"
#include "Components/Button.h"
#include "Animation/WidgetAnimation.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/World.h"
#include "GameFramework/PlayerController.h"

void UMP_MatchResultUI::NativeConstruct()
{
	Super::NativeConstruct();

	SetVisibility(ESlateVisibility::Collapsed);

	if (btn_MainMenu)
		btn_MainMenu->OnClicked.AddDynamic(this, &UMP_MatchResultUI::OnMainMenuClicked);

	if (AWaveGameState* GS = GetWorld()->GetGameState<AWaveGameState>())
	{
		GS->OnMatchPhaseChanged.AddDynamic(this, &UMP_MatchResultUI::OnMatchPhaseChanged);

		const EMatchPhase CurrentPhase = GS->GetMatchPhase();
		if (CurrentPhase == EMatchPhase::Victory || CurrentPhase == EMatchPhase::Defeat)
			ApplyResult(CurrentPhase);
	}
}

void UMP_MatchResultUI::NativeDestruct()
{
	if (AWaveGameState* GS = GetWorld() ? GetWorld()->GetGameState<AWaveGameState>() : nullptr)
		GS->OnMatchPhaseChanged.RemoveDynamic(this, &UMP_MatchResultUI::OnMatchPhaseChanged);

	if (btn_MainMenu)
		btn_MainMenu->OnClicked.RemoveAll(this);

	Super::NativeDestruct();
}

void UMP_MatchResultUI::InitAndShow(EMatchPhase Result)
{
	ApplyResult(Result);
}

void UMP_MatchResultUI::OnMatchPhaseChanged(EMatchPhase NewPhase)
{
	if (NewPhase == EMatchPhase::Victory || NewPhase == EMatchPhase::Defeat)
		ApplyResult(NewPhase);
}

void UMP_MatchResultUI::ApplyResult(EMatchPhase Result)
{
	if (txt_Result)
	{
		const FText ResultText = (Result == EMatchPhase::Victory)
			                         ? FText::FromString(TEXT("VICTORY"))
			                         : FText::FromString(TEXT("DEFEAT"));

		txt_Result->SetText(ResultText);
	}

	SetVisibility(ESlateVisibility::Visible);

	APlayerController* PC = GetOwningPlayer();
	if (PC && PC->IsLocalController())
	{
		FInputModeUIOnly InputMode;
		InputMode.SetWidgetToFocus(TakeWidget());
		InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
		PC->SetInputMode(InputMode);
		PC->SetShowMouseCursor(true);
	}
}

void UMP_MatchResultUI::OnMainMenuClicked()
{
	TravelToMainMenu();
}

void UMP_MatchResultUI::TravelToMainMenu()
{
	APlayerController* PC = GetOwningPlayer();
	if (!PC)
		return;

	PC->SetShowMouseCursor(false);
	PC->SetInputMode(FInputModeGameOnly());

	PC->ClientTravel(MainMenuMap, ETravelType::TRAVEL_Absolute);
}
