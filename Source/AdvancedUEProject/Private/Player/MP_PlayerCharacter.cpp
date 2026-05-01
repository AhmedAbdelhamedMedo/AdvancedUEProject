#include "Player/MP_PlayerCharacter.h"

#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "EnhancedInputComponent.h"
#include "InputActionValue.h"
#include "Net/UnrealNetwork.h"
#include "Player/MP_PlayerController.h"
#include "MP_Weapon.h"

AMP_PlayerCharacter::AMP_PlayerCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	SpringArm->SetupAttachment(RootComponent);
	SpringArm->TargetArmLength = 800.0f;
	SpringArm->bUsePawnControlRotation = true;
	SpringArm->bDoCollisionTest = false;

	CameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	CameraComponent->SetupAttachment(SpringArm);
	CameraComponent->bUsePawnControlRotation = false;

	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 1500.0f, 0.0f);

	GetCharacterMovement()->MaxWalkSpeed = BaseSpeed;
	GetCharacterMovement()->JumpZVelocity = 400.0f;
	GetCharacterMovement()->AirControl = 0.2f;

	GetCharacterMovement()->MinAnalogWalkSpeed = 20.0f;
	GetCharacterMovement()->BrakingDecelerationWalking = 2000.0f;
	GetCharacterMovement()->BrakingDecelerationFalling = 1500.0f;

	WeaponChild = CreateDefaultSubobject<UChildActorComponent>(TEXT("WeaponChild"));
	WeaponChild->SetupAttachment(GetMesh(), TEXT("HandGrip_R"));
	
	SetNetUpdateFrequency(100.0f);
	SetMinNetUpdateFrequency(30.0f);
}

void AMP_PlayerCharacter::BeginPlay()
{
	Super::BeginPlay();
	PlayerController = Cast<AMP_PlayerController>(GetController());
	
	FTimerHandle WeaponIntializationTimerHandle;
	GetWorldTimerManager().SetTimer(
		WeaponIntializationTimerHandle,
		this,
		&AMP_PlayerCharacter::InitWeapon,
		1.0f,
		false
	);
}

void AMP_PlayerCharacter::InitWeapon()
{
	if (WeaponChild)
	{
		WeaponMesh = Cast<AMP_Weapon>(WeaponChild->GetChildActor());

		if (WeaponMesh)
			WeaponMesh->SetOwner(this);
		else
			UE_LOG(LogTemp, Error, TEXT("Weapon still NULL on client"));
	}
}

void AMP_PlayerCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (IsLocallyControlled() && !Rep_bCanAttack && bIsDead && PlayerController)
	{
		FVector WorldLocation, WorldDirection;

		if (!PlayerController->DeprojectMousePositionToWorld(WorldLocation, WorldDirection))
			return;

		const float CharacterZ = GetActorLocation().Z;

		float T = 0.0f;

		if (FMath::Abs(WorldDirection.Z) > 0.01f)
			T = (CharacterZ - WorldLocation.Z) / WorldDirection.Z;

		if (T < 0.0f) return;

		const FVector CursorWorldPos = WorldLocation + WorldDirection * T;
		const float NewYawRotation = (CursorWorldPos - GetActorLocation()).Rotation().Yaw;
		SetActorRotation(FRotator(0.0f, NewYawRotation, 0.0f));
		SERVER_SetYawRotation(NewYawRotation);
	}
}

void AMP_PlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AMP_PlayerCharacter::HandleMove);

		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &AMP_PlayerCharacter::HandleJumpStart);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &AMP_PlayerCharacter::HandleJumpStop);

		EnhancedInputComponent->BindAction(SprintAction, ETriggerEvent::Started, this, &AMP_PlayerCharacter::HandleSprintStart);
		EnhancedInputComponent->BindAction(SprintAction, ETriggerEvent::Completed, this, &AMP_PlayerCharacter::HandleSprintStop);
		
		EnhancedInputComponent->BindAction(AttackAction, ETriggerEvent::Triggered, this, &AMP_PlayerCharacter::HandleAttack);
		EnhancedInputComponent->BindAction(PauseAction,	ETriggerEvent::Started,	this, &AMP_PlayerCharacter::TogglePauseMenu);
	}
}

void AMP_PlayerCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(AMP_PlayerCharacter, Rep_fYawRotation);
	DOREPLIFETIME(AMP_PlayerCharacter, Rep_bCanAttack);
}

