// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Kismet/BlueprintFunctionLibrary.h"
#include "WeaponUtility.generated.h"

// TODO: could change this and inherit if multiple weapons
USTRUCT(BlueprintInternalUseOnly)

struct FLaserWeaponData
{
	GENERATED_BODY()

	UPROPERTY(BlueprintInternalUseOnly)
	FTransform LookTransform;
	UPROPERTY(BlueprintInternalUseOnly)
	FTransform MuzzleTransform;

	UPROPERTY(BlueprintInternalUseOnly)
	float Damages;
	UPROPERTY(BlueprintInternalUseOnly)
	float Knockback;
	UPROPERTY(BlueprintInternalUseOnly)
	float Spread = 0.f;
	UPROPERTY(BlueprintInternalUseOnly)
	float MaxDistance = 10000.f;
};

UCLASS()

class SHOOTERMULTI_API UWeaponUtility : public UBlueprintFunctionLibrary
{
GENERATED_BODY()

public:

	UFUNCTION(BlueprintCallable, Category = Weapon)
	static bool ShootLaser(UWorld* World, AActor* Causer, FHitResult& HitResult, const FLaserWeaponData& WeaponData);

	UFUNCTION(BlueprintCallable, Category = Weapon)
	static void MakeImpactDecal(const FHitResult& FromHit, UMaterialInterface* ImpactDecalMaterial,
	                            float ImpactDecalSizeMin, float ImpactDecalSizeMax);

	UFUNCTION(BlueprintCallable, Category = Weapon)
	static void MakeLaserBeam(UWorld* World, FVector Start, FVector End, UParticleSystem* BeamParticles,
	                          float BeamIntensity, FLinearColor Color, UCurveFloat* BeamIntensityCurve);

	UFUNCTION(BlueprintCallable, Category = Weapon)
	static void MakeImpactParticles(UWorld* World, UParticleSystem* ImpactParticles, const FHitResult& FromHit,
	                                float Scale = 1.f);

	UFUNCTION(BlueprintCallable, Category = Weapon)
	static void MakeImpactSound(UWorld* World, USoundBase* Sound, const FHitResult& FromHit);
};
