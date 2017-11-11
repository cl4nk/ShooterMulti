// Fill out your copyright notice in the Description page of Project Settings.

#include "ShooterMulti.h"
#include "SpawnerTeamSwitch.h"
#include "UndeadCharacter.h"
#include "UndeadDirector.h"


void ASpawnerTeamSwitch::Destroyed()
{
	if (Role == ROLE_Authority)
		GetWorld()->GetTimerManager().ClearTimer(OwningTimeHandle);
}

void ASpawnerTeamSwitch::OnReceiveInteraction_Implementation(AShooterCharacter* Character)
{
	Super::OnReceiveInteraction_Implementation(Character);

	GetWorld()->GetTimerManager().SetTimer(OwningTimeHandle, this, &ASpawnerTeamSwitch::ResetSwitch, OwnerTime, false);
}

void ASpawnerTeamSwitch::Spawn()
{
	if (Role != ROLE_Authority)
		return;

	FActorSpawnParameters spawnParameters;
	spawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
	AUndeadCharacter* undead = GetWorld()->SpawnActor<AUndeadCharacter>(
		UndeadBlueprint,
		GetActorLocation() + GetActorForwardVector() * SpawnDistance,
		GetActorRotation(),
		spawnParameters);

	if (undead)
	{
		AUndeadDirector::GetCurrent()->OnUndeadSpawn(undead);
		undead->ChangeTeam(teamOwner);
	}
}
