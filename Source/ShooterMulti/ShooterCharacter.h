// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CharacterWithHealth.h"

class USoundBase;
class UCameraShake;

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

public:
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = ShooterCharacter)
	float SprintSpeed = 1000.f;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = ShooterCharacter)
	float AimWalkSpeed = 180.f;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = ShooterCharacter)
	float ReloadWalkSpeed = 200.f;

	UPROPERTY(Transient, BlueprintReadOnly)
	float RunSpeed;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = ShooterCharacter, meta = (ClampMin = "0.0", ClampMax = "1.0", UIMin = "0.0", UIMax = "1.0"))
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

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "ShooterCharacter|Weapon", meta = (ClampMin = "0"))
	int MaxAmmo = 50;

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

	UPROPERTY(Transient, BlueprintReadOnly)
	EShooterCharacterState CurrentState = EShooterCharacterState::IdleRun;

	UPROPERTY(Category = UndeadCharacter, VisibleDefaultsOnly, BlueprintReadOnly)
	USphereComponent* PunchCollision;

	DECLARE_EVENT_OneParam(ACharacterWithHealth, FShooterEvent, AShooterCharacter*)
	static FShooterEvent DeathEvent;

	// Sets default values for this character's properties
	AShooterCharacter();

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	// Called every frame
	virtual void Tick( float DeltaSeconds ) override;

	void Turn(float Value);
	void LookUp(float Value);

	void MoveForward(float Value);
	void MoveRight(float Value);

	void StartJump();
	void EndJump();

	void StartSprint();
	void EndSprint();

	void StartAim();
	void EndAim();

	UFUNCTION(BlueprintPure, Category = "Shooter|Character")
	bool IsShooting();
	UFUNCTION(BlueprintCallable, Category = "Shooter|Character")
	void StartShoot();
	UFUNCTION(BlueprintCallable, Category = "Shooter|Character")
	void EndShoot();

	UFUNCTION(BlueprintPure, Category = "Shooter|Character")
	int GetAmmo();
	UFUNCTION(BlueprintCallable, Category = "Shooter|Character")
	void Reload();
	UFUNCTION(BlueprintCallable, Category = "Shooter|Character")
	void EndReload();
	UFUNCTION(BlueprintCallable, Category = "Shooter|Character")
	void AbortReload();

	UFUNCTION(BlueprintCallable, Category = "Shooter|Character")
	void Punch();

	UFUNCTION(BlueprintCallable, Category = Character)
	void InflictPunch();

	void TakeShot();

	UFUNCTION(BlueprintCallable, Category = Health)
	virtual float TakeDamage(float DamageAmount, FDamageEvent const & DamageEvent, AController * EventInstigator, AActor * DamageCauser) override;

	virtual void Die() override;

	virtual void FinishDisapear() override;

	bool ConsumeHitTrigger();

private:
	bool CanSprint;

	USpringArmComponent* SpringArm;
	UCameraComponent* Camera;
	float TargetSpringArmLength;
	float TargetCameraFOV;

	int Ammo;

	bool Shooting;
	float ShootTimer;
	float StateTimer;

	bool GotHit;
};
