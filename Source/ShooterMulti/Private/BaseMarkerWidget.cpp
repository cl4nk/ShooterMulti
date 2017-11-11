// Fill out your copyright notice in the Description page of Project Settings.

#include "ShooterMulti.h"
#include "BaseMarkerWidget.h"

#include "CharacterWithHealth.h"
#include "InteractiveObject.h"
#include "ShooterCharacter.h"
#include "ShooterMultiGameState.h"



FLinearColor UBaseMarkerWidget::GetOutlineColor()
{
    ACharacterWithHealth * Charac = Cast<ACharacterWithHealth>(Target);
    if (Charac)
    {
        return GetShooterGameState()->GetTeamColor(Charac->GetTeamID());
    }

    return FLinearColor::Transparent;
    
}

FLinearColor UBaseMarkerWidget::GetMarkerColor() const
{
    ACharacterWithHealth * Charac = Cast<ACharacterWithHealth>(Target);
    if (Charac)
    {
        AShooterCharacter * ShooterCharac = Cast<AShooterCharacter>(Charac);
        if (ShooterCharac)
            return CharacterColor;
        else 
            return UndeadColor; 
    }
    AInteractiveObject * Object = Cast<AInteractiveObject>(Target);
    if (Object)
    {
        return ObjectColor;
    }
    
    return DefaultColor;
}

AShooterMultiGameState * UBaseMarkerWidget::GetShooterGameState()
{
    if (ShooterGameState == nullptr)
    {
        AShooterMultiGameState * GameState = GetWorld()->GetGameState<AShooterMultiGameState>();
        if (GameState)
            ShooterGameState = GameState;
    }
    return ShooterGameState;
}
