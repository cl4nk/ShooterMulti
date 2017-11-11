// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "InteractiveStaticMesh.h"
#include "RechargeAmmos.generated.h"

/**
 * 
 */
UCLASS()

class SHOOTERMULTI_API ARechargeAmmos : public AInteractiveStaticMesh
{
GENERATED_BODY()
public:
	FName GetObjectName() const override;

protected:
	void OnSelectableEnter_Implementation(AActor* Actor) override;

	UPROPERTY(EditInstanceOnly)
	int Ammo = 10;
};
