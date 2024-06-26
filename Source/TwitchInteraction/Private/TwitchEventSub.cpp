#include "TwitchEventSub.h"
#include "JsonObjectConverter.h"
#include "Serialization/JsonSerializer.h"

UTwitchEventSub::UTwitchEventSub() {
  // Set this component to be initialized when the game starts, and to be ticked
  // every frame.  You can turn these features off to improve performance if you
  // don't need them.
  PrimaryComponentTick.bCanEverTick = true;
};

void UTwitchEventSub::BeginPlay() {
  Super::BeginPlay();
  FModuleManager::Get().LoadModuleChecked("WebSockets");
};

void UTwitchEventSub::EndPlay(const EEndPlayReason::Type EndPlayReason) {
  Disconnect();
};

// Called every frame
void UTwitchEventSub::TickComponent(
    float DeltaTime, ELevelTick TickType,
    FActorComponentTickFunction *ThisTickFunction) {
  Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
};

void UTwitchEventSub::SetInfo(const FString _oauth, const FString _authType,
                              const FString _channelId,
                              const FString _clientId) {
  authToken = _oauth;
  authType = _authType;
  channelId = _channelId;
  init = true;
  clientId = _clientId;
};

bool UTwitchEventSub::Connect(FString &result) {
  Socket =
      FWebSocketsModule::Get().CreateWebSocket(eventSubUrl, eventSubProtocol);
  Socket->OnConnected().AddLambda([this]() -> void {
    UE_LOG(LogTemp, Warning, TEXT("Connected to twitch eventsub"));
    OnConnectionSucceeded.Broadcast();
  });
  Socket->OnConnectionError().AddLambda([this](const FString &error) -> void {
    UE_LOG(LogTemp, Warning, TEXT("Error connecting to twitch eventsub: %s"),
           *error);
    OnConnectionError.Broadcast(error);
  });
  Socket->OnClosed().AddLambda(
      [this](int32 statusCode, const FString &reason, bool wasClean) -> void {
        UE_LOG(LogTemp, Warning, TEXT("Connection closed: %d, %s, %d"),
               statusCode, *reason, wasClean);
        OnConnectionClosed.Broadcast(reason);
      });
  Socket->OnMessage().AddLambda(
      [this](const FString &message) -> void { ProcessMessage(message); });
  Socket->OnRawMessage().AddLambda([this](const void *data, SIZE_T length,
                                          SIZE_T bytesRemaining) -> void {});
  Socket->OnMessageSent().AddLambda([this](const FString &message) -> void {
    UE_LOG(LogTemp, Warning, TEXT("Message sent: %s"), *message);
  });
  Socket->Connect();
  return true;
};

bool UTwitchEventSub::Disconnect() {
  if (Socket != nullptr && Socket->IsConnected()) {
    Socket->Close();
  }
  return true;
};

bool UTwitchEventSub::SendMessage(FString _message) {
  if (Socket != nullptr && Socket->IsConnected()) {
    UE_LOG(LogTemp, Warning, TEXT("SEND - %s"), *_message);
    Socket->Send(_message);
    return true;
  }
  return false;
};

