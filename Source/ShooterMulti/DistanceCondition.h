// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "BehaviorTree/BTDecorator.h"
#include "DistanceCondition.generated.h"

/**
 * 
 */
UCLASS()

class SHOOTERMULTI_API UDistanceCondition : public UBTDecorator
{
GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category = DistanceCondition)
	FBlackboardKeySelector BlackboardKey0;

	UPROPERTY(EditAnywhere, Category = DistanceCondition)
	float Distance;

	UPROPERTY(EditAnywhere, Category = DistanceCondition)
	FBlackboardKeySelector BlackboardKey1;

	UDistanceCondition(const FObjectInitializer& ObjectInitializer);

	void InitializeFromAsset(UBehaviorTree& Asset) override;

	bool CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const override;

	FString GetStaticDescription() const override;

#if WITH_EDITOR
	FName GetNodeIconName() const override;
#endif // WITH_EDITOR

protected:
	float GetKeysDistance(const UBehaviorTreeComponent& OwnerComp) const;
	FVector GetKeyPosition(const UBlackboardComponent* blackboardComp, const FBlackboardKeySelector& blackboardKey) const;
};
