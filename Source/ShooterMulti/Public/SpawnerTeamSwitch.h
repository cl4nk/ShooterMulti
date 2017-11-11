// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "TeamSwitch.h"
#include "SpawnerTeamSwitch.generated.h"

/**
 * 
 */
UCLASS()

class SHOOTERMULTI_API ASpawnerTeamSwitch : public ATeamSwitch
{
GENERATED_BODY()

public:

	void Destroyed() override;

	void OnReceiveInteraction_Implementation(AShooterCharacter* Character) override;

	void Spawn();

	UPROPERTY(EditAnywhere)
	float UseFrequency = 4.0f;

private:
	UPROPERTY(EditAnywhere)
	TSubclassOf<class AUndeadCharacter> UndeadBlueprint;

	UPROPERTY(EditAnywhere)
	float OwnerTime = 20.0f;

	UPROPERTY(EditAnywhere)
	float SpawnDistance = 100.0f;

	FTimerHandle OwningTimeHandle;
};
