// Fill out your copyright notice in the Description page of Project Settings.

#include "ShooterMulti.h"
#include "InteractiveObject.h"
#include "ShooterCharacter.h"
#include "MarkerComponent.h"


AInteractiveObject::AInteractiveObject()
{
	MarkerComponent = CreateDefaultSubobject<UMarkerComponent>(TEXT("Marker Component"));
	AddOwnedComponent(MarkerComponent);
}

void AInteractiveObject::ReceiveInteraction(AShooterCharacter* Character)
{
	UE_LOG(LogTemp, Warning, TEXT("Receive Interaction"));
	if (Role == ROLE_Authority && Character)
	{
		UE_LOG(LogTemp, Warning, TEXT("Receive Interaction On Server"));
		OnReceiveInteraction(Character);
	}
}

void AInteractiveObject::OnReceiveInteraction_Implementation(AShooterCharacter* Character)
{
}

void AInteractiveObject::OnSelectableEnter_Implementation(AActor* Actor)
{
}

void AInteractiveObject::OnSelectableExit_Implementation(AActor* Actor)
{
}
