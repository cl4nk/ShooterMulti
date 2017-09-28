// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Character.h"
#include "CharacterWithHealth.generated.h"

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
	float TimeBeforeDisapearing = 10.f;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = Health, meta = (ClampMin = "0.0"))
	float DisapearingTime = 5.f;

	// Sets default values for this character's properties
	ACharacterWithHealth();

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintPure, Category = Health)
	float GetHealth();
	UFUNCTION(BlueprintCallable, Category = Health)
	virtual float TakeDamage(float DamageAmount, FDamageEvent const & DamageEvent, AController * EventInstigator, AActor * DamageCauser) override;
	UFUNCTION(BlueprintCallable, Category = Health)
	virtual float GainHealth(float GainAmount);
	UFUNCTION(BlueprintCallable, Category = Health)
	virtual void ResetHealth();

	UFUNCTION(BlueprintPure, Category = Health)
	bool IsDead();
	UFUNCTION(BlueprintCallable, Category = Health)
	virtual void Die();

	UFUNCTION(BlueprintCallable, Category = Health)
	virtual void StartDisapear();
	UFUNCTION()
	void UpdateDisapear();
	UFUNCTION(BlueprintCallable, Category = Health)
	virtual void FinishDisapear();
	UFUNCTION(BlueprintPure, Category = Health)
	FORCEINLINE bool IsDisapearing() { return Disapearing; }

private:
	float Health;
	float DisapearTimer;
	bool Disapearing;
	TArray<UMaterialInstanceDynamic*> DissolveMaterials;
};
