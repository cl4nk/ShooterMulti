// Fill out your copyright notice in the Description page of Project Settings.

#include "ShooterMulti.h"
#include "ShooterCharacter.h"

#include "ParticleDefinitions.h"
#include "HitDamage.h"
#include "WeaponUtility.h"
#include "ShooterGameMode.h"

AShooterCharacter::FShooterEvent AShooterCharacter::DeathEvent;

// Sets default values
AShooterCharacter::AShooterCharacter()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	
	//Create Punch Collision
	PunchCollision = CreateDefaultSubobject<USphereComponent>(TEXT("PunchCollision"));
	PunchCollision->SetupAttachment(RootComponent);
	PunchCollision->SetRelativeLocation(FVector(80.f, 0.f, 20.f));
	PunchCollision->InitSphereRadius(20.f);
}

// Called when the game starts or when spawned
void AShooterCharacter::BeginPlay()
{
	Super::BeginPlay();

	RunSpeed = GetCharacterMovement()->MaxWalkSpeed;

	SpringArm = FindComponentByClass<USpringArmComponent>();
	if (SpringArm)
	{
		CameraRestDistance = SpringArm->TargetArmLength;
		TargetSpringArmLength = CameraRestDistance;
	}

	Camera = FindComponentByClass<UCameraComponent>();
	if (Camera)
	{
		CameraRestFOV = Camera->FieldOfView;
		TargetCameraFOV = CameraRestFOV;
	}

	ResetHealth();

	CanSprint = false;
	Shooting = false;
	ShootTimer = 0.f;
	CurrentSpread = 0.f;
	Ammo = MaxAmmo;
	GotHit = false;
}

// Called every frame
void AShooterCharacter::Tick( float DeltaTime )
{
	Super::Tick( DeltaTime );

	//manage camera
	if (SpringArm && SpringArm->TargetArmLength != TargetSpringArmLength)
	{
		SpringArm->TargetArmLength = FMath::Lerp(SpringArm->TargetArmLength, TargetSpringArmLength, .1f);
	}
	if (Camera && Camera->FieldOfView != TargetCameraFOV)
	{
		Camera->FieldOfView = FMath::Lerp(Camera->FieldOfView, TargetCameraFOV, .1f);
	}

	if (IsDead())
		return;

	//update spread
	float minSpread = (CurrentState == EShooterCharacterState::Aim) ? WeaponMinSpreadAim : WeaponMinSpreadWalk;
	CurrentSpread = FMath::Max(minSpread, CurrentSpread - WeaponSpreadRecoveryRate * DeltaTime);

	//manage shot
	ShootTimer += DeltaTime;
	if (Shooting && ShootTimer > FireRate)
	{
		ShootTimer = 0.f;

		TakeShot();
	}

	//manage punch
	if (CurrentState == EShooterCharacterState::Punch)
	{
		StateTimer += DeltaTime;

		if (StateTimer >= PunchDuration)
		{
			CurrentState = EShooterCharacterState::IdleRun;
		}
	}

	//manage shake
	float movementIntensity = GetLastMovementInputVector().Size();

	auto playerController = Cast<APlayerController>(GetController());

	if (playerController)
	{
		if (CurrentState == EShooterCharacterState::Sprint && SprintShake)
			playerController->ClientPlayCameraShake(SprintShake, movementIntensity);
		else if (RunShake)
		{
			playerController->ClientPlayCameraShake(RunShake, movementIntensity);
		}
		if (FMath::Abs(movementIntensity) < .02f)
		{
			if (RunShake)
				playerController->ClientStopCameraShake(RunShake, false);
			if (SprintShake)
				playerController->ClientStopCameraShake(SprintShake, false);
		}
	}
}

void AShooterCharacter::Turn(float Value)
{
	AddControllerYawInput(Value);
}

void AShooterCharacter::LookUp(float Value)
{
	AddControllerPitchInput(Value);
}

void AShooterCharacter::MoveForward(float Value)
{
	if (IsDead())
		return;

	CanSprint = Value > MinSprintMagnitude;

	if (CurrentState == EShooterCharacterState::Sprint && !CanSprint)
		EndSprint ();

	FRotator rotator = GetControlRotation();
	rotator.Pitch = 0.f;
	rotator.Roll = 0.f;
	AddMovementInput(Value * rotator.Vector());
}

