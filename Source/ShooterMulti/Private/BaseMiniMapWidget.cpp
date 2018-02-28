// Fill out your copyright notice in the Description page of Project Settings.

#include "ShooterMulti.h"
#include "BaseMiniMapWidget.h"
#include "BaseMarkerWidget.h"
#include "Kismet/KismetMathLibrary.h"

TArray<AActor *> UBaseMiniMapWidget::ActorList;
FActorMove UBaseMiniMapWidget::OnActorAdded;
FActorMove UBaseMiniMapWidget::OnActorRemoved;


void UBaseMiniMapWidget::NativeConstruct()
{
    Super::NativeConstruct();

    if (alreadyInit)
        return;

    for (int i = 0; i < ActorList.Num(); ++i)
        AddMarker(ActorList[i]);

    OnActorAdded.AddDynamic(this, &UBaseMiniMapWidget::AddMarker);
    OnActorRemoved.AddDynamic(this, &UBaseMiniMapWidget::RemoveMarker);

    alreadyInit = true;

}

void UBaseMiniMapWidget::BeginDestroy()
{
    OnActorAdded.RemoveDynamic(this, &UBaseMiniMapWidget::AddMarker);
    OnActorRemoved.RemoveDynamic(this, &UBaseMiniMapWidget::RemoveMarker);

    alreadyInit = false;

    Super::BeginDestroy();
}

ESlateVisibility UBaseMiniMapWidget::GetMarkerVisibility(AActor * Actor) const
{
	APawn * Pawn = GetOwningPlayer()->GetPawn();
	if (Pawn == nullptr)
		return ESlateVisibility::Collapsed;

    if (Actor == nullptr)
        return ESlateVisibility::Collapsed; 

    if (Actor->GetHorizontalDistanceTo(Pawn) > RadarRange)
        return ESlateVisibility::Collapsed;
    else
        return ESlateVisibility::Visible;
}

FVector2D UBaseMiniMapWidget::GetMarkerPosition(AActor * Actor) const
{
	APawn * Pawn = GetOwningPlayer()->GetPawn();
	if (Pawn == nullptr)
		return FVector2D::ZeroVector;

    if (Actor == nullptr)
        return FVector2D::ZeroVector;

    FTransform PlayerTransform = Pawn->GetActorTransform();

    FVector2D Result;
    FVector Result3D = PlayerTransform.InverseTransformPosition(Actor->GetActorLocation()) / DistanceScale;

    Result.X = Result3D.Y;
    Result.Y = Result3D.X * -1;

    return Result;
}

float UBaseMiniMapWidget::GetMarkerAngle(AActor * Actor) const
{
    if (Actor == nullptr)
        return 0.0f; 

    FTransform PlayerTransform = GetOwningPlayer()->PlayerCameraManager->GetActorTransform();
    
    FVector InverseDirection = PlayerTransform.InverseTransformVector(Actor->GetActorForwardVector());

    return UKismetMathLibrary::FindLookAtRotation(FVector::ZeroVector, InverseDirection).Yaw;
}

bool UBaseMiniMapWidget::AddActor(AActor * Actor)
{
    if (ActorList.Contains(Actor))
        return false;

    ActorList.Add(Actor);

    OnActorAdded.Broadcast(Actor);

    return true;
}

bool UBaseMiniMapWidget::RemoveActor(AActor * Actor)
{
    if (!ActorList.Contains(Actor))
        return false;
    
    ActorList.Remove(Actor);
    OnActorRemoved.Broadcast(Actor);

    return true;
}

void UBaseMiniMapWidget::AddMarker(AActor * Actor)
{
    if (MarkerMap.Contains(Actor))
        return;
    APlayerController * Controller = GetOwningPlayer();
    UBaseMarkerWidget * Marker = CreateWidget<UBaseMarkerWidget>(Controller, MarkerTemplate);
    if (Marker == nullptr)
    {
        UE_LOG(LogTemp, Error, TEXT("Marker Null"));
        return;
    }
    Marker->Target = Actor;

    MarkerMap.Add(Actor, Marker);

    OnMarkerAdded(Marker);
}

void UBaseMiniMapWidget::RemoveMarker(AActor * Actor)
{
    if (!MarkerMap.Contains(Actor))
        return;

    UBaseMarkerWidget * Marker = MarkerMap[Actor];
    
    MarkerMap.Remove(Actor);
    OnMarkerRemoved(Marker);
}
