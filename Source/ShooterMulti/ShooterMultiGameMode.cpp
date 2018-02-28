// Fill out your copyright notice in the Description page of Project Settings.

#include "ShooterMulti.h"
#include "ShooterMultiGameMode.h"
#include "ShooterCharacter.h"
#include "ShooterMultiGameState.h"
#include "ShooterPlayerController.h"
#include "ShooterMultiPlayerState.h"
#include "ShooterMultiGameInstance.h"

AShooterMultiGameMode::AShooterMultiGameMode()
	: Super()
	, ScoreToWin( 0 )
	, NumberOfTeams( 2 )
	, CountdownTime( 3.f )
{
	bStartPlayersAsSpectators = true;
	bUseSeamlessTravel = true;
}

//#pragma region AGameModeBase
void AShooterMultiGameMode::BeginPlay()
{
	Super::BeginPlay();

	ACharacterWithHealth::DeathEvent.Clear();
	AShooterMultiPlayerState::OnReady.Clear();

	DeathEventHandle = ACharacterWithHealth::DeathEvent.AddLambda([this](ACharacterWithHealth* Killed, AActor* Causer)
	{
		AddDeath(Killed, Causer);
	});

	ReadyCountHandle = AShooterMultiPlayerState::OnReady.AddLambda([this](bool state)
	{
		OnPlayerReady();
	});

	TArray<AActor *> listActors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AShooterMultiPlayerStart::StaticClass(), listActors);

	int highestSpawnGroupID = -1;
	for (AActor* oneActor : listActors)
	{
		AShooterMultiPlayerStart* castedActor = Cast<AShooterMultiPlayerStart>(oneActor);
		if (castedActor)
		{
			const int groupID = castedActor->GetSpawnGroupID();
			if (groupID > highestSpawnGroupID)
				highestSpawnGroupID = groupID;

			ListPlayerStarts.AddUnique(castedActor);
		}
	}
	if (NumberOfTeams < highestSpawnGroupID)
	UE_LOG(LogTemp, Error, TEXT("AShooterMultiGameMode::BeginPlay() - there's more teams that spawn groups"));

	ChangeState(EShooterMultiState::SMS_BeforeStart);
}

void AShooterMultiGameMode::Destroyed()
{
	ACharacterWithHealth::DeathEvent.Remove(DeathEventHandle);
	AShooterMultiPlayerState::OnReady.Remove(ReadyCountHandle);
	Super::Destroyed();
}

void AShooterMultiGameMode::GenericPlayerInitialization(AController* NewController)
{
	Super::GenericPlayerInitialization(NewController);

	AShooterMultiGameState* castedGameState = GetShooterGameState();

	const EShooterMultiState shooterMultiState = castedGameState->GetCurrentState();
	if (shooterMultiState != EShooterMultiState::SMS_Playing)
	{
		AShooterPlayerController* playerController = Cast<AShooterPlayerController>(NewController);
		if ( playerController )
		{
			const bool bIgnoreLook = shooterMultiState == EShooterMultiState::SMS_BeforeStart ? true : false;
			playerController->Client_SetIgnoreAllInputs( bIgnoreLook );
			playerController->Client_SetIgnoreMoveLookInput( true, bIgnoreLook );
		}
		else
			UE_LOG( LogTemp, Warning, TEXT("AShooterMultiGameMode::GenericPlayerInitialization() - new controller is not a playercontroller") );
	}
}

void AShooterMultiGameMode::Logout( AController* controller )
{
	AShooterPlayerController* castedController = Cast<AShooterPlayerController>( controller );
	if ( castedController == nullptr )
	{
		UE_LOG(LogTemp, Warning, TEXT("AShooterMultiGameMode::PostLogin() - controller is not a AShooterPlayerController"));
		return;
	}

	DestroyPawnOfController( castedController );

	AShooterMultiPlayerState* playerState = Cast<AShooterMultiPlayerState>( castedController->PlayerState );
	if ( playerState )
	{
		AShooterMultiGameState* castedGameState = GetShooterGameState();

		if ( playerState->GetTeamID() != 0 )
			castedGameState->OnPlayerLeaveTeam( playerState->GetTeamID() );

		OnPlayerReady();
	}

	Super::Logout( controller );
}

APawn* AShooterMultiGameMode::SpawnDefaultPawnFor_Implementation(AController* NewPlayer, AActor* StartSpot)
{
	APawn* playerPawn = Super::SpawnDefaultPawnFor_Implementation(NewPlayer, StartSpot);

	AShooterCharacter* shooterCharacter = Cast<AShooterCharacter>(playerPawn);
	if (!shooterCharacter)
		return playerPawn;

	AShooterMultiPlayerState* playerState = Cast<AShooterMultiPlayerState>(NewPlayer->PlayerState);
	if (playerState)
		shooterCharacter->ChangeTeam(playerState->GetTeamID());

	return shooterCharacter;
}

