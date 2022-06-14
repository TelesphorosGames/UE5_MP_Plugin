// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Interfaces/OnlineSessionInterface.h"



#include "MultiplayerSessionSubsystem.generated.h"

// Declaring custom Delegates for the Menu Class to bind callbacks to

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FMultiplayerSubsessionOnCreateSessionComplete, bool, bWasSuccessful);
DECLARE_MULTICAST_DELEGATE_TwoParams(FMultiplayerSubsessionOnFindSessionComplete, const TArray<FOnlineSessionSearchResult>& SessionResults, bool bWasSuccessful);
DECLARE_MULTICAST_DELEGATE_OneParam(FMultiplayerSubssessionOnJoinSessionComplete, EOnJoinSessionCompleteResult::Type Result);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FMultiplayerSubsessionOnDestroySessionComplete, bool, bWasSuccessful);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FMultiplayerSubsessionOnStartSessionComplete, bool, bWasSuccessful);

/**
 * 
 */
UCLASS()
class COLINSMULTIPLAYERSESSIONS_API UMultiplayerSessionSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()


public:
	UMultiplayerSessionSubsystem();


	// Menu class calls these - Handles session functionality

	void CreateSession(int32 NumPublicConnections, FString MatchType);

	void FindSessions(int32 MaxSearchResults);
	void JoinSession(const FOnlineSessionSearchResult& SessionResult);

	void DestroySession();
	void StartSession();


	// Our custom delegates

	FMultiplayerSubsessionOnCreateSessionComplete MultiplayerSubsessionOnCreateSessionComplete;
	FMultiplayerSubsessionOnFindSessionComplete MultiplayerSubsessionOnFindSessionComplete;
	FMultiplayerSubssessionOnJoinSessionComplete MultiplayerSubssessionOnJoinSessionComplete;
	FMultiplayerSubsessionOnDestroySessionComplete MultiplayerSubsessionOnDestroySessionComplete;
	FMultiplayerSubsessionOnStartSessionComplete MultiplayerSubsessionOnStartSessionComplete;


	
protected:

	// Internal callbacks for the delegates we'll add to the Online Session Interface delegate list
	// -- these will not be called outside of this class

	void OnCreateSessionComplete(FName SessionName, bool bWasSuccessful);

	void OnFindSessionsComplete(bool bWasSuccessful);

	void OnJoinSessionComplete(FName SessionName, EOnJoinSessionCompleteResult::Type Result);

	void OnDestroySessionComplete(FName SessionName, bool bWasSuccessful);

	void OnStartSessionComplete(FName SessionName, bool bWasSuccessful);



	
	

private:

	IOnlineSessionPtr SessionInterface;

	TSharedPtr<FOnlineSessionSettings> LastSessionSettings;
	TSharedPtr<FOnlineSessionSearch> LastSessionSearch;

	// To add to the Online Session Interface Delegate list,
	// We will bind our MultiplayerSessionsSubsystem internal callbacks to these


	FOnCreateSessionCompleteDelegate CreateSessionCompleteDelegate;
	FDelegateHandle CreateSessionCompleteDelegateHandle;
	FOnFindSessionsCompleteDelegate FindSessionsCompleteDelegate;
	FDelegateHandle FindSessionCompleteDelegateHandle;
	FOnJoinSessionCompleteDelegate JoinSessionCompleteDelegate;
	FDelegateHandle JoinSessionCompleteDelegateHandle;
	FOnDestroySessionCompleteDelegate DestroySessionCompleteDelegate;
	FDelegateHandle DestroySessionCompleteDelegateHandle;
	FOnStartSessionCompleteDelegate StartSessionCompleteDelegate;
	FDelegateHandle StartSessionCompleteDelegateHandle;
	

	
};
