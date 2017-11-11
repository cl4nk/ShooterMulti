// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "InteractiveStaticMesh.h"
#include "TeamSwitch.generated.h"

/**
 * 
 */
UCLASS()

class SHOOTERMULTI_API ATeamSwitch : public AInteractiveStaticMesh
{
	GENERATED_BODY()

public:

	ATeamSwitch();

	void BeginPlay() override;

	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	FName GetObjectName() const override { return "Switch"; };

	void ResetSwitch();

protected:
	void OnReceiveInteraction_Implementation(AShooterCharacter* Character) override;

	void SetTeamID(const int id);

	UFUNCTION()
	void OnRep_ColorChange();

	UPROPERTY(VisibleAnywhere)
	int teamOwner = -1;

	UPROPERTY(EditDefaultsOnly)
	int MaterialIndex = -1;

	UMaterialInstanceDynamic* DynamicMat = nullptr;

	UPROPERTY(EditAnywhere)
	FName ColorParameterName = "ColorActive";

	UPROPERTY( ReplicatedUsing = OnRep_ColorChange, VisibleAnywhere)
	FLinearColor TeamColor;

public:

	FORCEINLINE int GetTeamOwner() const { return teamOwner; };
};
