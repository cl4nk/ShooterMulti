// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Services/BTService_BlackboardBase.h"
#include "TeamAgroService.generated.h"

/**
 * 
 */
UCLASS()

class SHOOTERMULTI_API UTeamAgroService : public UBTService_BlackboardBase
{
GENERATED_UCLASS_BODY()
protected:

	void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;

	UPROPERTY(EditAnywhere)
	FBlackboardKeySelector DirtyKey;
};
