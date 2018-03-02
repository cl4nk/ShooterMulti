// Fill out your copyright notice in the Description page of Project Settings.

#include "ShooterMulti.h"
#include "HUDBlueprintLibrary.h"


void UHUDBlueprintLibrary::FindScreenEdgeLocationForWorldLocation(APlayerController* PlayerController, const FVector& InLocation, const float EdgePercent, FVector2D& OutScreenPosition, float& OutRotationAngleDegrees, bool &bIsOnScreen)
{
	if (PlayerController == nullptr) return;

	bIsOnScreen = false;
	OutRotationAngleDegrees = 0.f;
	FVector2D ScreenPosition = FVector2D();

	int32 ViewportSizeX, ViewportSizeY;
	PlayerController->GetViewportSize(ViewportSizeX, ViewportSizeY);
	const FVector2D ViewportSize = FVector2D(ViewportSizeX, ViewportSizeY);
	const FVector2D ViewportCenter = ViewportSize / 2;
	const FVector2D ViewportHalf = ViewportCenter * EdgePercent;


	float m_invert = 1.0f;
	float m_rotateInvert = 0.0f;

	ACharacter *PlayerCharacter = static_cast<ACharacter *> (PlayerController->GetPawn());

	if (!PlayerCharacter) return;

	FVector m_PlayerLocation = PlayerCharacter->GetActorLocation();

	PlayerController->ProjectWorldLocationToScreen(InLocation, ScreenPosition, true);

	float AngleRadians = FMath::Atan2(ScreenPosition.Y, ScreenPosition.X);
	AngleRadians -= FMath::DegreesToRadians(90.f);

	OutRotationAngleDegrees = FMath::RadiansToDegrees(AngleRadians) + 180.f + m_rotateInvert;

	FVector2D relToCenter = ScreenPosition - ViewportCenter;
	FVector2D relToViewportCenter = relToCenter / ViewportCenter;

	relToViewportCenter.Normalize();

	OutScreenPosition = relToViewportCenter * ViewportHalf + ViewportCenter;
}

