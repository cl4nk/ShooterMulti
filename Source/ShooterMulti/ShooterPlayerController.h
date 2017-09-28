// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/PlayerController.h"

class AShooterCharacter;
class UUserWidget;

#include "ShooterPlayerController.generated.h"


/**
 * 
 */
UCLASS()
class SHOOTERMULTI_API AShooterPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = UI)
	TSubclassOf<UUserWidget> WidgetTemplate;

	UPROPERTY()
	UUserWidget* WidgetInstance;

	UPROPERTY(Transient, BlueprintReadOnly)
	AShooterCharacter* ShooterCharacter;

	// Called to bind functionality to input
	virtual void SetupInputComponent() override;

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	virtual void Possess (APawn * InPawn) override;
	virtual void UnPossess() override;

	// Called every frame
	virtual void Tick(float DeltaSeconds) override;

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

	void Reload();

	void Punch();

	void StartShoot();
	void EndShoot();
	
protected:

	UPROPERTY(Transient, BlueprintReadOnly)
	bool TrySprint = false;

	UPROPERTY(Transient, BlueprintReadOnly)
	bool TryAim = false;

	UPROPERTY(Transient, BlueprintReadOnly)
	bool TryShoot = false;

};
