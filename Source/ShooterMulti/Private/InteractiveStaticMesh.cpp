// Fill out your copyright notice in the Description page of Project Settings.

#include "ShooterMulti.h"
#include "InteractiveStaticMesh.h"

AInteractiveStaticMesh::AInteractiveStaticMesh()
{
	StaticMesh = CreateDefaultSubobject<UStaticMeshComponent>("StaticMeshRoot");
	RootComponent = StaticMesh;
}

void AInteractiveStaticMesh::OnSelectableEnter_Implementation(AActor* Actor)
{
	Super::OnSelectableEnter_Implementation(Actor);

	ACharacter* charac = UGameplayStatics::GetPlayerCharacter(GetWorld(), 0);

	if (Actor == charac && StaticMesh)
		StaticMesh->SetRenderCustomDepth(true);
}

void AInteractiveStaticMesh::OnSelectableExit_Implementation(AActor* Actor)
{
	Super::OnSelectableEnter_Implementation(Actor);

	ACharacter* charac = UGameplayStatics::GetPlayerCharacter(GetWorld(), 0);

	if (Actor == charac && StaticMesh)
		StaticMesh->SetRenderCustomDepth(false);
}
