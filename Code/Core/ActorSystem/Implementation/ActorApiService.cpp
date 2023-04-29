#include <Core/CorePCH.h>

#include <Core/ActorSystem/ActorApiService.h>

// clang-format off
WD_BEGIN_DYNAMIC_REFLECTED_TYPE(wdActorApiService, 1, wdRTTINoAllocator)
WD_END_DYNAMIC_REFLECTED_TYPE;
// clang-format on

wdActorApiService::wdActorApiService() = default;
wdActorApiService::~wdActorApiService() = default;



WD_STATICLINK_FILE(Core, Core_ActorSystem_Implementation_ActorApiService);
