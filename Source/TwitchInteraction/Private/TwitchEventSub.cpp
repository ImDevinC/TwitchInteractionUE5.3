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

void UTwitchEventSub::SetInfo(const FString _oauth, const FString _authType, const FString _channelId)
{
  authToken = _oauth;
  authType = _authType;
  channelId = _channelId;
  init = true;
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
  Socket->OnRawMessage().AddLambda([this](const void *data, SIZE_T length, SIZE_T bytesRemaining) -> void
                                   { UE_LOG(LogTemp, Warning, TEXT("Raw message received: %s"), data); });
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
  }
};

void UTwitchEventSub::RequestEventSubs()
{
  UE_LOG(LogTemp, Warning, TEXT("Request event subs"));
};

void UTwitchEventSub::UpdatePing()
{
  UE_LOG(LogTemp, Warning, TEXT("UpdatePing"));
};