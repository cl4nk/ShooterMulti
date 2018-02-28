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

	if (Owner == nullptr)
	{
		AController* Controller = Cast<AController>(OwnerComp.GetOwner());
		Owner = Controller->GetPawn();
	}

	FVector location = Owner->GetActorLocation();
	FNavLocation navResult;

	if (NavSystem == nullptr)
	{
		NavSystem = GetWorld()->GetNavigationSystem();
	}

	if (NavData == nullptr && NavSystem)
	{
		NavData = NavSystem->GetMainNavData(FNavigationSystem::ECreateIfEmpty::Invalid);
	}

	if (NavData && NavSystem)
	{
		if (!NavSystem->GetRandomPointInNavigableRadius(location, Radius, navResult, NavData))
		{
			return EBTNodeResult::Failed;
		}
		else
		{
			location = navResult.Location;
		}
	}
	else
	{

	}

	BlackBoard->SetValueAsVector(BlackboardKey.SelectedKeyName, location);
	return EBTNodeResult::Succeeded;
}


