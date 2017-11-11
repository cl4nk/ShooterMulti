// Fill out your copyright notice in the Description page of Project Settings.

#include "ShooterMulti.h"
#include "ShooterPlayerController.h"
#include "ShooterCharacter.h"
#include "ShooterMultiPlayerState.h"
#include "UserWidget.h"


// Called to bind functionality to input
void AShooterPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	InputComponent->BindAxis( "Turn", this, &AShooterPlayerController::Turn );
	InputComponent->BindAxis( "LookUp", this, &AShooterPlayerController::LookUp );

	InputComponent->BindAxis( "MoveForward", this, &AShooterPlayerController::MoveForward );
	InputComponent->BindAxis( "MoveRight", this, &AShooterPlayerController::MoveRight );

	//InputComponent->BindAction("Jump", EInputEvent::IE_Pressed, this, &AShooterPlayerController::StartJump);
	//InputComponent->BindAction("Jump", EInputEvent::IE_Released, this, &AShooterPlayerController::EndJump);

	InputComponent->BindAction( "Sprint", IE_Pressed, this, &AShooterPlayerController::StartSprint );
	InputComponent->BindAction( "Sprint", IE_Released, this, &AShooterPlayerController::EndSprint );

	InputComponent->BindAction( "Aim", IE_Pressed, this, &AShooterPlayerController::StartAim );
	InputComponent->BindAction( "Aim", IE_Released, this, &AShooterPlayerController::EndAim );

	InputComponent->BindAction( "Reload", IE_Pressed, this, &AShooterPlayerController::Reload );

	InputComponent->BindAction( "Punch", IE_Pressed, this, &AShooterPlayerController::Punch );

	InputComponent->BindAction( "Shoot", IE_Pressed, this, &AShooterPlayerController::StartShoot );
	InputComponent->BindAction( "Shoot", IE_Released, this, &AShooterPlayerController::EndShoot );

	InputComponent->BindAction( "Info", IE_Pressed, this, &AShooterPlayerController::ShowInfo );
	InputComponent->BindAction( "Info", IE_Released, this, &AShooterPlayerController::HideInfo );

	InputComponent->BindAction( "Interact", IE_Pressed, this, &AShooterPlayerController::Interact );

	InputComponent->BindAction( "Pause", IE_Pressed, this, &AShooterPlayerController::DisplayPause );
}

void AShooterPlayerController::Possess(APawn* InPawn)
{
	ShooterCharacter = Cast<AShooterCharacter>(InPawn);

	if (ShooterCharacter)
	{
		Super::Possess(InPawn);

		ShooterCharacter->Controller = this;
		ShooterCharacter->OnServerPossess();
	}
}

void AShooterPlayerController::UnPossess()
{
	ShooterCharacter = nullptr;

	TrySprint = false;
	TryAim = false;
	TryShoot = false;

	Super::UnPossess();
}

void AShooterPlayerController::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if (ShooterCharacter)
	{
		if (TrySprint &&
			ShooterCharacter->CurrentState != EShooterCharacterState::Sprint)
			ShooterCharacter->StartSprint();

		if (TryAim &&
			ShooterCharacter->CurrentState != EShooterCharacterState::Sprint &&
			ShooterCharacter->CurrentState != EShooterCharacterState::Aim)
			ShooterCharacter->StartAim();

		if (TryShoot &&
			!ShooterCharacter->IsShooting() &&
			(ShooterCharacter->CurrentState == EShooterCharacterState::Aim ||
				ShooterCharacter->CurrentState == EShooterCharacterState::IdleRun))
			ShooterCharacter->StartShoot();

		if (IsInputKeyDown(EKeys::G))
		{
			TSubclassOf<UDamageType> const DamageType = TSubclassOf<UDamageType>(UDamageType::StaticClass());
			FDamageEvent DamageEvent = FDamageEvent(DamageType);
			ShooterCharacter->TakeDamage(1.f, DamageEvent, this, nullptr);
		}
		if (IsInputKeyDown(EKeys::H))
		{
			ShooterCharacter->GainHealth(1.f);
		}
	}
}

void AShooterPlayerController::Turn(float Value)
{
	if (ShooterCharacter)
		ShooterCharacter->Turn(Value);
}

void AShooterPlayerController::LookUp(float Value)
{
	if (ShooterCharacter)
		ShooterCharacter->LookUp(Value);
}

void AShooterPlayerController::MoveForward(float Value)
{
	if (ShooterCharacter)
		ShooterCharacter->MoveForward(Value);
}

