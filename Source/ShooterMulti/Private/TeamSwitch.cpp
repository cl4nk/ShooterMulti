// Fill out your copyright notice in the Description page of Project Settings.

#include "ShooterMulti.h"
#include "TeamSwitch.h"
#include "ShooterCharacter.h"
#include "ShooterMultiGameState.h"
#include "Net/UnrealNetwork.h"


// Sets default values
ATeamSwitch::ATeamSwitch()
{
	bReplicates = true;

	TeamColor = FLinearColor::White;
}

void ATeamSwitch::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	// Replicate to everyone
	DOREPLIFETIME(ATeamSwitch, TeamColor);
}

void ATeamSwitch::BeginPlay()
{
	Super::BeginPlay();
	if (MaterialIndex > 0)
	{
		DynamicMat = UMaterialInstanceDynamic::Create(StaticMesh->GetMaterial(MaterialIndex), this);
		StaticMesh->SetMaterial(MaterialIndex, DynamicMat);
		DynamicMat->SetVectorParameterValue(ColorParameterName, TeamColor);
	}
}

void ATeamSwitch::OnRep_ColorChange()
{
	if (DynamicMat)
		DynamicMat->SetVectorParameterValue(ColorParameterName, TeamColor);
}

void ATeamSwitch::OnReceiveInteraction_Implementation(AShooterCharacter* Character)
{
	Super::OnReceiveInteraction_Implementation(Character);
	SetTeamID(Character->GetTeamID());
}

void ATeamSwitch::SetTeamID(const int id)
{
	if (Role == ROLE_Authority)
	{
		teamOwner = id;
		AShooterMultiGameState* gameState = GetWorld()->GetGameState<AShooterMultiGameState>();
		if (gameState)
		{
			TeamColor = gameState->GetTeamColor(teamOwner);
			OnRep_ColorChange();
		}
	}
}

void ATeamSwitch::ResetSwitch()
{
	if (Role == ROLE_Authority)
		SetTeamID(-1);
}
