// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "ShooterMultiGameMode.h"

class AShooterPlayerController;

#include "ShooterMultiPlayerState.generated.h"

/**
 * 
 */
UCLASS()

class SHOOTERMULTI_API AShooterMultiPlayerState : public APlayerState
{
GENERATED_BODY()

public:
	AShooterMultiPlayerState();

	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	/**
	 * \brief Sets if this player is alive
	 * \param isAlive _IN_ If player is alive or not
	 * \note Needs authority
	 */
	UFUNCTION(BlueprintCallable, Category = "Player's Request")
	void SetAlive(const bool isAlive);

	/**
	 * \brief Tells wether player's alive or not
	 * \return True if player's alive, false otherwise
	 */
	UFUNCTION(BlueprintCallable, Category = "State")
	bool IsAlive() const;

	UFUNCTION(BlueprintCallable, Category = "State")
	bool IsReady() const;

	UFUNCTION(BlueprintCallable, Category = "State")
	void AddDeath();

	UFUNCTION(BlueprintCallable, Category = "State")
	void AddKill();

	UFUNCTION(BlueprintCallable, Category = "State")
	int GetDeaths() const;

	UFUNCTION(BlueprintCallable, Category = "State")
	int GetKills() const;

	void ResetStats();

	// Used to copy properties from the current PlayerState to the passed one
	void CopyProperties(class APlayerState* PlayerState) override;

	// Used to override the current PlayerState with the properties of the passed one
	void OverrideWith(class APlayerState* PlayerState) override;

	/**
	 * \brief Request a respawn for controller
	 * \note Needs authority
	 */
	UFUNCTION(BlueprintCallable, Category = "Player's Request")
	void RequestRespawn() const;

//#pragma region Team Management
	/**
	 * \brief Request to change team for controller
	 * \param teamId _IN_ The team in which the player ask to join
	 * \note Needs authority
	 */
	UFUNCTION(BlueprintCallable, Category = "Player's Request")
	void RequestTeamChange(const int teamId);

	UFUNCTION(Server, Reliable, WithValidation, BlueprintCallable, Category = "Player's Request")
	void Server_RequestTeamChange(const int teamId);
	void Server_RequestTeamChange_Implementation(const int teamId);
	bool Server_RequestTeamChange_Validate(const int teamId);

	/**
	 * \brief Get the team ID of the player
	 * \return The team ID
	 */
	UFUNCTION(BlueprintCallable, Category = "State")
	int GetTeamID() const;
//#pragma endregion

	UFUNCTION(BlueprintCallable, Category = "Player's Request")
	void RequestReady(bool ready);

	UFUNCTION(Server, Reliable, WithValidation, BlueprintCallable, Category = "Player's Request")
	void Server_RequestReady(bool ready);
	void Server_RequestReady_Implementation(bool ready);
	bool Server_RequestReady_Validate(bool ready);

	DECLARE_EVENT_OneParam(AShooterMultiPlayerState, FBoolEvent, bool)

	static FBoolEvent OnReady;

protected:
	/**
	 * \brief Get the ShooterMultiGameMode of the level
	 * \return The ShooterMultiGameMode, null if there is none
	 */
	UFUNCTION(BlueprintCallable, Category = "PlayerState")
	AShooterMultiGameMode* GetShooterGameMode() const;

	/**
	 * \brief If player is alive
	 */
	UPROPERTY(VisibleAnywhere, Replicated, BlueprintReadOnly, Category = "State")
	bool m_bIsAlive;

	/**
	 * \brief The team ID of this player
	 */
	UPROPERTY(VisibleAnywhere, Replicated, BlueprintReadOnly, Category = "State")
	int m_teamId;

	UPROPERTY(VisibleAnywhere, Replicated, BlueprintReadOnly, Category = "State")
	int Deaths;

	UPROPERTY(VisibleAnywhere, Replicated, BlueprintReadOnly, Category = "State")
	int Kills;

	UPROPERTY(VisibleAnywhere, Replicated, BlueprintReadOnly, Category = "State")
	bool bIsReady;
};
