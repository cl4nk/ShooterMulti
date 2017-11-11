// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "BaseMarkerWidget.generated.h"

class AShooterMultiGameState;
/**
 * 
 */
UCLASS()
class SHOOTERMULTI_API UBaseMarkerWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:

	UFUNCTION(BlueprintCallable, BlueprintPure)
	FLinearColor GetOutlineColor();
	
	UFUNCTION(BlueprintCallable, BlueprintPure)
	FLinearColor GetMarkerColor() const;

	AShooterMultiGameState * GetShooterGameState();

	UPROPERTY(BlueprintReadOnly)
	AActor * Target;

protected:

	UPROPERTY(EditDefaultsOnly, Category = "Default Colors|Marker")
	FLinearColor DefaultColor = FLinearColor::Blue;

	UPROPERTY(EditDefaultsOnly, Category = "Default Colors|Marker")
	FLinearColor ObjectColor = FLinearColor::Green;

	UPROPERTY(EditDefaultsOnly, Category = "Default Colors|Marker")
	FLinearColor CharacterColor = FLinearColor::White;

	UPROPERTY(EditDefaultsOnly, Category = "Default Colors|Marker")
	FLinearColor UndeadColor = FLinearColor::Red;

private:

	AShooterMultiGameState * ShooterGameState;
};
