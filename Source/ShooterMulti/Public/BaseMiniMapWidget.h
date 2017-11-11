// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "BaseMiniMapWidget.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FActorMove, AActor*, Actor);


class UBaseMarkerWidget;

/**
 * 
 */
UCLASS()
class SHOOTERMULTI_API UBaseMiniMapWidget : public UUserWidget
{
	GENERATED_BODY()

public:

	virtual void NativeConstruct() override;

	virtual void BeginDestroy() override;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "MiniMap")
	ESlateVisibility GetMarkerVisibility(AActor * Actor) const;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "MiniMap")
	FVector2D GetMarkerPosition(AActor * Actor) const;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "MiniMap")
	float GetMarkerAngle(AActor * Actor) const;

	UFUNCTION(BlueprintCallable, Category = "MiniMap")
	static bool AddActor (AActor * Actor);

	UFUNCTION(BlueprintCallable, Category = "MiniMap")
	static bool RemoveActor(AActor * Actor);
	
	UFUNCTION(BlueprintCallable, Category = "MiniMap")
	void AddMarker(AActor * Actor);

	UFUNCTION(BlueprintCallable, Category = "MiniMap")
	void RemoveMarker(AActor * Actor);
	
	UFUNCTION(BlueprintImplementableEvent)
	void OnMarkerAdded(UBaseMarkerWidget * Marker);

	UFUNCTION(BlueprintImplementableEvent)
	void OnMarkerRemoved(UBaseMarkerWidget * Marker);
	
protected:

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "MiniMap")
	TSubclassOf<UBaseMarkerWidget> MarkerTemplate;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "MiniMap")
	bool IsCircular = true;

	UPROPERTY(EditAnywhere, Category = "MiniMap")
	float RadarRange = 2000;

	UPROPERTY(EditAnywhere, Category = "MiniMap")
	float DistanceScale = 10;
	
	UPROPERTY(BlueprintReadOnly, Category = "MiniMap")
	TMap<AActor *, UBaseMarkerWidget*> MarkerMap;

	bool alreadyInit = false;

	static FActorMove OnActorAdded;
	static FActorMove OnActorRemoved;

	static TArray<AActor *> ActorList;
};
