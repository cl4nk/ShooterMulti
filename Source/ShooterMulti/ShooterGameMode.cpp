// Fill out your copyright notice in the Description page of Project Settings.

#include "ShooterMulti.h"
#include "ShooterGameMode.h"
#include "UndeadCharacter.h"
#include "ShooterCharacter.h"

AShooterGameMode::AShooterGameMode() :
	Super()
{
	
}

void AShooterGameMode::BeginPlay()
{
	AUndeadCharacter::PunchEvent.Clear();
	AUndeadCharacter::DeathEvent.Clear();
	AShooterCharacter::DeathEvent.Clear();

	Deaths = 0;
	Kills = 0;

	KillEventHandle = AUndeadCharacter::DeathEvent.AddLambda([this](AUndeadCharacter* charac) { AddKill(); });
	DeathEventHandle = AShooterCharacter::DeathEvent.AddLambda([this](AShooterCharacter* charac) { AddDeath(); });
}

void AShooterGameMode::Destroyed()
{
	AShooterCharacter::DeathEvent.Remove(DeathEventHandle);
	AUndeadCharacter::DeathEvent.Remove(KillEventHandle);
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