// Fill out your copyright notice in the Description page of Project Settings.

#include "ShooterMulti.h"
#include "HUDBlueprintLibrary.h"


void UHUDBlueprintLibrary::FindScreenEdgeLocationForWorldLocation(APlayerController* PlayerController, const FVector& InLocation, const float EdgePercent, FVector2D& OutScreenPosition, float& OutRotationAngleDegrees, bool &bIsOnScreen)
{
	if (PlayerController == nullptr) return;

	bIsOnScreen = false;
	OutRotationAngleDegrees = 0.f;
	FVector2D ScreenPosition = FVector2D();

	const FVector2D ViewportSize = FVector2D(GEngine->GameViewport->Viewport->GetSizeXY());
	const FVector2D  ViewportCenter = FVector2D(ViewportSize.X / 2, ViewportSize.Y / 2);

	float m_invert = 1.0f;
	float m_rotateInvert = 0.0f;

	ACharacter *PlayerCharacter = static_cast<ACharacter *> (PlayerController->GetPawn());

	if (!PlayerCharacter) return;

	FVector m_PlayerLocation = PlayerCharacter->GetActorLocation();

	PlayerController->ProjectWorldLocationToScreen(InLocation, ScreenPosition);

	if (ScreenPosition.X == 0.0f)
	{
		FVector location = FMath::Lerp(InLocation, m_PlayerLocation, 10.0f);
		PlayerController->ProjectWorldLocationToScreen(InLocation, ScreenPosition);
		m_invert = -1.0f;
		m_rotateInvert = 180.0f;
	}
	else
	{
		// Check to see if it's on screen. If it is, ProjectWorldLocationToScreen is all we need, return it.
		if (ScreenPosition.X >= 0 && ScreenPosition.X <= ViewportSize.X
			&& ScreenPosition.Y >= 0 && ScreenPosition.Y <= ViewportSize.Y)
		{
			OutScreenPosition = ScreenPosition;
			OutRotationAngleDegrees = 0.0f;
			bIsOnScreen = true;
			return;
		}
	}

	ScreenPosition -= ViewportCenter;

	float AngleRadians = FMath::Atan2(ScreenPosition.Y, ScreenPosition.X);
	AngleRadians -= FMath::DegreesToRadians(90.f);

	OutRotationAngleDegrees = FMath::RadiansToDegrees(AngleRadians) + 180.f + m_rotateInvert;

	float Cos = cosf(AngleRadians);
	float Sin = -sinf(AngleRadians);

	ScreenPosition = FVector2D(ViewportCenter.X + (Sin * 150.f), ViewportCenter.Y + Cos * 150.f); //Todo: check if necessary

	float m = Cos / Sin;

	FVector2D ScreenBounds = ViewportCenter * EdgePercent;

	if (Cos > 0)
	{
		ScreenPosition = FVector2D(ScreenBounds.Y / m, ScreenBounds.Y);
	}
	else
	{
		ScreenPosition = FVector2D(-ScreenBounds.Y / m, -ScreenBounds.Y);
	}

	if (ScreenPosition.X > ScreenBounds.X)
	{
		ScreenPosition = FVector2D(ScreenBounds.X, ScreenBounds.X*m);
	}
	else if (ScreenPosition.X < -ScreenBounds.X)
	{
		ScreenPosition = FVector2D(-ScreenBounds.X, -ScreenBounds.X*m);
	}

	ScreenPosition *= m_invert;
	ScreenPosition += ViewportCenter;

	OutScreenPosition = ScreenPosition;

}