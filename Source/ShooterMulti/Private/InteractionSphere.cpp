// Fill out your copyright notice in the Description page of Project Settings.

#include "ShooterMulti.h"
#include "InteractionSphere.h"
#include "InteractiveObject.h"
#include "ShooterCharacter.h"

void UInteractionSphere::InteractInput()
{
	if (CurrentObject)
		Interact();
}

void UInteractionSphere::BeginPlay()
{
	Super::BeginPlay();

	OnComponentBeginOverlap.AddUniqueDynamic(this, &UInteractionSphere::OnBeginOverlap);
	OnComponentEndOverlap.AddUniqueDynamic(this, &UInteractionSphere::OnEndOverlap);
}

void UInteractionSphere::OnBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
                                        UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep,
                                        const FHitResult& SweepResult)
{
	if (AInteractiveObject* Object = Cast<AInteractiveObject>(OtherActor))
	{
		CurrentObject = Object;
		CurrentObject->OnSelectableEnter(GetOwner());
	}
}

void UInteractionSphere::OnEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
                                      UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (OtherActor == CurrentObject)
	{
		CurrentObject->OnSelectableExit(GetOwner());
		CurrentObject = nullptr;
	}
}

void UInteractionSphere::Interact()
{
	UE_LOG(LogTemp, Warning, TEXT("Try interaction"));

	ServerInteract();
}

bool UInteractionSphere::ServerInteract_Validate()
{
	return true;
}

void UInteractionSphere::ServerInteract_Implementation()
{
	AShooterCharacter* Character = Cast<AShooterCharacter>(GetOwner());
	CurrentObject->ReceiveInteraction(Character);
}
