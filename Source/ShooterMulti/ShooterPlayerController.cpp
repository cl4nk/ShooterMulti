// Fill out your copyright notice in the Description page of Project Settings.

#include "ShooterMulti.h"
#include "ShooterPlayerController.h"
#include "ShooterCharacter.h"
#include "ShooterGameMode.h"
#include "Blueprint/UserWidget.h"

// Called to bind functionality to input
void AShooterPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	InputComponent->BindAxis("Turn", this, &AShooterPlayerController::Turn);
	InputComponent->BindAxis("LookUp", this, &AShooterPlayerController::LookUp);

	InputComponent->BindAxis("MoveForward", this, &AShooterPlayerController::MoveForward);
	InputComponent->BindAxis("MoveRight", this, &AShooterPlayerController::MoveRight);

	//InputComponent->BindAction("Jump", EInputEvent::IE_Pressed, this, &AShooterPlayerController::StartJump);
	//InputComponent->BindAction("Jump", EInputEvent::IE_Released, this, &AShooterPlayerController::EndJump);

	InputComponent->BindAction("Sprint", EInputEvent::IE_Pressed, this, &AShooterPlayerController::StartSprint);
	InputComponent->BindAction("Sprint", EInputEvent::IE_Released, this, &AShooterPlayerController::EndSprint);

	InputComponent->BindAction("Aim", EInputEvent::IE_Pressed, this, &AShooterPlayerController::StartAim);
	InputComponent->BindAction("Aim", EInputEvent::IE_Released, this, &AShooterPlayerController::EndAim);

	InputComponent->BindAction("Reload", EInputEvent::IE_Pressed, this, &AShooterPlayerController::Reload);

	InputComponent->BindAction("Punch", EInputEvent::IE_Pressed, this, &AShooterPlayerController::Punch);

	InputComponent->BindAction("Shoot", EInputEvent::IE_Pressed, this, &AShooterPlayerController::StartShoot);
	InputComponent->BindAction("Shoot", EInputEvent::IE_Released, this, &AShooterPlayerController::EndShoot);

	AShooterGameMode* gameMode = Cast<AShooterGameMode>(UGameplayStatics::GetGameMode(GetWorld()));
	if (gameMode)
		InputComponent->BindAction("Quit", EInputEvent::IE_Pressed, gameMode, &AShooterGameMode::Quit);
}

void AShooterPlayerController::BeginPlay()
{
	if (WidgetTemplate)
	{
		WidgetInstance = CreateWidget<UUserWidget>(this, WidgetTemplate);
		
		if (WidgetInstance)
			WidgetInstance->AddToViewport();
	}
}

void AShooterPlayerController::Possess(APawn * InPawn)
{
	ShooterCharacter = Cast<AShooterCharacter>(InPawn);

	if (ShooterCharacter)
	{
		Super::Possess(InPawn);
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