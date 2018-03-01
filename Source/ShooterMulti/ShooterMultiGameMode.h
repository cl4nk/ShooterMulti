// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/GameMode.h"
#include "EnumContainer.h"
#include "ShooterMultiPlayerStart.h"

class AShooterMultiGameState;
class AShooterMultiPlayerState;
class AShooterPlayerController;
class ACharacterWithHealth;

#include "ShooterMultiGameMode.generated.h"

/**
 * 
 */
UCLASS()

class SHOOTERMULTI_API AShooterMultiGameMode : public AGameModeBase
{
GENERATED_BODY()

public:
	AShooterMultiGameMode();

//#pragma region AGameModeBase
	void BeginPlay() override;
	void Destroyed() override;

	void GenericPlayerInitialization(AController* NewController) override;

	void Logout(AController* OldPlayer) override;

	APawn* SpawnDefaultPawnFor_Implementation(AController* NewPlayer, AActor* StartSpot) override;

protected:
	FString InitNewPlayer(APlayerController* NewPlayerController, const FUniqueNetIdRepl& UniqueId,
	                      const FString& Options, const FString& Portal) override;
//#pragma endregion

//#pragma region PlayerStart choice
public:
	AActor* FindPlayerStart_Implementation(AController* Player, const FString& IncomingName) override;

protected:
	TArray<AShooterMultiPlayerStart *> GetAllPlayerStartsForTeam(const int teamID) const;

	void OnPlayerReady();

	void SetAllPlayersReady (bool value);

	bool AreAllPlayersReady() const;
//#pragma endregion

public:
	/**
	 * \brief Respawns every player in game
	 */
	void RespawnEveryPlayer();

	/**
	 * \brief Get the controller of a player using its PlayerState
	 * \param playerState _IN_ The PlayerState of the controller
	 * \return The controller of the PlayerState, null if an error occured
	 */
	APlayerController* GetController( const APlayerState* playerState ) const;

	/**
	 * \brief Respawns a player
	 * \param controller _IN_ The controller of the player that needs to be respawned
	 */
	void Respawn( APlayerController* controller );

	/**
	 * \brief Respawns a player
	 * \param playerState _IN_ The player state of the player that needs to be respawned
	 */
	void Respawn( const APlayerState* playerState );

	/**
	 * \brief Destroy and unpossess the playerController's APawn
	 * \param playerState _IN_ The PlayerState of the player
	 */
	void DestroyPawnOfPlayerState( const APlayerState* playerState ) const;

	/**
	 * \brief Destroy and unpossess the playerController's APawn
	 * \param playerController _IN_ The PlayerController of the player
	 */
	void DestroyPawnOfController( APlayerController* playerController ) const;

protected:
	/**
	 * \brief Autofill playerState in teams
	 */
	void AutofillTeam( AShooterMultiPlayerState* playerState ) const;

	/**
	 * \brief Function called when a character dies
	 * \param charac _IN_ The character that died
	 */
	void AddDeath( ACharacterWithHealth* charac, AActor* Causer );

	/**
	 * \brief Set if need to ignore move and/or look inputs
	 * \param moveInput _IN_ Has to block Move Input
	 * \param lookInput _IN_ Has to block Look Input
	 */
	void SetIgnoreMoveLookInput( bool moveInput, bool lookInput );

	void SetIgnoreAllInputs( bool bIgnoreInputs );
	
	void SetRemoveAllWidgetBeforeServerTravel();

//#pragma region State / Flow
public:
	/**
	 * \brief Changes state of the game
	 * \param shooterMultiState _IN_ The new state
	 */
	void ChangeState( const EShooterMultiState shooterMultiState );

	UFUNCTION(BlueprintCallable)
	bool StartMatch();

protected:
	void OnChangeStateBeforeStart();
	void OnChangeStateCountdown();
	void OnChangeStatePlaying();
	void OnChangeStateEndGame();
//#pragma endregion

protected:
	/**
	 * \brief Score needed to win
	 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Game Configuration")
	int32 ScoreToWin;

	/**
	 * \brief Number of teams
	 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Game Configuration")
	int32 NumberOfTeams;

	/**
	 * \brief Time of the timer before starting game
	 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Game Flow", meta = (ClampMin = "0.0"))
	float CountdownTime;

	/**
	 * \brief List of every playerstart in the world
	 */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Game World")
	TArray<AShooterMultiPlayerStart *> ListPlayerStarts;

	/**
	 * \brief Get the casted GameState
	 * \return The GameState only, will check validity
	 */
	AShooterMultiGameState* GetShooterGameState() const;

	/**
	 * \brief Parse the OptionsString and set values coming from it
	 */
	void ParseOptionsString();

private:

	FTimerHandle TimeBeforeSpawn;

	FDelegateHandle DeathEventHandle;

	FDelegateHandle ReadyCountHandle;
};
