// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/GameModeBase.h"
#include "ShooterGameMode.generated.h"

/**
 * 
 */
UCLASS()

class SHOOTERMULTI_API AShooterGameMode : public AGameModeBase
{
GENERATED_BODY()

public:

	UPROPERTY(Transient, BlueprintReadOnly, Category=GameMode)
	int Deaths;

	UPROPERTY(Transient, BlueprintReadOnly, Category = GameMode)
	int Kills;

	AShooterGameMode();

	// Called when the game starts or when spawned
	void BeginPlay() override;

	void Destroyed() override;

	virtual void Respawn();

	void AddKill();

	void AddDeath();

	void Quit();

protected:
	FDelegateHandle DeathEventHandle;
};
