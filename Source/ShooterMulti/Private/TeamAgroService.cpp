// Fill out your copyright notice in the Description page of Project Settings.

#include "ShooterMulti.h"
#include "TeamAgroService.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "ShooterMultiPlayerState.h"
#include "AIController.h"
#include "CharacterWithHealth.h"


UTeamAgroService::UTeamAgroService(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	bCallTickOnSearchStart = true;
	NodeName = "Team Agro Service";
}

void UTeamAgroService::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	UBlackboardComponent* BlackBoard = OwnerComp.GetBlackboardComponent();
	if (BlackBoard == nullptr)
		return;

	if (Undead == nullptr) 
	{
		AController* Controller = Cast<AController>(OwnerComp.GetOwner());
		Undead = Cast<ACharacterWithHealth>(Controller->GetPawn());

		if (Undead)
			TeamID = Undead->GetTeamID();
	}

	if (Undead == nullptr)
		return;

	UWorld* World = Undead->GetWorld();
	check (World);

	int PlayerCount = UGameplayStatics::GetGameMode(World)->GetNumPlayers();

	float MinDistance = TNumericLimits<float>::Max();
	FVector CurrentLocation = Undead->GetActorLocation();

	AActor* NewTarget = nullptr;

	for (int i = 0; i < PlayerCount; i++)
	{
		ACharacterWithHealth* Target = Cast<ACharacterWithHealth>(UGameplayStatics::GetPlayerCharacter(World, i));

		if (Target && (TeamID == -1 || Target->GetTeamID() != TeamID))
		{
			float DistanceSquared = FVector::DistSquared(CurrentLocation, Target->GetActorLocation());
			if (DistanceSquared < MinDistance && Target->IsDead() == false)
			{
				MinDistance = DistanceSquared;
				NewTarget = Target;
			}
		}
	}
	UObject * OldTarget = BlackBoard->GetValueAsObject(BlackboardKey.SelectedKeyName);

	if (OldTarget != NewTarget)
		BlackBoard->SetValueAsBool(DirtyKey.SelectedKeyName, true);
		
	BlackBoard->SetValueAsObject(BlackboardKey.SelectedKeyName, NewTarget);
}
