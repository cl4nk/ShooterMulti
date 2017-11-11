// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Kismet/BlueprintFunctionLibrary.h"
#include "AnimationUtilities.generated.h"

class UAnimInstance;
class UAnimMontage;

/**
 * 
 */
UCLASS()

class SHOOTERMULTI_API UAnimationUtilities : public UBlueprintFunctionLibrary
{
GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category=Animation)
	static void UpdateMontagePlayState(UAnimInstance* target, UAnimMontage* Montage, bool ShouldPlay,
	                                   float InPlayRate = 1.f, float OutBlendTime = .5f);
};
