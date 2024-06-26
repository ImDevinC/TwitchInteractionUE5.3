// Copyright (c) 2024 ImDevinC.

#pragma once

#include "Components/ActorComponent.h"
#include "CoreMinimal.h"
#include "HttpModule.h"
#include "IWebSocket.h"
#include "Interfaces/IHttpRequest.h"
#include "Interfaces/IHttpResponse.h"
#include "Misc/DateTime.h"
#include "Networking.h"
#include "Runtime/Engine/Public/TimerManager.h"
#include "WebSocketsModule.h"

#include "TwitchEventSub.generated.h"

// BEGIN Generic data structures for all events
USTRUCT(BlueprintType)
struct FTwitchEventSubSubscriptionRequestTransport {
  GENERATED_USTRUCT_BODY()
public:
  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TwitchInteraction")
  FString method;
  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TwitchInteraction")
  FString session_id;
};

USTRUCT(BlueprintType)
struct FTwitchEventNotificationReward {
  GENERATED_USTRUCT_BODY()
public:
  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TwitchInteraction")
  FString id;
  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TwitchInteraction")
  FString title;
  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TwitchInteraction")
  int cost;
  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TwitchInteraction")
  FString prompt;
};

USTRUCT(BlueprintType)
struct FTwitchEventSubMessageMetadata {
  GENERATED_USTRUCT_BODY()
public:
  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TwitchInteraction")
  FString message_id;
  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TwitchInteraction")
  FString message_type;
  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TwitchInteraction")
  FString message_timestamp;
  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TwitchInteraction")
  FString subscription_type;
  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TwitchInteraction")
  FString subscription_version;
};

USTRUCT(BlueprintType)
struct FTwitchEventSubMessage {
  GENERATED_USTRUCT_BODY()
public:
  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TwitchInteraction")
  FTwitchEventSubMessageMetadata metadata;
};

