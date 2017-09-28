// Fill out your copyright notice in the Description page of Project Settings.

#include "ShooterMulti.h"
#include "UndeadPunchTask.h"
#include "UndeadAIController.h"

UUndeadPunchTask::UUndeadPunchTask(const FObjectInitializer& ObjectInitializer) :
	Super(ObjectInitializer)
{
	NodeName = "Undead Punch";
}

EBTNodeResult::Type UUndeadPunchTask::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8 * NodeMemory)
{
	AUndeadAIController* owner = Cast<AUndeadAIController>(OwnerComp.GetAIOwner());

	if (owner == nullptr)
		return EBTNodeResult::Failed;

	if (owner->Punch())
		return EBTNodeResult::Succeeded;

	return EBTNodeResult::Failed;
}

FString UUndeadPunchTask::GetStaticDescription() const
{
	return FString::Printf(TEXT("Undead Punch"));
}