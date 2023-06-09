#include <Foundation/FoundationInternal.h>
WD_FOUNDATION_INTERNAL_HEADER

#include <Foundation/Basics/Platform/Android/AndroidJni.h>
#include <Foundation/Basics/Platform/Android/AndroidUtils.h>
#include <android_native_app_glue.h>

void wdUuid::CreateNewUuid()
{
  wdJniAttachment attachment;

  wdJniClass uuidClass("java/util/UUID");
  WD_ASSERT_DEBUG(!uuidClass.IsNull(), "UUID class not found.");
  wdJniObject javaUuid = uuidClass.CallStatic<wdJniObject>("randomUUID");
  jlong mostSignificant = javaUuid.Call<jlong>("getMostSignificantBits");
  jlong leastSignificant = javaUuid.Call<jlong>("getLeastSignificantBits");

  m_uiHigh = mostSignificant;
  m_uiLow = leastSignificant;

  //#TODO maybe faster to read /proc/sys/kernel/random/uuid, but that can't be done via wdOSFile
  // see https://stackoverflow.com/questions/11888055/include-uuid-h-into-android-ndk-project
}
