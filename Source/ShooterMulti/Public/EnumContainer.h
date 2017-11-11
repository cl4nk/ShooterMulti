// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

UENUM(BlueprintType) //"BlueprintType" is essential to include
enum class EGameInstanceState : uint8
{
	GMS_Startup UMETA(DisplayName="Startup"),
	GMS_MainMenu UMETA(DisplayName="Main Menu"),
	GMS_JoinMenu UMETA(DisplayName="Join Menu"),
	GMS_LoadingScreen UMETA(DisplayName="Loading Screen"),
	GMS_ErrorDialog UMETA(DisplayName="Error dialog"),
	GMS_Playing UMETA(DisplayName="Playing"),
	GMS_Unknow UMETA(DisplayName="Unknow")
};

UENUM( BlueprintType )

enum class EShooterMultiState : uint8
{
	SMS_BeforeStart UMETA( DisplayName = "BeforeStart" ),
	SMS_Countdown UMETA( DisplayName = "Countdown" ),
	SMS_Playing UMETA( DisplayName = "Playing" ),
	SMS_EndGame UMETA( DisplayName = "EndGame" ),

	SMS_Unknown UMETA( Hidden )
};