void AMP_PlayerCharacter::HandleMove(const FInputActionValue& Value)
{
	const FVector2D MoveInput = Value.Get<FVector2D>();

	const FRotator CameraYaw(0.0f, CameraComponent->GetComponentRotation().Yaw, 0.0f);

	const FVector ForwardVec = FRotationMatrix(CameraYaw).GetUnitAxis(EAxis::X);
	const FVector RightVec = FRotationMatrix(CameraYaw).GetUnitAxis(EAxis::Y);

	AddMovementInput(ForwardVec, MoveInput.Y);
	AddMovementInput(RightVec, MoveInput.X);
}

void AMP_PlayerCharacter::HandleJumpStart()
{
	if (bIsDead)
		return;
	Jump();
}

void AMP_PlayerCharacter::HandleJumpStop()
{
	StopJumping();
}

void AMP_PlayerCharacter::HandleSprintStart()
{
	GetCharacterMovement()->MaxWalkSpeed = BaseSpeed * SprintMultiplier;
	SERVER_HandleSprintStart();
}

void AMP_PlayerCharacter::HandleSprintStop()
{
	GetCharacterMovement()->MaxWalkSpeed = BaseSpeed;
	SERVER_HandleSprintStop();
}

void AMP_PlayerCharacter::HandleAttack()
{
	if (bIsDead)
		return;
	
	HandleSprintStop();
	
	if (Rep_bCanAttack)
	{
		GetCharacterMovement()->bOrientRotationToMovement = false;
		Rep_bCanAttack = false;

		PlayAttackMontage();
		if (HasAuthority())
		{
			MULTICAST_PlayAttack();
			StartAttackCooldown();
		}
		else
			SERVER_HandleAttack();
	}
}

void AMP_PlayerCharacter::TogglePauseMenu()
{
	PlayerController->TogglePauseMenu();
}

void AMP_PlayerCharacter::FireWeaponNotify() const
{
	if (WeaponMesh && IsLocallyControlled())
		WeaponMesh->StartFire();
}

void AMP_PlayerCharacter::SERVER_HandleSprintStart_Implementation()
{
	GetCharacterMovement()->MaxWalkSpeed = BaseSpeed * SprintMultiplier;
}

void AMP_PlayerCharacter::SERVER_HandleSprintStop_Implementation()
{
	GetCharacterMovement()->MaxWalkSpeed = BaseSpeed;
}

void AMP_PlayerCharacter::SERVER_HandleAttack_Implementation()
{
	if (!Rep_bCanAttack) return;

	Rep_bCanAttack = false;
	GetCharacterMovement()->bOrientRotationToMovement = false;

	MULTICAST_PlayAttack();
	StartAttackCooldown();
}

void AMP_PlayerCharacter::SERVER_SetYawRotation_Implementation(float NewYawRotation)
{
	Rep_fYawRotation = NewYawRotation;

	SetActorRotation(FRotator(0.0f, NewYawRotation, 0.0f));
}

void AMP_PlayerCharacter::MULTICAST_PlayAttack_Implementation()
{
	if (!IsLocallyControlled())
		PlayAttackMontage();
}

void AMP_PlayerCharacter::PlayAttackMontage() const
{
	if (!AttackMontage) return;

	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (!AnimInstance) return;

	if (AnimInstance->Montage_IsPlaying(AttackMontage)) return;

	AnimInstance->Montage_Play(AttackMontage);
}

void AMP_PlayerCharacter::StartAttackCooldown()
{
	Rep_bCanAttack = false;
	GetCharacterMovement()->bOrientRotationToMovement = false;

	GetWorldTimerManager().SetTimer(
		AttackCooldownHandle,
		this,
		&AMP_PlayerCharacter::ResetAttack,
		AttackCooldown,
		false
	);
}

void AMP_PlayerCharacter::ResetAttack()
{
	GetCharacterMovement()->bOrientRotationToMovement = true;
	Rep_bCanAttack = true;
}

void AMP_PlayerCharacter::OnRep_CanAttack()
{
	GetCharacterMovement()->bOrientRotationToMovement = Rep_bCanAttack;
}

void AMP_PlayerCharacter::OnRep_YawRotation()
{
	if (!IsLocallyControlled())
		SetActorRotation(FRotator(0.0f, Rep_fYawRotation, 0.0f));
}


void AMP_PlayerCharacter::NetMulticast_OnDeath_Implementation()
{
	Super::NetMulticast_OnDeath_Implementation();
	Death();
}

void AMP_PlayerCharacter::Death()
{
	bIsDead = true;
	
	if (GetMesh())
	{
		GetMesh()->SetCollisionProfileName(TEXT("Ragdoll"));
		GetMesh()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		GetMesh()->SetCollisionObjectType(ECC_PhysicsBody);
		GetMesh()->SetSimulatePhysics(true);
		GetMesh()->SetEnableGravity(true);
		GetMesh()->WakeAllRigidBodies();
	}
}