FString AShooterMultiGameMode::InitNewPlayer(APlayerController* NewPlayerController, const FUniqueNetIdRepl& UniqueId,
                                             const FString& Options, const FString& Portal)
{
	FString returnValue = Super::InitNewPlayer(NewPlayerController, UniqueId, Options, Portal);

	UE_LOG(LogTemp, Warning, TEXT("InitNewPlayer Options : %s"), *Options);

	if (NewPlayerController)
	{
		const FString playerName = UGameplayStatics::ParseOption(Options, TEXT("PlayerName"));
		if (!playerName.IsEmpty())
			NewPlayerController->PlayerState->SetPlayerName(playerName);
	}

	return returnValue;
}
//#pragma endregion

//#pragma region PlayerStart choice
AActor* AShooterMultiGameMode::FindPlayerStart_Implementation(AController* Player, const FString& IncomingName)
{
	AShooterMultiPlayerState* castedPlayerState = Cast<AShooterMultiPlayerState>(Player->PlayerState);
	if (!castedPlayerState)
		return Super::FindPlayerStart_Implementation(Player, IncomingName);

	TArray<AShooterMultiPlayerStart *> validPlayerStarts = GetAllPlayerStartsForTeam(castedPlayerState->GetTeamID());
	if (validPlayerStarts.Num() <= 0)
	{
		UE_LOG(LogTemp, Warning, TEXT(
				"AShooterMultiGameMode::FindPlayerStart_Implementation() - could not find valid start for teamID [%d]"),
			castedPlayerState->GetTeamID());
		return Super::FindPlayerStart_Implementation(Player, IncomingName);
	}

	return validPlayerStarts[FMath::RandRange(0, validPlayerStarts.Num() - 1)];
}

TArray<AShooterMultiPlayerStart *> AShooterMultiGameMode::GetAllPlayerStartsForTeam(const int teamID) const
{
	TArray<AShooterMultiPlayerStart *> validPlayerStarts;

	for (AShooterMultiPlayerStart* oneStart : ListPlayerStarts)
	{
		if (oneStart->GetSpawnGroupID() == teamID)
			validPlayerStarts.AddUnique(oneStart);
	}
	return validPlayerStarts;
}

void AShooterMultiGameMode::OnPlayerReady()
{
	if (AreAllPlayersReady())
	{
		AShooterMultiGameState* castedGameState = GetShooterGameState();
		if (EShooterMultiState::SMS_BeforeStart == castedGameState->GetCurrentState())
			ChangeState(EShooterMultiState::SMS_Countdown);
		else if (EShooterMultiState::SMS_EndGame == castedGameState->GetCurrentState())
		{
			UShooterMultiGameInstance * GameInstance = Cast<UShooterMultiGameInstance>(GetGameInstance());
			SetAllPlayersReady(false);
			GameInstance->ServerTravelToMainMap();
		}
	}
}

void AShooterMultiGameMode::SetAllPlayersReady (bool value)
{
	TArray < APlayerState * > PlayerArray = GameState->PlayerArray;

	for (int i = 0; i < PlayerArray.Num(); ++i)
	{
		AShooterMultiPlayerState* playerState = Cast<AShooterMultiPlayerState>(PlayerArray[i]);
		if (playerState)
		{
			playerState->RequestReady(value);
		} else
		{
			UE_LOG(LogTemp, Warning, TEXT("AShooterMultiGameMode::SetAllPlayersReady() - PlayerState Cast Invalid"));
		}
	}
}

bool AShooterMultiGameMode::AreAllPlayersReady() const
{
	TArray < APlayerState * > PlayerArray = GameState->PlayerArray;

	for (int i = 0; i < PlayerArray.Num(); ++i)
	{
		AShooterMultiPlayerState* playerState = Cast<AShooterMultiPlayerState>(PlayerArray[i]);
		if (playerState)
		{
			if (!playerState->IsReady())
				return false;
		} else
		{
			UE_LOG(LogTemp, Warning, TEXT("AShooterMultiGameMode::AreAllPlayersReady() - PlayerState Cast Invalid"));
			return false;
		}
	}
	return true;
}

//#pragma endregion

