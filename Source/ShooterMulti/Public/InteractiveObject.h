// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "InteractiveObject.generated.h"

class AShooterCharacter;
class UMarkerComponent;


UCLASS()

class SHOOTERMULTI_API AInteractiveObject : public AActor
{
GENERATED_BODY()

public:

	AInteractiveObject();
	/**
	 * \brief Called when the skill activation starts, when key is pressed (AShooterCharacter must be )
	 */
	void ReceiveInteraction(AShooterCharacter* Character);

	UFUNCTION(BlueprintNativeEvent, Category = "Events")
	void OnReceiveInteraction(AShooterCharacter* Character);


	UFUNCTION(BlueprintNativeEvent, Category = "Events")
	void OnSelectableEnter(AActor* Actor);

	UFUNCTION(BlueprintNativeEvent, Category = "Events")
	void OnSelectableExit(AActor* Actor);

	UFUNCTION (BlueprintCallable)
	virtual FName GetObjectName() const { return ""; };

protected:

	UPROPERTY(Category = "Interactive Object", VisibleDefaultsOnly, BlueprintReadOnly)
	UMarkerComponent * MarkerComponent;

	virtual void OnReceiveInteraction_Implementation(AShooterCharacter* Character);
	virtual void OnSelectableEnter_Implementation(AActor* Actor);
	virtual void OnSelectableExit_Implementation(AActor* Actor);
};
