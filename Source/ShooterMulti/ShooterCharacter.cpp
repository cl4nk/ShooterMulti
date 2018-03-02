// Fill out your copyright notice in the Description page of Project Settings.

#include "ShooterMulti.h"
#include "ShooterCharacter.h"

#include "ParticleDefinitions.h"
#include "HitDamage.h"
#include "WeaponUtility.h"
#include "InteractionSphere.h"
#include "ShooterPlayerController.h"
#include "ShooterCharacterAnim.h"

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

	//Create Punch Collision
	InteractionSphere = CreateDefaultSubobject<UInteractionSphere>(TEXT("InteractiveSphere"));
	InteractionSphere->SetupAttachment(RootComponent);
	InteractionSphere->SetRelativeLocation(FVector(0.f, 0.f, 0.f));
	InteractionSphere->InitSphereRadius(200.f);
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

	USkeletalMeshComponent* mesh = GetMesh();
	if (mesh)
	{
		ShooterCharacterAnim = Cast<UShooterCharacterAnim>(mesh->AnimScriptInstance);
		if (ShooterCharacterAnim)
			ShooterCharacterAnim->SetShooterCharacter(this);

		int count = mesh->GetNumChildrenComponents();
		for (int i = 0; i < count; ++i)
		{
			UMeshComponent * subMesh = Cast<UMeshComponent>(mesh->GetChildComponent(i));
			if (subMesh)
			{
				weaponMesh = subMesh;
				break;
			}
		}
	}

	ResetHealth();

	CanSprint = false;
	Shooting = false;
	ShootTimer = 0.f;
	CurrentSpread = 0.f;
	AmmoLeft = FMath::Min(MaxAmmoInMag * 2, MaxTotalAmmoLeft);
	AmmoInMag = MaxAmmoInMag;
	GotHit = false;

	bInvincible = true;
	GetMesh()->SetRenderCustomDepth(true);

	TimeBeforeEndInvincibility = InvincibilityAtSpawn;
}

// Called every frame
void AShooterCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	APlayerController* playerController = Cast<APlayerController>( GetController() );
	const bool bIsPossesed = IS_LOCAL_MULTIPLAYER // If PS4, consider Local multiplayer
		                         ? true
		                         : ( playerController == GetWorld()->GetFirstPlayerController() );

	// Only on possessed client
	if (bIsPossesed)
	{
		// Manage camera
		if (SpringArm && SpringArm->TargetArmLength != TargetSpringArmLength)
		{
			SpringArm->TargetArmLength = FMath::Lerp(SpringArm->TargetArmLength, TargetSpringArmLength, .1f);
		}
		if (Camera && Camera->FieldOfView != TargetCameraFOV)
		{
			Camera->FieldOfView = FMath::Lerp(Camera->FieldOfView, TargetCameraFOV, .1f);
		}
		////
	}

	if (IsDead())
		return;

	if (Role == ROLE_Authority)
		VerifyInvincibility(DeltaTime);

	// Manage punch / Animation only
	if (CurrentState == EShooterCharacterState::Punch)
	{
		StateTimer += DeltaTime;

		if (StateTimer >= PunchDuration)
		{
			CurrentState = EShooterCharacterState::IdleRun;
		}
	}
	////

	// Only on possessed client
	if (bIsPossesed)
	{
		// Update spread
		const float minSpread = (CurrentState == EShooterCharacterState::Aim) ? WeaponMinSpreadAim : WeaponMinSpreadWalk;
		CurrentSpread = FMath::Max(minSpread, CurrentSpread - WeaponSpreadRecoveryRate * DeltaTime);
		////

		// Manage shot
		ShootTimer += DeltaTime;
		if (Shooting && ShootTimer > FireRate)
		{
			ShootTimer = 0.f;

			// TODO: maybe verify if Ammo value is the same as the server ?
			Server_TakeShot(CurrentSpread);
		}
		////

		// Manage shake
		const float movementIntensity = GetLastMovementInputVector().Size();

		if (playerController)
		{
			if (CurrentState == EShooterCharacterState::Sprint && SprintShake)
			{
				playerController->ClientPlayCameraShake(SprintShake, movementIntensity);
			}
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
		////
	}
}

void AShooterCharacter::Client_Possess_Implementation()
{
	if ( IS_LOCAL_MULTIPLAYER )
	{
		UE_LOG( LogTemp, Warning, TEXT( "AShooterCharacter::Client_Possess_Implementation - WE ARE ON PS4, consider Local Multiplayer" ) );
		return;
	}

	AShooterPlayerController* playerController = Cast<AShooterPlayerController>(GetWorld()->GetFirstPlayerController());

	playerController->ShooterCharacter = this;
	Controller = playerController;
}

bool AShooterCharacter::Client_Possess_Validate()
{
	return true;
}