void AShooterMultiGameMode::RespawnEveryPlayer()
{
	UWorld* world = GetWorld();
	check(world);

	for (FConstControllerIterator controllerIter = world->GetControllerIterator(); controllerIter; ++controllerIter)
	{
		AShooterPlayerController* shooterController = Cast<AShooterPlayerController>(controllerIter->Get());
		if (shooterController)
		{
			AShooterMultiPlayerState* playerState = Cast<AShooterMultiPlayerState>(shooterController->PlayerState);
			if (playerState)
			{
				if (playerState->GetTeamID() == -1)
					AutofillTeam(playerState);
			}

			Respawn(shooterController);
		}
	}
}

APlayerController* AShooterMultiGameMode::GetController(const APlayerState* playerState) const
{
	for (FConstPlayerControllerIterator it = GetWorld()->GetPlayerControllerIterator(); it; ++it)
	{
		if (playerState == it->Get()->PlayerState)
		{
			return it->Get();
		}
	}

	return nullptr;
}

void AShooterMultiGameMode::Respawn(APlayerController* controller)
{
	DestroyPawnOfController(controller);

	RestartPlayer(controller);
}

void AShooterMultiGameMode::Respawn(const APlayerState* playerState)
{
	APlayerController* controller = GetController(playerState);
	if (controller == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("Player Controller not found!"));
		return;
	}

	Respawn(controller);
}

void AShooterMultiGameMode::DestroyPawnOfPlayerState(const APlayerState* playerState) const
{
	APlayerController* controller = GetController(playerState);

	if (controller == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("Player Controller not found!"));
		return;
	}

	DestroyPawnOfController(controller);
}

void AShooterMultiGameMode::DestroyPawnOfController(APlayerController* playerController) const
{
	AShooterPlayerController* castedController = Cast<AShooterPlayerController>(playerController);
	if (!castedController)
	{
		UE_LOG(LogTemp, Error, TEXT(
			"AShooterMultiGameMode::DestroyPawnOfController() - controller is not a ShooterPlayerController"));
		return;
	}

	APawn* pawn = castedController->ShooterCharacter;
	if (pawn != nullptr)
		pawn->Destroy();
	else
	{
		pawn = castedController->GetPawn();
		if (pawn != nullptr)
			pawn->Destroy();
	}

	castedController->UnPossess();
}

void AShooterMultiGameMode::AutofillTeam(AShooterMultiPlayerState* playerState) const
{
	AShooterMultiGameState* gameState = GetShooterGameState();
	if (gameState)
	{
		playerState->RequestTeamChange(gameState->GetTeamAutofill());
	}
}

void AShooterMultiGameMode::AddDeath(ACharacterWithHealth* charac, AActor* Causer)
{
	AShooterCharacter* DeadShooter = Cast<AShooterCharacter>(charac);
	ACharacterWithHealth* KillerPlayer = Cast<ACharacterWithHealth>(Causer);
	AShooterCharacter* KillerShooter = Cast<AShooterCharacter>(Causer);

	AShooterMultiGameState* gameState = GetShooterGameState();

	FString killerName = Causer ? Causer->GetHumanReadableName() : "Killer";
	FString victimName = charac ? charac->GetHumanReadableName() : "Victim";

	if (DeadShooter)
	{
		AShooterMultiPlayerState* PlayerState = Cast<AShooterMultiPlayerState>(DeadShooter->PlayerState);
		if (PlayerState)
		{
			PlayerState->AddDeath();
			victimName = PlayerState->PlayerName;
		}
	}

	if (KillerShooter)
	{
		AShooterMultiPlayerState* PlayerState = Cast<AShooterMultiPlayerState>(KillerShooter->PlayerState);
		if (PlayerState)
		{
			PlayerState->AddKill();
			killerName = PlayerState->PlayerName;
		}
	}

	if (charac && KillerPlayer)
	{
		gameState->NotifyKill(KillerPlayer->GetTeamID(), killerName, charac->GetTeamID(), victimName);
	}

	if (gameState->HasGameBeenWon())
		ChangeState(EShooterMultiState::SMS_EndGame);
}

void AShooterMultiGameMode::SetIgnoreMoveLookInput( bool moveInput, bool lookInput )
{
	for ( FConstControllerIterator controllerIter = GetWorld()->GetControllerIterator(); controllerIter; ++controllerIter )
	{
		AShooterPlayerController* playerController = Cast<AShooterPlayerController>( controllerIter->Get() );
		if ( playerController )
			playerController->Client_SetIgnoreMoveLookInput( moveInput, lookInput );
	}
}

