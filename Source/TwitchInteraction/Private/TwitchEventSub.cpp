#include "TwitchPubSub.h"
#include "Serialization/JsonSerializer.h"
#include "JsonObjectConverter.h"

UTwitchEventSub::UTwitchEventSub()
{
  // Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
  // off to improve performance if you don't need them.
  PrimaryComponentTick.bCanEverTick = true;
};

void UTwitchEventSub::BeginPlay()
{
  Super::BeginPlay();
  FModuleManager::Get().LoadModuleChecked("WebSockets");
};

void UTwitchEventSub::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
  Disconnect();
};

// Called every frame
void UTwitchEventSub::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction *ThisTickFunction)
{
  Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
};

void UTwitchEventSub::SetInfo(const FString _oauth, const FString _authType, const FString _channelId, const FString _clientId)
{
  authToken = _oauth;
  authType = _authType;
  channelId = _channelId;
  init = true;
  clientId = _clientId;
};

bool UTwitchEventSub::Connect(FString &result)
{
  Socket = FWebSocketsModule::Get().CreateWebSocket(eventSubUrl, eventSubProtocol);
  Socket->OnConnected().AddLambda([this]() -> void
                                  { UE_LOG(LogTemp, Warning, TEXT("Connected to twitch eventsub")); });
  Socket->OnConnectionError().AddLambda([this](const FString &error) -> void
                                        { UE_LOG(LogTemp, Warning, TEXT("Error connecting to twitch eventsub: %s"), *error); });
  Socket->OnClosed().AddLambda([this](int32 statusCode, const FString &reason, bool wasClean) -> void
                               { UE_LOG(LogTemp, Warning, TEXT("Connection closed: %d, %s, %d"), statusCode, *reason, wasClean); });
  Socket->OnMessage().AddLambda([this](const FString &message) -> void
                                { ProcessMessage(message); });
  Socket->OnRawMessage().AddLambda([this](const void *data, SIZE_T length, SIZE_T bytesRemaining) -> void {});
  Socket->OnMessageSent().AddLambda([this](const FString &message) -> void
                                    { UE_LOG(LogTemp, Warning, TEXT("Message sent: %s"), *message); });
  Socket->Connect();
  return true;
};

bool UTwitchEventSub::Disconnect()
{
  if (Socket != nullptr && Socket->IsConnected())
  {
    Socket->Close();
  }
  return true;
};

bool UTwitchEventSub::SendMessage(FString _message)
{
  if (Socket != nullptr && Socket->IsConnected())
  {
    UE_LOG(LogTemp, Warning, TEXT("SEND - %s"), *_message);
    Socket->Send(_message);
    return true;
  }
  return false;
};

void UTwitchEventSub::ProcessMessage(const FString _jsonStr)
{
  FTwitchEventSubMessage targetMessage;
  UE_LOG(LogTemp, Warning, TEXT("RECV - %s"), *_jsonStr);
  if (!FJsonObjectConverter::JsonObjectStringToUStruct(_jsonStr, &targetMessage, 0, 0))
  {
    UE_LOG(LogTemp, Error, TEXT("Deserialize failed - %s"), *_jsonStr);
    return;
  }
  if (targetMessage.metadata.message_type == "session_welcome")
  {
    FTwitchEventSubMessageWelcome welcomeMessage;
    if (!FJsonObjectConverter::JsonObjectStringToUStruct(_jsonStr, &welcomeMessage, 0, 0))
    {
      UE_LOG(LogTemp, Error, TEXT("Deserialize failed - %s"), *_jsonStr);
      return;
    }
    sessionId = welcomeMessage.payload.session.id;
    RequestEventSubs();
  }
};

void UTwitchEventSub::RequestEventSubs()
{
  FHttpModule &httpModule = FHttpModule::Get();
  TSharedRef<IHttpRequest, ESPMode::ThreadSafe> pRequest = httpModule.CreateRequest();
  pRequest->SetVerb(TEXT("POST"));
  pRequest->SetHeader(TEXT("Authorization"), FString::Printf(TEXT("%s %s"), *authType, *authToken));
  pRequest->SetHeader(TEXT("Client-Id"), clientId);
  pRequest->SetHeader(TEXT("Content-Type"), TEXT("application/json"));

  FTwitchEventSubSubscriptionRequest requestInfo;
  requestInfo.type = "channel.channel_points_custom_reward_redemption.add";
  requestInfo.version = "1";
  requestInfo.condition.user_id = channelId;
  requestInfo.transport.method = "websocket";
  requestInfo.transport.session_id = sessionId;

  FString result;
  FJsonObjectConverter::UStructToJsonObjectString(requestInfo, result, 0, 0, 0, nullptr, false);
  pRequest->SetContentAsString(result);
  pRequest->SetURL(apiUrl);
  pRequest->OnProcessRequestComplete().BindLambda(
      [&](
          FHttpRequestPtr pRequest,
          FHttpResponsePtr pResponse,
          bool connectedSuccessfully) mutable
      {
        if (connectedSuccessfully)
        {
          UE_LOG(LogTemp, Warning, TEXT("Request successful - %s"), *pResponse->GetContentAsString());
        }
        else
        {
          UE_LOG(LogTemp, Error, TEXT("Request failed - %s"), *pResponse->GetContentAsString());
        }
      });
  UE_LOG(LogTemp, Warning, TEXT("Verb - %s"), *pRequest->GetVerb());
  UE_LOG(LogTemp, Warning, TEXT("Authorization - %s"), *pRequest->GetHeader(TEXT("Authorization")));
  UE_LOG(LogTemp, Warning, TEXT("Client-Id - %s"), *pRequest->GetHeader(TEXT("Client-Id")));
  UE_LOG(LogTemp, Warning, TEXT("Content-Type - %s"), *pRequest->GetHeader(TEXT("Content-Type")));
  UE_LOG(LogTemp, Warning, TEXT("URL - %s"), *pRequest->GetURL());
  // UE_LOG(LogTemp, Warning, TEXT("Content - %s"), *pRequest->GetContentAsString());
  pRequest->ProcessRequest();
};

void UTwitchEventSub::UpdatePing()
{
  UE_LOG(LogTemp, Warning, TEXT("UpdatePing"));
};