USTRUCT(BlueprintType)
struct FTwitchEventSubMessageWelcomePayloadSession {
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
struct FTwitchEventSubMessageWelcomePayload {
  GENERATED_USTRUCT_BODY()
public:
  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TwitchInteraction")
  FTwitchEventSubMessageWelcomePayloadSession session;
};

USTRUCT(BlueprintType)
struct FTwitchEventSubMessageWelcome {
  GENERATED_USTRUCT_BODY()
public:
  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TwitchInteraction")
  FTwitchEventSubMessageMetadata metadata;
  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TwitchInteraction")
  FTwitchEventSubMessageWelcomePayload payload;
};
// END Generic data structures

// BEGIN channel.follow
USTRUCT(BlueprintType)
struct FTwitchEventNotificationEventChannelFollowEvent {
  GENERATED_USTRUCT_BODY()
public:
  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TwitchInteraction")
  FString user_id;
  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TwitchInteraction")
  FString user_login;
  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TwitchInteraction")
  FString user_name;
  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TwitchInteraction")
  FString broadcaster_user_id;
  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TwitchInteraction")
  FString broadcaster_user_login;
  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TwitchInteraction")
  FString broadcaster_user_name;
  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TwitchInteraction")
  FString followed_at;
};

USTRUCT(BlueprintType)
struct FTwitchEventSubChannelFollowPayload {
  GENERATED_USTRUCT_BODY()
public:
  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TwitchInteraction")
  FTwitchEventNotificationEventChannelFollowEvent event;
};

USTRUCT(BlueprintType)
struct FTwitchEventSubChannelFollowMessage {
  GENERATED_USTRUCT_BODY()
public:
  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TwitchInteraction")
  FTwitchEventSubChannelFollowPayload payload;
};

USTRUCT(BlueprintType)
struct FTwitchEventSubSubscriptionFollowRequestCondition {
  GENERATED_USTRUCT_BODY()
public:
  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TwitchInteraction")
  FString broadcaster_user_id;
  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TwitchInteraction")
  FString moderator_user_id;
};

USTRUCT(BlueprintType)
struct FTwitchEventSubSubscriptionChannelFollowRequest {
  GENERATED_USTRUCT_BODY()
public:
  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TwitchInteraction")
  FString type;
  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TwitchInteraction")
  FString version;
  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TwitchInteraction")
  FTwitchEventSubSubscriptionFollowRequestCondition condition;
  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TwitchInteraction")
  FTwitchEventSubSubscriptionRequestTransport transport;
};

// END channel.follow

// BEGIN channel.channel_points_custom_reward.add

USTRUCT(BlueprintType)
struct FTwitchEventNotificationEventChannelPointRedeemEvent {
  GENERATED_USTRUCT_BODY()
public:
  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TwitchInteraction")
  FString id;
  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TwitchInteraction")
  FString broadcaster_user_id;
  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TwitchInteraction")
  FString broadcaster_user_login;
  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TwitchInteraction")
  FString broadcaster_user_name;
  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TwitchInteraction")
  FString user_id;
  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TwitchInteraction")
  FString user_login;
  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TwitchInteraction")
  FString user_name;
  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TwitchInteraction")
  FString user_input;
  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TwitchInteraction")
  FString status;
  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TwitchInteraction")
  FString redeemed_at;
  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TwitchInteraction")
  FTwitchEventNotificationReward reward;
};

USTRUCT(BlueprintType)
struct FTwitchEventSubChannelPointRedeemPayload {
  GENERATED_USTRUCT_BODY()
public:
  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TwitchInteraction")
  FTwitchEventNotificationEventChannelPointRedeemEvent event;
};

USTRUCT(BlueprintType)
struct FTwitchEventSubChannelPointRedeemMessage {
  GENERATED_USTRUCT_BODY()
public:
  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TwitchInteraction")
  FTwitchEventSubChannelPointRedeemPayload payload;
};

USTRUCT(BlueprintType)
struct FTwitchEventSubSubscriptionChannelPointRequestCondition {
  GENERATED_USTRUCT_BODY()
public:
  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TwitchInteraction")
  FString broadcaster_user_id;
};

USTRUCT(BlueprintType)
struct FTwitchEventSubSubscriptionChannelPointRequest {
  GENERATED_USTRUCT_BODY()
public:
  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TwitchInteraction")
  FString type;
  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TwitchInteraction")
  FString version;
  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TwitchInteraction")
  FTwitchEventSubSubscriptionChannelPointRequestCondition condition;
  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TwitchInteraction")
  FTwitchEventSubSubscriptionRequestTransport transport;
};

// END channel.channel_points_custom_reward.add

// BEGIN channel.subscribe
USTRUCT(BlueprintType)
struct FTwitchEventNotificationEventSubscribe {
  GENERATED_USTRUCT_BODY()
public:
  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TwitchInteraction")
  FString user_id;
  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TwitchInteraction")
  FString user_login;
  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TwitchInteraction")
  FString user_name;
  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TwitchInteraction")
  FString broadcaster_user_id;
  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TwitchInteraction")
  FString broadcaster_user_login;
  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TwitchInteraction")
  FString broadcaster_user_name;
  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TwitchInteraction")
  FString tier;
  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TwitchInteraction")
  bool is_gift;
};

USTRUCT(BlueprintType) struct FTwitchEventSubSubscriptionPayload {
  GENERATED_USTRUCT_BODY()
public:
  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TwitchInteraction")
  FTwitchEventNotificationEventSubscribe event;
};

USTRUCT(BlueprintType)
struct FTwitchEventSubSubscriptionMessage {
  GENERATED_USTRUCT_BODY()
public:
  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TwitchInteraction")
  FTwitchEventSubSubscriptionPayload payload;
};

USTRUCT(BlueprintType)
struct FTwitchEventSubSubscriptionSubscribeRequestCondition {
  GENERATED_USTRUCT_BODY()
public:
  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TwitchInteraction")
  FString broadcaster_user_id;
};

USTRUCT(BlueprintType)
struct FTwitchEventSubSubscriptionSubscribeRequest {
  GENERATED_USTRUCT_BODY()
public:
  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TwitchInteraction")
  FString type;
  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TwitchInteraction")
  FString version;
  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TwitchInteraction")
  FTwitchEventSubSubscriptionSubscribeRequestCondition condition;
  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TwitchInteraction")
  FTwitchEventSubSubscriptionRequestTransport transport;
};

// END channel.subscribe

// BEGIN channel.subscription.gift
USTRUCT(BlueprintType)
struct FTwitchEventNotificationEventSubscriptionGift {
  GENERATED_USTRUCT_BODY()
public:
  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TwitchInteraction")
  FString user_id;
  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TwitchInteraction")
  FString user_login;
  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TwitchInteraction")
  FString user_name;
  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TwitchInteraction")
  FString broadcaster_user_id;
  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TwitchInteraction")
  FString broadcaster_user_login;
  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TwitchInteraction")
  FString broadcaster_user_name;
  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TwitchInteraction")
  int total;
  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TwitchInteraction")
  FString tier;
  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TwitchInteraction")
  int cumulative_total;
  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TwitchInteraction")
  bool is_anonymous;
};

USTRUCT(BlueprintType) struct FTwitchEventSubSubscriptionGiftPayload {
  GENERATED_USTRUCT_BODY()
public:
  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TwitchInteraction")
  FTwitchEventNotificationEventSubscriptionGift event;
};

USTRUCT(BlueprintType)
struct FTwitchEventSubSubscriptionGiftMessage {
  GENERATED_USTRUCT_BODY()
public:
  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TwitchInteraction")
  FTwitchEventSubSubscriptionGiftPayload payload;
};

USTRUCT(BlueprintType)
struct FTwitchEventSubSubscriptionGiftRequestCondition {
  GENERATED_USTRUCT_BODY()
public:
  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TwitchInteraction")
  FString broadcaster_user_id;
};

USTRUCT(BlueprintType)
struct FTwitchEventSubSubscriptionSubscriptionGiftRequest {
  GENERATED_USTRUCT_BODY()
public:
  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TwitchInteraction")
  FString type;
  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TwitchInteraction")
  FString version;
  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TwitchInteraction")
  FTwitchEventSubSubscriptionGiftRequestCondition condition;
  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TwitchInteraction")
  FTwitchEventSubSubscriptionRequestTransport transport;
};

// END channel.subscription.gift

// BEGIN channel.subscription.message
USTRUCT(BlueprintType)
struct FTwitchEventNotificationEventSubscriptionRenewMessage {
  GENERATED_USTRUCT_BODY()
public:
  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TwitchInteraction")
  FString text;
  // We ignore the emotes for now, will review later
};

USTRUCT(BlueprintType)
struct FTwitchEventNotificationEventSubscriptionRenew {
  GENERATED_USTRUCT_BODY()
public:
  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TwitchInteraction")
  FString user_id;
  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TwitchInteraction")
  FString user_login;
  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TwitchInteraction")
  FString user_name;
  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TwitchInteraction")
  FString broadcaster_user_id;
  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TwitchInteraction")
  FString broadcaster_user_login;
  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TwitchInteraction")
  FString broadcaster_user_name;
  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TwitchInteraction")
  FString tier;
  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TwitchInteraction")
  FTwitchEventNotificationEventSubscriptionRenewMessage message;
  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TwitchInteraction")
  int cumulative_months;
  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TwitchInteraction")
  int streak_months;
  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TwitchInteraction")
  int duration_months;
};

USTRUCT(BlueprintType) struct FTwitchEventSubSubscriptionRenewPayload {
  GENERATED_USTRUCT_BODY()
public:
  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TwitchInteraction")
  FTwitchEventNotificationEventSubscriptionRenew event;
};

USTRUCT(BlueprintType)
struct FTwitchEventSubSubscriptionRenewMessage {
  GENERATED_USTRUCT_BODY()
public:
  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TwitchInteraction")
  FTwitchEventSubSubscriptionRenewPayload payload;
};

USTRUCT(BlueprintType)
struct FTwitchEventSubSubscriptionRenewRequestCondition {
  GENERATED_USTRUCT_BODY()
public:
  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TwitchInteraction")
  FString broadcaster_user_id;
};

USTRUCT(BlueprintType)
struct FTwitchEventSubSubscriptionSubscriptionRenewRequest {
  GENERATED_USTRUCT_BODY()
public:
  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TwitchInteraction")
  FString type;
  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TwitchInteraction")
  FString version;
  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TwitchInteraction")
  FTwitchEventSubSubscriptionRenewRequestCondition condition;
  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TwitchInteraction")
  FTwitchEventSubSubscriptionRequestTransport transport;
};

// END channel.subscription.message

// BEGIN channel.raid
USTRUCT(BlueprintType)
struct FTwitchEventNotificationEventRaidMessage {
  GENERATED_USTRUCT_BODY()
public:
  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TwitchInteraction")
  FString text;
};

USTRUCT(BlueprintType)
struct FTwitchEventNotificationEventRaid {
  GENERATED_USTRUCT_BODY()
public:
  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TwitchInteraction")
  FString from_broadcaster_user_id;
  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TwitchInteraction")
  FString from_broadcaster_user_login;
  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TwitchInteraction")
  FString from_broadcaster_user_name;
  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TwitchInteraction")
  FString to_broadcaster_user_id;
  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TwitchInteraction")
  FString to_broadcaster_user_login;
  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TwitchInteraction")
  FString to_broadcaster_user_name;
  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TwitchInteraction")
  int viewers;
};

USTRUCT(BlueprintType) struct FTwitchEventSubRaidPayload {
  GENERATED_USTRUCT_BODY()
public:
  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TwitchInteraction")
  FTwitchEventNotificationEventRaid event;
};

USTRUCT(BlueprintType)
struct FTwitchEventSubRaidMessage {
  GENERATED_USTRUCT_BODY()
public:
  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TwitchInteraction")
  FTwitchEventSubRaidPayload payload;
};

USTRUCT(BlueprintType)
struct FTwitchEventSubSubscriptionRaidRequestCondition {
  GENERATED_USTRUCT_BODY()
public:
  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TwitchInteraction")
  FString broadcaster_user_id;
};

USTRUCT(BlueprintType)
struct FTwitchEventSubSubscriptionRaidRequest {
  GENERATED_USTRUCT_BODY()
public:
  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TwitchInteraction")
  FString type;
  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TwitchInteraction")
  FString version;
  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TwitchInteraction")
  FTwitchEventSubSubscriptionRaidRequestCondition condition;
  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TwitchInteraction")
  FTwitchEventSubSubscriptionRequestTransport transport;
};

// END channel.raid

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(
    FChannelPointsRedeemed,
    const FTwitchEventNotificationEventChannelPointRedeemEvent &,
    channelPointsInfo);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(
    FChannelFollowed, const FTwitchEventNotificationEventChannelFollowEvent &,
    channelFollowInfo);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(
    FChannelSubscribed, const FTwitchEventNotificationEventSubscribe &,
    channelSubscribeInfo);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(
    FChannelSubscriptionGifted,
    const FTwitchEventNotificationEventSubscriptionGift &,
    channelSubscriptionGiftInfo);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(
    FChannelSubscriptionRenewed,
    const FTwitchEventNotificationEventSubscriptionRenew &,
    channelSubscriptionRenewInfo);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(
    FChannelRaided, const FTwitchEventNotificationEventRaid &, channelRaidInfo);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FConnectionError, const FString &,
                                            reason);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FConnectionSucceeded);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FConnectionClosed, const FString &,
                                            reason);

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class TWITCHINTERACTION_API UTwitchEventSub : public UActorComponent {
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
  void Subscribe(const FString type);
  void HandleSubscriptionNotification(const FString _jsonStr);

public:
  // Called every frame
  virtual void
  TickComponent(float DeltaTime, ELevelTick TickType,
                FActorComponentTickFunction *ThisTickFunction) override;

  UFUNCTION(BlueprintCallable, Category = "Setup")
  void SetInfo(const FString _oauth, const FString _authType = "Bearer",
               const FString _channelId = "0", const FString _clientId = "0");

  UFUNCTION(BlueprintCallable, Category = "Setup")
  bool Connect(FString &result);
  UFUNCTION(BlueprintCallable, Category = "Setup")
  bool Disconnect();

  UFUNCTION(BlueprintCallable, Category = "Setup")
  bool SendMessage(FString _message);
  void UpdatePing();

  UPROPERTY(BlueprintAssignable, Category = "Message Events")
  FChannelPointsRedeemed OnChannelPointsRedeemed;

  UPROPERTY(BlueprintAssignable, Category = "Message Events")
  FChannelFollowed OnChannelFollowed;

  UPROPERTY(BlueprintAssignable, Category = "Message Events")
  FChannelSubscribed OnChannelSubscribed;

  UPROPERTY(BlueprintAssignable, Category = "Message Events")
  FChannelSubscriptionGifted OnChannelSubscriptionGifted;

  UPROPERTY(BlueprintAssignable, Category = "Message Events")
  FChannelSubscriptionRenewed OnChannelSubscriptionRenewed;

  UPROPERTY(BlueprintAssignable, Category = "Message Events")
  FChannelRaided OnChannelRaided;

  UPROPERTY(BlueprintAssignable, Category = "Message Events")
  FConnectionError OnConnectionError;

  UPROPERTY(BlueprintAssignable, Category = "Message Events")
  FConnectionSucceeded OnConnectionSucceeded;

  UPROPERTY(BlueprintAssignable, Category = "Message Events")
  FConnectionClosed OnConnectionClosed;

private:
  uint32 requestCounter = 0;
  TSharedPtr<IWebSocket> Socket;
  FTimerHandle UpdateTimer;
  uint32 pingTicker = 0;
  FDateTime LastUpdate;
};
