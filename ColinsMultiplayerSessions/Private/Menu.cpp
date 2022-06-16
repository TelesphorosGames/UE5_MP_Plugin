// Fill out your copyright notice in the Description page of Project Settings.


#include "Menu.h"

#include "OnlineSessionSettings.h"
#include "Components/Button.h"
#include "MultiplayerSessionSubsystem.h"
#include "OnlineSubsystem.h"

// #include "Interfaces/OnlineSessionInterface.h"

void UMenu::MenuSetup(int32 NumberOfPublicConnections, FString TypeOfMatch, FString LobbyPath) 
{
	PathToLobby = FString::Printf(TEXT("%s?listen"), *LobbyPath);
	NumPublicConnections = NumberOfPublicConnections;
	MatchType = TypeOfMatch;
	AddToViewport();
	SetVisibility(ESlateVisibility::Visible);
	bIsFocusable = true;


	UWorld* World = GetWorld();
	if(World)
	{
		APlayerController* PlayerController = World->GetFirstPlayerController();
		if(PlayerController)
		{
			FInputModeUIOnly InputModeData;
			InputModeData.SetWidgetToFocus(TakeWidget());
			InputModeData.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
			
			
			PlayerController->SetInputMode(InputModeData);

			PlayerController->SetShowMouseCursor(true);
			
		}
	}

	UGameInstance* GameInstance = GetGameInstance();
	if(GameInstance)
	{
		MultiplayerSessionSubsystem = GameInstance->GetSubsystem<UMultiplayerSessionSubsystem>();
	}
	// else
	// {
	// 	UE_LOG(LogTemp, Warning, TEXT("NO GAME INSTANCE"));
	// }

	if(MultiplayerSessionSubsystem)
	{
		MultiplayerSessionSubsystem->MultiplayerSubsessionOnCreateSessionComplete.AddDynamic(this, &ThisClass::OnCreateSession);
		MultiplayerSessionSubsystem->MultiplayerSubsessionOnFindSessionComplete.AddUObject(this, &ThisClass::OnFindSessions);
		MultiplayerSessionSubsystem->MultiplayerSubssessionOnJoinSessionComplete.AddUObject(this,&ThisClass::OnJoinSession);
		MultiplayerSessionSubsystem->MultiplayerSubsessionOnDestroySessionComplete.AddDynamic(this, &ThisClass::OnDestroySession);
		MultiplayerSessionSubsystem->MultiplayerSubsessionOnStartSessionComplete.AddDynamic(this, &ThisClass::OnStartSession);
	}

	
	
}

bool UMenu::Initialize()
{
	if(!Super::Initialize())
	{
		return false;
	}

	if(HostButton)
	{
		HostButton->OnClicked.AddDynamic(this, &UMenu::HostButtonClicked);
	
	}

	if(JoinButton)
	{
		JoinButton->OnClicked.AddDynamic(this, &UMenu::JoinButtonClicked);
	}

	if(StartButton)
	{
		StartButton->OnClicked.AddDynamic(this, &UMenu::StartButtonClicked);
	}
	

	
	return true;
}

void UMenu::OnLevelRemovedFromWorld(ULevel* InLevel, UWorld* InWorld)
{
	MenuTeardown();
	Super::OnLevelRemovedFromWorld(InLevel, InWorld);

	

}

void UMenu::OnCreateSession(bool bWasSuccessful)
{
	if(bWasSuccessful)
	{
		if(GEngine)
		{
			GEngine->AddOnScreenDebugMessage(-1, 15.f, FColor::Yellow, FString(TEXT("SESSION CREATED SUCCESSFULLY!")));
		}
	}
	else
	{
		if(GEngine)
		{
			GEngine->AddOnScreenDebugMessage(-1, 15.f, FColor::Red, FString(TEXT("SESSION CREATED FAILURE!")));
		}
	}
	
	
}

void UMenu::OnFindSessions(const TArray<FOnlineSessionSearchResult>& SessionResults, bool bWasSuccessful)
{
	if(MultiplayerSessionSubsystem == nullptr)
	{
		return;
	}
	
	for(auto Result: SessionResults)
	{

		FString SettingsValue;
		Result.Session.SessionSettings.Get(FName("MatchType"), SettingsValue);

		if(SettingsValue==MatchType)
		{
			MultiplayerSessionSubsystem->JoinSession(Result);
			return;
		}
		
	}


	if(!bWasSuccessful || SessionResults.Num() == 0)
	{
		if(GEngine)
		{
			GEngine->AddOnScreenDebugMessage(-1, 15.f, FColor::Red, FString(TEXT("NO SESSIONS FOUND!")));
		}
		JoinButton->SetIsEnabled(true);
	}
}

void UMenu::OnJoinSession(EOnJoinSessionCompleteResult::Type Result)
{
	
	IOnlineSubsystem* Subsystem = IOnlineSubsystem::Get();
	
	if(Subsystem)
	{
		IOnlineSessionPtr SessionInterface = Subsystem->GetSessionInterface();
		if(SessionInterface.IsValid())
		{
			FString Address;
			SessionInterface->GetResolvedConnectString(NAME_GameSession, Address);

			APlayerController* PlayerController = GetGameInstance()->GetFirstLocalPlayerController();
			if(PlayerController)
			{
				PlayerController->ClientTravel(Address, ETravelType::TRAVEL_Absolute);
				if(GEngine)
				{
					GEngine->AddOnScreenDebugMessage(-1, 15.f, FColor::Magenta, FString::Printf(TEXT("SESSION JOINED! NAME:  %s ADDRESS: %s "), *SessionInterface->GetNamedSession(NAME_GameSession)->SessionName.ToString(), *Address));
				}
			}
		}
		if(Result != EOnJoinSessionCompleteResult::Success)
		{
			JoinButton->SetIsEnabled(true);
			if(GEngine)
			{
				GEngine->AddOnScreenDebugMessage(-1, 15.f, FColor::Red, FString(TEXT("SESSION JOIN ERROR!")));
			}
		}
		

		
	}
	
}

void UMenu::OnDestroySession(bool bWasSuccessful)
{
}

void UMenu::OnStartSession(bool bWasSuccessful)
{
	if(bWasSuccessful)
	{
		UWorld* World = GetWorld();
        	if(World)
        	{
        		World->ServerTravel(PathToLobby);
        	}
	}
	
	
}


void UMenu::HostButtonClicked()
{
	if(MultiplayerSessionSubsystem)
	{
		MultiplayerSessionSubsystem->CreateSession(NumPublicConnections, MatchType);
		HostButton->SetIsEnabled(false);
	}
	
}

void UMenu::JoinButtonClicked()
{
	if(MultiplayerSessionSubsystem)
	{
		MultiplayerSessionSubsystem->FindSessions(10000);
		JoinButton->SetIsEnabled(false);
	}
}

void UMenu::StartButtonClicked()
{
	if(MultiplayerSessionSubsystem)
	{
		MultiplayerSessionSubsystem->StartSession();
	}
}

void UMenu::MenuTeardown()
{
	RemoveFromParent();
	UWorld* World = GetWorld();
	if(World)
	{
		APlayerController* PlayerController = World->GetFirstPlayerController();
		if(PlayerController)
		{
			FInputModeGameOnly InputModeGameOnly;
			PlayerController->SetInputMode(InputModeGameOnly);

			PlayerController->SetShowMouseCursor(false);
			
		}
		
		
	}

	
}
