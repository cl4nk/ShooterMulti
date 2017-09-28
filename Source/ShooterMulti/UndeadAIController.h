// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "AIController.h"
#include "UndeadAIController.generated.h"

/**
 * 
 */
UCLASS(ClassGroup = AI, BlueprintType, Blueprintable)
class SHOOTERMULTI_API AUndeadAIController : public AAIController
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UBlackboardData* BlackboardData;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UBehaviorTree* BehaviorTree;

	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintCallable, Category = Character)
	bool Punch();

protected:
	UBlackboardComponent* BlackboardComponent;

};