void AShooterMultiGameMode::SetIgnoreAllInputs( bool bIgnoreInputs )
{
	for ( FConstControllerIterator controllerIter = GetWorld()->GetControllerIterator(); controllerIter; ++controllerIter )
	{
		AShooterPlayerController* playerController = Cast<AShooterPlayerController>( controllerIter->Get() );
		if ( playerController )
			playerController->Client_SetIgnoreAllInputs( bIgnoreInputs );
	}
}

//#pragma region State / Game Flow
void AShooterMultiGameMode::ChangeState(const EShooterMultiState shooterMultiState )
{
	AShooterMultiGameState* castedGameState = GetShooterGameState();

	UE_LOG( LogTemp, Warning, TEXT("AShooterMultiGameMode::ChangeState() - [%d]"), static_cast<int8>( shooterMultiState ) );

	if ( shooterMultiState == castedGameState->GetCurrentState() )
		UE_LOG( LogTemp, Warning, TEXT( "AShooterMultiGameMode::ChangeState() - new and old state are equals [%d]" ), static_cast< int8 >( shooterMultiState ) );

	switch ( shooterMultiState )
	{
	case EShooterMultiState::SMS_BeforeStart:
		{
			OnChangeStateBeforeStart();
			break;
		}
	case EShooterMultiState::SMS_Countdown:
		{
			OnChangeStateCountdown();
			break;
		}
	case EShooterMultiState::SMS_Playing:
		{
			OnChangeStatePlaying();
			break;
		}
	case EShooterMultiState::SMS_EndGame:
		{
			OnChangeStateEndGame();
			break;
		}
	case EShooterMultiState::SMS_Unknown:
	default:
		break;
	}

	castedGameState->SetState(shooterMultiState);
}

bool AShooterMultiGameMode::StartMatch()
{
	TArray<APlayerState *> playerArray = GetShooterGameState()->PlayerArray;
	for ( APlayerState* playerState : playerArray )
	{
		AShooterMultiPlayerState* state = Cast<AShooterMultiPlayerState>( playerState );
		if ( !state )
		{
			UE_LOG(LogTemp, Warning, TEXT("AShooterMultiGameMode::StartMatch() - bad player state cast"));
			return false;
		}
		if ( state->IsReady() == false )
			return false;
	}

	ChangeState( EShooterMultiState::SMS_Countdown );
	return true;
}

void AShooterMultiGameMode::OnChangeStateBeforeStart()
{
	AShooterMultiGameState* gameState = GetShooterGameState();

	ParseOptionsString();

	gameState->SetScoreToWin( ScoreToWin );
	gameState->SetNumberOfTeams( NumberOfTeams );

	SetIgnoreAllInputs( true );
	SetIgnoreMoveLookInput( true, true );
}

void AShooterMultiGameMode::OnChangeStateCountdown()
{
	RespawnEveryPlayer();

	SetIgnoreAllInputs( true );
	SetIgnoreMoveLookInput( true, false );

	GetWorldTimerManager().SetTimer( TimeBeforeSpawn,
	                                 [this]() { ChangeState( EShooterMultiState::SMS_Playing ); },
	                                 CountdownTime, false );
}

void AShooterMultiGameMode::OnChangeStatePlaying()
{
	SetAllPlayersReady(false);
	SetIgnoreAllInputs( false );
	SetIgnoreMoveLookInput( false, false );

	GetShooterGameState()->StartTicking();
}

void AShooterMultiGameMode::OnChangeStateEndGame()
{
	SetIgnoreAllInputs( true );
}
//#pragma endregion

AShooterMultiGameState* AShooterMultiGameMode::GetShooterGameState() const
{
	AShooterMultiGameState* castedGameState = Cast<AShooterMultiGameState>(GameState);
	check(castedGameState);

	return castedGameState;
}

void AShooterMultiGameMode::ParseOptionsString()
{
	FString option = UGameplayStatics::ParseOption(OptionsString, TEXT("ScoreToWin"));
	if (!option.IsEmpty())
		ScoreToWin = FCString::Atoi(*option);

	option = UGameplayStatics::ParseOption(OptionsString, TEXT("NumberOfTeams"));
	if (!option.IsEmpty())
	{
		NumberOfTeams = FCString::Atoi(*option);
		AShooterMultiGameState* castedGameState = GetShooterGameState();
		NumberOfTeams = NumberOfTeams > castedGameState->GetMaxTeamCount() ? castedGameState->GetMaxTeamCount() : NumberOfTeams;
	}

	option = UGameplayStatics::ParseOption(OptionsString, TEXT("MaxPlayers"));
	if (!option.IsEmpty())
		GameSession->MaxPlayers = FCString::Atoi(*option);
}
