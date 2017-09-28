// Fill out your copyright notice in the Description page of Project Settings.

#include "ShooterMulti.h"
#include "UndeadCharacterAnim.h"

#include "Kismet/KismetMathLibrary.h"
#include "UndeadCharacter.h"
#include "AnimationUtilities.h"

void UUndeadCharacterAnim::NativeInitializeAnimation()
{
	UndeadCharacter = Cast<AUndeadCharacter>(TryGetPawnOwner());
}

void UUndeadCharacterAnim::NativeUpdateAnimation(float DeltaSeconds)
{
	if (!UndeadCharacter)
		return;

	FVector velocity = UndeadCharacter->GetVelocity();
	FRotator worldRotation = UndeadCharacter->GetActorRotation();

	FVector2D movement = (FVector2D)worldRotation.UnrotateVector(velocity);

	Direction = movement.GetSafeNormal();
	Speed = movement.Size();

	if (UndeadCharacter->HasPunched())
		Montage_Play(PunchMontage, .6f);
}

void UUndeadCharacterAnim::AnimNotify_PunchHit(UAnimNotify* Notify)
{
	AUndeadCharacter* character = Cast<AUndeadCharacter>(TryGetPawnOwner());
	
	if (character)
		character->InflictPunch();
}