#pragma once

#include <Core/CoreDLL.h>
#include <Foundation/Communication/Message.h>

struct wdSetColorMode
{
  typedef wdUInt32 StorageType;

  enum Enum
  {
    SetRGBA,
    SetRGB,
    SetAlpha,

    AlphaBlend,
    Additive,
    Modulate,

    Default = SetRGBA
  };
};

WD_DECLARE_REFLECTABLE_TYPE(WD_CORE_DLL, wdSetColorMode);

struct WD_CORE_DLL wdMsgSetColor : public wdMessage
{
  WD_DECLARE_MESSAGE_TYPE(wdMsgSetColor, wdMessage);

  wdColor m_Color;
  wdEnum<wdSetColorMode> m_Mode;

  void ModifyColor(wdColor& ref_color) const;
  void ModifyColor(wdColorGammaUB& ref_color) const;

  //////////////////////////////////////////////////////////////////////////
  // wdMessage interface
  //

  virtual void Serialize(wdStreamWriter& inout_stream) const override;
  virtual void Deserialize(wdStreamReader& inout_stream, wdUInt8 uiTypeVersion) override;
};
