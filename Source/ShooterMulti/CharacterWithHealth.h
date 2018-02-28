// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Character.h"
#include "CharacterWithHealth.generated.h"

class UMarkerComponent;
UCLASS()

class SHOOTERMULTI_API ACharacterWithHealth : public ACharacter
{
GENERATED_BODY()

public:

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = Health, meta = (ClampMin = "0.0"))
	float MaxHealth = 100.f;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = Health, meta = (ClampMin = "0.0"))
	bool IsPlayerFriendly = true;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = Health, meta = (ClampMin = "0.0"))
	float FadeInDuration = .5f;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = Health, meta = (ClampMin = "0.0"))
	float TimeBeforeDisapearing = 10.f;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = Health, meta = (ClampMin = "0.0"))
	float DisapearingTime = 5.f;

	// Sets default values for this character's properties
	ACharacterWithHealth();

	// Called when the game starts or when spawned
	void BeginPlay() override;

	// Called every frame
	void Tick(float DeltaTime) override;

	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UFUNCTION(BlueprintPure, Category = Health)
	float GetHealth();
	UFUNCTION(BlueprintCallable, Category = Health)
	float TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator,
	                 AActor* DamageCauser) override;
	UFUNCTION(BlueprintCallable, Category = Health)
	virtual float GainHealth(float GainAmount);
	UFUNCTION(BlueprintCallable, Category = Health)
	virtual void ResetHealth();

	

	UFUNCTION(BlueprintPure, Category = Health)
	bool IsDead();
	UFUNCTION(BlueprintCallable, Category = Health)
	virtual void Die(AActor* Causer);

	UFUNCTION(NetMulticast, Reliable, WithValidation)
	virtual void Netmulticast_Die();
	bool Netmulticast_Die_Validate();
	void Netmulticast_Die_Implementation();

	UFUNCTION(BlueprintCallable, Category = Health)
	virtual void StartDisapear();
	UFUNCTION()
	void UpdateDisapear();
	UFUNCTION(BlueprintCallable, Category = Health)
	virtual void FinishDisapear();
	UFUNCTION(BlueprintPure, Category = Health)
	FORCEINLINE bool IsDisapearing() { return Disapearing; }

	UFUNCTION(BlueprintCallable, Category = Feedback)
	void SetDissolveAmount(float t);

	DECLARE_EVENT_TwoParams(ACharacterWithHealth, FShooterEvent, ACharacterWithHealth*, AActor*)

	static FShooterEvent DeathEvent;

protected:

	UPROPERTY(replicated)
	float Health;

	UPROPERTY(Category = CharacterWithHealth, VisibleDefaultsOnly, BlueprintReadOnly)
	UMarkerComponent * MarkerComponent;

	float DisapearTimer;
	float FadeInTimer;
	bool FadingIn;
	bool Disapearing;
	TArray<UMaterialInstanceDynamic*> DissolveMaterials;

public:
	/**
	 * \brief Changes the team of the pawn
	 * \param newTeamId _IN_ The id of the new team of the player
	 * \note Needs authority
	 */
	void ChangeTeam(const int newTeamId);

	/**
	 * \brief Changes the color of the pawn
	 * \param Color _IN_ The color to be setted
	 * \note Will not be replicated
	 */
	UFUNCTION(BlueprintCallable, Category = "Tricky")
	void ForceColor(FLinearColor Color);

protected:

	UFUNCTION()
	void OnRep_ChangeColor();

	UPROPERTY(ReplicatedUsing = OnRep_ChangeColor, VisibleAnywhere, Category = "Status")
	FLinearColor BodyColor;

	UPROPERTY(VisibleAnywhere, Replicated, BlueprintReadOnly, Category = "Status")
	int TeamId = -1;

public:

	FORCEINLINE FLinearColor GetColor() const { return BodyColor; };

	FORCEINLINE int GetTeamID() const { return TeamId; };
};
