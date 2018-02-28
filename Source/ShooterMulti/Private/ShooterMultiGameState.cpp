// Fill out your copyright notice in the Description page of Project Settings.

#include "ShooterMulti.h"
#include "ShooterMultiGameState.h"
#include "Net/UnrealNetwork.h"


AShooterMultiGameState::AShooterMultiGameState()
	: Super()
	  , CurrentState(EShooterMultiState::SMS_Unknown)
	  , bGameHasBeenWon(false)
	  , TimeSinceGameStart(0)
	  , TimeBeforeGameStartReplication(0)
	  , ScoresPerTeam()
	  , DeathsPerTeam()
	  , KillsPerTeam()
	  , NbTeams(0)
	  , ScoreToWin(0)
	  , PlayersPerTeam()
	  , bCanChangeTeamWhilePlaying(true)
{
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = false;
}

void AShooterMultiGameState::Tick(float deltaSeconds)
{
	Super::Tick(deltaSeconds);

	TimeSinceGameStart += deltaSeconds;

	if (Role == ROLE_Authority)
	{
		TimeBeforeGameStartReplication -= deltaSeconds;
		if (TimeBeforeGameStartReplication <= 0.f)
		{
			TimeBeforeGameStartReplication += TimeBetweenReplication;
			NetMulticast_ReplicateTimeSinceGameStart(TimeSinceGameStart);
		}
	}
}

void AShooterMultiGameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME( AShooterMultiGameState, CurrentState );
	DOREPLIFETIME( AShooterMultiGameState, bGameHasBeenWon );

	DOREPLIFETIME( AShooterMultiGameState, ScoresPerTeam );
	DOREPLIFETIME( AShooterMultiGameState, DeathsPerTeam );
	DOREPLIFETIME( AShooterMultiGameState, KillsPerTeam );

	DOREPLIFETIME( AShooterMultiGameState, NbTeams );
	DOREPLIFETIME( AShooterMultiGameState, ScoreToWin );

	DOREPLIFETIME( AShooterMultiGameState, PlayersPerTeam );

	DOREPLIFETIME( AShooterMultiGameState, bCanChangeTeamWhilePlaying );
}

void AShooterMultiGameState::SetState(const EShooterMultiState newState)
{
	if (Role == ROLE_Authority && newState != CurrentState)
	{
		CurrentState = newState;
		OnRep_State();
	}
}

//#pragma region Game Flow

void AShooterMultiGameState::OnRep_State()
{
	OnStateChange.Broadcast(CurrentState);
}

EShooterMultiState AShooterMultiGameState::GetCurrentState() const
{
	return CurrentState;
}

bool AShooterMultiGameState::CanChangeTeam() const
{
	if (CurrentState == EShooterMultiState::SMS_Playing)
		return CanChangeTeamWhilePlaying();
	return true;
}

bool AShooterMultiGameState::HasGameBeenWon() const
{
	return bGameHasBeenWon;
}

bool AShooterMultiGameState::IsDraw() const
{
	if (bGameHasBeenWon == false)
	{
		UE_LOG( LogTemp, Warning, TEXT("AShooterMultiGameState::IsDraw() - called without game being won") );
		return false;
	}

	int8 nbWinners = 0;

	for (int32 oneScore : ScoresPerTeam)
	{
		if (oneScore >= ScoreToWin)
		{
			++nbWinners;
			if (nbWinners > 1)
				return true;
		}
	}
	return false;
}

int AShooterMultiGameState::GetMaxTeamCount() const
{
	return ColorList.Num();
}


//#pragma region Time Since Game Start
float AShooterMultiGameState::GetTimeSinceGameStart() const
{
	return TimeSinceGameStart;
}

void AShooterMultiGameState::StartTicking()
{
	NetMulticast_StartTicking();
}

void AShooterMultiGameState::NetMulticast_StartTicking_Implementation()
{
	SetActorTickEnabled(true);

	TimeBeforeGameStartReplication = TimeBetweenReplication;
}

bool AShooterMultiGameState::NetMulticast_StartTicking_Validate()
{
	return true;
}

void AShooterMultiGameState::NetMulticast_ReplicateTimeSinceGameStart_Implementation(
	const float replicatedTimeSinceGameStart)
{
	if (Role != ROLE_Authority)
	{
		// Supposing if bigger, latency is low enough
		if (TimeSinceGameStart < replicatedTimeSinceGameStart)
			TimeSinceGameStart = replicatedTimeSinceGameStart;
	}
}

bool AShooterMultiGameState::NetMulticast_ReplicateTimeSinceGameStart_Validate(
	const float replicatedTimeSinceGameStart)
{
	return true;
}
//#pragma endregion

//#pragma endregion

