// Copyright (c) 2024 ImDevinC.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Networking.h"
#include "Misc/DateTime.h"
#include "Runtime/Engine/Public/TimerManager.h"
#include "WebSocketsModule.h"
#include "IWebSocket.h"
#include "HttpModule.h"
#include "Interfaces/IHttpRequest.h"
#include "Interfaces/IHttpResponse.h"
#include "TwitchEventSub.generated.h"

USTRUCT(BlueprintType)
struct FTwitchEventSubSubscriptionRequestTransport
{
  GENERATED_USTRUCT_BODY()
public:
  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TwitchInteraction")
  FString method;
  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TwitchInteraction")
  FString session_id;
};

USTRUCT(BlueprintType)
struct FTwitchEventSubSubscriptionRequestCondition
{
  GENERATED_USTRUCT_BODY()
public:
  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TwitchInteraction")
  FString user_id;
};

USTRUCT(BlueprintType)
struct FTwitchEventSubSubscriptionRequest
{
  GENERATED_USTRUCT_BODY()
public:
  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TwitchInteraction")
  FString type;
  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TwitchInteraction")
  FString version;
  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TwitchInteraction")
  FTwitchEventSubSubscriptionRequestCondition condition;
  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TwitchInteraction")
  FTwitchEventSubSubscriptionRequestTransport transport;
};

USTRUCT(BlueprintType)
struct FTwitchEventSubMessageMetadata
{
  GENERATED_USTRUCT_BODY()
public:
  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TwitchInteraction")
  FString message_id;
  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TwitchInteraction")
  FString message_type;
  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TwitchInteraction")
  FString message_timestamp;
};

USTRUCT(BlueprintType)
struct FTwitchEventSubMessage
{
  GENERATED_USTRUCT_BODY()
public:
  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TwitchInteraction")
  FTwitchEventSubMessageMetadata metadata;
};

USTRUCT(BlueprintType)
struct FTwitchEventSubMessageWelcomePayloadSession
{
  GENERATED_USTRUCT_BODY()
public:
  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TwitchInteraction")
  FString id;
  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TwitchInteraction")
  FString status;
  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TwitchInteraction")
  FString connected_at;
  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TwitchInteraction")
  FString keepalive_timeout_seconds;
  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TwitchInteraction")
  FString reconnect_url;
};

USTRUCT(BlueprintType)
struct FTwitchEventSubMessageWelcomePayload
{
  GENERATED_USTRUCT_BODY()
public:
  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TwitchInteraction")
  FTwitchEventSubMessageWelcomePayloadSession session;
};

USTRUCT(BlueprintType)
struct FTwitchEventSubMessageWelcome
{
  GENERATED_USTRUCT_BODY()
public:
  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TwitchInteraction")
  FTwitchEventSubMessageMetadata metadata;
  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TwitchInteraction")
  FTwitchEventSubMessageWelcomePayload payload;
};

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class TWITCHINTERACTION_API UTwitchEventSub : public UActorComponent
{
  GENERATED_BODY()

public:
  UTwitchEventSub();
  bool init;

  UPROPERTY()
  FString channelId;
  UPROPERTY()
  FString authToken;
  UPROPERTY()
  FString authType;
  UPROPERTY()
  TArray<FString> listenTopics;
  UPROPERTY()
  FString clientId;
  UPROPERTY()
  FString sessionId;

  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Setup")
  FString eventSubUrl = "wss://eventsub.wss.twitch.tv/ws";
  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Setup")
  FString eventSubProtocol = "wss";
  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Setup")
  FString apiUrl = "https://api.twitch.tv/helix/eventsub/subscriptions";

protected:
  // Called when the game starts
  virtual void BeginPlay() override;
  virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

  void ProcessMessage(const FString _jsonStr);
  void RequestEventSubs();

public:
  // Called every frame
  virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction *ThisTickFunction) override;

  UFUNCTION(BlueprintCallable, Category = "Setup")
  void SetInfo(const FString _oauth, const FString _authType = "Bearer", const FString _channelId = "0", const FString _clientId = "0");

  UFUNCTION(BlueprintCallable, Category = "Setup")
  bool Connect(FString &result);
  UFUNCTION(BlueprintCallable, Category = "Setup")
  bool Disconnect();

  UFUNCTION(BlueprintCallable, Category = "Setup")
  bool SendMessage(FString _message);
  void UpdatePing();

  UPROPERTY(BlueprintAssignable, Category = "Message Events")
  FBitsEventReceived OnBitsEventReceived;
  UPROPERTY(BlueprintAssignable, Category = "Message Events")
  FBitsBadgeEventReceived OnBitsBadgeEventReceived;

  UPROPERTY(BlueprintAssignable, Category = "Message Events")
  FRedeemEventReceived OnRedeemEventReceived;

  UPROPERTY(BlueprintAssignable, Category = "Message Events")
  FSubscribeEventReceived OnSubscribeEventReceived;

private:
  uint32 requestCounter = 0;
  TSharedPtr<IWebSocket> Socket;
  FTimerHandle UpdateTimer;
  uint32 pingTicker = 0;
  FDateTime LastUpdate;
};