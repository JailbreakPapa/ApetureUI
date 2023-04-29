#pragma once

#include <Foundation/Basics.h>
#include <Foundation/Reflection/Reflection.h>
#include <Foundation/Strings/String.h>

class WD_CORE_DLL wdWorldModuleConfig
{
public:
  wdResult Save();
  void Load();
  void Apply();

  void AddInterfaceImplementation(wdStringView sInterfaceName, wdStringView sImplementationName);
  void RemoveInterfaceImplementation(wdStringView sInterfaceName);

  struct InterfaceImpl
  {
    wdString m_sInterfaceName;
    wdString m_sImplementationName;

    bool operator<(const InterfaceImpl& rhs) const { return m_sInterfaceName < rhs.m_sInterfaceName; }
  };

  wdHybridArray<InterfaceImpl, 8> m_InterfaceImpls;
};