void AShooterCharacter::OnServerPossess()
{
	Client_Possess();
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
		EndSprint();

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
	AddMovementInput(Value * rotator.RotateVector(FVector::RightVector));
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

void AShooterCharacter::Interact()
{
	InteractionSphere->InteractInput();
}

int AShooterCharacter::GetAmmoInMag() const
{
	return AmmoInMag;
}

int AShooterCharacter::GetAmmoLeft() const
{
	return AmmoLeft;
}

//#pragma region Reload
void AShooterCharacter::AddAmmo(const int ammoReload)
{
	Server_AskAddAmmo(ammoReload);
}

void AShooterCharacter::Reload()
{
	if ((CurrentState != EShooterCharacterState::IdleRun
			&& CurrentState != EShooterCharacterState::Aim)
		|| AmmoInMag >= MaxAmmoInMag
		|| AmmoLeft <= 0
		|| IsDead())
		return;

	Server_AskReload();
}

void AShooterCharacter::EndReload()
{
	if (Role != ROLE_Authority
		|| CurrentState != EShooterCharacterState::Reload)
		return;

	NetMulticast_ExecuteEndReload();
}

void AShooterCharacter::AbortReload()
{
	if (Role != ROLE_Authority
		|| CurrentState != EShooterCharacterState::Reload)
		return;

	NetMulticast_ExecuteAbortReload();
}

void AShooterCharacter::Server_AskAddAmmo_Implementation(const int ammoReload)
{
	NetMulticast_ExecuteAddAmmo(ammoReload);
}

bool AShooterCharacter::Server_AskAddAmmo_Validate(const int ammoReload)
{
	return true;
}

void AShooterCharacter::NetMulticast_ExecuteAddAmmo_Implementation(const int ammoReload)
{
	AmmoLeft += ammoReload;

	if (AmmoLeft > MaxTotalAmmoLeft)
		AmmoLeft = MaxTotalAmmoLeft;
}

bool AShooterCharacter::NetMulticast_ExecuteAddAmmo_Validate(const int ammoReload)
{
	return true;
}

void AShooterCharacter::Server_AskReload_Implementation()
{
	NetMulticast_ExecuteReload();
}

bool AShooterCharacter::Server_AskReload_Validate()
{
	return true;
}

void AShooterCharacter::NetMulticast_ExecuteReload_Implementation()
{
	if (CurrentState == EShooterCharacterState::Aim)
		EndAim();

	if (Shooting)
		EndShoot();

	if (AmmoLeft <= 0)
		return;

	CurrentState = EShooterCharacterState::Reload;
	GetCharacterMovement()->MaxWalkSpeed = ReloadWalkSpeed;
}

bool AShooterCharacter::NetMulticast_ExecuteReload_Validate()
{
	return true;
}

void AShooterCharacter::NetMulticast_ExecuteEndReload_Implementation()
{
	CurrentState = EShooterCharacterState::IdleRun;
	GetCharacterMovement()->MaxWalkSpeed = RunSpeed;

	if (AmmoLeft > 0)
	{
		const int missingAmmo = MaxAmmoInMag - AmmoInMag;
		if (missingAmmo > AmmoLeft)
		{
			AmmoInMag += AmmoLeft;
			AmmoLeft = 0;
		}
		else
		{
			AmmoInMag = MaxAmmoInMag;
			AmmoLeft -= missingAmmo;
		}
	}
}

bool AShooterCharacter::NetMulticast_ExecuteEndReload_Validate()
{
	return true;
}

void AShooterCharacter::NetMulticast_ExecuteAbortReload_Implementation()
{
	CurrentState = EShooterCharacterState::IdleRun;
	GetCharacterMovement()->MaxWalkSpeed = RunSpeed;
}

bool AShooterCharacter::NetMulticast_ExecuteAbortReload_Validate()
{
	return true;
}
//#pragma endregion

//#pragma region Shooting
void AShooterCharacter::StartShoot()
{
	if ((CurrentState != EShooterCharacterState::IdleRun
		&& CurrentState != EShooterCharacterState::Aim)
		|| IsDead())
		return;

	Shooting = true;
}

void AShooterCharacter::EndShoot()
{
	Shooting = false;
}

bool AShooterCharacter::IsShooting() const
{
	return Shooting;
}

void AShooterCharacter::Server_TakeShot_Implementation(float shootSpread)
{
	if (AmmoInMag <= 0)
	{
		// EndShoot(); Already in Reload()
		// NET MULTICAST
		NetMulticast_ExecuteReload();
	}
	else
	{
		FLaserWeaponData weaponData;
		weaponData.MuzzleTransform = weaponMesh->GetSocketTransform("MuzzleFlashSocket");
		weaponData.LookTransform = Camera->GetComponentTransform();
		weaponData.Damages = WeaponDamages;
		weaponData.Knockback = WeaponKnokback;
		weaponData.Spread = shootSpread;

		FHitResult hitResult;
		if (UWeaponUtility::ShootLaser(GetWorld(), this, hitResult, weaponData))
		{
			NetMulticast_MakeImpactFeedback(hitResult);
		}

		NetMulticast_MakeBeamFeedback(hitResult, weaponData);
	}
}

bool AShooterCharacter::Server_TakeShot_Validate(float shootSpread)
{
	return true;
}

void AShooterCharacter::NetMulticast_MakeImpactFeedback_Implementation(FHitResult hitResult)
{
	//make impact decal
	UWeaponUtility::MakeImpactDecal(hitResult, ImpactDecalMat, .9f * ImpactDecalSize, 1.1f * ImpactDecalSize);

	//create impact particles
	UWeaponUtility::MakeImpactParticles(GetWorld(), ImpactParticle, hitResult, .66f);
	UWeaponUtility::MakeImpactSound(GetWorld(), ImpactSound, hitResult);

}

bool AShooterCharacter::NetMulticast_MakeImpactFeedback_Validate(FHitResult hitResult)
{
	return true;
}

void AShooterCharacter::NetMulticast_MakeBeamFeedback_Implementation( FHitResult hitResult,
                                                                      FLaserWeaponData weaponData )
{
	// TODO: remove if working and maybe send it
	UMeshComponent* weaponMesh = Cast<UMeshComponent>( GetMesh()->GetChildComponent( 0 ) );

	//make the beam visuals
	UWeaponUtility::MakeLaserBeam( GetWorld(), weaponData.MuzzleTransform.GetLocation(), hitResult.ImpactPoint,
	                               BeamParticle, BeamIntensity, FLinearColor( 1.f, 0.857892f, 0.036923f ),
	                               BeamIntensityCurve );

	//play the shot sound
	UGameplayStatics::PlaySoundAtLocation( GetWorld(), ShotSound, weaponData.MuzzleTransform.GetLocation() );

	//make muzzle smoke
	UParticleSystemComponent* smokeParticle = UGameplayStatics::SpawnEmitterAttached( MuzzleSmokeParticle, weaponMesh,
	                                                                                  FName( "MuzzleFlashSocket" ) );
	if ( smokeParticle == nullptr )
		UE_LOG( LogTemp, Warning, TEXT("AShooterCharacter::NetMulticast_MakeBeamFeedback_Implementation() - could not emit smokeParticle") )

	//apply shake
	if ( Role != ROLE_SimulatedProxy )
	{
		auto playerController = Cast<APlayerController>( GetController() );
		if ( playerController && ShootShake )
			playerController->ClientPlayCameraShake( ShootShake );
	}

	//add spread
	CurrentSpread = FMath::Min( WeaponMaxSpread, CurrentSpread + WeaponSpreadPerShot );

	--AmmoInMag;

	//play sound if gun empty
	if ( AmmoInMag == 0 )
		UGameplayStatics::PlaySoundAtLocation( GetWorld(), ShotEmptySound, GetActorLocation() );
}

bool AShooterCharacter::NetMulticast_MakeBeamFeedback_Validate(FHitResult hitResult, FLaserWeaponData weaponData)
{
	return true;
}
//#pragma endregion

//#pragma region Punch
void AShooterCharacter::Punch()
{
	Server_AskPunch();
}

void AShooterCharacter::InflictPunch()
{
	if (Role != ROLE_Authority)
		return;

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
			FPointDamageEvent damageEvent = FPointDamageEvent(WeaponPunchDamages, hit, GetActorForwardVector(),
			                                                  TSubclassOf<UDamageType>(UHitDamage::StaticClass()));
			character->TakeDamage(WeaponPunchDamages, damageEvent, nullptr, this);
			hitActors.Add(character);
		}
	}
}

