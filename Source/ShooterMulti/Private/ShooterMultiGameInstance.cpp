// Fill out your copyright notice in the Description page of Project Settings.

#include "ShooterMulti.h"
#include "ShooterMultiGameInstance.h"
#include "MoviePlayer.h"
#include "UserWidget.h"
#include "Networking.h"
#include "SocketSubsystem.h"

UShooterMultiGameInstance::UShooterMultiGameInstance( const FObjectInitializer& ObjectInitializer )
	: Super( ObjectInitializer )
{
	/** Bind function for CREATING a Session */
	OnCreateSessionCompleteDelegate = FOnCreateSessionCompleteDelegate::CreateUObject(
		this, &UShooterMultiGameInstance::OnCreateSessionComplete );
	OnStartSessionCompleteDelegate = FOnStartSessionCompleteDelegate::CreateUObject(
		this, &UShooterMultiGameInstance::OnStartOnlineGameComplete );

	/** Bind function for FINDING a Session */
	OnFindSessionsCompleteDelegate = FOnFindSessionsCompleteDelegate::CreateUObject(
		this, &UShooterMultiGameInstance::OnFindSessionsComplete );

	/** Bind function for JOINING a Session */
	OnJoinSessionCompleteDelegate = FOnJoinSessionCompleteDelegate::CreateUObject(
		this, &UShooterMultiGameInstance::OnJoinSessionComplete );

	/** Bind function for DESTROYING a Session */
	OnDestroySessionCompleteDelegate = FOnDestroySessionCompleteDelegate::CreateUObject(
		this, &UShooterMultiGameInstance::OnDestroySessionComplete );
}

void UShooterMultiGameInstance::Init()
{
	Super::Init();

	GEngine->NetworkFailureEvent.AddUObject( this, &UShooterMultiGameInstance::OnNetworkFailure );
	GEngine->TravelFailureEvent.AddUObject( this, &UShooterMultiGameInstance::OnTravelFailure );
	//FCoreUObjectDelegates::PreLoadMap.AddUObject( this, &UShooterMultiGameInstance::BeginLoadingScreen );
	//FCoreUObjectDelegates::PostLoadMapWithWorld.AddUObject(this, &UShooterMultiGameInstance::EndLoadingScreen);
}

void UShooterMultiGameInstance::BeginLoadingScreen( const FString& MapName )
{
	if ( !IsRunningDedicatedServer() && loadingWidget )
	{
		FLoadingScreenAttributes LoadingScreen;
		LoadingScreen.bAutoCompleteWhenLoadingCompletes = false;
		LoadingScreen.WidgetLoadingScreen = loadingWidget.GetDefaultObject()->TakeWidget();

		GetMoviePlayer()->SetupLoadingScreen( LoadingScreen );
	}
}

void UShooterMultiGameInstance::EndLoadingScreen()
{
}

void UShooterMultiGameInstance::Shutdown()
{
	bAllowMainMap = false;
	DestroySessionAndLeaveGame();
}

FString UShooterMultiGameInstance::GetIPAdress() const
{
	ISocketSubsystem* SocketSubsystem = ISocketSubsystem::Get();

	bool bindable = false;
	TSharedRef<FInternetAddr> IPref = SocketSubsystem->GetLocalHostAddr( *GLog, bindable );

	return IPref.Get().ToString( false );
}

void UShooterMultiGameInstance::ServerTravelToMainMap()
{
	if ( IsHosting() )
	{
		UWorld* world = WorldContext->World();
		if ( world )
			world->ServerTravel( MainMapName.ToString(), false, false );
	}
}

void UShooterMultiGameInstance::CreateOnlineGame( FName SessionName, FName MapName, bool bIsLAN, bool bIsPresence,
                                                  int32 MaxNumPlayers, FString Options )
{
	GameOptions = Options;
	// Creating a local player where we can get the UserID from
	ULocalPlayer* const Player = GetFirstGamePlayer();

	// Call our custom HostSession function. GameSessionName is a GameInstance variable
	HostSession( Player->GetPreferredUniqueNetId(), SessionName, MapName, bIsLAN, bIsPresence, MaxNumPlayers );
}

