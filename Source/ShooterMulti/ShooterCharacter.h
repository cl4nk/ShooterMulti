// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CharacterWithHealth.h"

class USoundBase;
class UCameraShake;
class UInteractionSphere;
class UShooterCharacterAnim;

// TODO: find a way to remove this
#include "WeaponUtility.h"

#include "ShooterCharacter.generated.h"

UENUM(BlueprintType)

enum class EShooterCharacterState : uint8
{
	IdleRun,
	Aim,
	Sprint,
	Reload,
	Jump,
	Punch,
	Dead
};

UCLASS()

class SHOOTERMULTI_API AShooterCharacter : public ACharacterWithHealth
{
GENERATED_BODY()

private:
	const bool IS_LOCAL_MULTIPLAYER = UGameplayStatics::GetPlatformName() == "PS4"; // remove "|| true" to deactivate local multiplayer on PC

public:
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = ShooterCharacter)
	float SprintSpeed = 1000.f;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = ShooterCharacter)
	float AimWalkSpeed = 180.f;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = ShooterCharacter)
	float ReloadWalkSpeed = 200.f;

	UPROPERTY(Transient, BlueprintReadOnly)
	float RunSpeed;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = ShooterCharacter, meta = (ClampMin = "0.0", ClampMax = "1.0",
		UIMin = "0.0", UIMax = "1.0"))
	float MinSprintMagnitude = .3f;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = ShooterCharacter, meta = (ClampMin = "0.0"))
	float CameraAimDistance = 100.f;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = ShooterCharacter, meta = (ClampMin = "0.0"))
	float CameraAimFOV = 75.f;

	UPROPERTY(Transient, BlueprintReadOnly)
	float CameraRestDistance;

	UPROPERTY(Transient, BlueprintReadOnly)
	float CameraRestFOV;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = ShooterCharacter, meta = (ClampMin = "0"))
	float PunchDuration = 1.f;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = ShooterCharacter)
	TSubclassOf<UCameraShake> RunShake;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = ShooterCharacter)
	TSubclassOf<UCameraShake> SprintShake;

//#pragma region
public:
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "ShooterCharacter|Weapon", meta = (ClampMin = "0"))
	int MaxTotalAmmoLeft = 120;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "ShooterCharacter|Weapon", meta = (ClampMin = "0"))
	int MaxAmmoInMag = 20;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "ShooterCharacter|Weapon", meta = (ClampMin = "0"))
	float FireRate = .24f;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "ShooterCharacter|Weapon")
	USoundBase* ShotSound;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "ShooterCharacter|Weapon")
	USoundBase* ShotEmptySound;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "ShooterCharacter|Weapon")
	UMaterial* ImpactDecalMat;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "ShooterCharacter|Weapon")
	float ImpactDecalLifeSpan = 30.f;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "ShooterCharacter|Weapon")
	float ImpactDecalSize = 10.f;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "ShooterCharacter|Weapon")
	class UParticleSystem* ImpactParticle;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "ShooterCharacter|Weapon")
	class USoundBase* ImpactSound;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "ShooterCharacter|Weapon")
	class UParticleSystem* BeamParticle;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "ShooterCharacter|Weapon")
	float BeamIntensity = 5000.f;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "ShooterCharacter|Weapon")
	UCurveFloat* BeamIntensityCurve;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "ShooterCharacter|Weapon")
	class UParticleSystem* MuzzleSmokeParticle;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "ShooterCharacter|Weapon")
	float WeaponDamages = 15.f;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "ShooterCharacter|Weapon")
	float WeaponPunchDamages = 30.f;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "ShooterCharacter|Weapon")
	float WeaponKnokback = 300000.f;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "ShooterCharacter|Weapon")
	float WeaponMinSpreadAim = 2.f;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "ShooterCharacter|Weapon")
	float WeaponMinSpreadWalk = 4.f;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "ShooterCharacter|Weapon")
	float WeaponMaxSpread = 15.f;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "ShooterCharacter|Weapon")
	float WeaponSpreadPerShot = 4.f;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "ShooterCharacter|Weapon")
	float WeaponSpreadRecoveryRate = 1.5f;

	UPROPERTY(Transient, BlueprintReadOnly, Category = "ShooterCharacter|Weapon")
	float CurrentSpread;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "ShooterCharacter|Weapon")
	TSubclassOf<UCameraShake> ShootShake;
