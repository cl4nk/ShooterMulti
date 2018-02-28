// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"

#include "EnumContainer.h"

#include "ShooterMultiGameState.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_FourParams(FOnKillEvent, int32, killerTeamId, FString, killerName, int32, victimTeamId, FString, victimName);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnStateChange, EShooterMultiState, state);

/**
 * 
 */
UCLASS()

class SHOOTERMULTI_API AShooterMultiGameState : public AGameStateBase
{
GENERATED_BODY()

public:
	AShooterMultiGameState();

public:
	void Tick(float deltaSeconds) override;

	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

//#pragma region Game Flow
public:
	/**
	 * \brief Set current state
	 * \param newState _IN_ State to be set
	 * \note Needs authority
	 */
	void SetState(const EShooterMultiState newState);

	UFUNCTION()
	void OnRep_State();

	/**
	 * \brief Get the current state of the game
	 * \return The current state of the game
	 */
	UFUNCTION(BlueprintCallable, Category = "Game Flow")
	EShooterMultiState GetCurrentState() const;

	/**
	 * \brief Ask if player can change team
	 * \return True if can change team, false otherwise
	 */
	UFUNCTION(BlueprintCallable, Category = "Game Flow")
	bool CanChangeTeam() const;

	/**
	 * \brief Ask if game has been won by one or more teams
	 * \return True if game has been won, false otherwise
	 */
	UFUNCTION(BlueprintCallable, Category = "Game Flow")
	bool HasGameBeenWon() const;

	/**
	 * \brief Ask if game has been won by more than one team
	 * \return True if is a draw, false otherwise
	 */
	UFUNCTION(BlueprintCallable, Category = "Game Flow")
	bool IsDraw() const;

	UFUNCTION(BlueprintCallable, Category = "Game Flow")
	int GetMaxTeamCount() const;

protected:
	/**
	 * \brief Current state of the game
	 */
	UPROPERTY(ReplicatedUsing = OnRep_State, Transient, VisibleAnywhere, BlueprintReadOnly, Category = "Game Flow")
	EShooterMultiState CurrentState;

	/**
	 * \brief If the game has been won by one team or more
	 */
	UPROPERTY(Replicated, Transient, VisibleAnywhere, BlueprintReadOnly, Category = "Game Flow")
	bool bGameHasBeenWon;

//#pragma region Time Since Game Start
public:
	/**
	 * \brief Get the current time since game start on client
	 * \return The time since game start in seconds
	 */
	UFUNCTION(BlueprintCallable, Category = "Game Flow|Time Game Start")
	float GetTimeSinceGameStart() const;

	/**
	 * \brief Make the GameState tick
	 */
	UFUNCTION(BlueprintCallable, Category = "Game Flow|Time Game Start")
	void StartTicking();

protected:
	/**
	 * \brief Time since the game has started
	 */
	UPROPERTY(Transient, VisibleAnywhere, BlueprintReadOnly, Category = "Game Flow|Time Game Start")
	float TimeSinceGameStart;

	/**
	 * \brief Time before the TimeSinceGameStart self-replication
	 */
	UPROPERTY(Transient, VisibleAnywhere, BlueprintReadOnly, Category = "Game Flow|Time Game Start")
	float TimeBeforeGameStartReplication;

	/**
	 * \brief Time between two TimeSinceGameStart self-replication
	 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Game Flow|Time Game Start")
	float TimeBetweenReplication = 5.f;

	/**
	 * \brief Sets every GameState to tick
	 */
	UFUNCTION(NetMulticast, Reliable, WithValidation, Category = "Game Flow|Time Game Start")
	void NetMulticast_StartTicking();
	void NetMulticast_StartTicking_Implementation();
	bool NetMulticast_StartTicking_Validate();

	/**
	 * \brief Replicates TimeSinceGameStart to clients
	 * \param replicatedTimeSinceGameStart _IN_ The replicated server value
	 */
	UFUNCTION(NetMulticast, Reliable, WithValidation, Category = "Game Flow|Time Game Start")
	void NetMulticast_ReplicateTimeSinceGameStart(const float replicatedTimeSinceGameStart);
	void NetMulticast_ReplicateTimeSinceGameStart_Implementation(const float replicatedTimeSinceGameStart);
	bool NetMulticast_ReplicateTimeSinceGameStart_Validate(const float replicatedTimeSinceGameStart);
//#pragma endregion

//#pragma endregion

//#pragma region Scores
public:
	/**
	 * \brief Notify GameState a player has died
	 * \param killerTeamId _IN_ The team of the player that has killedz
	 * \param killerName _IN_ The name of the player that has killed
	 * \param victimTeamId _IN_ The team of the player that has been killed
	 * \param victimName _IN_ The name of the player that has been killed
	 * \note Needs authority
	 */
	UFUNCTION(BlueprintCallable, Category = "Score")
	void NotifyKill(const int32 killerTeamId, const FString& killerName, const int32 victimTeamId,
	                const FString& victimName);

	/**
	 * \brief Sets the score needed to win
	 * \param scoreToWin _IN_ Score to be set
	 * \note Needs authority
	 */
	UFUNCTION(BlueprintCallable, Category = "Score")
	void SetScoreToWin(const int32 scoreToWin);

