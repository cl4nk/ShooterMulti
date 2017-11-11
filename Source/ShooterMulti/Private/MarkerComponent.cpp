// Fill out your copyright notice in the Description page of Project Settings.

#include "ShooterMulti.h"
#include "MarkerComponent.h"
#include "BaseMiniMapWidget.h"



// Sets default values for this component's properties
UMarkerComponent::UMarkerComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;
}


// Called when the game starts
void UMarkerComponent::BeginPlay()
{
	Super::BeginPlay();

	if (!IsNetMode(ENetMode::NM_DedicatedServer))
		UBaseMiniMapWidget::AddActor(GetOwner());	
}

// Called when the game starts
void UMarkerComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (!IsNetMode(ENetMode::NM_DedicatedServer))
		UBaseMiniMapWidget::RemoveActor(GetOwner());
	
	Super::EndPlay(EndPlayReason);
}

