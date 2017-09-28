// Fill out your copyright notice in the Description page of Project Settings.

#include "ShooterMulti.h"
#include "DistanceCondition.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/Blackboard/BlackboardKeyType_Object.h"
#include "BehaviorTree/Blackboard/BlackboardKeyType_Vector.h"

UDistanceCondition::UDistanceCondition(const FObjectInitializer& ObjectInitializer) :
	Super(ObjectInitializer)
{
	NodeName = "Distance";

	Distance = 500.f;

	BlackboardKey0.AddObjectFilter(this, FName("Object"), AActor::StaticClass());
	BlackboardKey0.AddVectorFilter(this, FName("Vector"));

	BlackboardKey1.AddObjectFilter(this, FName("Object"), AActor::StaticClass());
	BlackboardKey1.AddVectorFilter(this, FName("Vector"));
}

void UDistanceCondition::InitializeFromAsset(UBehaviorTree& Asset)
{
	Super::InitializeFromAsset(Asset);

	UBlackboardData* BBAsset = GetBlackboardAsset();
	if (BBAsset)
	{
		BlackboardKey0.ResolveSelectedKey(*BBAsset);
		BlackboardKey1.ResolveSelectedKey(*BBAsset);
	}
	else
	{
		UE_LOG(LogBehaviorTree, Warning, TEXT("Can't initialize %s due to missing blackboard data."), *GetName());
		BlackboardKey0.InvalidateResolvedKey();
		BlackboardKey1.InvalidateResolvedKey();
	}
}

bool UDistanceCondition::CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const
{
	float currentDistance = GetKeysDistance(OwnerComp);

	return currentDistance < Distance;
}

float UDistanceCondition::GetKeysDistance(const UBehaviorTreeComponent& OwnerComp) const
{
	const UBlackboardComponent* myBlackboard = OwnerComp.GetBlackboardComponent();

	FVector key0 = GetKeyPosition(myBlackboard, BlackboardKey0);
	FVector key1 = GetKeyPosition(myBlackboard, BlackboardKey1);

	return (key0 - key1).Size();
}

FVector UDistanceCondition::GetKeyPosition(const UBlackboardComponent* blackboardComp, const FBlackboardKeySelector& blackboardKey) const
{
	if (blackboardKey.SelectedKeyType == UBlackboardKeyType_Object::StaticClass())
	{
		AActor* actor = Cast<AActor>(blackboardComp->GetValue<UBlackboardKeyType_Object>(blackboardKey.GetSelectedKeyID()));

		if (actor == nullptr)
			return FVector::ZeroVector;

		return actor->GetActorLocation();
	}
	else
	{
		return blackboardComp->GetValue<UBlackboardKeyType_Vector>(blackboardKey.GetSelectedKeyID());
	}
}

FString UDistanceCondition::GetStaticDescription() const
{
	FString Key0Desc("invalid");

	if (BlackboardKey0.SelectedKeyType == UBlackboardKeyType_Object::StaticClass() ||
		BlackboardKey0.SelectedKeyType == UBlackboardKeyType_Vector::StaticClass())
	{
		Key0Desc = BlackboardKey0.SelectedKeyName.ToString();
	}

	FString Key1Desc("invalid");
	if (BlackboardKey1.SelectedKeyType == UBlackboardKeyType_Object::StaticClass() ||
		BlackboardKey1.SelectedKeyType == UBlackboardKeyType_Vector::StaticClass())
	{
		Key1Desc = BlackboardKey1.SelectedKeyName.ToString();
	}
	
	return FString::Printf(TEXT("%s Is %s Than %f Away From %s"), *Key0Desc, IsInversed() ? TEXT("More") : TEXT("Less"), Distance, *Key1Desc);
}

#if WITH_EDITOR

FName UDistanceCondition::GetNodeIconName() const
{
	return FName("BTEditor.Graph.BTNode.Task.MoveDirectlyToward.Icon");
}

#endif	// WITH_EDITOR