void AShooterCharacter::MoveRight(float Value)
{
	if (CurrentState == EShooterCharacterState::Sprint || IsDead())
		return;

	FRotator rotator = GetControlRotation();
	rotator.Pitch = 0.f;
	rotator.Roll = 0.f;
	AddMovementInput(Value * rotator.RotateVector (FVector::RightVector));
}

void AShooterCharacter::StartJump()
{
	if (IsDead())
		return;

	if (Shooting)
		EndShoot();

	if (CanJump())
		Jump();
}

void AShooterCharacter::EndJump()
{
	StopJumping();
}

void AShooterCharacter::StartSprint()
{
	if ((CurrentState != EShooterCharacterState::IdleRun &&
		 CurrentState != EShooterCharacterState::Aim) ||
		!CanSprint ||
		IsDead())
		return;

	if (CurrentState == EShooterCharacterState::Aim)
		EndAim();

	if (Shooting)
		EndShoot();

	CurrentState = EShooterCharacterState::Sprint;
	GetCharacterMovement()->MaxWalkSpeed = SprintSpeed;
}

void AShooterCharacter::EndSprint()
{
	if (CurrentState != EShooterCharacterState::Sprint)
		return;

	CurrentState = EShooterCharacterState::IdleRun;
	GetCharacterMovement()->MaxWalkSpeed = RunSpeed;
}

void AShooterCharacter::StartAim()
{
	if (CurrentState != EShooterCharacterState::IdleRun ||
		IsDead())
		return;

	CurrentState = EShooterCharacterState::Aim;
	GetCharacterMovement()->MaxWalkSpeed = AimWalkSpeed;
	TargetSpringArmLength = CameraAimDistance;
	TargetCameraFOV = CameraAimFOV;
}

void AShooterCharacter::EndAim()
{
	if (CurrentState != EShooterCharacterState::Aim)
		return;

	CurrentState = EShooterCharacterState::IdleRun;
	GetCharacterMovement()->MaxWalkSpeed = RunSpeed;
	TargetSpringArmLength = CameraRestDistance;
	TargetCameraFOV = CameraRestFOV;
}

int AShooterCharacter::GetAmmo()
{
	return Ammo;
}

bool AShooterCharacter::IsShooting()
{
	return Shooting;
}

void AShooterCharacter::StartShoot()
{
	if (CurrentState != EShooterCharacterState::IdleRun &&
		CurrentState != EShooterCharacterState::Aim ||
		IsDead())
		return;
	
	Shooting = true;
}

void AShooterCharacter::EndShoot()
{
	Shooting = false;
}

void AShooterCharacter::Reload()
{
	if ((CurrentState != EShooterCharacterState::IdleRun &&
		 CurrentState != EShooterCharacterState::Aim) ||
		Ammo >= MaxAmmo ||
		IsDead())
		return;

	if (CurrentState == EShooterCharacterState::Aim)
		EndAim();

	if (Shooting)
		EndShoot();

	CurrentState = EShooterCharacterState::Reload;
	GetCharacterMovement()->MaxWalkSpeed = ReloadWalkSpeed;
}

void AShooterCharacter::EndReload()
{
	if (CurrentState != EShooterCharacterState::Reload)
		return;

	CurrentState = EShooterCharacterState::IdleRun;
	GetCharacterMovement()->MaxWalkSpeed = RunSpeed;
	Ammo = MaxAmmo;
}

void AShooterCharacter::AbortReload()
{
	if (CurrentState != EShooterCharacterState::Reload)
		return;

	CurrentState = EShooterCharacterState::IdleRun;
	GetCharacterMovement()->MaxWalkSpeed = RunSpeed;
}

void AShooterCharacter::Punch()
{
	if (CurrentState == EShooterCharacterState::Aim ||
		IsDead())
		EndAim();

	if (CurrentState != EShooterCharacterState::IdleRun)
		return;

	CurrentState = EShooterCharacterState::Punch;
	StateTimer = 0.f;
}

