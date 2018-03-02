// Fill out your copyright notice in the Description page of Project Settings.

#include "ShooterMulti.h"
#include "FindWanderPoint.h"

#include "BehaviorTree/BlackboardComponent.h"


EBTNodeResult::Type UFindWanderPoint::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	UBlackboardComponent* BlackBoard = OwnerComp.GetBlackboardComponent();
	if (BlackBoard == nullptr)
	{
		return EBTNodeResult::Failed;
	}

	TArray<AActor*> FoundActors;
	UGameplayStatics::GetAllActorsOfClass(OwnerComp.GetWorld(), ATargetPoint::StaticClass(), FoundActors);

	BlackBoard->SetValueAsVector(BlackboardKey.SelectedKeyName, FoundActors[FMath::RandRange(0, FoundActors.Num() - 1)]->GetActorLocation());
	return EBTNodeResult::Succeeded;
}


