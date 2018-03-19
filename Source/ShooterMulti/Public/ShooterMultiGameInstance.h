// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "UnrealNetwork.h"
#include "Online.h"
#include "FindSessionsCallbackProxy.h"

class UUserWidget;

#include "ShooterMultiGameInstance.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FActionCompletedEvent, bool, value);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FJoinSessionEvent, int, EOnJoinSessionCompleteResult);

UENUM(BlueprintType) //"BlueprintType" is essential to include
enum class ENetworkState : uint8
{
	NS_Host UMETA(DisplayName="Host"),
	NS_Join UMETA(DisplayName="Join"),
	NS_None UMETA(DisplayName="None"),
};

USTRUCT(BlueprintType)
struct FSessionResultData
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	FString ServerName;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	int32 CurrentPlayers;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	int32 MaxPlayers;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	int32 Ping;
};

/**
 * 
 */
UCLASS()
class SHOOTERMULTI_API UShooterMultiGameInstance : public UGameInstance
{
	GENERATED_BODY()

public:
	UShooterMultiGameInstance( const FObjectInitializer& ObjectInitializer );

	virtual void Init() override;

	UFUNCTION()
	virtual void BeginLoadingScreen( const FString& MapName );
	UFUNCTION()
	virtual void EndLoadingScreen();

	void Shutdown() override;

	UFUNCTION(BlueprintCallable, Category = "Network")
	FString GetIPAdress() const;

	UFUNCTION(BlueprintCallable, Category = "Network")
	void ServerTravelToMainMap();

	UFUNCTION(BlueprintCallable, Category = "Network")
	void CreateOnlineGame( FName SessionName, FName MapName, bool bIsLAN, bool bIsPresence, int32 MaxNumPlayers,
	                       FString Options = "" );

	UFUNCTION(BlueprintCallable, Category = "Network")
	void CreateDefaultOnlineGame( bool bIsLAN, bool bIsPresence, int32 MaxNumPlayers, FString Options = "" );

	UFUNCTION(BlueprintCallable, Category = "Network")
	void StartOnlineGame( FName SessionName, FString Options = "" );

	UFUNCTION(BlueprintCallable, Category = "Network")
	void StartDefaultOnlineGame( FString Options = "" );

	UFUNCTION(BlueprintCallable, Category = "Network")
	void JoinOnlineGame_BP( FBlueprintSessionResult Session, const FString& Options );

	UFUNCTION(BlueprintCallable, Category = "Network")
	void JoinFirstOnlineGame( const FString& Options );

	UFUNCTION(BlueprintCallable, Category = "Network")
	void JoinOnlineGame( int32 index, const FString& Options );

	UFUNCTION(BlueprintCallable, Category = "Network")
	void FindOnlineGames();

	UFUNCTION(BlueprintCallable, Category = "Network")
	void DestroySessionAndLeaveGame();

	UFUNCTION(BlueprintCallable, Category = "Network")
	bool IsHosting() const;

	UFUNCTION(BlueprintCallable, Category = "Network")
	bool IsJoining() const;

	UFUNCTION(BlueprintCallable, Category = "Network")
	void StartOnlineGameDirty( FString Options = "" );

	UFUNCTION(BlueprintCallable, Category = "Network")
	void JoinOnlineGameDirty( FString IpAdress, const FString& Options );

	UFUNCTION(BlueprintCallable, Category = "PlayerState")
	void SetPlayerStateName( const FString& Name );

	UFUNCTION(BlueprintCallable, Category = "Network")
	TArray<FSessionResultData> GetSearchResults() const;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	FString LastIpAdress;

//#pragma region BP Events
	UPROPERTY(BlueprintAssignable)
	FActionCompletedEvent OnSessionCreated;

	UPROPERTY(BlueprintAssignable)
	FActionCompletedEvent OnGameStarted;