void UTwitchEventSub::ProcessMessage(const FString _jsonStr) {
  FTwitchEventSubMessage targetMessage;
  UE_LOG(LogTemp, Warning, TEXT("RECV - %s"), *_jsonStr);
  if (!FJsonObjectConverter::JsonObjectStringToUStruct(_jsonStr, &targetMessage,
                                                       0, 0)) {
    UE_LOG(LogTemp, Error, TEXT("Deserialize failed - %s"), *_jsonStr);
    return;
  }
  if (targetMessage.metadata.message_type == "session_keepalive") {
    return;
  }
  if (targetMessage.metadata.message_type == "session_welcome") {
    FTwitchEventSubMessageWelcome welcomeMessage;
    if (!FJsonObjectConverter::JsonObjectStringToUStruct(
            _jsonStr, &welcomeMessage, 0, 0)) {
      UE_LOG(LogTemp, Error, TEXT("Deserialize failed - %s"), *_jsonStr);
      return;
    }
    sessionId = welcomeMessage.payload.session.id;
    RequestEventSubs();
  }
  if (targetMessage.metadata.message_type == "notification") {
    if (targetMessage.metadata.subscription_type ==
        "channel.channel_points_custom_reward_redemption.add") {
      FTwitchEventSubChannelPointRedeemMessage redemptionMessage;
      if (!FJsonObjectConverter::JsonObjectStringToUStruct(
              _jsonStr, &redemptionMessage, 0, 0)) {
        UE_LOG(LogTemp, Error, TEXT("Deserialize failed - %s"), *_jsonStr);
        return;
      }
      UE_LOG(LogTemp, Warning, TEXT("Redemption - %s"),
             *redemptionMessage.payload.event.reward.title);
      OnChannelPointsRedeemed.Broadcast(redemptionMessage.payload.event);
    } else if (targetMessage.metadata.subscription_type == "channel.follow") {
      FTwitchEventSubChannelFollowMessage redemptionMessage;
      if (!FJsonObjectConverter::JsonObjectStringToUStruct(
              _jsonStr, &redemptionMessage, 0, 0)) {
        UE_LOG(LogTemp, Error, TEXT("Deserialize failed - %s"), *_jsonStr);
        return;
      }
      OnChannelFollowed.Broadcast(redemptionMessage.payload.event);
    } else if (targetMessage.metadata.subscription_type ==
               "channel.subscribe") {
      FTwitchEventSubSubscriptionMessage redemptionMessage;
      if (!FJsonObjectConverter::JsonObjectStringToUStruct(
              _jsonStr, &redemptionMessage, 0, 0)) {
        UE_LOG(LogTemp, Error, TEXT("Deserialize failed - %s"), *_jsonStr);
        return;
      }
      OnChannelSubscribed.Broadcast(redemptionMessage.payload.event);
    } else if (targetMessage.metadata.subscription_type ==
               "channel.subscription.gift") {
      FTwitchEventSubSubscriptionGiftMessage redemptionMessage;
      if (!FJsonObjectConverter::JsonObjectStringToUStruct(
              _jsonStr, &redemptionMessage, 0, 0)) {
        UE_LOG(LogTemp, Error, TEXT("Deserialize failed - %s"), *_jsonStr);
        return;
      }
      OnChannelSubscriptionGifted.Broadcast(redemptionMessage.payload.event);
    } else if (targetMessage.metadata.subscription_type ==
               "channel.subscription.message") {
      FTwitchEventSubSubscriptionRenewMessage redemptionMessage;
      if (!FJsonObjectConverter::JsonObjectStringToUStruct(
              _jsonStr, &redemptionMessage, 0, 0)) {
        UE_LOG(LogTemp, Error, TEXT("Deserialize failed - %s"), *_jsonStr);
        return;
      }
      OnChannelSubscriptionRenewed.Broadcast(redemptionMessage.payload.event);
    } else if (targetMessage.metadata.subscription_type == "channel.raid") {
      FTwitchEventSubRaidMessage redemptionMessage;
      if (!FJsonObjectConverter::JsonObjectStringToUStruct(
              _jsonStr, &redemptionMessage, 0, 0)) {
        UE_LOG(LogTemp, Error, TEXT("Deserialize failed - %s"), *_jsonStr);
        return;
      }
      OnChannelRaided.Broadcast(redemptionMessage.payload.event);
    }
  }
};

void UTwitchEventSub::Subscribe(const FString subInfo) {
  TSharedRef<IHttpRequest, ESPMode::ThreadSafe> pRequest =
      FHttpModule::Get().CreateRequest();
  UE_LOG(LogTemp, Warning,
         TEXT("Connecting to EventSub with Client-Id: %s, Auth header is %s "
              "%s. %s"),
         *clientId, *authType, *authToken, *subInfo)
  pRequest->SetVerb(TEXT("POST"));
  pRequest->SetHeader(TEXT("Authorization"),
                      FString::Printf(TEXT("%s %s"), *authType, *authToken));
  pRequest->SetHeader(TEXT("Client-Id"), clientId);
  pRequest->SetHeader(TEXT("Content-Type"), TEXT("application/json"));
  pRequest->SetContentAsString(subInfo);
  pRequest->SetURL(apiUrl);
  pRequest->OnProcessRequestComplete().BindLambda(
      [&](FHttpRequestPtr pRequest, FHttpResponsePtr pResponse,
          bool connectedSuccessfully) mutable {
        if (connectedSuccessfully) {
          UE_LOG(LogTemp, Warning, TEXT("Request successful - %s"),
                 *pResponse->GetContentAsString());
        } else {
          UE_LOG(LogTemp, Error, TEXT("Request failed - %s"),
                 *pResponse->GetContentAsString());
        }
      });
  pRequest->ProcessRequest();
};

