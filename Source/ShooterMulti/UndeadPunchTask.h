// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "BehaviorTree/BTTaskNode.h"
#include "UndeadPunchTask.generated.h"

/**
 * 
 */
UCLASS()
class SHOOTERMULTI_API UUndeadPunchTask : public UBTTaskNode
{
	GENERATED_BODY()

	UUndeadPunchTask(const FObjectInitializer& ObjectInitializer);

	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8 * NodeMemory) override;

	virtual FString GetStaticDescription() const override;

};
