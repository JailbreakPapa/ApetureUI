#pragma once

#include <Foundation/Basics.h>

#include <PacManPlugin/PacManPluginDLL.h>

#include <Core/Collection/CollectionResource.h>
#include <Core/Input/Declarations.h>
#include <Core/Input/InputManager.h>
#include <Core/Interfaces/PhysicsWorldModule.h>
#include <Core/Interfaces/SoundInterface.h>
#include <Core/Prefabs/PrefabResource.h>
#include <Core/ResourceManager/ResourceHandle.h>
#include <Core/ResourceManager/ResourceManager.h>
#include <Core/Utils/Blackboard.h>
#include <Core/World/Component.h>
#include <Core/World/ComponentManager.h>
#include <Core/World/Declarations.h>
#include <Core/World/World.h>
#include <Core/WorldSerializer/WorldReader.h>
#include <Core/WorldSerializer/WorldWriter.h>
#include <Foundation/Configuration/CVar.h>
#include <Foundation/Configuration/Singleton.h>
#include <Foundation/Containers/Deque.h>
#include <Foundation/IO/FileSystem/FileReader.h>
#include <Foundation/IO/FileSystem/FileWriter.h>
#include <Foundation/IO/MemoryStream.h>
#include <Foundation/Logging/Log.h>
#include <Foundation/Serialization/ReflectionSerializer.h>
#include <Foundation/Types/Bitflags.h>
#include <Foundation/Types/SharedPtr.h>
#include <Foundation/Types/Uuid.h>
#include <GameEngine/DearImgui/DearImgui.h>
#include <GameEngine/GameApplication/GameApplication.h>
#include <GameEngine/GameState/FallbackGameState.h>
#include <GameEngine/GameState/GameState.h>
#include <GameEngine/Gameplay/InputComponent.h>
#include <Imgui/imgui.h>
#include <RendererCore/Debug/DebugRenderer.h>
#include <RendererCore/Material/MaterialResource.h>
#include <RendererCore/Meshes/MeshComponent.h>
#include <RendererCore/Pipeline/View.h>
#include <RendererCore/RenderWorld/RenderWorld.h>
#include <Utilities/DataStructures/GameGrid.h>

enum PacManState : wdInt32
{
  Alive,
  EatenByGhost,
  WonGame,
};

enum WalkDirection : wdUInt8
{
  Up = 0,
  Right = 1,
  Down = 2,
  Left = 3
};