//#pragma endregion

public:
	UPROPERTY(Transient, BlueprintReadOnly)
	EShooterCharacterState CurrentState = EShooterCharacterState::IdleRun;

	UPROPERTY(Category = UndeadCharacter, VisibleDefaultsOnly, BlueprintReadOnly)
	USphereComponent* PunchCollision;

	UPROPERTY(Category = UndeadCharacter, VisibleDefaultsOnly, BlueprintReadOnly)
	UInteractionSphere* InteractionSphere;

	// Sets default values for this character's properties
	AShooterCharacter();

	// Called when the game starts or when spawned
	void BeginPlay() override;

	// Called every frame
	void Tick(float DeltaSeconds) override;

	UFUNCTION(Client, Reliable, WithValidation)
	void Client_Possess();
	void Client_Possess_Implementation();
	bool Client_Possess_Validate();

	void OnServerPossess();

	void Turn(float Value);
	void LookUp(float Value);

	void MoveForward(float Value);
	void MoveRight(float Value);

	void StartJump();
	void EndJump();

	void Interact();

	UFUNCTION(BlueprintCallable, Category = Health)
	float TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator,
	                 AActor* DamageCauser) override;

	void FinishDisapear() override;

	bool ConsumeHitTrigger();

	UFUNCTION(BlueprintCallable)
	UCameraComponent* GetCamera() const;

//#pragma region Start Invincibility
public:
	UFUNCTION( BlueprintCallable, Category = "ShooterCharacter|Invincibility" )
	bool IsInvincible() const;

protected:
	UPROPERTY( BlueprintReadOnly, EditDefaultsOnly, Category = "ShooterCharacter|Invincibility", meta = ( ClampMin = "0.0"
	) )
	float InvincibilityAtSpawn = 1.f;
	UPROPERTY( BlueprintReadOnly, VisibleAnywhere, Transient, Category = "ShooterCharacter|Invincibility" )
	float TimeBeforeEndInvincibility;
	UPROPERTY( BlueprintReadOnly, VisibleAnywhere, Transient, Category = "ShooterCharacter|Invincibility" )
	bool bInvincible;

	UFUNCTION( BlueprintCallable, Category = "ShooterCharacter|Invincibility" )
	void VerifyInvincibility(const float deltaTime);
//#pragma endregion

//#pragma region Reload
public:
	UFUNCTION(BlueprintPure, Category = "ShooterCharacter|Munitions")
	int GetAmmoInMag() const;
	UFUNCTION(BlueprintPure, Category = "ShooterCharacter|Munitions")
	int GetAmmoLeft() const;

	UFUNCTION(BlueprintCallable, Category = "ShooterCharacter|Munitions")
	void AddAmmo(const int ammoReload);
	UFUNCTION(BlueprintCallable, Category = "ShooterCharacter|Munitions")
	void Reload();
	UFUNCTION(BlueprintCallable, Category = "ShooterCharacter|Munitions")
	void EndReload();
	UFUNCTION(BlueprintCallable, Category = "ShooterCharacter|Munitions")
	void AbortReload();

