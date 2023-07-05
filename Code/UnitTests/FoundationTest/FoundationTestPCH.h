#pragma once

#include <TestFramework/Framework/TestFramework.h>
#include <TestFramework/Utilities/ConstructionCounter.h>

#include <Foundation/Basics.h>
#include <Foundation/Basics/Assert.h>
#include <Foundation/Containers/Deque.h>
#include <Foundation/Containers/DynamicArray.h>
#include <Foundation/Reflection/Reflection.h>
#include <Foundation/Reflection/ReflectionUtils.h>
#include <Foundation/Strings/String.h>
#include <Foundation/Strings/StringBuilder.h>
#include <Foundation/Types/Bitflags.h>
#include <Foundation/Types/TypeTraits.h>
#include <Foundation/Types/Types.h>
#include <Foundation/Types/Variant.h>

#include <Foundation/Math/Declarations.h>

typedef float wdMathTestType;

typedef wdVec2Template<wdMathTestType> wdVec2T;                           ///< This is only for testing purposes
typedef wdVec3Template<wdMathTestType> wdVec3T;                           ///< This is only for testing purposes
typedef wdVec4Template<wdMathTestType> wdVec4T;                           ///< This is only for testing purposes
typedef wdMat3Template<wdMathTestType> wdMat3T;                           ///< This is only for testing purposes
typedef wdMat4Template<wdMathTestType> wdMat4T;                           ///< This is only for testing purposes
typedef wdQuatTemplate<wdMathTestType> wdQuatT;                           ///< This is only for testing purposes
typedef wdPlaneTemplate<wdMathTestType> wdPlaneT;                         ///< This is only for testing purposes
typedef wdBoundingBoxTemplate<wdMathTestType> wdBoundingBoxT;             ///< This is only for testing purposes
typedef wdBoundingBoxSphereTemplate<wdMathTestType> wdBoundingBoxSphereT; ///< This is only for testing purposes
typedef wdBoundingSphereTemplate<wdMathTestType> wdBoundingSphereT;       ///< This is only for testing purposes
typedef wdTransformTemplate<wdMathTestType> wdTransformT;

#define wdFoundationTest_Plugin1 "wdFoundationTest_Plugin1"
#define wdFoundationTest_Plugin2 "wdFoundationTest_Plugin2"
