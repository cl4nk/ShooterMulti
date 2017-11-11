// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ShooterCube.generated.h"

UENUM(BlueprintType)

enum class EImpactType : uint8
{
	IT_Radial,
	IT_Impulse,
	IT_Force,
	None
};

UCLASS()

class SHOOTERMULTI_API AShooterCube : public AActor
{
GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AShooterCube();

	float TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent,
	                 AController* EventInstigator, AActor* DamageCauser) override;
protected:
	void BeginPlay() override;

protected:
	UPROPERTY( Category = "ShooterCube", Transient, BlueprintReadWrite )
	UStaticMeshComponent* RootStaticMeshComponent = nullptr;

	UPROPERTY( Category = "ShooterCube", EditAnywhere, BlueprintReadOnly )
	float RadiusImpact = 10.f;
	UPROPERTY( Category = "ShooterCube", EditAnywhere, BlueprintReadOnly )
	float PowerImpulse = 50.f;
	UPROPERTY( Category = "ShooterCube", EditAnywhere, BlueprintReadOnly )
	float PowerForce = 5000.f;

	UPROPERTY( Category = "ShooterCube", Transient, VisibleAnywhere, BlueprintReadOnly )
	float InitialGravityZ;

	UPROPERTY( Category = "ShooterCube", EditAnywhere, BlueprintReadOnly )
	EImpactType ImpactType = EImpactType::IT_Impulse;
};
