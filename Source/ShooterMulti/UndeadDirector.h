// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "UndeadCharacter.h"
#include "UndeadDirector.generated.h"

class ASpawnerTeamSwitch;

UCLASS()

class SHOOTERMULTI_API AUndeadDirector : public AActor
{
GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere, BlueprintInternalUseOnly, Category = Director)
	TArray<ASpawnerTeamSwitch*> SpawnPoints;

	UPROPERTY(VisibleAnywhere, Category = Director)
	TArray<AUndeadCharacter*> UndeadCharacterList;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Director)
	int MaxUndeadCount = 15;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Director)
	int BaseUndeadCount = 15;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Director)
	int PerPlayerUndeadCount = 7;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Director)
	int MaxPunchPerSecond = 2;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Director, meta=(ClampMin = 0.1f))
	float SecondPerSpawn = 5.f;

	// Sets default values for this actor's properties
	AUndeadDirector();

	// Called when the game starts or when spawned
	void BeginPlay() override;

	void Destroyed() override;

	// Called every frame
	void Tick(float DeltaSeconds) override;


	UFUNCTION(BlueprintPure, Category = Director)
	static AUndeadDirector* GetCurrent();

	UFUNCTION(BlueprintPure, Category = Director)
	bool SpawnAvailable();

	UFUNCTION(BlueprintPure, Category = Director)
	bool PunchAvailable();

	void OnUndeadPunch(AUndeadCharacter* instigator);

	void OnUndeadSpawn(AUndeadCharacter* Char);
	void OnUndeadDeath(AUndeadCharacter* Char);

	void SpawnEnemy();


protected:

	UFUNCTION()
	void HandleGameState(EShooterMultiState state);

	void StartSpawn();
	void StopSpawn();

	static AUndeadDirector* Current;

	FTimerHandle SpawnTimerHandle;

	TArray<float> PunchTimers;

	FDelegateHandle PunchEventHandle;
	FDelegateHandle DeathEventHandle;
};
