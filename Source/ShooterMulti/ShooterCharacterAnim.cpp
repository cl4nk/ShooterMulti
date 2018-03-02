// Fill out your copyright notice in the Description page of Project Settings.

#include "ShooterMulti.h"
#include "ShooterCharacterAnim.h"

#include "Kismet/KismetMathLibrary.h"
#include "AnimationUtilities.h"

void UShooterCharacterAnim::NativeInitializeAnimation()
{
	// Removed, set by the pawn itself for multiplayer
	// ShooterCharacter = Cast<AShooterCharacter>( TryGetPawnOwner() );
}

void UShooterCharacterAnim::NativeUpdateAnimation(float DeltaSeconds)
{
	if (!ShooterCharacter)
		return;

	State = ShooterCharacter->CurrentState;

	FVector velocity = ShooterCharacter->GetVelocity();
	FRotator worldRotation = ShooterCharacter->GetActorRotation();

	FVector2D movement = (FVector2D)worldRotation.UnrotateVector(velocity);

	Direction = movement.GetSafeNormal();
	RelativeSpeed = movement.Size() / ShooterCharacter->GetMovementComponent()->GetMaxSpeed();

	FRotator lookRotation = ShooterCharacter->GetAimOffsets();
	AimPitch = UKismetMathLibrary::ClampAngle(lookRotation.Pitch, -90.f, 90.f);
	AimYaw = UKismetMathLibrary::ClampAngle(lookRotation.Yaw, -90.f, 90.f);

	UAnimationUtilities::UpdateMontagePlayState(this, ReloadMontage, State == EShooterCharacterState::Reload);

	if (State == EShooterCharacterState::Punch && !PunchMontagePlayed)
	{
		Montage_Play(PunchMontage, .6f);
		PunchMontagePlayed = true;
	}

	if (State != EShooterCharacterState::Punch)
		PunchMontagePlayed = false;

	float playRate = FireAnimLength / ShooterCharacter->FireRate;
	bool shooting = ShooterCharacter->IsShooting();
	UAnimationUtilities::UpdateMontagePlayState(this, FireMontage, shooting && State == EShooterCharacterState::IdleRun,
	                                            playRate, .2f);
	UAnimationUtilities::UpdateMontagePlayState(this, FireAimMontage, shooting && State == EShooterCharacterState::Aim,
	                                            playRate, .2f);

	if (ShooterCharacter->ConsumeHitTrigger())
		Montage_Play(HitMontage);
}

void UShooterCharacterAnim::SetShooterCharacter(AShooterCharacter* newShooterCharacter)
{
	ShooterCharacter = newShooterCharacter;
}

void UShooterCharacterAnim::AnimNotify_PunchHit(UAnimNotify* Notify)
{
	if (ShooterCharacter)
		ShooterCharacter->InflictPunch();
}

void UShooterCharacterAnim::AnimNotify_FootLeft(UAnimNotify* Notify)
{
	if (ShooterCharacter)
		ShooterCharacter->FootLeftDown();
}

void UShooterCharacterAnim::AnimNotify_FootRight(UAnimNotify* Notify)
{
	if (ShooterCharacter)
		ShooterCharacter->FootRightDown();
}