void UShooterMultiGameInstance::CreateDefaultOnlineGame( bool bIsLAN, bool bIsPresence, int32 MaxNumPlayers,
                                                         FString Options )
{
	GameOptions = Options;

	// Creating a local player where we can get the UserID from
	ULocalPlayer* const Player = GetFirstGamePlayer();

	// Call our custom HostSession function. GameSessionName is a GameInstance variable
	HostSession( Player->GetPreferredUniqueNetId(), GameSessionName, MainMapName, bIsLAN, bIsPresence, MaxNumPlayers );

	if ( UGameplayStatics::GetPlatformName() == "PS4" || true ) // TODO remove || true for not test PC
	{
		UE_LOG( LogTemp, Warning, TEXT( "CreateDefaultOnlineGame - WE ARE ON PS4" ) );
		for ( int playerId = 1; playerId < MaxNumPlayers; ++playerId )
		{
			UE_LOG( LogTemp, Warning, TEXT( "CreateDefaultOnlineGame - trying to CreateLocalPlayer with playerId[%d]" ), playerId );
			FString outError = "";
			if ( !CreateLocalPlayer( playerId, outError, false ) )
				UE_LOG( LogTemp, Warning, TEXT( "CreateDefaultOnlineGame - error while adding local player [%s]" ), *outError );
		}
	}
}

void UShooterMultiGameInstance::StartOnlineGame( FName SessionName, FString Options )
{
	HostStartSession( SessionName, Options );
}

void UShooterMultiGameInstance::StartDefaultOnlineGame( FString Options )
{
	HostStartSession( GameSessionName, Options );
}

void UShooterMultiGameInstance::JoinOnlineGame_BP( FBlueprintSessionResult Session, const FString& Options )
{
	ULocalPlayer* const Player = GetFirstGamePlayer();

	// Just a SearchResult where we can save the one we want to use, for the case we find more than one!
	FOnlineSessionSearchResult SearchResult = Session.OnlineResult;

	// To avoid something crazy, we filter sessions from ourself
	if ( SearchResult.Session.OwningUserId != Player->GetPreferredUniqueNetId() )
	{
		// Once we found sounce a Session that is not ours, just join it. Instead of using a for loop, you could
		// use a widget where you click on and have a reference for the GameSession it represents which you can use
		// here
		GameOptions = Options;

		CustomJoinSession( Player->GetPreferredUniqueNetId(), GameSessionName, SearchResult );
	}
}

void UShooterMultiGameInstance::JoinOnlineGame( int32 index, const FString& Options )
{
	ULocalPlayer* const Player = GetFirstGamePlayer();
	GameOptions = Options;

	JoinSession( Player, index );
}

void UShooterMultiGameInstance::JoinFirstOnlineGame( const FString& Options )
{
	ULocalPlayer* const Player = GetFirstGamePlayer();

	// Just a SearchResult where we can save the one we want to use, for the case we find more than one!
	FOnlineSessionSearchResult SearchResult;

	FName SessionName = GameSessionName;
	// If the Array is not empty, we can go through it
	if ( SessionSearch->SearchResults.Num() > 0 )
	{
		for ( int32 i = 0; i < SessionSearch->SearchResults.Num(); i++ )
		{
			// To avoid something crazy, we filter sessions from ourself
			if ( SessionSearch->SearchResults[i].Session.OwningUserId != Player->GetPreferredUniqueNetId() )
			{
				SearchResult = SessionSearch->SearchResults[i];

				/*FNamedOnlineSession NamedSession = Cast<FNamedOnlineSession>(SearchResult.Session);
				if (!NamedSession)
					GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Red, "JoinFirstOnlineGame : Session has no name");
				else
					SessionName = NamedSession.SessionName;*/

				// Once we found sounce a Session that is not ours, just join it. Instead of using a for loop, you could
				// use a widget where you click on and have a reference for the GameSession it represents which you can use
				// here
				GameOptions = Options;

				CustomJoinSession( Player->GetPreferredUniqueNetId(), SessionName, SearchResult );
				break;
			}
		}
	}
}

void UShooterMultiGameInstance::FindOnlineGames()
{
	ULocalPlayer* const Player = GetFirstGamePlayer();

	FindSessions( Player->GetPreferredUniqueNetId(), true, true );
}

void UShooterMultiGameInstance::DestroySessionAndLeaveGame()
{
	DestroySession( CurrentSessionName );
}

