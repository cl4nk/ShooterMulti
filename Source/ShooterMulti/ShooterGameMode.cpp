// Fill out your copyright notice in the Description page of Project Settings.

#include "ShooterMulti.h"
#include "ShooterGameMode.h"
#include "UndeadCharacter.h"
#include "CharacterWithHealth.h"

AShooterGameMode::AShooterGameMode() :
	Super()
{
}

void AShooterGameMode::BeginPlay()
{
	AUndeadCharacter::PunchEvent.Clear();
	AUndeadCharacter::DeathEvent.Clear();
	ACharacterWithHealth::DeathEvent.Clear();

	Deaths = 0;
	Kills = 0;

	DeathEventHandle = ACharacterWithHealth::DeathEvent.AddLambda([this](ACharacterWithHealth* charac, AActor* Causer)
	{
		AddDeath();
	});
}

void AShooterGameMode::Destroyed()
{
	ACharacterWithHealth::DeathEvent.Remove(DeathEventHandle);
}

void AShooterGameMode::Respawn()
{
	APlayerController* playerController = GetWorld()->GetFirstPlayerController();
	AActor* playerStart = ChoosePlayerStart(playerController);
	RestartPlayerAtPlayerStart(playerController, playerStart);
}

void AShooterGameMode::AddKill()
{
	++Kills;
}

void AShooterGameMode::AddDeath()
{
	++Deaths;
}

void AShooterGameMode::Quit()
{
	UKismetSystemLibrary::QuitGame(GetWorld(), nullptr, EQuitPreference::Quit);
}