void AShooterCharacter::Server_AskPunch_Implementation()
{
	NetMulticast_ExecutePunch();
}

bool AShooterCharacter::Server_AskPunch_Validate()
{
	return true;
}

void AShooterCharacter::NetMulticast_ExecutePunch_Implementation()
{
	if (CurrentState == EShooterCharacterState::Aim ||
		IsDead())
		EndAim();

	if (CurrentState != EShooterCharacterState::IdleRun)
		return;

	CurrentState = EShooterCharacterState::Punch;
	StateTimer = 0.f;
}

bool AShooterCharacter::NetMulticast_ExecutePunch_Validate()
{
	return true;
}
//#pragma endregion

//#pragma region Aim
void AShooterCharacter::StartAim()
{
	if (CurrentState != EShooterCharacterState::IdleRun
		|| IsDead())
		return;

	Server_AskStartAim();
}

void AShooterCharacter::EndAim()
{
	if (CurrentState != EShooterCharacterState::Aim)
		return;

	Server_AskEndAim();
}

FRotator AShooterCharacter::GetAimOffsets() const
{
	const FVector aimDirWS = GetBaseAimRotation().Vector();
	const FVector aimDirLS = ActorToWorld().InverseTransformVectorNoScale(aimDirWS);
	const FRotator aimRotLS = aimDirLS.Rotation();

	return aimRotLS;
}