void UTwitchEventSub::RequestEventSubs() {
  TArray<FString> subscriptions;
  FTwitchEventSubSubscriptionChannelPointRequest cpRequest;
  cpRequest.type = "channel.channel_points_custom_reward_redemption.add";
  cpRequest.version = "1";
  cpRequest.condition.broadcaster_user_id = channelId;
  cpRequest.transport.method = "websocket";
  cpRequest.transport.session_id = sessionId;
  FString cpResult;
  FJsonObjectConverter::UStructToJsonObjectString(cpRequest, cpResult, 0, 0, 0,
                                                  nullptr, false);
  subscriptions.Add(cpResult);

  FTwitchEventSubSubscriptionChannelFollowRequest followRequest;
  followRequest.type = "channel.follow";
  followRequest.version = "2";
  followRequest.condition.broadcaster_user_id = channelId;
  followRequest.condition.moderator_user_id = channelId;
  followRequest.transport.method = "websocket";
  followRequest.transport.session_id = sessionId;
  FString followResult;
  FJsonObjectConverter::UStructToJsonObjectString(followRequest, followResult,
                                                  0, 0, 0, nullptr, false);
  subscriptions.Add(followResult);

  FTwitchEventSubSubscriptionSubscribeRequest subscribeRequest;
  subscribeRequest.type = "channel.subscribe";
  subscribeRequest.version = "1";
  subscribeRequest.condition.broadcaster_user_id = channelId;
  subscribeRequest.transport.method = "websocket";
  subscribeRequest.transport.session_id = sessionId;
  FString subscribeResult;
  FJsonObjectConverter::UStructToJsonObjectString(
      subscribeRequest, subscribeResult, 0, 0, 0, nullptr, false);
  subscriptions.Add(subscribeResult);

  FTwitchEventSubSubscriptionSubscriptionGiftRequest subscriptionGiftRequest;
  subscriptionGiftRequest.type = "channel.subscription.gift";
  subscriptionGiftRequest.version = "1";
  subscriptionGiftRequest.condition.broadcaster_user_id = channelId;
  subscriptionGiftRequest.transport.method = "websocket";
  subscriptionGiftRequest.transport.session_id = sessionId;
  FString subscriptionGiftResult;
  FJsonObjectConverter::UStructToJsonObjectString(
      subscriptionGiftRequest, subscriptionGiftResult, 0, 0, 0, nullptr, false);
  subscriptions.Add(subscriptionGiftResult);

  FTwitchEventSubSubscriptionSubscriptionRenewRequest subscriptionRenewRequest;
  subscriptionRenewRequest.type = "channel.subscription.message";
  subscriptionRenewRequest.version = "1";
  subscriptionRenewRequest.condition.broadcaster_user_id = channelId;
  subscriptionRenewRequest.transport.method = "websocket";
  subscriptionRenewRequest.transport.session_id = sessionId;
  FString subscriptionRenewResult;
  FJsonObjectConverter::UStructToJsonObjectString(subscriptionRenewRequest,
                                                  subscriptionRenewResult, 0, 0,
                                                  0, nullptr, false);
  subscriptions.Add(subscriptionRenewResult);

  FTwitchEventSubSubscriptionRaidRequest raidRequest;
  raidRequest.type = "channel.raid";
  raidRequest.version = "1";
  raidRequest.condition.broadcaster_user_id = channelId;
  raidRequest.transport.method = "websocket";
  raidRequest.transport.session_id = sessionId;
  FString raidResult;
  FJsonObjectConverter::UStructToJsonObjectString(raidRequest, raidResult, 0, 0,
                                                  0, nullptr, false);
  subscriptions.Add(raidResult);

  for (auto &sub : subscriptions) {
    Subscribe(sub);
  }
};

void UTwitchEventSub::UpdatePing() {
  UE_LOG(LogTemp, Warning, TEXT("UpdatePing"));
};