protected:
	UFUNCTION(Server, Reliable, WithValidation, BlueprintCallable, Category = "ShooterCharacter|Munitions")
	void Server_AskAddAmmo(const int ammoReload);
	void Server_AskAddAmmo_Implementation(const int ammoReload);
	bool Server_AskAddAmmo_Validate(const int ammoReload);

	UFUNCTION(NetMulticast, Reliable, WithValidation, BlueprintCallable, Category = "ShooterCharacter|Munitions")
	void NetMulticast_ExecuteAddAmmo(const int ammoReload);
	void NetMulticast_ExecuteAddAmmo_Implementation(const int ammoReload);
	bool NetMulticast_ExecuteAddAmmo_Validate(const int ammoReload);

	UFUNCTION( Server, Reliable, WithValidation, BlueprintCallable, Category = "ShooterCharacter|Munitions" )
	void Server_AskReload();
	void Server_AskReload_Implementation();
	bool Server_AskReload_Validate();

	UFUNCTION( NetMulticast, Reliable, WithValidation, BlueprintCallable, Category = "ShooterCharacter|Munitions" )
	void NetMulticast_ExecuteReload();
	void NetMulticast_ExecuteReload_Implementation();
	bool NetMulticast_ExecuteReload_Validate();

	UFUNCTION( NetMulticast, Reliable, WithValidation, BlueprintCallable, Category = "ShooterCharacter|Munitions" )
	void NetMulticast_ExecuteEndReload();
	void NetMulticast_ExecuteEndReload_Implementation();
	bool NetMulticast_ExecuteEndReload_Validate();

	UFUNCTION( NetMulticast, Reliable, WithValidation, BlueprintCallable, Category = "ShooterCharacter|Munitions" )
	void NetMulticast_ExecuteAbortReload();
	void NetMulticast_ExecuteAbortReload_Implementation();
	bool NetMulticast_ExecuteAbortReload_Validate();
//#pragma endregion

//#pragma region Shooting
public:
	UFUNCTION( BlueprintCallable, Category = "ShooterCharacter|Shooting" )
	void StartShoot();
	UFUNCTION( BlueprintCallable, Category = "ShooterCharacter|Shooting" )
	void EndShoot();
	UFUNCTION(BlueprintPure, Category = "ShooterCharacter")
	bool IsShooting() const;

protected:
	UFUNCTION( Server, Reliable, WithValidation, BlueprintCallable, Category = "ShooterCharacter|Shooting" )
	void Server_TakeShot(float shootSpread);
	void Server_TakeShot_Implementation(float shootSpread);
	bool Server_TakeShot_Validate(float shootSpread);

	UFUNCTION( NetMulticast, Reliable, WithValidation, BlueprintCallable, Category = "ShooterCharacter|Shooting" )
	void NetMulticast_MakeImpactFeedback(FHitResult hitResult);
	void NetMulticast_MakeImpactFeedback_Implementation(FHitResult hitResult);
	bool NetMulticast_MakeImpactFeedback_Validate(FHitResult hitResult);

	UFUNCTION( NetMulticast, Reliable, WithValidation, BlueprintCallable, Category = "ShooterCharacter|Shooting" )
	void NetMulticast_MakeBeamFeedback(FHitResult hitResult, FLaserWeaponData weaponData);
	void NetMulticast_MakeBeamFeedback_Implementation(FHitResult hitResult, FLaserWeaponData weaponData);
	bool NetMulticast_MakeBeamFeedback_Validate(FHitResult hitResult, FLaserWeaponData weaponData);
//#pragma endregion

//#pragma region Punch
public:
	UFUNCTION( BlueprintCallable, Category = "ShooterCharacter|Punch" )
	void Punch();
	UFUNCTION( BlueprintCallable, Category = "ShooterCharacter|Punch" )
	void InflictPunch();

protected:
	UFUNCTION( Server, Reliable, WithValidation, BlueprintCallable, Category = "ShooterCharacter|Punch" )
	void Server_AskPunch();
	void Server_AskPunch_Implementation();
	bool Server_AskPunch_Validate();

	UFUNCTION( NetMulticast, Reliable, WithValidation, BlueprintCallable, Category = "ShooterCharacter|Punch" )
	void NetMulticast_ExecutePunch();
	void NetMulticast_ExecutePunch_Implementation();
	bool NetMulticast_ExecutePunch_Validate();