bool UShooterMultiGameInstance::IsHosting() const
{
	return State == ENetworkState::NS_Host;
}

bool UShooterMultiGameInstance::IsJoining() const
{
	return State == ENetworkState::NS_Join;
}

void UShooterMultiGameInstance::StartOnlineGameDirty( FString Options )
{
	GameOptions = Options;
	UGameplayStatics::OpenLevel( GetWorld(), MainMapName, true, "?listen" + GameOptions );
	State = ENetworkState::NS_Host;
}

void UShooterMultiGameInstance::JoinOnlineGameDirty( FString IpAdress, const FString& Options )
{
	GameOptions = Options;

	FIPv4Address outAdress;
	if ( IpAdress.IsEmpty() || FIPv4Address::Parse( IpAdress, outAdress ) == false )
	{
		UE_LOG( LogTemp, Warning, TEXT("UShooterMultiGameInstance::JoinOnlineGameDirty() - IpAdress is not valid") );
		return;
	}

	LastIpAdress = IpAdress;

	// Assuming you are not already in the PlayerController (if you are, just call ClientTravel directly)
	APlayerController* PlayerController = UGameplayStatics::GetPlayerController( GetWorld(), 0 );
	UE_LOG(LogTemp, Warning, TEXT("JoinOnlineGameDirty Options : %s"), *Options);

	PlayerController->ClientTravel( IpAdress + Options, TRAVEL_Absolute );
	State = ENetworkState::NS_Join;
}

void UShooterMultiGameInstance::SetPlayerStateName( const FString& Name )
{
	APlayerController* PlayerController = UGameplayStatics::GetPlayerController( GetWorld(), 0 );
	APlayerState* PlayerState = PlayerController->PlayerState;

	if ( PlayerState )
		PlayerState->SetPlayerName( Name );
}

TArray<FSessionResultData> UShooterMultiGameInstance::GetSearchResults() const
{
	TArray<FSessionResultData> results;

	for ( int32 i = 0; i < SessionSearch->SearchResults.Num(); i++ )
	{
		FOnlineSessionSearchResult SearchResult = SessionSearch->SearchResults[i];
		FSessionResultData data;
		data.ServerName = SearchResult.Session.OwningUserName;
		data.CurrentPlayers = SearchResult.Session.SessionSettings.NumPublicConnections - SearchResult.Session.
		                                                                                               NumOpenPublicConnections;
		data.MaxPlayers = SearchResult.Session.SessionSettings.NumPublicConnections;
		data.Ping = SearchResult.PingInMs;

		results.Add( data );
	}

	return results;
}

bool UShooterMultiGameInstance::HostSession( TSharedPtr<const FUniqueNetId> UserId, FName SessionName, FName MapName,
                                             bool bIsLAN, bool bIsPresence, int32 MaxNumPlayers )
{
	// Get the Online Subsystem to work with
	IOnlineSubsystem* const OnlineSub = IOnlineSubsystem::Get();

	if ( OnlineSub )
	{
		// Get the Session Interface, so we can call the "CreateSession" function on it
		IOnlineSessionPtr Sessions = OnlineSub->GetSessionInterface();

		if ( Sessions.IsValid() && UserId.IsValid() )
		{
			/* 
				Fill in all the Session Settings that we want to use.
				
				There are more with SessionSettings.Set(...);
				For example the Map or the GameMode/Type.
			*/
			SessionSettings = MakeShareable( new FOnlineSessionSettings() );

			SessionSettings->bIsLANMatch = bIsLAN;
			SessionSettings->bUsesPresence = bIsPresence;
			SessionSettings->NumPublicConnections = MaxNumPlayers;
			SessionSettings->NumPrivateConnections = 0;
			SessionSettings->bAllowInvites = true;
			SessionSettings->bAllowJoinInProgress = true;
			SessionSettings->bShouldAdvertise = true;
			SessionSettings->bAllowJoinViaPresence = true;
			SessionSettings->bAllowJoinViaPresenceFriendsOnly = false;

			SessionSettings->Set( SETTING_MAPNAME, MapName.ToString(), EOnlineDataAdvertisementType::ViaOnlineService );

			// Set the delegate to the Handle of the SessionInterface
			OnCreateSessionCompleteDelegateHandle = Sessions->AddOnCreateSessionCompleteDelegate_Handle(
				OnCreateSessionCompleteDelegate );

			// Our delegate should get called when this is complete (doesn't need to be successful!)
			return Sessions->CreateSession( *UserId, SessionName, *SessionSettings );
		}
	}
	else
	{
		GEngine->AddOnScreenDebugMessage( -1, 10.f, FColor::Red, TEXT("No OnlineSubsytem found!") );
	}

	return false;
}

