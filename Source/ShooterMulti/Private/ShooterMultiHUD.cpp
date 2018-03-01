// Fill out your copyright notice in the Description page of Project Settings.

#include "ShooterMulti.h"
#include "ShooterPlayerController.h"
#include "ShooterMultiGameState.h"
#include "UserWidget.h"


#include "ShooterMultiHUD.h"

void AShooterMultiHUD::BeginPlay()
{
	Super::BeginPlay();

	RemoveAllWidgets();

	OwningController = Cast<AShooterPlayerController>(PlayerOwner);
	if (OwningController == nullptr)
	{

		return;
	}

	AShooterMultiGameState * gameState = GetWorld()->GetGameState<AShooterMultiGameState>();
	if (gameState == nullptr)
	{

		return;
	}

	OnBeginGame();
	gameState->OnStateChange.AddDynamic(this, &AShooterMultiHUD::OnStateChange);
}

void AShooterMultiHUD::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	RemoveAllWidgets();

	AShooterMultiGameState * gameState = GetWorld()->GetGameState<AShooterMultiGameState>();
	if (gameState == nullptr)
	{

		return;
	}

	gameState->OnStateChange.RemoveDynamic(this, &AShooterMultiHUD::OnStateChange);

	Super::EndPlay(EndPlayReason);
}

void AShooterMultiHUD::OnStateChange(EShooterMultiState state)
{
	RemoveAllWidgets();
	switch (state)
	{
	case EShooterMultiState::SMS_BeforeStart:
	{
		OnBeginGame();
		break;
	}
	case EShooterMultiState::SMS_Countdown:
	{
		OnCountdownGame();
		break;
	}
	case EShooterMultiState::SMS_Playing:
	{
		OnPlayingGame();
		break;
	}
	case EShooterMultiState::SMS_EndGame:
	{
		OnEndGame();
		break;
	}
	case EShooterMultiState::SMS_Unknown:
	default:
		break;
	}
}

void AShooterMultiHUD::CreateMemberWidget(UUserWidget *& instance, TSubclassOf<UUserWidget> widgetClass)
{
	if (instance == nullptr && widgetClass)
	{
		instance = CreateWidget<UUserWidget>(OwningController, widgetClass);
		instance->AddToPlayerScreen();
		instance->SetKeyboardFocus();
		instance->SetUserFocus(OwningController);
	}
}

void AShooterMultiHUD::RemoveWidget(UUserWidget *& instance)
{
	if (instance)
	{
		instance->RemoveFromParent();
		instance = nullptr;
	}
}

void AShooterMultiHUD::InfoPanelEnabled(bool enabled)
{
	if (enabled)
	{
		OwningController->SetInputMode(FInputModeGameAndUI());
		CreateMemberWidget(InfoPanelInstance, InfoPanel);
	}
	else
	{
		OwningController->SetInputMode(FInputModeGameOnly());
		RemoveWidget(InfoPanelInstance);
	}
}

void AShooterMultiHUD::OnBeginGame()
{
	OwningController->SetInputMode(FInputModeUIOnly());
	OwningController->bShowMouseCursor = true;
	CreateMemberWidget(LobbyWidgetInstance, GetLobbyClass());
}

void AShooterMultiHUD::OnCountdownGame()
{
	OwningController->SetInputMode(FInputModeGameOnly());
	OwningController->bShowMouseCursor = false;
	CreateMemberWidget(CountDownWidgetInstance, CountDownWidget);
}

void AShooterMultiHUD::OnPlayingGame()
{
	OwningController->SetInputMode(FInputModeGameOnly());

	OwningController->bShowMouseCursor = false;
	CreateMemberWidget(UIWidgetInstance, UIWidget);
	OwningController->OnInfoEnabled.AddDynamic(this, &AShooterMultiHUD::InfoPanelEnabled);

}

void AShooterMultiHUD::OnEndGame()
{
	OwningController->SetInputMode(FInputModeUIOnly());

	OwningController->bShowMouseCursor = true;
	CreateMemberWidget(EndGameWidgetInstance, EndGameWidget);
	OwningController->OnInfoEnabled.RemoveDynamic(this, &AShooterMultiHUD::InfoPanelEnabled);
}

TSubclassOf<UUserWidget> AShooterMultiHUD::GetLobbyClass() const
{
	FString platformName = UGameplayStatics::GetPlatformName();
	int count = LobbyWidgets.Num();
	for (int i = 0; count; ++i)
	{
		if (LobbyWidgets[i].Platforms.Find(platformName) != INDEX_NONE)
		{
			return LobbyWidgets[i].WidgetClass;
		}
	}
	return nullptr;
}


void AShooterMultiHUD::RemoveAllWidgets()
{
	RemoveWidget(InfoPanelInstance);
	RemoveWidget(LobbyWidgetInstance);
	RemoveWidget(CountDownWidgetInstance);
	RemoveWidget(StartWidgetInstance);
	RemoveWidget(UIWidgetInstance);
	RemoveWidget(EndGameWidgetInstance);
}