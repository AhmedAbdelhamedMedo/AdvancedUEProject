#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "WaveGameState.h"
#include "MP_MatchResultUI.generated.h"

class UTextBlock;
class UButton;
class UWidgetAnimation;

UCLASS()
class ADVANCEDUEPROJECT_API UMP_MatchResultUI : public UUserWidget
{
	GENERATED_BODY()

public:
	UFUNCTION()
	void InitAndShow(EMatchPhase Result);

protected:
	virtual void NativeConstruct() override;

	virtual void NativeDestruct() override;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> txt_Result;
	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> txt_RestartVotes;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> btn_MainMenu;
 
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> btn_Restart;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Match Result")
	FString MainMenuMap = TEXT("/Game/FirstPerson/MainMenu");
	
private:
	UFUNCTION()
	void OnRestartClicked();
	
	UFUNCTION()
	void OnMainMenuClicked();

	UFUNCTION()
	void OnMatchPhaseChanged(EMatchPhase NewPhase);

	UFUNCTION()
	void OnRestartVoteChanged(int VoteCount, int VotesNeeded);
	
	void ApplyResult(EMatchPhase Result);

	void TravelToMainMenu();
	
	bool bHasVotedRestart = false;
};
