// Fill out your copyright notice in the Description page of Project Settings.

#include "ShooterMulti.h"
#include "BeamLight.h"
#include "Components/PointLightComponent.h"

ABeamLight::ABeamLight()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	//Create Punch Collision
	RootComponent = LightComponent = CreateDefaultSubobject<UPointLightComponent>(TEXT("Light"));
}

void ABeamLight::Initialize(FVector Start, FVector End, FLinearColor Color, float _Lifetime, float StartIntensity,
                            UCurveFloat* _IntensityCurve)
{
	FVector beamMiddle = (Start + End) * .5f;
	FVector beamVector = End - Start;
	FRotator beamRotation = beamVector.Rotation();
	float beamLength = beamVector.Size();

	SetActorLocation(beamMiddle);
	SetActorRotation(beamRotation);

	LightComponent->SetLightColor(Color);
	LightComponent->SetSourceLength(beamLength);
	LightComponent->SetIntensity(StartIntensity);

	TotalLifetime = _Lifetime;
	Lifetime = _Lifetime;
	Intensity = StartIntensity;
	IntensityCurve = _IntensityCurve;
}

void ABeamLight::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	Lifetime -= DeltaSeconds;

	if (Lifetime > 0.f)
	{
		if (IntensityCurve)
			LightComponent->SetIntensity(Intensity * IntensityCurve->GetFloatValue(1.f - (Lifetime / TotalLifetime)));
		else
			LightComponent->SetIntensity(Intensity * Lifetime / TotalLifetime);
	}
	else if (TotalLifetime > 0.f)
	{
		Destroy();
	}
}
