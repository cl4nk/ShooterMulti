// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Tasks/BTTask_BlackboardBase.h"
#include "FindWanderPoint.generated.h"

/**
 * 
 */
UCLASS()
class SHOOTERMULTI_API UFindWanderPoint : public UBTTask_BlackboardBase
{
	GENERATED_BODY()

public:
	EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

protected:

	APawn * Owner;
};
