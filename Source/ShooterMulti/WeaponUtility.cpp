// Fill out your copyright notice in the Description page of Project Settings.

#include "ShooterMulti.h"
#include "WeaponUtility.h"
#include "HitDamage.h"
#include "BeamLight.h"
#include "Kismet/KismetMathLibrary.h"

bool UWeaponUtility::ShootLaser(UWorld* World, AActor* Causer, FHitResult& HitResult,
                                const FLaserWeaponData& WeaponData)
{
	FVector lookLoc = WeaponData.LookTransform.GetLocation();
	FVector lookDir = WeaponData.LookTransform.GetRotation().GetForwardVector();

	//apply spread
	if (WeaponData.Spread > 0.f)
	{
		// REMOVED FOR 4.18 - lookDir = UKismetMathLibrary::RandomUnitVectorInCone(lookDir, FMath::DegreesToRadians(WeaponData.Spread * .5f));
		lookDir = UKismetMathLibrary::RandomUnitVectorInConeInDegrees( lookDir, WeaponData.Spread * .5f );
	}

	FCollisionQueryParams CollisionParams;
	CollisionParams.AddIgnoredActor(Causer);
	CollisionParams.bTraceComplex = true;
	CollisionParams.bReturnPhysicalMaterial = true;
	//in case of actor hit
	if (World->LineTraceSingleByChannel(HitResult, lookLoc, lookLoc + lookDir * WeaponData.MaxDistance, ECC_Visibility,
	                                    CollisionParams))
	{
		//make damages
		FPointDamageEvent damageEvent = FPointDamageEvent(WeaponData.Damages, HitResult, lookDir,
		                                                  TSubclassOf<UDamageType>(UHitDamage::StaticClass()));
		HitResult.Actor->TakeDamage(WeaponData.Damages, damageEvent, nullptr, Causer);

		//push hit actors (physics)
		for (auto aSkeletalMesh : HitResult.Actor->GetComponentsByClass(USkeletalMeshComponent::StaticClass()))
		{
			USkeletalMeshComponent* skeletalMesh = Cast<USkeletalMeshComponent>(aSkeletalMesh);
			if (skeletalMesh->IsSimulatingPhysics())
				skeletalMesh->AddForceAtLocation(lookDir * WeaponData.Knockback, HitResult.ImpactPoint, HitResult.BoneName);
		}
		for (auto aStaticMesh : HitResult.Actor->GetComponentsByClass(UStaticMeshComponent::StaticClass()))
		{
			UStaticMeshComponent* staticMesh = Cast<UStaticMeshComponent>(aStaticMesh);
			if (staticMesh->IsSimulatingPhysics())
				staticMesh->AddForceAtLocation(lookDir * WeaponData.Knockback, HitResult.ImpactPoint, HitResult.BoneName);
		}

		return true;
	}
	//when no actor hit
	HitResult.ImpactPoint = lookLoc + lookDir * WeaponData.MaxDistance;
	HitResult.Distance = WeaponData.MaxDistance;
	return false;
}

void UWeaponUtility::MakeImpactDecal(const FHitResult& FromHit, UMaterialInterface* ImpactDecalMaterial,
                                     float ImpactDecalSizeMin, float ImpactDecalSizeMax)
{
	auto sm = FromHit.Actor->FindComponentByClass<UStaticMeshComponent>();
	if (sm)
	{
		FVector decalPos = FromHit.ImpactPoint;
		FRotator decalRot = (FromHit.ImpactNormal.Rotation().Quaternion() * FRotator(
			0.f, 0.f, FMath::RandRange(-180.f, 180.f)).Quaternion()).Rotator();
		float sdsize = FMath::RandRange(ImpactDecalSizeMin, ImpactDecalSizeMax);
		FVector dsize = FVector(sdsize, sdsize, sdsize);

		if (sm->Mobility == EComponentMobility::Static)
			UGameplayStatics::SpawnDecalAtLocation(FromHit.Actor->GetWorld(), ImpactDecalMaterial, dsize, decalPos, decalRot,
			                                       0.f)->FadeScreenSize = .002f;
		else
			UGameplayStatics::SpawnDecalAttached(ImpactDecalMaterial, dsize, sm, NAME_None, decalPos, decalRot,
			                                     EAttachLocation::KeepWorldPosition)->FadeScreenSize = .002f;
	}
}

void UWeaponUtility::MakeLaserBeam(UWorld* World, FVector Start, FVector End, UParticleSystem* BeamParticles,
                                   float BeamIntensity, FLinearColor Color, UCurveFloat* BeamIntensityCurve)
{
	FTransform particleTransform;
	particleTransform.SetLocation(Start);
	particleTransform.SetRotation((End - Start).Rotation().Quaternion());

	//create a beam particle
	auto beamParticle = UGameplayStatics::SpawnEmitterAtLocation(World, BeamParticles, particleTransform, true);
	beamParticle->SetBeamSourcePoint(0, Start, 0);
	beamParticle->SetBeamTargetPoint(0, End, 0);

	//spawn light
	World->SpawnActor<ABeamLight>()->Initialize(Start, End, Color, .8f, BeamIntensity, BeamIntensityCurve);
}

void UWeaponUtility::MakeImpactParticles(UWorld* World, UParticleSystem* ImpactParticles, const FHitResult& FromHit,
                                         float Scale)
{
	FTransform hitTransform;
	hitTransform.SetLocation(FromHit.ImpactPoint);
	hitTransform.SetRotation(FromHit.Normal.Rotation().Quaternion());
	hitTransform.SetScale3D(FVector(Scale, Scale, Scale));

	UGameplayStatics::SpawnEmitterAtLocation(World, ImpactParticles, hitTransform, true);
}

void UWeaponUtility::MakeImpactSound(UWorld* World, USoundBase* Sound, const FHitResult& FromHit)
{
	if (Sound)
	{
		UGameplayStatics::PlaySoundAtLocation(World, Sound, FromHit.ImpactPoint);
	}
}
