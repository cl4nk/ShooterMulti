// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "UndeadCharacter.h"
#include "UndeadDirector.generated.h"

USTRUCT()
struct FUndeadSpawnPoint
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintInternalUseOnly)
	TSubclassOf<class AUndeadCharacter> UndeadBlueprint;
	UPROPERTY(EditAnywhere, BlueprintInternalUseOnly)
	float UseFrequency;
	UPROPERTY(EditAnywhere, BlueprintInternalUseOnly)
	AActor* Point;
};

UCLASS()
class SHOOTERMULTI_API AUndeadDirector : public AActor
{
	GENERATED_BODY()
	
public:

	UPROPERTY(EditInstanceOnly, BlueprintInternalUseOnly, Category = Director)
	TArray<FUndeadSpawnPoint> SpawnPoints;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Director)
	int MaxPunchPerSecond = 2;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Director, meta=(ClampMin = 0.1f))
	float SecondPerSpawn = 5.f;

	// Sets default values for this actor's properties
	AUndeadDirector();

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	virtual void Destroyed() override;

	// Called every frame
	virtual void Tick( float DeltaSeconds ) override;
	
	UFUNCTION(BlueprintPure, Category = Director)
	static AUndeadDirector* GetCurrent();

	UFUNCTION(BlueprintPure, Category = Director)
	bool PunchAvailable();

	void OnUndeadPunch(AUndeadCharacter* instigator);

	void SpawnEnemy();

protected:
	static AUndeadDirector* Current;

	FTimerHandle SpawnTimerHandle;

	TArray<float> PunchTimers;

	FDelegateHandle PunchEventHandle;
};
