// Fill out your copyright notice in the Description page of Project Settings.

#include "ShooterMulti.h"
#include "ObjectDirector.h"
#include "InteractiveObject.h"


AObjectDirector* AObjectDirector::Current = nullptr;

AObjectDirector::AObjectDirector()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}


// Called when the game starts or when spawned
void AObjectDirector::BeginPlay()
{
	Super::BeginPlay();

	Current = this;

	if (SpawnPoints.Num() == 0)
	UE_LOG(LogTemp, Warning, TEXT("Object Director has no spawn point."));

	SpawnTimers.SetNum(ObjectList.Num());
}

void AObjectDirector::Destroyed()
{
	if (Current == this)
		Current = nullptr;
}

// Called every frame
void AObjectDirector::Tick( float DeltaTime )
{
	Super::Tick( DeltaTime );

	for ( int i = 0; i < SpawnTimers.Num(); ++i )
	{
		SpawnTimers[i] += DeltaTime;
		if ( SpawnTimers[i] >= ObjectList[i].SpawnFrequency )
		{
			SpawnInteractiveObject( i );
		}
	}
}

void AObjectDirector::SpawnInteractiveObject( const int index )
{
	SpawnTimers[index] = 0;
	int SpawnPointIndex = FMath::FRandRange( 0, SpawnPoints.Num() - 1 );

	FActorSpawnParameters spawnParameters;
	spawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
	AInteractiveObject* object = GetWorld()->SpawnActor<AInteractiveObject>(
		ObjectList[index].SpawningObject,
		SpawnPoints[SpawnPointIndex]->GetActorLocation(),
		SpawnPoints[SpawnPointIndex]->GetActorRotation(),
		spawnParameters );
	if ( object == nullptr )
	{
		UE_LOG( LogTemp, Warning, TEXT("AObjectDirector::SpawnInteractiveObject() - object has not been spawned") );
	}
}

AObjectDirector* AObjectDirector::GetCurrent()
{
	return Current;
}
