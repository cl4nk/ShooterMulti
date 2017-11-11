// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerStart.h"
#include "ShooterMultiPlayerStart.generated.h"

/**
 * 
 */
UCLASS()

class SHOOTERMULTI_API AShooterMultiPlayerStart : public APlayerStart
{
GENERATED_BODY()

public:
	/**
	 * \brief Get the Spawn Group ID
	 * \return The Spawn Group ID
	 */
	UFUNCTION( Category = "ShooterMulti|Spawn", BlueprintCallable )
	int GetSpawnGroupID() const;

protected:
	/**
	 * \brief Spawn Group ID
	 */
	UPROPERTY( Category = "ShooterMulti|Spawn", EditAnywhere, BlueprintReadOnly )
	int SpawnGroupID = -1;
};
