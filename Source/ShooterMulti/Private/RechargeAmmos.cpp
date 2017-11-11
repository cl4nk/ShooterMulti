// Fill out your copyright notice in the Description page of Project Settings.

#include "ShooterMulti.h"
#include "RechargeAmmos.h"

#include "ShooterCharacter.h"

void ARechargeAmmos::OnSelectableEnter_Implementation(AActor* Actor)
{
	Super::OnSelectableEnter_Implementation(Actor);

	AShooterCharacter* Character = Cast<AShooterCharacter>(Actor);
	if (Character)
	{
		Character->AddAmmo(Ammo);
		Destroy();
	}
}

FName ARechargeAmmos::GetObjectName() const
{
	return "Ammos";
}