	/**
	 * \brief Gets the score needed to win
	 * \return The score to win
	 */
	UFUNCTION(BlueprintCallable, Category = "Score")
	int32 GetScoreToWin() const;

	/**
	 * \param teamId _IN_ The team id
	 * \return The score of the team \param teamId
	 */
	UFUNCTION(BlueprintCallable, Category = "Score")
	int32 GetScoreOfTeam(const int32 teamId) const;

	/**
	 * \param teamId _IN_ The team id
	 * \return The death count of the team \param teamId
	 */
	UFUNCTION(BlueprintCallable, Category = "Score")
	int32 GetDeathsOfTeam(const int32 teamId) const;

	/**
	 * \param teamId _IN_ The team id
	 * \return The kill count of the team \param teamId
	 */
	UFUNCTION(BlueprintCallable, Category = "Score")
	int32 GetKillsOfTeam(const int32 teamId) const;

	/**
	 * \brief Set the new number of teams
	 * \param newNbTeams _IN_ New number of teams
	 * \note Needs authority
	 */
	UFUNCTION(BlueprintCallable, Category = "Score")
	void SetNumberOfTeams(const int32 newNbTeams);

	/**
	 * \brief Get the number of team of this gameplay
	 * \return The number of teams of this GameState
	 */
	UFUNCTION(BlueprintCallable, Category = "Score")
	int32 GetNumberOfTeams() const;

	/**
	 * \brief Check if the id parameter is valid or not
	 * \param teamId _IN_ The teamId to be checked
	 * \return True if valid, false otherwise
	 */
	bool CheckIfTeamIdIsValid(const int32 teamId) const;

	UPROPERTY(BlueprintAssignable)
	FOnKillEvent OnKill;

	UPROPERTY(BlueprintAssignable)
	FOnStateChange OnStateChange;

protected:
	UFUNCTION(NetMulticast, Reliable, WithValidation, BlueprintCallable, Category = "Events")
	void NetMulticast_OnKill(const int32 killerTeamId, const FString& killerName, const int32 victimTeamId,
	                         const FString& victimName);
	void NetMulticast_OnKill_Implementation(const int32 killerTeamId, const FString& killerName, const int32 victimTeamId,
	                                        const FString& victimName);
	bool NetMulticast_OnKill_Validate(const int32 killerTeamId, const FString& killerName, const int32 victimTeamId,
	                                  const FString& victimName);

	/**
	 * \brief Array of every team scores
	 */
	UPROPERTY(Replicated, Transient, VisibleAnywhere, BlueprintReadWrite, Category = "Score")
	TArray<int32> ScoresPerTeam;

	/**
	 * \brief Array of every team deaths
	 */
	UPROPERTY(Replicated, Transient, VisibleAnywhere, BlueprintReadWrite, Category = "Score")
	TArray<int32> DeathsPerTeam;

	/**
	 * \brief Array of every team kills
	 */
	UPROPERTY(Replicated, Transient, VisibleAnywhere, BlueprintReadWrite, Category = "Score")
	TArray<int32> KillsPerTeam;

	/**
	 * \brief Number of teams of this GameState
	 */
	UPROPERTY(Replicated, Transient, VisibleAnywhere, BlueprintReadOnly, Category = "Score")
	int32 NbTeams;

	/**
	 * \brief Score needed to win
	 */
	UPROPERTY(Replicated, Transient, VisibleAnywhere, BlueprintReadOnly, Category = "Score")
	int32 ScoreToWin;
//#pragma endregion

//#pragma region Teams
public:
	/**
	 * \brief Get the team in which the autofill algorithm would add a new player
	 * \return The team id of the autofilled team
	 */
	int32 GetTeamAutofill() const;

	/**
	 * \brief Function called when a player is added to a team
	 * \param teamId _IN_ The team id of the new player
	 * \param oldTeamId _IN_OPT_ The old team if of the new player
	 */
	void OnPlayerSetTeam(const int32 teamId, const int32 oldTeamId = -1);

	/**
	 * \brief Function called when a player gets disconnected or other and leaves a team
	 * \param teamId _IN_ The team id of the team left by the player
	 */
	void OnPlayerLeaveTeam( const int32 teamId );

	UFUNCTION(BlueprintCallable, Category = "Team")
	FLinearColor GetTeamColor(const int32 teamId) const;

protected:
	UPROPERTY(Replicated, Transient, VisibleAnywhere, BlueprintReadOnly, Category = "Team")
	TArray<int32> PlayersPerTeam;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "status")
	TArray<FLinearColor> ColorList;
//#pragma endregion

//#pragma region Game Configuration
public:
	/**
	 * \brief Ask if players can change teams while game is playing
	 * \return True if can change team while playing, false otherwise
	 */
	UFUNCTION(BlueprintCallable, Category = "Game Configuration")
	bool CanChangeTeamWhilePlaying() const;

protected:
	/**
	 * \brief If players change team while the game is playing
	 */
	UPROPERTY(Replicated, Transient, VisibleAnywhere, BlueprintReadOnly, Category = "Game Configuration")
	bool bCanChangeTeamWhilePlaying;
//#pragma endregion
};
