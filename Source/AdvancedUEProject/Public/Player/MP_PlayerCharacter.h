#pragma once

#include "CoreMinimal.h"
#include "Common/BaseCharacter.h"
#include "GenericTeamAgentInterface.h"
#include "MP_PlayerCharacter.generated.h"

class USpringArmComponent;
class UCameraComponent;
class UInputAction;
class UAnimMontage;
class AMP_Weapon;
class AMP_PlayerController;
struct FInputActionValue;

UCLASS()
class ADVANCEDUEPROJECT_API AMP_PlayerCharacter : public ABaseCharacter, public IGenericTeamAgentInterface
{
	GENERATED_BODY()

public:
	AMP_PlayerCharacter();

private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<USpringArmComponent> SpringArm;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UCameraComponent> CameraComponent;

	UPROPERTY(ReplicatedUsing = OnRep_CanAttack)
	bool Rep_bCanAttack = true;

	UPROPERTY(ReplicatedUsing = OnRep_YawRotation)
	float Rep_fYawRotation = 0.0f;
	
	UFUNCTION()
	void OnRep_CanAttack();

	UFUNCTION()
	void OnRep_YawRotation();

	UFUNCTION()
	void StartAttackCooldown();
	
	void InitWeapon();

	void PlayAttackMontage() const;

	void ResetAttack();
	
	FTimerHandle AttackCooldownHandle;
	
protected:
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	float BaseSpeed = 600.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	float SprintMultiplier = 2.0f;

	UPROPERTY(EditAnywhere, Category = "Combat")
	float AttackCooldown = 0.5f;

	UPROPERTY()
	TObjectPtr<AMP_PlayerController> PlayerController;

	UPROPERTY(EditAnywhere, Category = "Components")
	TObjectPtr<AMP_Weapon> WeaponMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UChildActorComponent* WeaponChild;
	
	//Input Actions
	UPROPERTY(EditAnywhere, Category = "Input")
	TObjectPtr<UInputAction> MoveAction;

	UPROPERTY(EditAnywhere, Category = "Input")
	TObjectPtr<UInputAction> SprintAction;

	UPROPERTY(EditAnywhere, Category = "Input")
	TObjectPtr<UInputAction> JumpAction;

	UPROPERTY(EditAnywhere, Category = "Input")
	TObjectPtr<UInputAction> PauseAction;

	UPROPERTY(EditAnywhere, Category = "Input")
	TObjectPtr<UInputAction> AttackAction;


	UPROPERTY(EditAnywhere, Category = "Animation")
	TObjectPtr<UAnimMontage> AttackMontage;

	//Controls
	void HandleMove(const FInputActionValue& Value);
	void HandleJumpStart();
	void HandleJumpStop();
	void HandleSprintStart();
	void HandleSprintStop();
	void HandleAttack();
	void TogglePauseMenu();

	UFUNCTION(Server, Reliable)
	void SERVER_HandleSprintStart();

	UFUNCTION(Server, Reliable)
	void SERVER_HandleSprintStop();

	UFUNCTION(Server, Reliable)
	void SERVER_HandleAttack();
	
	UFUNCTION(Server, Reliable)
	void SERVER_SetYawRotation(float NewYawRotation);

	UFUNCTION(NetMulticast, Reliable)
	void MULTICAST_PlayAttack();

public:
	FGenericTeamId TeamID = FGenericTeamId(5);
	
	void FireWeaponNotify() const;
	
	virtual FGenericTeamId GetGenericTeamId() const override { return TeamID; }
	
	virtual void Tick(float DeltaTime) override;

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
};