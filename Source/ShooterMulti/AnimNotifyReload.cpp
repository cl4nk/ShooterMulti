// Fill out your copyright notice in the Description page of Project Settings.

#include "ShooterMulti.h"
#include "AnimNotifyReload.h"
#include "ShooterCharacter.h"


void UAnimNotifyReload::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	AShooterCharacter* ShooterCharacter = Cast<AShooterCharacter>(MeshComp->GetOwner());

	if (ShooterCharacter)
		ShooterCharacter->EndReload();
	else
		UE_LOG(LogTemp, Warning, TEXT("Trying to reload but no character found."));
}
