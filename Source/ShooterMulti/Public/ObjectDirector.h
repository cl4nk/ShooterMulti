// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ObjectDirector.generated.h"

USTRUCT()

struct FSpawnableObject
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintInternalUseOnly)
	TSubclassOf<class AInteractiveObject> SpawningObject;
	UPROPERTY(EditAnywhere, BlueprintInternalUseOnly)
	float SpawnFrequency = 5.0f;
};


UCLASS()

class SHOOTERMULTI_API AObjectDirector : public AActor
{
GENERATED_BODY()
public:
	AObjectDirector();

protected:
	// Called when the game starts or when spawned
	void BeginPlay() override;

	void Destroyed() override;

	// Called every frame
	void Tick(float DeltaSeconds) override;

public:
	UFUNCTION(BlueprintPure, Category = Director)
	static AObjectDirector* GetCurrent();

protected:

	void SpawnInteractiveObject(const int index);

	static AObjectDirector* Current;

	UPROPERTY(EditInstanceOnly, BlueprintInternalUseOnly, Category = Director)
	TArray<FSpawnableObject> ObjectList;

	TArray<float> SpawnTimers;

	UPROPERTY(EditInstanceOnly, BlueprintInternalUseOnly, Category = Director)
	TArray<AActor *> SpawnPoints;
};
