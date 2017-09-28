// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "BehaviorTree/BTDecorator.h"
#include "DirectorAllowPunchCondition.generated.h"

/**
 * 
 */
UCLASS()
class SHOOTERMULTI_API UDirectorAllowPunchCondition : public UBTDecorator
{
	GENERATED_BODY()
	
public:

	UDirectorAllowPunchCondition();

	virtual bool CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const override;

	virtual FString GetStaticDescription() const override;
	
};
