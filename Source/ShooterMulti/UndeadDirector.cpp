// Fill out your copyright notice in the Description page of Project Settings.

#include "ShooterMulti.h"
#include "UndeadDirector.h"
#include "SpawnerTeamSwitch.h"
#include "ShooterMultiGameState.h"

AUndeadDirector* AUndeadDirector::Current = nullptr;

// Sets default values
AUndeadDirector::AUndeadDirector()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	bNetLoadOnClient = false;
}

// Called when the game starts or when spawned
void AUndeadDirector::BeginPlay()
{
	Super::BeginPlay();

	Current = this;

	UndeadCharacterList.Empty();

	if (SpawnPoints.Num() == 0)
	UE_LOG(LogTemp, Warning, TEXT("Undead Director has no spawn point."));


	//normalize frequencies
	float mag = 0.f;
	for (int i = 0; i < SpawnPoints.Num(); ++i)
		mag += SpawnPoints[i]->UseFrequency;
	for (int i = 0; i < SpawnPoints.Num(); ++i)
		SpawnPoints[i]->UseFrequency /= mag;

	AShooterMultiGameState* const ShooterGameState = GetWorld()->GetGameState<AShooterMultiGameState>();

	ShooterGameState->OnStateChange.AddDynamic(this, &AUndeadDirector::HandleGameState);
}

void AUndeadDirector::EndPlay( const EEndPlayReason::Type EndPlayReason )
{
	AShooterMultiGameState* const ShooterGameState = GetWorld()->GetGameState<AShooterMultiGameState>();

	if ( ShooterGameState )
		ShooterGameState->OnStateChange.RemoveDynamic(this, &AUndeadDirector::HandleGameState);

	StopSpawn();

	if (Current == this)
		Current = nullptr;
}

// Called every frame
void AUndeadDirector::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	for (int i = 0; i < PunchTimers.Num(); ++i)
	{
		PunchTimers[i] += DeltaTime;
		if (PunchTimers[i] >= 1.f)
			PunchTimers.RemoveAt(i--);
	}
}

void AUndeadDirector::HandleGameState(EShooterMultiState state)
{
	switch (state)
	{
	case EShooterMultiState::SMS_Playing:
		{
			StartSpawn();
			break;
		}
	case EShooterMultiState::SMS_EndGame:
		{
			StopSpawn();
			break;
		}
	default:
		break;
	}
}

void AUndeadDirector::StartSpawn()
{
	PunchTimers.Empty();

	AUndeadCharacter::PunchEvent.Clear();
	AUndeadCharacter::DeathEvent.Clear();

	GetWorld()->GetTimerManager().SetTimer(SpawnTimerHandle, this, &AUndeadDirector::SpawnEnemy, SecondPerSpawn, true);

	PunchEventHandle = AUndeadCharacter::PunchEvent.AddLambda([this](AUndeadCharacter* charac) { OnUndeadPunch(charac); });
	DeathEventHandle = AUndeadCharacter::DeathEvent.AddLambda([this](AUndeadCharacter* charac) { OnUndeadDeath(charac); });

	AShooterMultiGameState* const ShooterGameState = GetWorld()->GetGameState<AShooterMultiGameState>();

	MaxUndeadCount = BaseUndeadCount + PerPlayerUndeadCount * (ShooterGameState->PlayerArray.Num() - 1);
}

void AUndeadDirector::StopSpawn()
{
	AUndeadCharacter::PunchEvent.Remove(PunchEventHandle);
	AUndeadCharacter::DeathEvent.Remove(DeathEventHandle);

	GetWorld()->GetTimerManager().ClearTimer(SpawnTimerHandle);

	for (int i = 0; i < UndeadCharacterList.Num(); ++ i)
	{
		AUndeadCharacter* charac = UndeadCharacterList[i];
		if ( charac->Controller )
		 charac->Controller->UnPossess();
	}
	UndeadCharacterList.Empty();
}

void AUndeadDirector::SpawnEnemy()
{
	if (SpawnPoints.Num() == 0 || SpawnAvailable() == false)
		return;

	float rand = FMath::FRand();
	float fq = 0.f;
	ASpawnerTeamSwitch* selectedSpawnPoint = nullptr;
	for (int i = 0; i < SpawnPoints.Num(); ++i)
	{
		fq += SpawnPoints[i]->UseFrequency;
		if (fq > rand)
		{
			selectedSpawnPoint = SpawnPoints[i];
			break;
		}
	}

	if (!selectedSpawnPoint)
		return;

	selectedSpawnPoint->Spawn();
}

void AUndeadDirector::OnUndeadSpawn(AUndeadCharacter* Char)
{
	if (Char == nullptr)
		return;

	UndeadCharacterList.AddUnique(Char);
}

void AUndeadDirector::OnUndeadDeath(AUndeadCharacter* Char)
{
	UndeadCharacterList.Remove(Char);
}

AUndeadDirector* AUndeadDirector::GetCurrent()
{
	return Current;
}

bool AUndeadDirector::SpawnAvailable()
{
	return UndeadCharacterList.Num() < MaxUndeadCount;
}

bool AUndeadDirector::PunchAvailable()
{
	return PunchTimers.Num() < MaxPunchPerSecond;
}

void AUndeadDirector::OnUndeadPunch(AUndeadCharacter* instigator)
{
	PunchTimers.Add(0.f);
}