void AShooterCharacter::Server_AskStartAim_Implementation()
{
	NetMulticast_ExecuteStartAim();
}

bool AShooterCharacter::Server_AskStartAim_Validate()
{
	return true;
}

void AShooterCharacter::NetMulticast_ExecuteStartAim_Implementation()
{
	CurrentState = EShooterCharacterState::Aim;
	GetCharacterMovement()->MaxWalkSpeed = AimWalkSpeed;
	TargetSpringArmLength = CameraAimDistance;
	TargetCameraFOV = CameraAimFOV;
}

bool AShooterCharacter::NetMulticast_ExecuteStartAim_Validate()
{
	return true;
}

void AShooterCharacter::Server_AskEndAim_Implementation()
{
	NetMulticast_ExecuteEndAim();
}

bool AShooterCharacter::Server_AskEndAim_Validate()
{
	return true;
}

void AShooterCharacter::NetMulticast_ExecuteEndAim_Implementation()
{
	CurrentState = EShooterCharacterState::IdleRun;
	GetCharacterMovement()->MaxWalkSpeed = RunSpeed;
	TargetSpringArmLength = CameraRestDistance;
	TargetCameraFOV = CameraRestFOV;
}

bool AShooterCharacter::NetMulticast_ExecuteEndAim_Validate()
{
	return true;
}
//#pragma endregion

//#pragma region Sprint
void AShooterCharacter::StartSprint()
{
	if ((CurrentState != EShooterCharacterState::IdleRun &&
			CurrentState != EShooterCharacterState::Aim) ||
		!CanSprint ||
		IsDead())
		return;

	Server_AskStartSprint();
}

void AShooterCharacter::EndSprint()
{
	if (CurrentState != EShooterCharacterState::Sprint)
		return;

	Server_AskEndSprint();
}

void AShooterCharacter::Server_AskStartSprint_Implementation()
{
	NetMulticast_ExecuteStartSprint();
}

bool AShooterCharacter::Server_AskStartSprint_Validate()
{
	return true;
}

void AShooterCharacter::NetMulticast_ExecuteStartSprint_Implementation()
{
	if (CurrentState == EShooterCharacterState::Aim)
		EndAim();

	if (Shooting)
		EndShoot();

	CurrentState = EShooterCharacterState::Sprint;
	GetCharacterMovement()->MaxWalkSpeed = SprintSpeed;
}

bool AShooterCharacter::NetMulticast_ExecuteStartSprint_Validate()
{
	return true;
}

void AShooterCharacter::Server_AskEndSprint_Implementation()
{
	NetMulticast_ExecuteEndSprint();
}

bool AShooterCharacter::Server_AskEndSprint_Validate()
{
	return true;
}

void AShooterCharacter::NetMulticast_ExecuteEndSprint_Implementation()
{
	CurrentState = EShooterCharacterState::IdleRun;
	GetCharacterMovement()->MaxWalkSpeed = RunSpeed;
}

bool AShooterCharacter::NetMulticast_ExecuteEndSprint_Validate()
{
	return true;
}
//#pragma endregion

float AShooterCharacter::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator,
                                    AActor* DamageCauser)
{
	if (Role != ROLE_Authority || IsInvincible())
		return 0;

	if (CurrentState == EShooterCharacterState::Reload)
		AbortReload();

	const float actualDamages = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);

	if (actualDamages > 0)
		NetMulticast_HitFeedback();

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

UCameraComponent* AShooterCharacter::GetCamera() const
{
	return Camera;
}

bool AShooterCharacter::IsInvincible() const
{
	return bInvincible;
}

void AShooterCharacter::VerifyInvincibility(const float deltaTime)
{
	if (IsInvincible())
	{
		TimeBeforeEndInvincibility -= deltaTime;
		if (TimeBeforeEndInvincibility <= 0)
		{
			bInvincible = false;
			GetMesh()->SetRenderCustomDepth(false);
		}
	}
}

void AShooterCharacter::FinishDisapear()
{
	if (Role != ROLE_SimulatedProxy)
	{
		AShooterPlayerController* playerController = Cast<AShooterPlayerController>(GetController());
		if (playerController)
		{
			playerController->Server_RequestRespawn();
		}
	}

	Super::FinishDisapear();
}

void AShooterCharacter::NetMulticast_HitFeedback_Implementation()
{
	GotHit = true;
}

bool AShooterCharacter::NetMulticast_HitFeedback_Validate()
{
	return true;
}
