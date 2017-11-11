// Fill out your copyright notice in the Description page of Project Settings.

#include "ShooterMulti.h"
#include "SetBoolTask.h"
#include "BehaviorTree/BlackboardComponent.h"


EBTNodeResult::Type USetBoolTask::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
    UBlackboardComponent* BlackBoard = OwnerComp.GetBlackboardComponent();
	if (BlackBoard == nullptr)
	{
		return EBTNodeResult::Failed;
	}

	BlackBoard->SetValueAsBool(BlackboardKey.SelectedKeyName, value);
    return EBTNodeResult::Succeeded;

}



