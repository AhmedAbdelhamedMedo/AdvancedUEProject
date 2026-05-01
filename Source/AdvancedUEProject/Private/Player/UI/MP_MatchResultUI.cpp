#include "Player/UI/MP_MatchResultUI.h"

#include "Engine/World.h"
#include "GameFramework/PlayerController.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "Player/MP_PlayerController.h"
#include "WaveGameState.h"

void UMP_MatchResultUI::NativeConstruct()
{
	Super::NativeConstruct();

	SetVisibility(ESlateVisibility::Collapsed);
	bHasVotedRestart = false;
	
	if (btn_MainMenu)
		btn_MainMenu->OnClicked.AddDynamic(this, &UMP_MatchResultUI::OnMainMenuClicked);

	if (btn_Restart)
		btn_Restart->OnClicked.AddDynamic(this, &UMP_MatchResultUI::OnRestartClicked);
		
	if (txt_RestartVotes)
		txt_RestartVotes->SetText(FText::FromString(TEXT("Restart (0/?)")));
	
	if (AWaveGameState* GS = GetWorld()->GetGameState<AWaveGameState>())
	{
		GS->OnMatchPhaseChanged.AddDynamic(this, &UMP_MatchResultUI::OnMatchPhaseChanged);
		GS->OnRestartVoteChanged.AddDynamic(this, &UMP_MatchResultUI::OnRestartVoteChanged);

		const EMatchPhase CurrentPhase = GS->GetMatchPhase();
		if (CurrentPhase == EMatchPhase::Victory || CurrentPhase == EMatchPhase::Defeat)
			ApplyResult(CurrentPhase);
	}
}

void UMP_MatchResultUI::NativeDestruct()
{
	if (AWaveGameState* GS = GetWorld() ? GetWorld()->GetGameState<AWaveGameState>() : nullptr)
	{
		GS->OnMatchPhaseChanged.RemoveDynamic(this, &UMP_MatchResultUI::OnMatchPhaseChanged);
		GS->OnRestartVoteChanged.RemoveDynamic(this, &UMP_MatchResultUI::OnRestartVoteChanged);
	}

	if (btn_MainMenu)
		btn_MainMenu->OnClicked.RemoveAll(this);

	if (btn_Restart)
		btn_Restart->OnClicked.RemoveAll(this);
	
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

void UMP_MatchResultUI::OnRestartVoteChanged(int VoteCount, int VotesNeeded)
{
	if (txt_RestartVotes)
	{
		const FString Label = FString::Printf(TEXT("Restart ( %d / %d )"), VoteCount, VotesNeeded);
		txt_RestartVotes->SetText(FText::FromString(Label));
	}
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

	if (AWaveGameState* GS = GetWorld()->GetGameState<AWaveGameState>())
		OnRestartVoteChanged(GS->GetRestartVotes(), GS->PlayerArray.Num());

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

void UMP_MatchResultUI::OnRestartClicked()
{
	if (bHasVotedRestart)
		return;
	
	bHasVotedRestart = true;
 
	if (btn_Restart)
		btn_Restart->SetIsEnabled(false);

	if (APlayerController* PC = GetOwningPlayer())
	{
		if (AMP_PlayerController* MPC = Cast<AMP_PlayerController>(PC))
		MPC->SERVER_RequestRestart();
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
