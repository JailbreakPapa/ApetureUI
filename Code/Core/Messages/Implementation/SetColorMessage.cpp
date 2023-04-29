#include <Core/CorePCH.h>

#include <Core/Messages/SetColorMessage.h>

// clang-format off
WD_BEGIN_STATIC_REFLECTED_ENUM(wdSetColorMode, 1)
WD_ENUM_CONSTANTS(wdSetColorMode::SetRGBA, wdSetColorMode::SetRGB, wdSetColorMode::SetAlpha, wdSetColorMode::AlphaBlend, wdSetColorMode::Additive, wdSetColorMode::Modulate)
WD_END_STATIC_REFLECTED_ENUM;

WD_IMPLEMENT_MESSAGE_TYPE(wdMsgSetColor);
WD_BEGIN_DYNAMIC_REFLECTED_TYPE(wdMsgSetColor, 1, wdRTTIDefaultAllocator<wdMsgSetColor>)
{
  WD_BEGIN_PROPERTIES
  {
    WD_MEMBER_PROPERTY("Color", m_Color),
    WD_ENUM_MEMBER_PROPERTY("Mode", wdSetColorMode, m_Mode)
  }
  WD_END_PROPERTIES;

  WD_BEGIN_ATTRIBUTES
  {
    new wdAutoGenVisScriptMsgSender,
  }
  WD_END_ATTRIBUTES;
}
WD_END_DYNAMIC_REFLECTED_TYPE;
// clang-format on

void wdMsgSetColor::ModifyColor(wdColor& ref_color) const
{
  switch (m_Mode)
  {
    case wdSetColorMode::SetRGB:
      ref_color.SetRGB(m_Color.r, m_Color.g, m_Color.b);
      break;

    case wdSetColorMode::SetAlpha:
      ref_color.a = m_Color.a;
      break;

    case wdSetColorMode::AlphaBlend:
      ref_color = wdMath::Lerp(ref_color, m_Color, m_Color.a);
      break;

    case wdSetColorMode::Additive:
      ref_color += m_Color;
      break;

    case wdSetColorMode::Modulate:
      ref_color *= m_Color;
      break;

    case wdSetColorMode::SetRGBA:
    default:
      ref_color = m_Color;
      break;
  }
}

void wdMsgSetColor::ModifyColor(wdColorGammaUB& ref_color) const
{
  wdColor temp = ref_color;
  ModifyColor(temp);
  ref_color = temp;
}

void wdMsgSetColor::Serialize(wdStreamWriter& inout_stream) const
{
  inout_stream << m_Color;
  inout_stream << m_Mode;
}

void wdMsgSetColor::Deserialize(wdStreamReader& inout_stream, wdUInt8 uiTypeVersion)
{
  inout_stream >> m_Color;
  inout_stream >> m_Mode;
}



WD_STATICLINK_FILE(Core, Core_Messages_Implementation_SetColorMessage);
