// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "EnumContainer.h"

#include "ShooterMultiHUD.generated.h"

class UUserWidget;
class AShooterPlayerController;

USTRUCT(BlueprintType)
struct FSpecificWidget
{
	GENERATED_BODY()
public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TArray<FString> Platforms;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TSubclassOf<UUserWidget> WidgetClass;
};

/**
 * 
 */
UCLASS()
class SHOOTERMULTI_API AShooterMultiHUD : public AHUD
{
	GENERATED_BODY()
	
	
protected:

	void BeginPlay() override;

	void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	UFUNCTION()
	void OnStateChange(EShooterMultiState state);

	void CreateMemberWidget(UUserWidget* & instance, TSubclassOf<UUserWidget> widgetClass, int32 ZOrder = 0);

	void RemoveWidget(UUserWidget* & instance);

	UFUNCTION()
	void RemoveStartWidget();

	UFUNCTION()
	void InfoPanelEnabled(bool enabled);

	UFUNCTION()
	void OnBeginGame();

	UFUNCTION()
	void OnCountdownGame();

	UFUNCTION()
	void OnPlayingGame();

	UFUNCTION()
	void OnEndGame();
	
	TSubclassOf<UUserWidget> GetLobbyClass() const;

	void RemoveAllWidgets();

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TSubclassOf<UUserWidget> InfoPanel;

	UUserWidget* InfoPanelInstance = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TArray<FSpecificWidget> LobbyWidgets;

	UUserWidget* LobbyWidgetInstance = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TSubclassOf<UUserWidget> CountDownWidget;

	UUserWidget* CountDownWidgetInstance = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TSubclassOf<UUserWidget> StartWidget;

	UUserWidget* StartWidgetInstance = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float StartDuration = 1.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TSubclassOf<UUserWidget> UIWidget;

	UUserWidget* UIWidgetInstance = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TSubclassOf<UUserWidget> EndGameWidget;

	UUserWidget* EndGameWidgetInstance = nullptr;

	AShooterPlayerController * OwningController = nullptr;

	FDelegateHandle StateEventHandle;
	FDelegateHandle InfoEventHandle;

	FTimerHandle  StartWidgetTimer;
};