bool UShooterMultiGameInstance::HostStartSession( FName SessionName, FString Options )
{
	GameOptions = Options;

	// Get the OnlineSubsystem so we can get the Session Interface
	IOnlineSubsystem* OnlineSub = IOnlineSubsystem::Get();
	if ( OnlineSub )
	{
		// Get the Session Interface to call the StartSession function
		IOnlineSessionPtr Sessions = OnlineSub->GetSessionInterface();

		if ( Sessions.IsValid() )
		{
			// Set the StartSession delegate handle
			OnStartSessionCompleteDelegateHandle = Sessions->AddOnStartSessionCompleteDelegate_Handle(
				OnStartSessionCompleteDelegate );

			// Our StartSessionComplete delegate should get called after this
			Sessions->StartSession( SessionName );
			return true;
		}
	}
	else
	{
		GEngine->AddOnScreenDebugMessage( -1, 10.f, FColor::Red, TEXT("No OnlineSubsytem found!") );
	}

	return false;
}

void UShooterMultiGameInstance::FindSessions( TSharedPtr<const FUniqueNetId> UserId, bool bIsLAN, bool bIsPresence )
{
	// Get the OnlineSubsystem we want to work with
	IOnlineSubsystem* OnlineSub = IOnlineSubsystem::Get();

	if ( OnlineSub )
	{
		// Get the SessionInterface from our OnlineSubsystem
		IOnlineSessionPtr Sessions = OnlineSub->GetSessionInterface();

		if ( Sessions.IsValid() && UserId.IsValid() )
		{
			/*
				Fill in all the SearchSettings, like if we are searching for a LAN game and how many results we want to have!
			*/
			SessionSearch = MakeShareable( new FOnlineSessionSearch() );

			SessionSearch->bIsLanQuery = bIsLAN;
			SessionSearch->MaxSearchResults = 20;
			SessionSearch->PingBucketSize = 50;

			// We only want to set this Query Setting if "bIsPresence" is true
			if ( bIsPresence )
			{
				SessionSearch->QuerySettings.Set( SEARCH_PRESENCE, bIsPresence, EOnlineComparisonOp::Equals );
			}

			TSharedRef<FOnlineSessionSearch> SearchSettingsRef = SessionSearch.ToSharedRef();

			// Set the Delegate to the Delegate Handle of the FindSession function
			OnFindSessionsCompleteDelegateHandle = Sessions->AddOnFindSessionsCompleteDelegate_Handle(
				OnFindSessionsCompleteDelegate );

			// Finally call the SessionInterface function. The Delegate gets called once this is finished
			Sessions->FindSessions( *UserId, SearchSettingsRef );
		}
	}
	else
	{
		// If something goes wrong, just call the Delegate Function directly with "false".
		OnFindSessionsComplete( false );
	}
}

bool UShooterMultiGameInstance::CustomJoinSession( TSharedPtr<const FUniqueNetId> UserId, FName SessionName,
                                                   const FOnlineSessionSearchResult& SearchResult )
{
	// Return bool
	bool bSuccessful = false;

	// Get OnlineSubsystem we want to work with
	IOnlineSubsystem* OnlineSub = IOnlineSubsystem::Get();

	if ( OnlineSub )
	{
		// Get SessionInterface from the OnlineSubsystem
		IOnlineSessionPtr Sessions = OnlineSub->GetSessionInterface();

		if ( Sessions.IsValid() && UserId.IsValid() )
		{
			// Set the Handle again
			OnJoinSessionCompleteDelegateHandle = Sessions->AddOnJoinSessionCompleteDelegate_Handle(
				OnJoinSessionCompleteDelegate );

			// Call the "JoinSession" Function with the passed "SearchResult". The "SessionSearch->SearchResults" can be used to get such a
			// "FOnlineSessionSearchResult" and pass it. Pretty straight forward!
			bSuccessful = Sessions->JoinSession( *UserId, SessionName, SearchResult );
		}
	}

	return bSuccessful;
}

