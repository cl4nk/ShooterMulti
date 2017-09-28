// Fill out your copyright notice in the Description page of Project Settings.

#include "ShooterMulti.h"
#include "DirectorAllowPunchCondition.h"
#include "UndeadDirector.h"

UDirectorAllowPunchCondition::UDirectorAllowPunchCondition() :
	Super()
{
	NodeName = "Director Allow Punch";
}

bool UDirectorAllowPunchCondition::CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const
{
	AUndeadDirector* directior = AUndeadDirector::GetCurrent();
	return !directior || AUndeadDirector::GetCurrent()->PunchAvailable();
}

FString UDirectorAllowPunchCondition::GetStaticDescription() const
{
	return FString(TEXT("Director Allow Punch"));
}