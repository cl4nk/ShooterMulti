// Fill out your copyright notice in the Description page of Project Settings.

#include "ShooterMulti.h"
#include "ShooterCube.h"

// Sets default values
AShooterCube::AShooterCube()
	: InitialGravityZ(0)
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	bReplicates = true;
	bReplicateMovement = true;
}

void AShooterCube::BeginPlay()
{
	Super::BeginPlay();

	UWorld* world = GetWorld();

	if (world)
		InitialGravityZ = -world->GetGravityZ();
}

float AShooterCube::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator,
                               AActor* DamageCauser)
{
	if (!RootStaticMeshComponent)
	{
		UE_LOG( LogTemp, Warning, TEXT("AShooterCube::TakeDamage() - RootStaticMeshComponent is nullptr") );
		return 0.f;
	}
	if (DamageEvent.IsOfType(FPointDamageEvent::ClassID))
	{
		const FPointDamageEvent& castedDamageEvent = (const FPointDamageEvent&)(DamageEvent);

		// TODO: choose best
		switch (ImpactType)
		{
		case EImpactType::IT_Radial:
			RootStaticMeshComponent->AddRadialImpulse(castedDamageEvent.HitInfo.ImpactPoint, RadiusImpact,
			                                          PowerImpulse * InitialGravityZ, RIF_Linear);
			UE_LOG( LogTemp, Warning, TEXT("AShooterCube::TakeDamage() - Radial impulse with RadiusImpact[%f] / PowerImpulse[%f]"
			), RadiusImpact, PowerImpulse );
			break;
		case EImpactType::IT_Impulse:
			RootStaticMeshComponent->AddImpulseAtLocation(castedDamageEvent.ShotDirection * PowerImpulse * InitialGravityZ,
			                                              castedDamageEvent.HitInfo.ImpactPoint,
			                                              castedDamageEvent.HitInfo.BoneName);
			UE_LOG( LogTemp, Warning, TEXT("AShooterCube::TakeDamage() - Impulse with PowerImpulse[%f]"), PowerImpulse );
			break;
		case EImpactType::IT_Force:
			RootStaticMeshComponent->AddForceAtLocation(castedDamageEvent.ShotDirection * PowerForce * InitialGravityZ,
			                                            castedDamageEvent.HitInfo.ImpactPoint,
			                                            castedDamageEvent.HitInfo.BoneName);
			UE_LOG( LogTemp, Warning, TEXT("AShooterCube::TakeDamage() - Force with PowerForce[%f]"), PowerForce );
			break;
		default:
		UE_LOG( LogTemp, Warning, TEXT("AShooterCube::TakeDamage() = bad ImpactType") );
		}
	}
	else
	UE_LOG( LogTemp, Warning, TEXT("AShooterCube::TakeDamage() - damage is not of FPointDamageEvent") );
	return 0.f;
}
