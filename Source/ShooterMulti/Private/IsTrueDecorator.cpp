// Fill out your copyright notice in the Description page of Project Settings.

#include "ShooterMulti.h"
#include "IsTrueDecorator.h"
#include "BehaviorTree/BlackboardComponent.h"



bool UIsTrueDecorator::CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const
{
	
	UBlackboardComponent* BlackBoard = OwnerComp.GetBlackboardComponent();
	if (BlackBoard == nullptr)
	{
		return false;
	}

	return BlackBoard->GetValueAsBool(BlackboardKey.SelectedKeyName);

}