bool UShooterMultiGameInstance::JoinSession( ULocalPlayer* LocalPlayer, int32 SessionIndexInSearchResults )
{
	if ( SessionIndexInSearchResults >= 0 && SessionIndexInSearchResults < SessionSearch->SearchResults.Num() )
	{
		FName SessionName = GameSessionName;
		FOnlineSessionSearchResult SearchResult = SessionSearch->SearchResults[SessionIndexInSearchResults];

		/*FNamedOnlineSession NamedSession = Cast<FNamedOnlineSession>(SearchResult.Session);
		if (!NamedSession)
			GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Red, "JoinSession : Session has no name");
		else
			SessionName = NamedSession.SessionName;*/

		ULocalPlayer* const Player = GetFirstGamePlayer();
		return CustomJoinSession( Player->GetPreferredUniqueNetId(), SessionName, SearchResult );
	}
	return false;
}

void UShooterMultiGameInstance::EndSession( FName SessionName )
{
	IOnlineSubsystem* OnlineSub = IOnlineSubsystem::Get();
	if ( OnlineSub )
	{
		IOnlineSessionPtr Sessions = OnlineSub->GetSessionInterface();

		if ( Sessions.IsValid() )
		{
			//TODO: Add it for end session
			//Sessions->AddOnDestroySessionCompleteDelegate_Handle(OnDestroySessionCompleteDelegate);

			Sessions->EndSession( SessionName );
		}
	}

	// if ROLE_Authority tell the clients to end
	/*for (FConstPlayerControllerIterator It 
		= GetWorld()->GetPlayerControllerIterator(); It; ++It)
	{
		AClientPlayerController* PC = Cast<AClientPlayerController>(*It);
		if (PC && !PC->IsLocalPlayerController())
		{
		PC->ClientEndOnlineGame();
		}
	}*/
}

void UShooterMultiGameInstance::DestroySession( FName SessionName )
{
	IOnlineSubsystem* OnlineSub = IOnlineSubsystem::Get();
	if ( OnlineSub )
	{
		IOnlineSessionPtr Sessions = OnlineSub->GetSessionInterface();

		if ( Sessions.IsValid() )
		{
			Sessions->AddOnDestroySessionCompleteDelegate_Handle( OnDestroySessionCompleteDelegate );

			Sessions->DestroySession( SessionName );
		}
	}
}

void UShooterMultiGameInstance::OnFindSessionsComplete( bool bWasSuccessful )
{
	GEngine->AddOnScreenDebugMessage( -1, 10.f, FColor::Red,
	                                  FString::Printf( TEXT("OFindSessionsComplete bSuccess: %d"), bWasSuccessful ) );
	OnFindSessionCompleted.Broadcast( bWasSuccessful );

	// Get OnlineSubsystem we want to work with
	IOnlineSubsystem* const OnlineSub = IOnlineSubsystem::Get();
	if ( OnlineSub )
	{
		// Get SessionInterface of the OnlineSubsystem
		IOnlineSessionPtr Sessions = OnlineSub->GetSessionInterface();
		if ( Sessions.IsValid() )
		{
			// Clear the Delegate handle, since we finished this call
			Sessions->ClearOnFindSessionsCompleteDelegate_Handle( OnFindSessionsCompleteDelegateHandle );

			// Just debugging the Number of Search results. Can be displayed in UMG or something later on
			GEngine->AddOnScreenDebugMessage( -1, 10.f, FColor::Red,
			                                  FString::Printf(
				                                  TEXT("Num Search Results: %d"), SessionSearch->SearchResults.Num() ) );

			// If we have found at least 1 session, we just going to debug them. You could add them to a list of UMG Widgets, like it is done in the BP version!
			if ( SessionSearch->SearchResults.Num() > 0 )
			{
				// "SessionSearch->SearchResults" is an Array that contains all the information. You can access the Session in this and get a lot of information.
				// This can be customized later on with your own classes to add more information that can be set and displayed
				for ( int32 SearchIdx = 0; SearchIdx < SessionSearch->SearchResults.Num(); SearchIdx++ )
				{
					// OwningUserName is just the SessionName for now. I guess you can create your own Host Settings class and GameSession Class and add a proper GameServer Name here.
					// This is something you can't do in Blueprint for example!
					GEngine->AddOnScreenDebugMessage( -1, 10.f, FColor::Red,
					                                  FString::Printf(
						                                  TEXT("Session Number: %d | Sessionname: %s "), SearchIdx + 1,
						                                  *( SessionSearch->SearchResults[SearchIdx].Session.OwningUserName ) ) );
				}
			}
		}
	}
}

