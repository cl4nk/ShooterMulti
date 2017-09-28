// Fill out your copyright notice in the Description page of Project Settings.

#include "ShooterMulti.h"
#include "UndeadDirector.h"


AUndeadDirector* AUndeadDirector::Current = nullptr;

// Sets default values
AUndeadDirector::AUndeadDirector()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void AUndeadDirector::BeginPlay()
{
	Super::BeginPlay();

	PunchTimers.Empty();

	Current = this;

	if (SpawnPoints.Num() == 0)
		UE_LOG(LogTemp, Warning, TEXT("Undead Director has no spawn point."));

	GetWorld()->GetTimerManager().SetTimer(SpawnTimerHandle, this, &AUndeadDirector::SpawnEnemy, SecondPerSpawn, true);
	
	PunchEventHandle = AUndeadCharacter::PunchEvent.AddLambda([this](AUndeadCharacter* charac) { OnUndeadPunch(charac); });

	//normalize frequencies
	float mag = 0.f;
	for (int i = 0; i < SpawnPoints.Num(); ++i)
		mag += SpawnPoints[i].UseFrequency;
	for (int i = 0; i < SpawnPoints.Num(); ++i)
		SpawnPoints[i].UseFrequency /= mag;
}

void AUndeadDirector::Destroyed()
{
	AUndeadCharacter::PunchEvent.Remove(PunchEventHandle);

	GetWorld()->GetTimerManager().ClearTimer(SpawnTimerHandle);

	if (Current == this)
		Current = nullptr;
}

// Called every frame
void AUndeadDirector::Tick(float DeltaTime)
{
	Super::Tick( DeltaTime );

	for (int i = 0; i < PunchTimers.Num(); ++i)
	{
		PunchTimers[i] += DeltaTime;
		if (PunchTimers[i] >= 1.f)
		PunchTimers.RemoveAt(i--);
	}
}

void AUndeadDirector::SpawnEnemy()
{
	if (SpawnPoints.Num() == 0)
		return;

	float rand = FMath::FRand();
	float fq = 0.f;
	FUndeadSpawnPoint* selectedSpawnPoint = nullptr;
	for (int i = 0; i < SpawnPoints.Num(); ++i)
	{
		fq += SpawnPoints[i].UseFrequency;
		if (fq > rand)
		{
			selectedSpawnPoint = &SpawnPoints[i];
			break;
		}
	}

	if (!selectedSpawnPoint)
		return;

	FActorSpawnParameters spawnParameters;
	spawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
	auto undead = GetWorld()->SpawnActor<AUndeadCharacter>(
		selectedSpawnPoint->UndeadBlueprint,
		selectedSpawnPoint->Point->GetActorLocation(),
		selectedSpawnPoint->Point->GetActorRotation(),
		spawnParameters);
}

AUndeadDirector* AUndeadDirector::GetCurrent()
{
	return Current;
}

bool AUndeadDirector::PunchAvailable()
{
	return PunchTimers.Num() < MaxPunchPerSecond;
}

void AUndeadDirector::OnUndeadPunch(AUndeadCharacter* instigator)
{
	PunchTimers.Add(0.f);
}