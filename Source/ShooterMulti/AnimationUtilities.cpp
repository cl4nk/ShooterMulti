// Fill out your copyright notice in the Description page of Project Settings.

#include "ShooterMulti.h"
#include "AnimationUtilities.h"
#include "Animation/AnimInstance.h"

void UAnimationUtilities::UpdateMontagePlayState(UAnimInstance* target, UAnimMontage* Montage, bool ShouldPlay, float InPlayRate, float OutBlendTime)
{
	if (ShouldPlay)
	{
		if (!target->Montage_IsPlaying(Montage))
			target->Montage_Play(Montage, InPlayRate);
	}
	else
	{
		if (target->Montage_IsPlaying(Montage))
			target->Montage_Stop(OutBlendTime, Montage);
	}
}