//#pragma endregion

//#pragma region Aim
public:
	UFUNCTION( BlueprintCallable, Category = "ShooterCharacter|Aim" )
	void StartAim();
	UFUNCTION( BlueprintCallable, Category = "ShooterCharacter|Aim" )
	void EndAim();
	UFUNCTION( BlueprintCallable, Category = "ShooterCharacter|Aim" )
	FRotator GetAimOffsets() const;

protected:
	UFUNCTION( Server, Reliable, WithValidation, BlueprintCallable, Category = "ShooterCharacter|Aim" )
	void Server_AskStartAim();
	void Server_AskStartAim_Implementation();
	bool Server_AskStartAim_Validate();

	UFUNCTION( NetMulticast, Reliable, WithValidation, BlueprintCallable, Category = "ShooterCharacter|Aim" )
	void NetMulticast_ExecuteStartAim();
	void NetMulticast_ExecuteStartAim_Implementation();
	bool NetMulticast_ExecuteStartAim_Validate();

	UFUNCTION( Server, Reliable, WithValidation, BlueprintCallable, Category = "ShooterCharacter|Aim" )
	void Server_AskEndAim();
	void Server_AskEndAim_Implementation();
	bool Server_AskEndAim_Validate();

	UFUNCTION( NetMulticast, Reliable, WithValidation, BlueprintCallable, Category = "ShooterCharacter|Aim" )
	void NetMulticast_ExecuteEndAim();
	void NetMulticast_ExecuteEndAim_Implementation();
	bool NetMulticast_ExecuteEndAim_Validate();
//#pragma endregion

//#pragma region Sprint
public:
	UFUNCTION( BlueprintCallable, Category = "ShooterCharacter|Sprint" )
	void StartSprint();
	UFUNCTION( BlueprintCallable, Category = "ShooterCharacter|Sprint" )
	void EndSprint();

protected:
	UFUNCTION( Server, Reliable, WithValidation, BlueprintCallable, Category = "ShooterCharacter|Sprint" )
	void Server_AskStartSprint();
	void Server_AskStartSprint_Implementation();
	bool Server_AskStartSprint_Validate();

	UFUNCTION( NetMulticast, Reliable, WithValidation, BlueprintCallable, Category = "ShooterCharacter|Sprint" )
	void NetMulticast_ExecuteStartSprint();
	void NetMulticast_ExecuteStartSprint_Implementation();
	bool NetMulticast_ExecuteStartSprint_Validate();

	UFUNCTION( Server, Reliable, WithValidation, BlueprintCallable, Category = "ShooterCharacter|Sprint" )
	void Server_AskEndSprint();
	void Server_AskEndSprint_Implementation();
	bool Server_AskEndSprint_Validate();

	UFUNCTION( NetMulticast, Reliable, WithValidation, BlueprintCallable, Category = "ShooterCharacter|Sprint" )
	void NetMulticast_ExecuteEndSprint();
	void NetMulticast_ExecuteEndSprint_Implementation();
	bool NetMulticast_ExecuteEndSprint_Validate();
//#pragma endregion

	UFUNCTION( NetMulticast, Reliable, WithValidation, BlueprintCallable, Category = "ShooterCharacter" )
	void NetMulticast_HitFeedback();
	void NetMulticast_HitFeedback_Implementation();
	bool NetMulticast_HitFeedback_Validate();

	bool CanSprint;

	UCameraComponent* Camera;
	USpringArmComponent* SpringArm;
	UMeshComponent* weaponMesh;
	float TargetSpringArmLength;
	float TargetCameraFOV;

	int AmmoLeft;
	int AmmoInMag;

	bool Shooting;
	float ShootTimer;
	float StateTimer;

	bool GotHit;

	UShooterCharacterAnim* ShooterCharacterAnim;
};