void AShooterPlayerController::MoveRight(float Value)
{
	if (ShooterCharacter)
		ShooterCharacter->MoveRight(Value);
}

void AShooterPlayerController::StartJump()
{
	if (ShooterCharacter)
		ShooterCharacter->StartJump();
}

void AShooterPlayerController::EndJump()
{
	if (ShooterCharacter)
		ShooterCharacter->EndJump();
}

void AShooterPlayerController::StartSprint()
{
	TrySprint = true;

	if (ShooterCharacter)
		ShooterCharacter->StartSprint();
}

void AShooterPlayerController::EndSprint()
{
	TrySprint = false;

	if (ShooterCharacter)
		ShooterCharacter->EndSprint();
}

void AShooterPlayerController::StartAim()
{
	TryAim = true;

	if (ShooterCharacter)
		ShooterCharacter->StartAim();
}

void AShooterPlayerController::EndAim()
{
	TryAim = false;

	if (ShooterCharacter)
		ShooterCharacter->EndAim();
}

void AShooterPlayerController::Interact()
{
	if (ShooterCharacter)
		ShooterCharacter->Interact();
}

void AShooterPlayerController::Reload()
{
	if (ShooterCharacter)
		ShooterCharacter->Reload();
}

void AShooterPlayerController::Punch()
{
	if (ShooterCharacter)
		ShooterCharacter->Punch();
}

void AShooterPlayerController::StartShoot()
{
	TryShoot = true;

	if (ShooterCharacter)
		ShooterCharacter->StartShoot();
}

void AShooterPlayerController::EndShoot()
{
	TryShoot = false;

	if (ShooterCharacter)
		ShooterCharacter->EndShoot();
}

void AShooterPlayerController::Client_SetIgnoreMoveLookInput_Implementation(bool bNewMoveInput, bool bNewLookInput)
{
	SetIgnoreMoveInput(bNewMoveInput);
	SetIgnoreLookInput(bNewLookInput);
}

bool AShooterPlayerController::Client_SetIgnoreMoveLookInput_Validate(bool bNewMoveInput, bool bNewLookInput)
{
	return true;
}

void AShooterPlayerController::Client_SetIgnoreAllInputs_Implementation(bool bIgnoreInputs)
{
	if (bIgnoreInputs)
	{
		DisableInput(this);

		EndShoot();
		EndAim();
		EndSprint();
	}
	else
		EnableInput(this);
}

bool AShooterPlayerController::Client_SetIgnoreAllInputs_Validate(bool bIgnoreInputs)
{
	return true;
}

void AShooterPlayerController::ShowInfo()
{
	OnInfoEnabled.Broadcast(true);
}

void AShooterPlayerController::HideInfo()
{
	OnInfoEnabled.Broadcast(false);
}

void AShooterPlayerController::DisplayPause()
{
	if ( WidgetPauseInstance == nullptr )
	{
		if ( CreatePauseWidgetInstance() == false )
		{
			UE_LOG( LogTemp, Warning, TEXT("AShooterPlayerController::DisplayPause() - could not create widget pause instance") );
			return;
		}
	}

	if ( bIsPauseActive )
		WidgetPauseInstance->RemoveFromParent();
	else
		WidgetPauseInstance->AddToViewport();

	bShowMouseCursor = !bIsPauseActive;
	bIsPauseActive = !bIsPauseActive;
}

bool AShooterPlayerController::CreatePauseWidgetInstance()
{
	if ( WidgetPauseTemplate )
	{
		if ( WidgetPauseInstance == nullptr )
		{
			WidgetPauseInstance = CreateWidget<UUserWidget>( this, WidgetPauseTemplate );
			if ( WidgetPauseInstance != nullptr )
				return true;
		}
	}
	return false;
}

#pragma region Respawning
void AShooterPlayerController::Client_OnPostLogin_Implementation()
{
	Server_RequestRespawn();
}

bool AShooterPlayerController::Client_OnPostLogin_Validate()
{
	return true;
}

void AShooterPlayerController::Server_RequestRespawn_Implementation()
{
	if (Role == ROLE_Authority)
	{
		AShooterMultiPlayerState* playerState = Cast<AShooterMultiPlayerState>(PlayerState);
		check( playerState );

		playerState->RequestRespawn();
	}
}

bool AShooterPlayerController::Server_RequestRespawn_Validate()
{
	return true;
}
#pragma endregion
