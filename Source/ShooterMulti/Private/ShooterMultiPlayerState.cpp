// Fill out your copyright notice in the Description page of Project Settings.

#include "ShooterMulti.h"
#include "ShooterMultiPlayerState.h"
#include "Net/UnrealNetwork.h"
#include "ShooterMultiGameMode.h"
#include "ShooterMultiGameState.h"

AShooterMultiPlayerState::FBoolEvent AShooterMultiPlayerState::OnReady;

AShooterMultiPlayerState::AShooterMultiPlayerState()
	: Super()
	  , m_bIsAlive(true)
	  , m_teamId(-1)
	  , Deaths(0)
	  , Kills(0)
	  , bIsReady(false)
{
}

void AShooterMultiPlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AShooterMultiPlayerState, m_bIsAlive);
	DOREPLIFETIME(AShooterMultiPlayerState, m_teamId);
	DOREPLIFETIME(AShooterMultiPlayerState, Deaths);
	DOREPLIFETIME(AShooterMultiPlayerState, Kills);
	DOREPLIFETIME(AShooterMultiPlayerState, bIsReady);
}

void AShooterMultiPlayerState::SetAlive(const bool bIsAlive)
{
	if (Role == ROLE_Authority)
	{
		m_bIsAlive = bIsAlive;
	}
}

bool AShooterMultiPlayerState::IsAlive() const
{
	return m_bIsAlive;
}

void AShooterMultiPlayerState::RequestRespawn() const
{
	if (Role == ROLE_Authority)
	{
		AShooterMultiGameMode* gameMode = GetShooterGameMode();
		gameMode->Respawn(this);
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Client-side should not be able to ask for Respawn"));
	}
}

#pragma region Team Management
void AShooterMultiPlayerState::RequestTeamChange(const int teamId)
{
	AShooterMultiGameState* gameState = GetWorld()->GetGameState<AShooterMultiGameState>();
	if (!gameState || !gameState->CanChangeTeam())
	{
		UE_LOG(LogTemp, Warning, TEXT("[%s] is changing teams ! Can't go on team [%d], STAYING on team [%d]"), *GetName(),
			teamId, m_teamId)
		;
		return;
	}

	UE_LOG(LogTemp, Warning, TEXT("[%s] is changing teams ! Going on team [%d]"), *GetName(), teamId);

	Server_RequestTeamChange(teamId);
}

void AShooterMultiPlayerState::Server_RequestTeamChange_Implementation(const int teamId)
{
	AShooterMultiGameState* gameState = GetWorld()->GetGameState<AShooterMultiGameState>();
	if (!gameState)
		return;

	gameState->OnPlayerSetTeam(teamId, m_teamId);
	m_teamId = teamId;

	GetShooterGameMode()->DestroyPawnOfPlayerState(this);
}

bool AShooterMultiPlayerState::Server_RequestTeamChange_Validate(const int teamId)
{
	return true;
}

int AShooterMultiPlayerState::GetTeamID() const
{
	return m_teamId;
}
#pragma endregion

void AShooterMultiPlayerState::RequestReady(bool ready)
{
	Server_RequestReady(ready);
}

void AShooterMultiPlayerState::Server_RequestReady_Implementation(bool ready)
{
	bIsReady = ready;
	OnReady.Broadcast(bIsReady);
}

bool AShooterMultiPlayerState::Server_RequestReady_Validate(bool ready)
{
	return true;
}

AShooterMultiGameMode* AShooterMultiPlayerState::GetShooterGameMode() const
{
	UWorld* world = GetWorld();
	check(world);

	return world->GetAuthGameMode<AShooterMultiGameMode>();
}

bool AShooterMultiPlayerState::IsReady() const
{
	return bIsReady;
}

void AShooterMultiPlayerState::AddDeath()
{
	if (Role == ROLE_Authority)
		Deaths++;
}

void AShooterMultiPlayerState::AddKill()
{
	if (Role == ROLE_Authority)
		Kills++;
}

int AShooterMultiPlayerState::GetDeaths() const
{
	return Deaths;
}

int AShooterMultiPlayerState::GetKills() const
{
	return Kills;
}

void AShooterMultiPlayerState::ResetStats()
{
	if (Role == ROLE_Authority)
	{
		Deaths = 0;
		Kills = 0;
	}
}

void AShooterMultiPlayerState::CopyProperties(class APlayerState* PlayerState)
{
	Super::CopyProperties(PlayerState);

	if (PlayerState)
	{
		AShooterMultiPlayerState* newPlayerState = Cast<AShooterMultiPlayerState>(PlayerState);

		if (newPlayerState)
		{
			newPlayerState->Deaths = Deaths;
			newPlayerState->Kills = Kills;
			newPlayerState->m_teamId = m_teamId;
		}
	}
}

void AShooterMultiPlayerState::OverrideWith(class APlayerState* PlayerState)
{
	Super::OverrideWith(PlayerState);

	if (PlayerState)
	{
		AShooterMultiPlayerState* OldPlayerState = Cast<AShooterMultiPlayerState>(PlayerState);

		if (OldPlayerState)
		{
			Deaths = OldPlayerState->Deaths;
			Kills = OldPlayerState->Kills;
			m_teamId = OldPlayerState->m_teamId;
		}
	}
}