	UPROPERTY(BlueprintAssignable)
	FActionCompletedEvent OnFindSessionCompleted;

	UPROPERTY(BlueprintAssignable)
	FJoinSessionEvent OnSessionJoined;

	UPROPERTY(BlueprintAssignable)
	FActionCompletedEvent OnSessionLeaved;
//#pragma endregion

protected:
	bool HostSession( TSharedPtr<const FUniqueNetId> UserId, FName SessionName, FName MapName, bool bIsLAN,
	                  bool bIsPresence, int32 MaxNumPlayers );

	bool HostStartSession( FName SessionName, FString GameOptions );

	void FindSessions( TSharedPtr<const FUniqueNetId> UserId, bool bIsLAN, bool bIsPresence );

	bool CustomJoinSession( TSharedPtr<const FUniqueNetId> UserId, FName SessionName,
	                        const FOnlineSessionSearchResult& SearchResult );
	bool JoinSession( ULocalPlayer* LocalPlayer, int32 SessionIndexInSearchResults ) override;

	void EndSession( FName SessionName );
	void DestroySession( FName SessionName );
	void OnFindSessionsComplete( bool bWasSuccessful );

	virtual void OnCreateSessionComplete( FName SessionName, bool bWasSuccessful );

	void OnStartOnlineGameComplete( FName SessionName, bool bWasSuccessful );

	void OnJoinSessionComplete( FName SessionName, EOnJoinSessionCompleteResult::Type Result );

	virtual void OnDestroySessionComplete( FName SessionName, bool bWasSuccessful );

	void OnNetworkFailure( UWorld* world, UNetDriver* netDrive, ENetworkFailure::Type type, const FString& message );
	void OnTravelFailure( UWorld* world, ETravelFailure::Type type, const FString& message );

	void GoToMainMap();

//#pragma region Host Delegates
	/* Delegate called when session created */
	FOnCreateSessionCompleteDelegate OnCreateSessionCompleteDelegate;
	/* Delegate called when session started */
	FOnStartSessionCompleteDelegate OnStartSessionCompleteDelegate;

	/** Handles to registered delegates for creating/starting a session */
	FDelegateHandle OnCreateSessionCompleteDelegateHandle;
	FDelegateHandle OnStartSessionCompleteDelegateHandle;
//#pragma endregion

//#pragma region Find Delegates
	/** Delegate for joining a session */
	FOnJoinSessionCompleteDelegate OnJoinSessionCompleteDelegate;

	/** Handle to registered delegate for joining a session */
	FDelegateHandle OnJoinSessionCompleteDelegateHandle;
//#pragma endregion

//#pragma region Find session Delegates
	/** Delegate for searching for sessions */
	FOnFindSessionsCompleteDelegate OnFindSessionsCompleteDelegate;

	/** Handle to registered delegate for searching a session */
	FDelegateHandle OnFindSessionsCompleteDelegateHandle;
//#pragma endregion

//#pragma region Destroy session Delegates
	/** Delegate for destroying a session */
	FOnDestroySessionCompleteDelegate OnDestroySessionCompleteDelegate;

	/** Handle to registered delegate for destroying a session */
	FDelegateHandle OnDestroySessionCompleteDelegateHandle;
//#pragma endregion

	TSharedPtr<class FOnlineSessionSettings> SessionSettings;

	TSharedPtr<class FOnlineSessionSearch> SessionSearch;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Network")
	FName GameSessionName = "Test";

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Network")
	FName CurrentSessionName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Network")
	FName MainMapName = "MainMap";

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Network")
	FName MenuMapName = "MainMenuMap";

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Network")
	ENetworkState State = ENetworkState::NS_None;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Network")
	FString GameOptions;

	UPROPERTY( EditDefaultsOnly, BlueprintReadOnly, Category = "Loading Interface" )
	TSubclassOf<UUserWidget> loadingWidget;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Game Instance")
	bool bAllowMainMap = true;
};
