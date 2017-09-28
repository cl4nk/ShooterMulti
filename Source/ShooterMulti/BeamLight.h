// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
class UPointLightComponent;
#include "BeamLight.generated.h"

/**
 * 
 */
UCLASS()
class SHOOTERMULTI_API ABeamLight : public AActor
{
	GENERATED_BODY()
	
public:

	ABeamLight();

	UFUNCTION(BlueprintCallable, Category = Light)
	void Initialize(FVector Start, FVector End, FLinearColor Color, float Lifetime, float StartIntensity = 5000.f, UCurveFloat* IntensityCurve = nullptr);

	virtual void Tick(float DeltaSeconds) override;

private:
	float TotalLifetime = 0.f;
	float Lifetime;
	UCurveFloat* IntensityCurve;
	float Intensity;
	UPointLightComponent* LightComponent;
};