void UShooterMultiGameInstance::OnCreateSessionComplete( FName SessionName, bool bWasSuccessful )
{
	GEngine->AddOnScreenDebugMessage( -1, 10.f, FColor::Red,
	                                  FString::Printf(
		                                  TEXT("OnCreateSessionComplete %s, %d"), *SessionName.ToString(), bWasSuccessful ) );
	OnSessionCreated.Broadcast( bWasSuccessful );

	// Get the OnlineSubsystem so we can get the Session Interface
	IOnlineSubsystem* OnlineSub = IOnlineSubsystem::Get();
	if ( OnlineSub )
	{
		// Get the Session Interface to call the StartSession function
		IOnlineSessionPtr Sessions = OnlineSub->GetSessionInterface();

		if ( Sessions.IsValid() )
		{
			// Clear the SessionComplete delegate handle, since we finished this call
			Sessions->ClearOnCreateSessionCompleteDelegate_Handle( OnCreateSessionCompleteDelegateHandle );
			if ( bWasSuccessful )
			{
				// Set the StartSession delegate handle
				OnStartSessionCompleteDelegateHandle = Sessions->AddOnStartSessionCompleteDelegate_Handle(
					OnStartSessionCompleteDelegate );

				// Our StartSessionComplete delegate should get called after this
				Sessions->StartSession( SessionName );
				State = ENetworkState::NS_Host;
			}
		}
	}
}

void UShooterMultiGameInstance::OnStartOnlineGameComplete( FName SessionName, bool bWasSuccessful )
{
	GEngine->AddOnScreenDebugMessage( -1, 10.f, FColor::Red,
	                                  FString::Printf(
		                                  TEXT("OnStartSessionComplete %s, %d"), *SessionName.ToString(), bWasSuccessful ) );
	OnGameStarted.Broadcast( bWasSuccessful );

	FString MapName = MainMapName.ToString();
	// Get the Online Subsystem so we can get the Session Interface
	IOnlineSubsystem* OnlineSub = IOnlineSubsystem::Get();
	if ( OnlineSub )
	{
		// Get the Session Interface to clear the Delegate
		IOnlineSessionPtr Sessions = OnlineSub->GetSessionInterface();
		if ( Sessions.IsValid() )
		{
			// Clear the delegate, since we are done with this call
			Sessions->ClearOnStartSessionCompleteDelegate_Handle( OnStartSessionCompleteDelegateHandle );
			FOnlineSessionSettings* CurrentSettings = Sessions->GetSessionSettings( SessionName );
			CurrentSettings->Get( SETTING_MAPNAME, MapName );
		}
	}

	// If the start was successful, we can open a NewMap if we want. Make sure to use "listen" as a parameter!
	if ( bWasSuccessful )
	{
		CurrentSessionName = SessionName;
		UGameplayStatics::OpenLevel( GetWorld(), FName( *MapName ), true, "?listen" + GameOptions );
		State = ENetworkState::NS_Host;
	}
}

