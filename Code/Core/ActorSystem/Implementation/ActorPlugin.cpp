#include <Core/CorePCH.h>

#include <Core/ActorSystem/ActorPlugin.h>

// clang-format off
WD_BEGIN_DYNAMIC_REFLECTED_TYPE(wdActorPlugin, 1, wdRTTINoAllocator)
WD_END_DYNAMIC_REFLECTED_TYPE;
// clang-format on

wdActorPlugin::wdActorPlugin() = default;
wdActorPlugin::~wdActorPlugin() = default;

wdActor* wdActorPlugin::GetActor() const
{
  return m_pOwningActor;
}


WD_STATICLINK_FILE(Core, Core_ActorSystem_Implementation_ActorPlugin);
