// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/PlayerController.h"

class AShooterCharacter;
class UUserWidget;

#include "ShooterPlayerController.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FBoolEvent, bool, value);

/**
 * 
 */
UCLASS()

class SHOOTERMULTI_API AShooterPlayerController : public APlayerController
{
GENERATED_BODY()

public:
	UPROPERTY(Transient, BlueprintReadOnly)
	AShooterCharacter* ShooterCharacter;

	// Called to bind functionality to input
	void SetupInputComponent() override;

	void Possess(APawn* InPawn) override;
	void UnPossess() override;

	// Called every frame
	void Tick(float DeltaSeconds) override;

	void Turn(float Value);
	void LookUp(float Value);

	void MoveForward(float Value);
	void MoveRight(float Value);

	void StartJump();
	void EndJump();

	void StartSprint();
	void EndSprint();

	void StartAim();
	void EndAim();

	void ShowInfo();
	void HideInfo();

	void Interact();

	void Reload();

	void Punch();

	void StartShoot();
	void EndShoot();

	UPROPERTY( BlueprintAssignable )
	FBoolEvent OnInfoEnabled;

	UFUNCTION( Client, Reliable, WithValidation )
	void Client_SetIgnoreMoveLookInput(bool bNewMoveInput, bool bNewLookInput);
	void Client_SetIgnoreMoveLookInput_Implementation(bool bNewMoveInput, bool bNewLookInput);
	bool Client_SetIgnoreMoveLookInput_Validate(bool bNewMoveInput, bool bNewLookInput);

	UFUNCTION( Client, Reliable, WithValidation )
	void Client_SetIgnoreAllInputs(bool bIgnoreInputs);
	void Client_SetIgnoreAllInputs_Implementation(bool bIgnoreInputs);
	bool Client_SetIgnoreAllInputs_Validate(bool bIgnoreInputs);

	UFUNCTION( Client, Reliable, WithValidation )
	void Client_RemoveAllWidgets();
	void Client_RemoveAllWidgets_Implementation();
	bool Client_RemoveAllWidgets_Validate();

//#pragma region Respawning
	/**
	 * \brief Function called by the server during ShooterMultiGameMode::PostLogin()
	 */
	UFUNCTION( Client, Reliable, WithValidation )
	void Client_OnPostLogin();
	void Client_OnPostLogin_Implementation();
	bool Client_OnPostLogin_Validate();

	/**
	 * \brief Request the AShooterMultiPlayerState to ask ShooterMultiGameMode to respawn player
	 */
	UFUNCTION( Server, Reliable, WithValidation )
	void Server_RequestRespawn();
	void Server_RequestRespawn_Implementation();
	bool Server_RequestRespawn_Validate();
//#pragma endregion

protected:
	UPROPERTY( Transient, BlueprintReadOnly )
	bool TrySprint = false;

	UPROPERTY( Transient, BlueprintReadOnly )
	bool TryAim = false;

	UPROPERTY( Transient, BlueprintReadOnly )
	bool TryShoot = false;

//#pragma region PauseMenu
protected:
	void DisplayPause();

	bool CreatePauseWidgetInstance();

	UPROPERTY( EditDefaultsOnly, BlueprintReadOnly )
	TSubclassOf<UUserWidget> WidgetPauseTemplate;

	UUserWidget* WidgetPauseInstance = nullptr;

	bool bIsPauseActive = false;
//#pragma endregion
};