void AShooterCharacter::InflictPunch()
{
	TArray<struct FHitResult> outHits;

	FVector pos = PunchCollision->GetComponentLocation();

	FCollisionQueryParams params;
	params.AddIgnoredActor(this);
	params.bTraceComplex = true;

	GetWorld()->SweepMultiByObjectType(
		outHits, pos, pos, FQuat::Identity, 0,
		FCollisionShape::MakeSphere(PunchCollision->GetUnscaledSphereRadius()),
		params);

	TArray<ACharacterWithHealth*> hitActors;

	for (auto& hit : outHits)
	{
		ACharacterWithHealth* character = Cast<ACharacterWithHealth>(hit.Actor.Get());

		if (character && character->IsPlayerFriendly != IsPlayerFriendly && !hitActors.Contains(character))
		{
			FPointDamageEvent damageEvent = FPointDamageEvent(WeaponPunchDamages, hit, GetActorForwardVector(), TSubclassOf<UDamageType>(UHitDamage::StaticClass()));
			character->TakeDamage(WeaponPunchDamages, damageEvent, nullptr, this);
			hitActors.Add(character);
		}
	}
}

void AShooterCharacter::TakeShot()
{
	UMeshComponent* weaponMesh = Cast<UMeshComponent>(GetMesh()->GetChildComponent(0));

	if (Ammo <= 0)
	{
		EndShoot();
		Reload();
	}
	else
	{
		--Ammo;

		FLaserWeaponData weaponData;
		weaponData.MuzzleTransform = weaponMesh->GetSocketTransform("MuzzleFlashSocket");
		weaponData.LookTransform = Camera->GetComponentTransform();
		weaponData.Damages = WeaponDamages;
		weaponData.Knockback = WeaponKnokback;
		weaponData.Spread = CurrentSpread;

		FHitResult hitResult;
		if (UWeaponUtility::ShootLaser(GetWorld(), this, hitResult, weaponData))
		{
			//make impact decal
			UWeaponUtility::MakeImpactDecal(hitResult, ImpactDecalMat, .9f * ImpactDecalSize, 1.1f * ImpactDecalSize);

			//create impact particles
			UWeaponUtility::MakeImpactParticles(GetWorld(), ImpactParticle, hitResult, .66f);
		}

		//make the beam visuals
		UWeaponUtility::MakeLaserBeam(GetWorld(), weaponData.MuzzleTransform.GetLocation(), hitResult.ImpactPoint, BeamParticle, BeamIntensity, FLinearColor(1.f, 0.857892f, 0.036923f), BeamIntensityCurve);

		//play the shot sound
		UGameplayStatics::PlaySoundAtLocation(GetWorld(), ShotSound, weaponData.MuzzleTransform.GetLocation());

		//make muzzle smoke
		auto smokeParticle = UGameplayStatics::SpawnEmitterAttached(MuzzleSmokeParticle, weaponMesh, FName("MuzzleFlashSocket"));

		//apply shake
		auto playerController = Cast<APlayerController>(GetController());
		if (playerController && ShootShake)
			playerController->ClientPlayCameraShake(ShootShake);

		//add spread
		CurrentSpread = FMath::Min(WeaponMaxSpread, CurrentSpread + WeaponSpreadPerShot);

		//play sound if gun empty
		if (Ammo == 0)
			UGameplayStatics::PlaySoundAtLocation(GetWorld(), ShotEmptySound, GetActorLocation());
	}
}

float AShooterCharacter::TakeDamage(float DamageAmount, FDamageEvent const & DamageEvent, AController * EventInstigator, AActor * DamageCauser)
{
	if (CurrentState == EShooterCharacterState::Reload)
		AbortReload();

	float actualDamages = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);

	if (actualDamages > 0)
		GotHit = true;

	return actualDamages;
}

bool AShooterCharacter::ConsumeHitTrigger()
{
	if (GotHit)
	{
		GotHit = false;
		return true;
	}

	return false;
}

void AShooterCharacter::FinishDisapear()
{
	Super::FinishDisapear();

	AShooterGameMode* gm = Cast<AShooterGameMode>(GetWorld()->GetAuthGameMode());
	gm->Respawn();
}

void AShooterCharacter::Die()
{
	Super::Die();

	DeathEvent.Broadcast(this);
}
