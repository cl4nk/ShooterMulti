// Fill out your copyright notice in the Description page of Project Settings.

#include "ShooterMulti.h"
#include "CharacterWithHealth.h"
#include "Net/UnrealNetwork.h"
#include "ShooterMultiGameState.h"
#include "MarkerComponent.h"

ACharacterWithHealth::FShooterEvent ACharacterWithHealth::DeathEvent;

// Sets default values
ACharacterWithHealth::ACharacterWithHealth()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	bReplicates = true;
	bReplicateMovement = true;

	BodyColor = FLinearColor::White;

	MarkerComponent = CreateDefaultSubobject<UMarkerComponent>(TEXT("Marker Component"));
	AddOwnedComponent(MarkerComponent);
}

// Called when the game starts or when spawned
void ACharacterWithHealth::BeginPlay()
{
	Super::BeginPlay();

	Disapearing = false;
	ResetHealth();

	for (auto uskelMesh : GetComponentsByClass(USkeletalMeshComponent::StaticClass()))
	{
		auto skelMesh = Cast<USkeletalMeshComponent>(uskelMesh);
		auto mats = skelMesh->GetMaterials();

		for (int i = 0; i < mats.Num(); ++i)
		{
			auto newMat = UMaterialInstanceDynamic::Create(mats[i], this);
			skelMesh->SetMaterial(i, newMat);
			DissolveMaterials.Add(newMat);
		}
	}

	for (auto ustaticMesh : GetComponentsByClass(UStaticMeshComponent::StaticClass()))
	{
		auto staticMesh = Cast<UStaticMeshComponent>(ustaticMesh);
		auto mats = staticMesh->GetMaterials();

		for (int i = 0; i < mats.Num(); ++i)
		{
			auto newMat = UMaterialInstanceDynamic::Create(mats[i]->GetMaterial(), this);
			staticMesh->SetMaterial(i, newMat);
			DissolveMaterials.Add(newMat);
		}
	}
	FVector vectorColor(BodyColor);

	for (auto mat : DissolveMaterials)
	{
		mat->SetScalarParameterValue("DissolveAmmount", 0.f);
		mat->SetVectorParameterValue("BodyColor", vectorColor);
	}
}

void ACharacterWithHealth::Tick(float DeltaTime)
{
	if (IsDead())
	{
		DisapearTimer += DeltaTime;

		if (!Disapearing && DisapearTimer > TimeBeforeDisapearing)
			StartDisapear();

		if (Disapearing)
			UpdateDisapear();
	}
}

void ACharacterWithHealth::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	// Replicate to everyone
	DOREPLIFETIME(ACharacterWithHealth, Health);
	DOREPLIFETIME(ACharacterWithHealth, TeamId);
	DOREPLIFETIME(ACharacterWithHealth, BodyColor);
}

float ACharacterWithHealth::GetHealth()
{
	return Health;
}

float ACharacterWithHealth::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent,
                                       class AController* EventInstigator, AActor* DamageCauser)
{
	float doneDamages = 0.f;

	if (Role != ROLE_Authority)
		return doneDamages;

	const FPointDamageEvent* pointDamage = nullptr;

	if (DamageEvent.IsOfType(FPointDamageEvent::ClassID))
		pointDamage = (FPointDamageEvent *)&DamageEvent;

	if (!IsDead())
	{
		if (pointDamage && pointDamage->HitInfo.PhysMaterial.Get())
			doneDamages = DamageAmount * pointDamage->HitInfo.PhysMaterial->DestructibleDamageThresholdScale;
		else
			doneDamages = DamageAmount;

		Health = FMath::Max(0.f, Health - doneDamages);

		if (Health <= 0.f)
		{
			Die(DamageCauser);
		}
	}

	return doneDamages;
}

float ACharacterWithHealth::GainHealth(float GainAmount)
{
	if (!IsDead())
	{
		if (Health + GainAmount > MaxHealth)
		{
			GainAmount = MaxHealth - Health;
			Health = MaxHealth;
		}
		else
		{
			Health = FMath::Min(MaxHealth, Health + GainAmount);
		}

		return GainAmount;
	}

	return 0.f;
}

void ACharacterWithHealth::ResetHealth()
{
	Health = MaxHealth;
}

bool ACharacterWithHealth::IsDead()
{
	return Health <= 0.f;
}

void ACharacterWithHealth::Die(AActor* Causer)
{
	Health = 0.f;

	DeathEvent.Broadcast(this, Causer);

	Netmulticast_Die();
}

bool ACharacterWithHealth::Netmulticast_Die_Validate()
{
	return true;
}

void ACharacterWithHealth::Netmulticast_Die_Implementation()
{
	auto capsule = GetCapsuleComponent();
	capsule->SetCollisionResponseToAllChannels( ECR_Ignore );
	capsule->SetCollisionResponseToChannel( ECC_WorldStatic, ECR_Block );
	capsule->SetCollisionResponseToChannel( ECC_WorldDynamic, ECR_Block );

	for ( auto aSkeletalMesh : GetComponentsByClass( USkeletalMeshComponent::StaticClass() ) )
	{
		USkeletalMeshComponent* skeletalMesh = Cast<USkeletalMeshComponent>( aSkeletalMesh );
		skeletalMesh->DetachFromComponent( FDetachmentTransformRules::KeepWorldTransform );
		skeletalMesh->bPauseAnims = true;
		skeletalMesh->SetCollisionObjectType( ECC_GameTraceChannel1 );
		skeletalMesh->SetCollisionResponseToAllChannels( ECR_Block );
		skeletalMesh->SetCollisionResponseToChannel( ECC_Pawn, ECR_Ignore );
		skeletalMesh->SetCollisionResponseToChannel( ECC_Camera, ECR_Ignore );
		skeletalMesh->SetCollisionEnabled( ECollisionEnabled::QueryAndPhysics );
		skeletalMesh->SetSimulatePhysics( true );
	}

	MarkerComponent->DestroyComponent(false);
}

void ACharacterWithHealth::StartDisapear()
{
	DisapearTimer = 0.f;
	Disapearing = true;
}

void ACharacterWithHealth::UpdateDisapear()
{
	if (DisapearTimer > DisapearingTime)
		return FinishDisapear();

	float t = DisapearTimer / DisapearingTime;
	for (auto mat : DissolveMaterials)
	{
		mat->SetScalarParameterValue("DissolveAmmount", t);
	}
}

void ACharacterWithHealth::FinishDisapear()
{
	Destroy();
}

void ACharacterWithHealth::ForceColor(FLinearColor Color)
{
	BodyColor = Color;
	OnRep_ChangeColor();
}

void ACharacterWithHealth::ChangeTeam(const int newTeamId)
{
	if (Role == ROLE_Authority)
	{
		TeamId = newTeamId;
		AShooterMultiGameState* gameState = GetWorld()->GetGameState<AShooterMultiGameState>();
		if (gameState)
		{
			BodyColor = gameState->GetTeamColor(TeamId);
			OnRep_ChangeColor();
		}
	}
}

void ACharacterWithHealth::OnRep_ChangeColor()
{
	FVector vectorColor(BodyColor);

	for (auto mat : DissolveMaterials)
	{
		mat->SetVectorParameterValue("BodyColor", vectorColor);
	}
}