//#pragma region Scores
void AShooterMultiGameState::NotifyKill(const int32 killerTeamId, const FString& killerName, const int32 victimTeamId,
                                        const FString& victimName)
{
	if (Role == ROLE_Authority)
	{
		if (CheckIfTeamIdIsValid(killerTeamId))
		{
			++(KillsPerTeam[killerTeamId]);
			++(ScoresPerTeam[killerTeamId]);
			if (ScoresPerTeam[killerTeamId] >= ScoreToWin)
				bGameHasBeenWon = true;
		}
		else if (CheckIfTeamIdIsValid(victimTeamId))
		{
			ScoresPerTeam[victimTeamId] = ScoresPerTeam[victimTeamId] == 0 ? 0 : ScoresPerTeam[victimTeamId] - 1;
		}

		if (CheckIfTeamIdIsValid(victimTeamId))
			++(DeathsPerTeam[victimTeamId]);


		NetMulticast_OnKill(killerTeamId, killerName, victimTeamId, victimName);
	}
}

void AShooterMultiGameState::SetScoreToWin(const int32 scoreToWin)
{
	if (Role == ROLE_Authority)
	{
		ScoreToWin = scoreToWin;
	}
}

int32 AShooterMultiGameState::GetScoreToWin() const
{
	return ScoreToWin;
}

int32 AShooterMultiGameState::GetScoreOfTeam(const int32 teamId) const
{
	if (!CheckIfTeamIdIsValid(teamId))
	{
		UE_LOG( LogTemp, Warning, TEXT("AShooterMultiGameState::GetScoreOfTeam() - bad teamId value [%d]"), teamId );
		return -1;
	}

	return ScoresPerTeam[teamId];
}

int32 AShooterMultiGameState::GetDeathsOfTeam(const int32 teamId) const
{
	if (!CheckIfTeamIdIsValid(teamId))
	{
		UE_LOG( LogTemp, Warning, TEXT("AShooterMultiGameState::GetDeathsOfTeam() - bad teamId value [%d]"), teamId );
		return -1;
	}

	return DeathsPerTeam[teamId];
}

int32 AShooterMultiGameState::GetKillsOfTeam(const int32 teamId) const
{
	if (!CheckIfTeamIdIsValid(teamId))
	{
		UE_LOG( LogTemp, Warning, TEXT("AShooterMultiGameState::GetScoreOfTeam() - bad teamId value [%d]"), teamId );
		return -1;
	}

	return KillsPerTeam[teamId];
}

void AShooterMultiGameState::SetNumberOfTeams(const int32 newNbTeams)
{
	if (Role == ROLE_Authority)
	{
		NbTeams = newNbTeams;
		ScoresPerTeam.SetNumZeroed(NbTeams);
		KillsPerTeam.SetNumZeroed(NbTeams);
		DeathsPerTeam.SetNumZeroed(NbTeams);
		PlayersPerTeam.SetNumZeroed(NbTeams);
	}
}

int32 AShooterMultiGameState::GetNumberOfTeams() const
{
	return NbTeams;
}

bool AShooterMultiGameState::CheckIfTeamIdIsValid(const int32 teamId) const
{
	return teamId >= 0 && teamId < NbTeams;
}

void AShooterMultiGameState::NetMulticast_OnKill_Implementation(const int32 killerTeamId, const FString& killerName,
                                                                const int32 victimTeamId, const FString& victimName)
{
	OnKill.Broadcast(killerTeamId, killerName, victimTeamId, victimName);
}

bool AShooterMultiGameState::NetMulticast_OnKill_Validate(const int32 killerTeamId, const FString& killerName,
                                                          const int32 victimTeamId, const FString& victimName)
{
	return true;
}

//#pragma endregion

//#pragma region Teams
int32 AShooterMultiGameState::GetTeamAutofill() const
{
	return PlayersPerTeam.Find(FMath::Min(PlayersPerTeam));
}

void AShooterMultiGameState::OnPlayerSetTeam(const int32 teamId, const int32 oldTeamId)
{
	if (!CheckIfTeamIdIsValid(teamId))
	{
		UE_LOG( LogTemp, Warning, TEXT("AShooterMultiGameState::OnPlayerAddedToTeam() - bad teamId value [%d]"), teamId );
		return;
	}

	if (teamId == oldTeamId)
		UE_LOG( LogTemp, Warning, TEXT("AShooterMultiGameState::OnPlayerAddedToTeam() - new and old team id are the same") );

	++(PlayersPerTeam[teamId]);

	if (CheckIfTeamIdIsValid(oldTeamId))
		--(PlayersPerTeam[teamId]);
}

void AShooterMultiGameState::OnPlayerLeaveTeam( const int32 teamId )
{
	if ( !CheckIfTeamIdIsValid( teamId ) )
	{
		UE_LOG( LogTemp, Warning, TEXT("AShooterMultiGameState::OnPlayerLeaveTeam() - bad teamId value [%d]"), teamId );
		return;
	}

	--(PlayersPerTeam[teamId]);
}

FLinearColor AShooterMultiGameState::GetTeamColor(const int32 teamId) const
{
	if (teamId < 0 || teamId >= ColorList.Num())
		return FLinearColor::White;

	return ColorList[teamId];
}
//#pragma endregion

bool AShooterMultiGameState::CanChangeTeamWhilePlaying() const
{
	return bCanChangeTeamWhilePlaying;
}
