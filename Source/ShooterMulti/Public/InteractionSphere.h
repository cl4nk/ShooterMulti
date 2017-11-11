// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/SphereComponent.h"
#include "InteractionSphere.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FInteraction, AShooterCharacter*, Character);

/**
 * 
 */
UCLASS()

class SHOOTERMULTI_API UInteractionSphere : public USphereComponent
{
GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadOnly)
	class AInteractiveObject* CurrentObject;

	void InteractInput();

protected:
	void BeginPlay() override;

	UFUNCTION()
	void OnBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,
	                    int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,
	                  int32 OtherBodyIndex);

	void Interact();

	UFUNCTION(Server, Reliable, WithValidation)
	void ServerInteract();
};