void UShooterMultiGameInstance::OnJoinSessionComplete( FName SessionName, EOnJoinSessionCompleteResult::Type Result )
{
	GEngine->AddOnScreenDebugMessage( -1, 10.f, FColor::Red,
	                                  FString::Printf(
		                                  TEXT("OnJoinSessionComplete %s, %d"), *SessionName.ToString(),
		                                  static_cast<int32>(Result) ) );
	OnSessionJoined.Broadcast( (int)Result );

	// Get the OnlineSubsystem we want to work with
	IOnlineSubsystem* OnlineSub = IOnlineSubsystem::Get();
	if ( OnlineSub )
	{
		// Get SessionInterface from the OnlineSubsystem
		IOnlineSessionPtr Sessions = OnlineSub->GetSessionInterface();

		if ( Sessions.IsValid() )
		{
			// Clear the Delegate again
			Sessions->ClearOnJoinSessionCompleteDelegate_Handle( OnJoinSessionCompleteDelegateHandle );

			// Get the first local PlayerController, so we can call "ClientTravel" to get to the Server Map
			// This is something the Blueprint Node "Join Session" does automatically!
			APlayerController* const PlayerController = GetFirstLocalPlayerController();

			// We need a FString to use ClientTravel and we can let the SessionInterface contruct such a
			// String for us by giving him the SessionName and an empty String. We want to do this, because
			// Every OnlineSubsystem uses different TravelURLs
			FString TravelURL;

			if ( PlayerController && Sessions->GetResolvedConnectString( SessionName, TravelURL ) )
			{
				CurrentSessionName = SessionName;

				// Finally call the ClienTravel. If you want, you could print the TravelURL to see
				// how it really looks like
				PlayerController->ClientTravel( TravelURL + GameOptions, TRAVEL_Absolute );
				State = ENetworkState::NS_Join;
			}
		}
	}
}

void UShooterMultiGameInstance::OnDestroySessionComplete( FName SessionName, bool bWasSuccessful )
{
	GEngine->AddOnScreenDebugMessage( -1, 10.f, FColor::Red,
	                                  FString::Printf(
		                                  TEXT("OnDestroySessionComplete %s, %d"), *SessionName.ToString(),
		                                  bWasSuccessful ) );
	OnSessionLeaved.Broadcast( bWasSuccessful );

	// Get the OnlineSubsystem we want to work with
	IOnlineSubsystem* OnlineSub = IOnlineSubsystem::Get();
	if ( OnlineSub )
	{
		// Get the SessionInterface from the OnlineSubsystem
		IOnlineSessionPtr Sessions = OnlineSub->GetSessionInterface();

		if ( Sessions.IsValid() )
		{
			// Clear the Delegate
			Sessions->ClearOnDestroySessionCompleteDelegate_Handle( OnDestroySessionCompleteDelegateHandle );

			// If it was successful, we just load another level (could be a MainMenu!)
			if ( bWasSuccessful || State != ENetworkState::NS_None )
			{
				State = ENetworkState::NS_None;
				CurrentSessionName = "";
				GoToMainMap();
			}
		}
	}
}

void UShooterMultiGameInstance::OnNetworkFailure( UWorld* world, UNetDriver* netDriver, ENetworkFailure::Type type,
                                                  const FString& message )
{
	GEngine->AddOnScreenDebugMessage( -1, 10.f, FColor::Red,
	                                  FString::Printf(
		                                  TEXT(
			                                  "UShooterMultiGameInstance::OnNetworkFailure() - world [%p] | netDriver [%p] | type [%d] | message [%s]"
		                                  ), world, netDriver, (int)type, *message ) );

	UE_LOG( LogTemp, Error, TEXT(
			"UShooterMultiGameInstance::OnNetworkFailure() - world [%p] | netDriver [%p] | type [%d] | message [%s]"), world,
		netDriver, (int)type, *message )
	;
}

void UShooterMultiGameInstance::OnTravelFailure( UWorld* world, ETravelFailure::Type type, const FString& message )
{
	GEngine->AddOnScreenDebugMessage( -1, 10.f, FColor::Red,
	                                  FString::Printf(
		                                  TEXT(
			                                  "UShooterMultiGameInstance::OnTravelFailure() - world [%p] | type [%d] | message [%s]"
		                                  ), world, (int)type, *message ) );

	UE_LOG( LogTemp, Error, TEXT("UShooterMultiGameInstance::OnTravelFailure() - world [%p] | type [%d] | message [%s]"),
		world, (int)type, *message );
}

void UShooterMultiGameInstance::GoToMainMap()
{
	if ( bAllowMainMap )
	{
		UWorld* world = WorldContext->World();
		if ( world )
		{
			const int32 localPlayersNum = LocalPlayers.Num();
			for ( int i = localPlayersNum - 1; i >= 0 ; --i )
				RemoveLocalPlayer( LocalPlayers[i] );

			UGameplayStatics::OpenLevel( world, MenuMapName, true );
		}
	}
}
