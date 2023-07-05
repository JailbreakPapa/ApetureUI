#include <FoundationTest/FoundationTestPCH.h>

#include <Foundation/CodeUtils/Expression/ExpressionByteCode.h>
#include <Foundation/CodeUtils/Expression/ExpressionCompiler.h>
#include <Foundation/CodeUtils/Expression/ExpressionParser.h>
#include <Foundation/CodeUtils/Expression/ExpressionVM.h>
#include <Foundation/IO/FileSystem/FileSystem.h>
#include <Foundation/IO/FileSystem/FileWriter.h>
#include <Foundation/Math/Float16.h>
#include <Foundation/Types/UniquePtr.h>

namespace
{
  static wdUInt32 s_uiNumASTDumps = 0;

  void MakeASTOutputPath(wdStringView sOutputName, wdStringBuilder& out_sOutputPath)
  {
    wdUInt32 uiCounter = s_uiNumASTDumps;
    ++s_uiNumASTDumps;

    out_sOutputPath.Format(":output/Expression/{}_{}_AST.dgml", wdArgU(uiCounter, 2, true), sOutputName);
  }

  void DumpDisassembly(const wdExpressionByteCode& byteCode, wdStringView sOutputName, wdUInt32 uiCounter)
  {
    wdStringBuilder sDisassembly;
    byteCode.Disassemble(sDisassembly);

    wdStringBuilder sFileName;
    sFileName.Format(":output/Expression/{}_{}_ByteCode.txt", wdArgU(uiCounter, 2, true), sOutputName);

    wdFileWriter fileWriter;
    if (fileWriter.Open(sFileName).Succeeded())
    {
      fileWriter.WriteBytes(sDisassembly.GetData(), sDisassembly.GetElementCount()).IgnoreResult();

      wdLog::Error("Disassembly was dumped to: {}", sFileName);
    }
    else
    {
      wdLog::Error("Failed to dump Disassembly to: {}", sFileName);
    }
  }

  static wdUInt32 s_uiNumByteCodeComparisons = 0;

  bool CompareByteCode(const wdExpressionByteCode& testCode, const wdExpressionByteCode& referenceCode)
  {
    wdUInt32 uiCounter = s_uiNumByteCodeComparisons;
    ++s_uiNumByteCodeComparisons;

    if (testCode != referenceCode)
    {
      DumpDisassembly(referenceCode, "Reference", uiCounter);
      DumpDisassembly(testCode, "Test", uiCounter);
      return false;
    }

    return true;
  }

  static wdHashedString s_sA = wdMakeHashedString("a");
  static wdHashedString s_sB = wdMakeHashedString("b");
  static wdHashedString s_sC = wdMakeHashedString("c");
  static wdHashedString s_sD = wdMakeHashedString("d");
  static wdHashedString s_sOutput = wdMakeHashedString("output");

  static wdUniquePtr<wdExpressionParser> s_pParser;
  static wdUniquePtr<wdExpressionCompiler> s_pCompiler;
  static wdUniquePtr<wdExpressionVM> s_pVM;

  template <typename T>
  struct StreamDataTypeDeduction
  {
  };

  template <>
  struct StreamDataTypeDeduction<wdFloat16>
  {
    static constexpr wdProcessingStream::DataType Type = wdProcessingStream::DataType::Half;
    static wdFloat16 Default() { return wdMath::MinValue<float>(); }
  };

  template <>
  struct StreamDataTypeDeduction<float>
  {
    static constexpr wdProcessingStream::DataType Type = wdProcessingStream::DataType::Float;
    static float Default() { return wdMath::MinValue<float>(); }
  };

  template <>
  struct StreamDataTypeDeduction<wdInt8>
  {
    static constexpr wdProcessingStream::DataType Type = wdProcessingStream::DataType::Byte;
    static wdInt8 Default() { return wdMath::MinValue<wdInt8>(); }
  };

  template <>
  struct StreamDataTypeDeduction<wdInt16>
  {
    static constexpr wdProcessingStream::DataType Type = wdProcessingStream::DataType::Short;
    static wdInt16 Default() { return wdMath::MinValue<wdInt16>(); }
  };

  template <>
  struct StreamDataTypeDeduction<int>
  {
    static constexpr wdProcessingStream::DataType Type = wdProcessingStream::DataType::Int;
    static int Default() { return wdMath::MinValue<int>(); }
  };

  template <>
  struct StreamDataTypeDeduction<wdVec3>
  {
    static constexpr wdProcessingStream::DataType Type = wdProcessingStream::DataType::Float3;
    static wdVec3 Default() { return wdVec3(wdMath::MinValue<float>()); }
  };

  template <>
  struct StreamDataTypeDeduction<wdVec3I32>
  {
    static constexpr wdProcessingStream::DataType Type = wdProcessingStream::DataType::Int3;
    static wdVec3I32 Default() { return wdVec3I32(wdMath::MinValue<int>()); }
  };

  template <typename T>
  void Compile(wdStringView sCode, wdExpressionByteCode& out_byteCode, wdStringView sDumpAstOutputName = wdStringView())
  {
    wdExpression::StreamDesc inputs[] = {
      {s_sA, StreamDataTypeDeduction<T>::Type},
      {s_sB, StreamDataTypeDeduction<T>::Type},
      {s_sC, StreamDataTypeDeduction<T>::Type},
      {s_sD, StreamDataTypeDeduction<T>::Type},
    };

    wdExpression::StreamDesc outputs[] = {
      {s_sOutput, StreamDataTypeDeduction<T>::Type},
    };

    wdExpressionAST ast;
    WD_TEST_BOOL(s_pParser->Parse(sCode, inputs, outputs, {}, ast).Succeeded());

    wdStringBuilder sOutputPath;
    if (sDumpAstOutputName.IsEmpty() == false)
    {
      MakeASTOutputPath(sDumpAstOutputName, sOutputPath);
    }
    WD_TEST_BOOL(s_pCompiler->Compile(ast, out_byteCode, sOutputPath).Succeeded());
  }

  template <typename T>
  T Execute(const wdExpressionByteCode& byteCode, T a = T(0), T b = T(0), T c = T(0), T d = T(0))
  {
    wdProcessingStream inputs[] = {
      wdProcessingStream(s_sA, wdMakeArrayPtr(&a, 1).ToByteArray(), StreamDataTypeDeduction<T>::Type),
      wdProcessingStream(s_sB, wdMakeArrayPtr(&b, 1).ToByteArray(), StreamDataTypeDeduction<T>::Type),
      wdProcessingStream(s_sC, wdMakeArrayPtr(&c, 1).ToByteArray(), StreamDataTypeDeduction<T>::Type),
      wdProcessingStream(s_sD, wdMakeArrayPtr(&d, 1).ToByteArray(), StreamDataTypeDeduction<T>::Type),
    };

    T output = StreamDataTypeDeduction<T>::Default();
    wdProcessingStream outputs[] = {
      wdProcessingStream(s_sOutput, wdMakeArrayPtr(&output, 1).ToByteArray(), StreamDataTypeDeduction<T>::Type),
    };

    WD_TEST_BOOL(s_pVM->Execute(byteCode, inputs, outputs, 1).Succeeded());

    return output;
  };

  template <typename T>
  T TestInstruction(wdStringView sCode, T a = T(0), T b = T(0), T c = T(0), T d = T(0), bool bDumpASTs = false)
  {
    wdExpressionByteCode byteCode;
    Compile<T>(sCode, byteCode, bDumpASTs ? "TestInstruction" : "");
    return Execute<T>(byteCode, a, b, c, d);
  }

  template <typename T>
  T TestConstant(wdStringView sCode, bool bDumpASTs = false)
  {
    wdExpressionByteCode byteCode;
    Compile<T>(sCode, byteCode, bDumpASTs ? "TestConstant" : "");
    WD_TEST_INT(byteCode.GetNumInstructions(), 2); // MovX_C, StoreX
    WD_TEST_INT(byteCode.GetNumTempRegisters(), 1);
    return Execute<T>(byteCode);
  }

  enum TestBinaryFlags
  {
    LeftConstantOptimization = WD_BIT(0),
    NoInstructionsCountCheck = WD_BIT(2),
  };

  template <typename R, typename T, wdUInt32 flags>
  void TestBinaryInstruction(wdStringView sOp, T a, T b, T expectedResult, bool bDumpASTs = false)
  {
    constexpr bool boolInputs = std::is_same<T, bool>::value;
    using U = typename std::conditional<boolInputs, int, T>::type;

    U aAsU;
    U bAsU;
    U expectedResultAsU;
    if constexpr (boolInputs)
    {
      aAsU = a ? 1 : 0;
      bAsU = b ? 1 : 0;
      expectedResultAsU = expectedResult ? 1 : 0;
    }
    else
    {
      aAsU = a;
      bAsU = b;
      expectedResultAsU = expectedResult;
    }

    auto TestRes = [](U res, U expectedRes, const char* szCode, const char* szAValue, const char* szBValue) {
      if constexpr (std::is_same<T, float>::value)
      {
        WD_TEST_FLOAT_MSG(res, expectedRes, wdMath::DefaultEpsilon<float>(), "%s (a=%s, b=%s)", szCode, szAValue, szBValue);
      }
      else if constexpr (std::is_same<T, int>::value)
      {
        WD_TEST_INT_MSG(res, expectedRes, "%s (a=%s, b=%s)", szCode, szAValue, szBValue);
      }
      else if constexpr (std::is_same<T, bool>::value)
      {
        const char* szRes = (res != 0) ? "true" : "false";
        const char* szExpectedRes = (expectedRes != 0) ? "true" : "false";
        WD_TEST_STRING_MSG(szRes, szExpectedRes, "%s (a=%s, b=%s)", szCode, szAValue, szBValue);
      }
      else if constexpr (std::is_same<T, wdVec3>::value)
      {
        WD_TEST_VEC3_MSG(res, expectedRes, wdMath::DefaultEpsilon<float>(), "%s (a=%s, b=%s)", szCode, szAValue, szBValue);
      }
      else if constexpr (std::is_same<T, wdVec3I32>::value)
      {
        WD_TEST_INT_MSG(res.x, expectedRes.x, "%s (a=%s, b=%s)", szCode, szAValue, szBValue);
        WD_TEST_INT_MSG(res.y, expectedRes.y, "%s (a=%s, b=%s)", szCode, szAValue, szBValue);
        WD_TEST_INT_MSG(res.z, expectedRes.z, "%s (a=%s, b=%s)", szCode, szAValue, szBValue);
      }
      else
      {
        WD_ASSERT_NOT_IMPLEMENTED;
      }
    };

    const bool functionStyleSyntax = sOp.FindSubString("(");
    const char* formatString = functionStyleSyntax ? "output = {0}{1}, {2})" : "output = {1} {0} {2}";
    const char* aInput = boolInputs ? "(a != 0)" : "a";
    const char* bInput = boolInputs ? "(b != 0)" : "b";

    wdStringBuilder aValue;
    wdStringBuilder bValue;
    if constexpr (std::is_same<T, wdVec3>::value || std::is_same<T, wdVec3I32>::value)
    {
      aValue.Format("vec3({}, {}, {})", a.x, a.y, a.z);
      bValue.Format("vec3({}, {}, {})", b.x, b.y, b.z);
    }
    else
    {
      aValue.Format("{}", a);
      bValue.Format("{}", b);
    }

    int oneConstantInstructions = 3; // LoadX, OpX_RC, StoreX
    int oneConstantRegisters = 1;
    if constexpr (std::is_same<R, bool>::value)
    {
      oneConstantInstructions += 3; // + MovX_C, MovX_C, SelI_RRR
      oneConstantRegisters += 2;    // Two more registers needed for constants above
    }
    if constexpr (boolInputs)
    {
      oneConstantInstructions += 1; // + NotEqI_RC
    }

    int numOutputElements = 1;
    bool hasDifferentOutputElements = false;
    if constexpr (std::is_same<T, wdVec3>::value || std::is_same<T, wdVec3I32>::value)
    {
      numOutputElements = 3;

      for (int i = 1; i < 3; ++i)
      {
        if (expectedResult.GetData()[i] != expectedResult.GetData()[i - 1])
        {
          hasDifferentOutputElements = true;
          break;
        }
      }
    }

    wdStringBuilder code;
    wdExpressionByteCode byteCode;

    code.Format(formatString, sOp, aInput, bInput);
    Compile<U>(code, byteCode, bDumpASTs ? "BinaryNoConstants" : "");
    TestRes(Execute<U>(byteCode, aAsU, bAsU), expectedResultAsU, code, aValue, bValue);

    code.Format(formatString, sOp, aValue, bInput);
    Compile<U>(code, byteCode, bDumpASTs ? "BinaryLeftConstant" : "");
    if constexpr ((flags & NoInstructionsCountCheck) == 0)
    {
      int leftConstantInstructions = oneConstantInstructions;
      int leftConstantRegisters = oneConstantRegisters;
      if constexpr ((flags & LeftConstantOptimization) == 0)
      {
        leftConstantInstructions += 1;
        leftConstantRegisters += 1;
      }

      if (byteCode.GetNumInstructions() != leftConstantInstructions || byteCode.GetNumTempRegisters() != leftConstantRegisters)
      {
        DumpDisassembly(byteCode, "BinaryLeftConstant", 0);
        WD_TEST_INT(byteCode.GetNumInstructions(), leftConstantInstructions);
        WD_TEST_INT(byteCode.GetNumTempRegisters(), leftConstantRegisters);
      }
    }
    TestRes(Execute<U>(byteCode, aAsU, bAsU), expectedResultAsU, code, aValue, bValue);

    code.Format(formatString, sOp, aInput, bValue);
    Compile<U>(code, byteCode, bDumpASTs ? "BinaryRightConstant" : "");
    if constexpr ((flags & NoInstructionsCountCheck) == 0)
    {
      if (byteCode.GetNumInstructions() != oneConstantInstructions || byteCode.GetNumTempRegisters() != oneConstantRegisters)
      {
        DumpDisassembly(byteCode, "BinaryRightConstant", 0);
        WD_TEST_INT(byteCode.GetNumInstructions(), oneConstantInstructions);
        WD_TEST_INT(byteCode.GetNumTempRegisters(), oneConstantRegisters);
      }
    }
    TestRes(Execute<U>(byteCode, aAsU, bAsU), expectedResultAsU, code, aValue, bValue);

    code.Format(formatString, sOp, aValue, bValue);
    Compile<U>(code, byteCode, bDumpASTs ? "BinaryConstant" : "");
    if (hasDifferentOutputElements == false)
    {
      int bothConstantsInstructions = 1 + numOutputElements; // MovX_C + StoreX * numOutputElements
      int bothConstantsRegisters = 1;
      if (byteCode.GetNumInstructions() != bothConstantsInstructions || byteCode.GetNumTempRegisters() != bothConstantsRegisters)
      {
        DumpDisassembly(byteCode, "BinaryConstant", 0);
        WD_TEST_INT(byteCode.GetNumInstructions(), bothConstantsInstructions);
        WD_TEST_INT(byteCode.GetNumTempRegisters(), bothConstantsRegisters);
      }
    }
    TestRes(Execute<U>(byteCode), expectedResultAsU, code, aValue, bValue);
  }

  template <typename T>
  bool CompareCode(wdStringView sTestCode, wdStringView sReferenceCode, wdExpressionByteCode& out_testByteCode, bool bDumpASTs = false)
  {
    Compile<T>(sTestCode, out_testByteCode, bDumpASTs ? "Test" : "");

    wdExpressionByteCode referenceByteCode;
    Compile<T>(sReferenceCode, referenceByteCode, bDumpASTs ? "Reference" : "");

    return CompareByteCode(out_testByteCode, referenceByteCode);
  }

  template <typename T>
  void TestInputOutput()
  {
    wdStringView testCode = "output = a + b * 2";
    wdExpressionByteCode testByteCode;
    Compile<T>(testCode, testByteCode);

    constexpr wdUInt32 uiCount = 17;
    wdHybridArray<T, uiCount> a;
    wdHybridArray<T, uiCount> b;
    wdHybridArray<T, uiCount> o;
    wdHybridArray<float, uiCount> expectedOutput;
    a.SetCountUninitialized(uiCount);
    b.SetCountUninitialized(uiCount);
    o.SetCount(uiCount);
    expectedOutput.SetCountUninitialized(uiCount);

    for (wdUInt32 i = 0; i < uiCount; ++i)
    {
      a[i] = static_cast<T>(3.0f * i);
      b[i] = static_cast<T>(1.5f * i);
      expectedOutput[i] = a[i] + b[i] * 2.0f;
    }

    wdProcessingStream inputs[] = {
      wdProcessingStream(s_sA, a.GetByteArrayPtr(), StreamDataTypeDeduction<T>::Type),
      wdProcessingStream(s_sB, b.GetByteArrayPtr(), StreamDataTypeDeduction<T>::Type),
    };

    wdProcessingStream outputs[] = {
      wdProcessingStream(s_sOutput, o.GetByteArrayPtr(), StreamDataTypeDeduction<T>::Type),
    };

    WD_TEST_BOOL(s_pVM->Execute(testByteCode, inputs, outputs, uiCount).Succeeded());

    for (wdUInt32 i = 0; i < uiCount; ++i)
    {
      WD_TEST_FLOAT(static_cast<float>(o[i]), expectedOutput[i], wdMath::DefaultEpsilon<float>());
    }
  }

  static const wdEnum<wdExpression::RegisterType> s_TestFunc1InputTypes[] = {wdExpression::RegisterType::Float, wdExpression::RegisterType::Int};
  static const wdEnum<wdExpression::RegisterType> s_TestFunc2InputTypes[] = {wdExpression::RegisterType::Float, wdExpression::RegisterType::Float, wdExpression::RegisterType::Int};

  static void TestFunc1(wdExpression::Inputs inputs, wdExpression::Output output, const wdExpression::GlobalData& globalData)
  {
    const wdExpression::Register* pX = inputs[0].GetPtr();
    const wdExpression::Register* pY = inputs[1].GetPtr();
    const wdExpression::Register* pXEnd = inputs[0].GetEndPtr();
    wdExpression::Register* pOutput = output.GetPtr();

    while (pX < pXEnd)
    {
      pOutput->f = pX->f.CompMul(pY->i.ToFloat());

      ++pX;
      ++pY;
      ++pOutput;
    }
  }

  static void TestFunc2(wdExpression::Inputs inputs, wdExpression::Output output, const wdExpression::GlobalData& globalData)
  {
    const wdExpression::Register* pX = inputs[0].GetPtr();
    const wdExpression::Register* pY = inputs[1].GetPtr();
    const wdExpression::Register* pXEnd = inputs[0].GetEndPtr();
    wdExpression::Register* pOutput = output.GetPtr();

    if (inputs.GetCount() >= 3)
    {
      const wdExpression::Register* pZ = inputs[2].GetPtr();

      while (pX < pXEnd)
      {
        pOutput->f = pX->f.CompMul(pY->f) * 2.0f + pZ->i.ToFloat();

        ++pX;
        ++pY;
        ++pZ;
        ++pOutput;
      }
    }
    else
    {
      while (pX < pXEnd)
      {
        pOutput->f = pX->f.CompMul(pY->f) * 2.0f;

        ++pX;
        ++pY;
        ++pOutput;
      }
    }
  }

  wdExpressionFunction s_TestFunc1 = {
    {wdMakeHashedString("TestFunc"), wdMakeArrayPtr(s_TestFunc1InputTypes), 2, wdExpression::RegisterType::Float},
    &TestFunc1,
  };

  wdExpressionFunction s_TestFunc2 = {
    {wdMakeHashedString("TestFunc"), wdMakeArrayPtr(s_TestFunc2InputTypes), 3, wdExpression::RegisterType::Float},
    &TestFunc2,
  };

} // namespace

WD_CREATE_SIMPLE_TEST(CodeUtils, Expression)
{
  s_uiNumByteCodeComparisons = 0;

  wdStringBuilder outputPath = wdTestFramework::GetInstance()->GetAbsOutputPath();
  WD_TEST_BOOL(wdFileSystem::AddDataDirectory(outputPath.GetData(), "test", "output", wdFileSystem::AllowWrites) == WD_SUCCESS);

  s_pParser = WD_DEFAULT_NEW(wdExpressionParser);
  s_pCompiler = WD_DEFAULT_NEW(wdExpressionCompiler);
  s_pVM = WD_DEFAULT_NEW(wdExpressionVM);
  WD_SCOPE_EXIT(s_pParser = nullptr; s_pCompiler = nullptr; s_pVM = nullptr;);

  WD_TEST_BLOCK(wdTestBlock::Enabled, "Unary instructions")
  {
    // Negate
    WD_TEST_INT(TestInstruction("output = -a", 2), -2);
    WD_TEST_FLOAT(TestInstruction("output = -a", 2.5f), -2.5f, wdMath::DefaultEpsilon<float>());
    WD_TEST_INT(TestConstant<int>("output = -2"), -2);
    WD_TEST_FLOAT(TestConstant<float>("output = -2.5"), -2.5f, wdMath::DefaultEpsilon<float>());

    // Absolute
    WD_TEST_INT(TestInstruction("output = abs(a)", -2), 2);
    WD_TEST_FLOAT(TestInstruction("output = abs(a)", -2.5f), 2.5f, wdMath::DefaultEpsilon<float>());
    WD_TEST_INT(TestConstant<int>("output = abs(-2)"), 2);
    WD_TEST_FLOAT(TestConstant<float>("output = abs(-2.5)"), 2.5f, wdMath::DefaultEpsilon<float>());

    // Saturate
    WD_TEST_INT(TestInstruction("output = saturate(a)", -1), 0);
    WD_TEST_INT(TestInstruction("output = saturate(a)", 2), 1);
    WD_TEST_FLOAT(TestInstruction("output = saturate(a)", -1.5f), 0.0f, wdMath::DefaultEpsilon<float>());
    WD_TEST_FLOAT(TestInstruction("output = saturate(a)", 2.5f), 1.0f, wdMath::DefaultEpsilon<float>());

    WD_TEST_INT(TestConstant<int>("output = saturate(-1)"), 0);
    WD_TEST_INT(TestConstant<int>("output = saturate(2)"), 1);
    WD_TEST_FLOAT(TestConstant<float>("output = saturate(-1.5)"), 0.0f, wdMath::DefaultEpsilon<float>());
    WD_TEST_FLOAT(TestConstant<float>("output = saturate(2.5)"), 1.0f, wdMath::DefaultEpsilon<float>());

    // Sqrt
    WD_TEST_FLOAT(TestInstruction("output = sqrt(a)", 25.0f), 5.0f, wdMath::DefaultEpsilon<float>());
    WD_TEST_FLOAT(TestInstruction("output = sqrt(a)", 2.0f), wdMath::Sqrt(2.0f), wdMath::DefaultEpsilon<float>());
    WD_TEST_FLOAT(TestConstant<float>("output = sqrt(25)"), 5.0f, wdMath::DefaultEpsilon<float>());
    WD_TEST_FLOAT(TestConstant<float>("output = sqrt(2)"), wdMath::Sqrt(2.0f), wdMath::DefaultEpsilon<float>());

    // Exp
    WD_TEST_FLOAT(TestInstruction("output = exp(a)", 0.0f), 1.0f, wdMath::DefaultEpsilon<float>());
    WD_TEST_FLOAT(TestInstruction("output = exp(a)", 2.0f), wdMath::Exp(2.0f), wdMath::DefaultEpsilon<float>());
    WD_TEST_FLOAT(TestConstant<float>("output = exp(0.0)"), 1.0f, wdMath::DefaultEpsilon<float>());
    WD_TEST_FLOAT(TestConstant<float>("output = exp(2.0)"), wdMath::Exp(2.0f), wdMath::DefaultEpsilon<float>());

    // Ln
    WD_TEST_FLOAT(TestInstruction("output = ln(a)", 1.0f), 0.0f, wdMath::DefaultEpsilon<float>());
    WD_TEST_FLOAT(TestInstruction("output = ln(a)", 2.0f), wdMath::Ln(2.0f), wdMath::DefaultEpsilon<float>());
    WD_TEST_FLOAT(TestConstant<float>("output = ln(1.0)"), 0.0f, wdMath::DefaultEpsilon<float>());
    WD_TEST_FLOAT(TestConstant<float>("output = ln(2.0)"), wdMath::Ln(2.0f), wdMath::DefaultEpsilon<float>());

    // Log2
    WD_TEST_INT(TestInstruction("output = log2(a)", 1), 0);
    WD_TEST_INT(TestInstruction("output = log2(a)", 8), 3);
    WD_TEST_FLOAT(TestInstruction("output = log2(a)", 1.0f), 0.0f, wdMath::DefaultEpsilon<float>());
    WD_TEST_FLOAT(TestInstruction("output = log2(a)", 4.0f), 2.0f, wdMath::DefaultEpsilon<float>());

    WD_TEST_INT(TestConstant<int>("output = log2(1)"), 0);
    WD_TEST_INT(TestConstant<int>("output = log2(16)"), 4);
    WD_TEST_FLOAT(TestConstant<float>("output = log2(1.0)"), 0.0f, wdMath::DefaultEpsilon<float>());
    WD_TEST_FLOAT(TestConstant<float>("output = log2(32.0)"), 5.0f, wdMath::DefaultEpsilon<float>());

    // Log10
    WD_TEST_FLOAT(TestInstruction("output = log10(a)", 10.0f), 1.0f, wdMath::DefaultEpsilon<float>());
    WD_TEST_FLOAT(TestInstruction("output = log10(a)", 1000.0f), 3.0f, wdMath::DefaultEpsilon<float>());
    WD_TEST_FLOAT(TestConstant<float>("output = log10(10.0)"), 1.0f, wdMath::DefaultEpsilon<float>());
    WD_TEST_FLOAT(TestConstant<float>("output = log10(100.0)"), 2.0f, wdMath::DefaultEpsilon<float>());

    // Pow2
    WD_TEST_INT(TestInstruction("output = pow2(a)", 0), 1);
    WD_TEST_INT(TestInstruction("output = pow2(a)", 3), 8);
    WD_TEST_FLOAT(TestInstruction("output = pow2(a)", 4.0f), 16.0f, wdMath::DefaultEpsilon<float>());
    WD_TEST_FLOAT(TestInstruction("output = pow2(a)", 6.0f), 64.0f, wdMath::DefaultEpsilon<float>());

    WD_TEST_INT(TestConstant<int>("output = pow2(0)"), 1);
    WD_TEST_INT(TestConstant<int>("output = pow2(3)"), 8);
    WD_TEST_FLOAT(TestConstant<float>("output = pow2(3.0)"), 8.0f, wdMath::DefaultEpsilon<float>());
    WD_TEST_FLOAT(TestConstant<float>("output = pow2(5.0)"), 32.0f, wdMath::DefaultEpsilon<float>());

    // Sin
    WD_TEST_FLOAT(TestInstruction("output = sin(a)", wdAngle::Degree(90.0f).GetRadian()), 1.0f, wdMath::DefaultEpsilon<float>());
    WD_TEST_FLOAT(TestInstruction("output = sin(a)", wdAngle::Degree(45.0f).GetRadian()), wdMath::Sin(wdAngle::Degree(45.0f)), wdMath::DefaultEpsilon<float>());
    WD_TEST_FLOAT(TestConstant<float>("output = sin(PI / 2)"), 1.0f, wdMath::DefaultEpsilon<float>());
    WD_TEST_FLOAT(TestConstant<float>("output = sin(PI / 4)"), wdMath::Sin(wdAngle::Degree(45.0f)), wdMath::DefaultEpsilon<float>());

    // Cos
    WD_TEST_FLOAT(TestInstruction("output = cos(a)", 0.0f), 1.0f, wdMath::DefaultEpsilon<float>());
    WD_TEST_FLOAT(TestInstruction("output = cos(a)", wdAngle::Degree(45.0f).GetRadian()), wdMath::Cos(wdAngle::Degree(45.0f)), wdMath::DefaultEpsilon<float>());
    WD_TEST_FLOAT(TestConstant<float>("output = cos(0)"), 1.0f, wdMath::DefaultEpsilon<float>());
    WD_TEST_FLOAT(TestConstant<float>("output = cos(PI / 4)"), wdMath::Cos(wdAngle::Degree(45.0f)), wdMath::DefaultEpsilon<float>());

    // Tan
    WD_TEST_FLOAT(TestInstruction("output = tan(a)", 0.0f), 0.0f, wdMath::DefaultEpsilon<float>());
    WD_TEST_FLOAT(TestInstruction("output = tan(a)", wdAngle::Degree(45.0f).GetRadian()), 1.0f, wdMath::DefaultEpsilon<float>());
    WD_TEST_FLOAT(TestConstant<float>("output = tan(0)"), 0.0f, wdMath::DefaultEpsilon<float>());
    WD_TEST_FLOAT(TestConstant<float>("output = tan(PI / 4)"), 1.0f, wdMath::DefaultEpsilon<float>());

    // ASin
    WD_TEST_FLOAT(TestInstruction("output = asin(a)", 1.0f), wdAngle::Degree(90.0f).GetRadian(), wdMath::DefaultEpsilon<float>());
    WD_TEST_FLOAT(TestInstruction("output = asin(a)", wdMath::Sin(wdAngle::Degree(45.0f))), wdAngle::Degree(45.0f).GetRadian(), wdMath::LargeEpsilon<float>());
    WD_TEST_FLOAT(TestConstant<float>("output = asin(1)"), wdAngle::Degree(90.0f).GetRadian(), wdMath::DefaultEpsilon<float>());
    WD_TEST_FLOAT(TestConstant<float>("output = asin(sin(PI / 4))"), wdAngle::Degree(45.0f).GetRadian(), wdMath::LargeEpsilon<float>());

    // ACos
    WD_TEST_FLOAT(TestInstruction("output = acos(a)", 1.0f), 0.0f, wdMath::DefaultEpsilon<float>());
    WD_TEST_FLOAT(TestInstruction("output = acos(a)", wdMath::Cos(wdAngle::Degree(45.0f))), wdAngle::Degree(45.0f).GetRadian(), wdMath::LargeEpsilon<float>());
    WD_TEST_FLOAT(TestConstant<float>("output = acos(1)"), 0.0f, wdMath::DefaultEpsilon<float>());
    WD_TEST_FLOAT(TestConstant<float>("output = acos(cos(PI / 4))"), wdAngle::Degree(45.0f).GetRadian(), wdMath::LargeEpsilon<float>());

    // ATan
    WD_TEST_FLOAT(TestInstruction("output = atan(a)", 0.0f), 0.0f, wdMath::DefaultEpsilon<float>());
    WD_TEST_FLOAT(TestInstruction("output = atan(a)", 1.0f), wdAngle::Degree(45.0f).GetRadian(), wdMath::DefaultEpsilon<float>());
    WD_TEST_FLOAT(TestConstant<float>("output = atan(0)"), 0.0f, wdMath::DefaultEpsilon<float>());
    WD_TEST_FLOAT(TestConstant<float>("output = atan(1)"), wdAngle::Degree(45.0f).GetRadian(), wdMath::DefaultEpsilon<float>());

    // RadToDeg
    WD_TEST_FLOAT(TestInstruction("output = radToDeg(a)", wdAngle::Degree(135.0f).GetRadian()), 135.0f, wdMath::DefaultEpsilon<float>());
    WD_TEST_FLOAT(TestInstruction("output = rad_to_deg(a)", wdAngle::Degree(180.0f).GetRadian()), 180.0f, wdMath::DefaultEpsilon<float>());
    WD_TEST_FLOAT(TestConstant<float>("output = radToDeg(PI / 2)"), 90.0f, wdMath::DefaultEpsilon<float>());
    WD_TEST_FLOAT(TestConstant<float>("output = rad_to_deg(PI/4)"), 45.0f, wdMath::DefaultEpsilon<float>());

    // DegToRad
    WD_TEST_FLOAT(TestInstruction("output = degToRad(a)", 135.0f), wdAngle::Degree(135.0f).GetRadian(), wdMath::DefaultEpsilon<float>());
    WD_TEST_FLOAT(TestInstruction("output = deg_to_rad(a)", 180.0f), wdAngle::Degree(180.0f).GetRadian(), wdMath::DefaultEpsilon<float>());
    WD_TEST_FLOAT(TestConstant<float>("output = degToRad(90.0)"), wdAngle ::Degree(90.0f).GetRadian(), wdMath::DefaultEpsilon<float>());
    WD_TEST_FLOAT(TestConstant<float>("output = deg_to_rad(45)"), wdAngle ::Degree(45.0f).GetRadian(), wdMath::DefaultEpsilon<float>());

    // Round
    WD_TEST_FLOAT(TestInstruction("output = round(a)", 12.34f), 12, wdMath::DefaultEpsilon<float>());
    WD_TEST_FLOAT(TestInstruction("output = round(a)", -12.34f), -12, wdMath::DefaultEpsilon<float>());
    WD_TEST_FLOAT(TestInstruction("output = round(a)", 12.54f), 13, wdMath::DefaultEpsilon<float>());
    WD_TEST_FLOAT(TestInstruction("output = round(a)", -12.54f), -13, wdMath::DefaultEpsilon<float>());
    WD_TEST_FLOAT(TestConstant<float>("output = round(4.3)"), 4, wdMath::DefaultEpsilon<float>());
    WD_TEST_FLOAT(TestConstant<float>("output = round(4.51)"), 5, wdMath::DefaultEpsilon<float>());

    // Floor
    WD_TEST_FLOAT(TestInstruction("output = floor(a)", 12.34f), 12, wdMath::DefaultEpsilon<float>());
    WD_TEST_FLOAT(TestInstruction("output = floor(a)", -12.34f), -13, wdMath::DefaultEpsilon<float>());
    WD_TEST_FLOAT(TestInstruction("output = floor(a)", 12.54f), 12, wdMath::DefaultEpsilon<float>());
    WD_TEST_FLOAT(TestInstruction("output = floor(a)", -12.54f), -13, wdMath::DefaultEpsilon<float>());
    WD_TEST_FLOAT(TestConstant<float>("output = floor(4.3)"), 4, wdMath::DefaultEpsilon<float>());
    WD_TEST_FLOAT(TestConstant<float>("output = floor(4.51)"), 4, wdMath::DefaultEpsilon<float>());

    // Ceil
    WD_TEST_FLOAT(TestInstruction("output = ceil(a)", 12.34f), 13, wdMath::DefaultEpsilon<float>());
    WD_TEST_FLOAT(TestInstruction("output = ceil(a)", -12.34f), -12, wdMath::DefaultEpsilon<float>());
    WD_TEST_FLOAT(TestInstruction("output = ceil(a)", 12.54f), 13, wdMath::DefaultEpsilon<float>());
    WD_TEST_FLOAT(TestInstruction("output = ceil(a)", -12.54f), -12, wdMath::DefaultEpsilon<float>());
    WD_TEST_FLOAT(TestConstant<float>("output = ceil(4.3)"), 5, wdMath::DefaultEpsilon<float>());
    WD_TEST_FLOAT(TestConstant<float>("output = ceil(4.51)"), 5, wdMath::DefaultEpsilon<float>());

    // Trunc
    WD_TEST_FLOAT(TestInstruction("output = trunc(a)", 12.34f), 12, wdMath::DefaultEpsilon<float>());
    WD_TEST_FLOAT(TestInstruction("output = trunc(a)", -12.34f), -12, wdMath::DefaultEpsilon<float>());
    WD_TEST_FLOAT(TestInstruction("output = trunc(a)", 12.54f), 12, wdMath::DefaultEpsilon<float>());
    WD_TEST_FLOAT(TestInstruction("output = trunc(a)", -12.54f), -12, wdMath::DefaultEpsilon<float>());
    WD_TEST_FLOAT(TestConstant<float>("output = trunc(4.3)"), 4, wdMath::DefaultEpsilon<float>());
    WD_TEST_FLOAT(TestConstant<float>("output = trunc(4.51)"), 4, wdMath::DefaultEpsilon<float>());

    // Frac
    WD_TEST_FLOAT(TestInstruction("output = frac(a)", 12.34f), 0.34f, wdMath::DefaultEpsilon<float>());
    WD_TEST_FLOAT(TestInstruction("output = frac(a)", -12.34f), -0.34f, wdMath::DefaultEpsilon<float>());
    WD_TEST_FLOAT(TestInstruction("output = frac(a)", 12.54f), 0.54f, wdMath::DefaultEpsilon<float>());
    WD_TEST_FLOAT(TestInstruction("output = frac(a)", -12.54f), -0.54f, wdMath::DefaultEpsilon<float>());
    WD_TEST_FLOAT(TestConstant<float>("output = frac(4.3)"), 0.3f, wdMath::DefaultEpsilon<float>());
    WD_TEST_FLOAT(TestConstant<float>("output = frac(4.51)"), 0.51f, wdMath::DefaultEpsilon<float>());

    // Length
    WD_TEST_VEC3(TestInstruction<wdVec3>("output = length(a)", wdVec3(0, 4, 3)), wdVec3(5), wdMath::DefaultEpsilon<float>());
    WD_TEST_VEC3(TestInstruction<wdVec3>("output = length(a)", wdVec3(-3, 4, 0)), wdVec3(5), wdMath::DefaultEpsilon<float>());

    // Normalize
    WD_TEST_VEC3(TestInstruction<wdVec3>("output = normalize(a)", wdVec3(1, 4, 3)), wdVec3(1, 4, 3).GetNormalized(), wdMath::DefaultEpsilon<float>());
    WD_TEST_VEC3(TestInstruction<wdVec3>("output = normalize(a)", wdVec3(-3, 7, 22)), wdVec3(-3, 7, 22).GetNormalized(), wdMath::DefaultEpsilon<float>());

    // Length and normalize optimization
    {
      wdStringView testCode = "var x = length(a); var na = normalize(a); output = b * x + na";
      wdStringView referenceCode = "var x = length(a); var na = a / x; output = b * x + na";

      wdExpressionByteCode testByteCode;
      WD_TEST_BOOL(CompareCode<wdVec3>(testCode, referenceCode, testByteCode));

      wdVec3 a = wdVec3(0, 4, 3);
      wdVec3 b = wdVec3(1, 0, 0);
      wdVec3 res = b * a.GetLength() + a.GetNormalized();
      WD_TEST_VEC3(Execute(testByteCode, a, b), res, wdMath::DefaultEpsilon<float>());
    }

    // BitwiseNot
    WD_TEST_INT(TestInstruction("output = ~a", 1), ~1);
    WD_TEST_INT(TestInstruction("output = ~a", 8), ~8);
    WD_TEST_INT(TestConstant<int>("output = ~1"), ~1);
    WD_TEST_INT(TestConstant<int>("output = ~17"), ~17);

    // LogicalNot
    WD_TEST_INT(TestInstruction("output = !(a == 1)", 1), 0);
    WD_TEST_INT(TestInstruction("output = !(a == 1)", 8), 1);
    WD_TEST_INT(TestConstant<int>("output = !(1 == 1)"), 0);
    WD_TEST_INT(TestConstant<int>("output = !(8 == 1)"), 1);

    // All
    WD_TEST_VEC3(TestInstruction("var t = (a == b); output = all(t)", wdVec3(1, 2, 3), wdVec3(1, 2, 3)), wdVec3(1), wdMath::DefaultEpsilon<float>());
    WD_TEST_VEC3(TestInstruction("var t = (a == b); output = all(t)", wdVec3(1, 2, 3), wdVec3(1, 2, 4)), wdVec3(0), wdMath::DefaultEpsilon<float>());

    // Any
    WD_TEST_VEC3(TestInstruction("var t = (a == b); output = any(t)", wdVec3(1, 2, 3), wdVec3(4, 5, 3)), wdVec3(1), wdMath::DefaultEpsilon<float>());
    WD_TEST_VEC3(TestInstruction("var t = (a == b); output = any(t)", wdVec3(1, 2, 3), wdVec3(4, 5, 6)), wdVec3(0), wdMath::DefaultEpsilon<float>());
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "Binary instructions")
  {
    // Add
    TestBinaryInstruction<int, int, LeftConstantOptimization>("+", 3, 5, 8);
    TestBinaryInstruction<float, float, LeftConstantOptimization>("+", 3.5f, 5.3f, 8.8f);

    // Subtract
    TestBinaryInstruction<int, int, 0>("-", 9, 5, 4);
    TestBinaryInstruction<float, float, 0>("-", 9.5f, 5.3f, 4.2f);

    // Multiply
    TestBinaryInstruction<int, int, LeftConstantOptimization>("*", 3, 5, 15);
    TestBinaryInstruction<float, float, LeftConstantOptimization>("*", 3.5f, 5.3f, 18.55f);

    // Divide
    TestBinaryInstruction<int, int, 0>("/", 11, 5, 2);
    TestBinaryInstruction<int, int, NoInstructionsCountCheck>("/", -11, 4, -2); // divide by power of 2 optimization
    TestBinaryInstruction<int, int, 0>("/", 11, -4, -2);                        // divide by power of 2 optimization only works for positive divisors
    TestBinaryInstruction<float, float, 0>("/", 12.6f, 3.0f, 4.2f);

    // Modulo
    TestBinaryInstruction<int, int, NoInstructionsCountCheck>("%", 13, 5, 3);
    TestBinaryInstruction<int, int, NoInstructionsCountCheck>("%", -13, 5, -3);
    TestBinaryInstruction<int, int, NoInstructionsCountCheck>("%", 13, 4, 1);
    TestBinaryInstruction<int, int, NoInstructionsCountCheck>("%", -13, 4, -1);
    TestBinaryInstruction<float, float, NoInstructionsCountCheck>("%", 13.5, 5.0, 3.5);
    TestBinaryInstruction<float, float, NoInstructionsCountCheck>("mod(", -13.5, 5.0, -3.5);

    // Log
    TestBinaryInstruction<float, float, NoInstructionsCountCheck>("log(", 2.0f, 1024.0f, 10.0f);
    TestBinaryInstruction<float, float, NoInstructionsCountCheck>("log(", 7.1f, 81.62f, wdMath::Log(7.1f, 81.62f));

    // Pow
    TestBinaryInstruction<int, int, NoInstructionsCountCheck>("pow(", 2, 5, 32);
    TestBinaryInstruction<int, int, NoInstructionsCountCheck>("pow(", 3, 3, 27);

    // Pow is replaced by multiplication for constant exponents up until 16.
    // Test all of them to ensure the multiplication tables are correct.
    for (int i = 0; i <= 16; ++i)
    {
      wdStringBuilder testCode;
      testCode.Format("output = pow(a, {})", i);

      wdExpressionByteCode testByteCode;
      Compile<int>(testCode, testByteCode);
      WD_TEST_INT(Execute(testByteCode, 3), wdMath::Pow(3, i));
    }

    {
      wdStringView testCode = "output = pow(a, 7)";
      wdStringView referenceCode = "var a2 = a * a; var a3 = a2 * a; var a6 = a3 * a3; output = a6 * a";

      wdExpressionByteCode testByteCode;
      WD_TEST_BOOL(CompareCode<int>(testCode, referenceCode, testByteCode));
      WD_TEST_INT(Execute(testByteCode, 3), 2187);
    }

    TestBinaryInstruction<float, float, NoInstructionsCountCheck>("pow(", 2.0, 5.0, 32.0);
    TestBinaryInstruction<float, float, NoInstructionsCountCheck>("pow(", 3.0f, 7.9f, wdMath::Pow(3.0f, 7.9f));

    {
      wdStringView testCode = "output = pow(a, 15.0)";
      wdStringView referenceCode = "var a2 = a * a; var a3 = a2 * a; var a6 = a3 * a3; var a12 = a6 * a6; output = a12 * a3";

      wdExpressionByteCode testByteCode;
      WD_TEST_BOOL(CompareCode<float>(testCode, referenceCode, testByteCode));
      WD_TEST_FLOAT(Execute(testByteCode, 2.1f), wdMath::Pow(2.1f, 15.0f), wdMath::DefaultEpsilon<float>());
    }

    // Min
    TestBinaryInstruction<int, int, LeftConstantOptimization>("min(", 11, 5, 5);
    TestBinaryInstruction<float, float, LeftConstantOptimization>("min(", 12.6f, 3.0f, 3.0f);

    // Max
    TestBinaryInstruction<int, int, LeftConstantOptimization>("max(", 11, 5, 11);
    TestBinaryInstruction<float, float, LeftConstantOptimization>("max(", 12.6f, 3.0f, 12.6f);

    // Dot
    TestBinaryInstruction<wdVec3, wdVec3, NoInstructionsCountCheck>("dot(", wdVec3(1, -2, 3), wdVec3(-5, -6, 7), wdVec3(28));
    TestBinaryInstruction<wdVec3I32, wdVec3I32, NoInstructionsCountCheck>("dot(", wdVec3I32(1, -2, 3), wdVec3I32(-5, -6, 7), wdVec3I32(28));

    // Cross
    TestBinaryInstruction<wdVec3, wdVec3, NoInstructionsCountCheck>("cross(", wdVec3(1, 0, 0), wdVec3(0, 1, 0), wdVec3(0, 0, 1));
    TestBinaryInstruction<wdVec3, wdVec3, NoInstructionsCountCheck>("cross(", wdVec3(0, 1, 0), wdVec3(0, 0, 1), wdVec3(1, 0, 0));
    TestBinaryInstruction<wdVec3, wdVec3, NoInstructionsCountCheck>("cross(", wdVec3(0, 0, 1), wdVec3(1, 0, 0), wdVec3(0, 1, 0));

    // Reflect
    TestBinaryInstruction<wdVec3, wdVec3, NoInstructionsCountCheck>("reflect(", wdVec3(1, 2, -1), wdVec3(0, 0, 1), wdVec3(1, 2, 1));

    // BitshiftLeft
    TestBinaryInstruction<int, int, 0>("<<", 11, 5, 11 << 5);

    // BitshiftRight
    TestBinaryInstruction<int, int, 0>(">>", 0xABCD, 8, 0xAB);

    // BitwiseAnd
    TestBinaryInstruction<int, int, LeftConstantOptimization>("&", 0xFFCD, 0xABFF, 0xABCD);

    // BitwiseXor
    TestBinaryInstruction<int, int, LeftConstantOptimization>("^", 0xFFCD, 0xABFF, 0xFFCD ^ 0xABFF);

    // BitwiseOr
    TestBinaryInstruction<int, int, LeftConstantOptimization>("|", 0x00CD, 0xAB00, 0xABCD);

    // Equal
    TestBinaryInstruction<bool, int, LeftConstantOptimization>("==", 11, 5, 0);
    TestBinaryInstruction<bool, float, LeftConstantOptimization>("==", 12.6f, 3.0f, 0.0f);
    TestBinaryInstruction<bool, bool, LeftConstantOptimization>("==", true, false, false);

    // NotEqual
    TestBinaryInstruction<bool, int, LeftConstantOptimization>("!=", 11, 5, 1);
    TestBinaryInstruction<bool, float, LeftConstantOptimization>("!=", 12.6f, 3.0f, 1.0f);
    TestBinaryInstruction<bool, bool, LeftConstantOptimization>("!=", true, false, true);

    // Less
    TestBinaryInstruction<bool, int, LeftConstantOptimization>("<", 11, 5, 0);
    TestBinaryInstruction<bool, int, LeftConstantOptimization>("<", 11, 11, 0);
    TestBinaryInstruction<bool, float, LeftConstantOptimization>("<", 12.6f, 3.0f, 0.0f);
    TestBinaryInstruction<bool, float, LeftConstantOptimization>("<", 12.6f, 12.6f, 0.0f);

    // LessEqual
    TestBinaryInstruction<bool, int, LeftConstantOptimization>("<=", 11, 5, 0);
    TestBinaryInstruction<bool, int, LeftConstantOptimization>("<=", 11, 11, 1);
    TestBinaryInstruction<bool, float, LeftConstantOptimization>("<=", 12.6f, 3.0f, 0.0f);
    TestBinaryInstruction<bool, float, LeftConstantOptimization>("<=", 12.6f, 12.6f, 1.0f);

    // Greater
    TestBinaryInstruction<bool, int, LeftConstantOptimization>(">", 11, 5, 1);
    TestBinaryInstruction<bool, int, LeftConstantOptimization>(">", 11, 11, 0);
    TestBinaryInstruction<bool, float, LeftConstantOptimization>(">", 12.6f, 3.0f, 1.0f);
    TestBinaryInstruction<bool, float, LeftConstantOptimization>(">", 12.6f, 12.6f, 0.0f);

    // GreaterEqual
    TestBinaryInstruction<bool, int, LeftConstantOptimization>(">=", 11, 5, 1);
    TestBinaryInstruction<bool, int, LeftConstantOptimization>(">=", 11, 11, 1);
    TestBinaryInstruction<bool, float, LeftConstantOptimization>(">=", 12.6f, 3.0f, 1.0f);
    TestBinaryInstruction<bool, float, LeftConstantOptimization>(">=", 12.6f, 12.6f, 1.0f);

    // LogicalAnd
    TestBinaryInstruction<bool, bool, LeftConstantOptimization | NoInstructionsCountCheck>("&&", true, false, false);
    TestBinaryInstruction<bool, bool, LeftConstantOptimization>("&&", true, true, true);

    // LogicalOr
    TestBinaryInstruction<bool, bool, LeftConstantOptimization | NoInstructionsCountCheck>("||", true, false, true);
    TestBinaryInstruction<bool, bool, LeftConstantOptimization>("||", false, false, false);
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "Ternary instructions")
  {
    // Clamp
    WD_TEST_INT(TestInstruction("output = clamp(a, b, c)", -1, 0, 10), 0);
    WD_TEST_INT(TestInstruction("output = clamp(a, b, c)", 2, 0, 10), 2);
    WD_TEST_FLOAT(TestInstruction("output = clamp(a, b, c)", -1.5f, 0.0f, 1.0f), 0.0f, wdMath::DefaultEpsilon<float>());
    WD_TEST_FLOAT(TestInstruction("output = clamp(a, b, c)", 2.5f, 0.0f, 1.0f), 1.0f, wdMath::DefaultEpsilon<float>());

    WD_TEST_INT(TestConstant<int>("output = clamp(-1, 0, 10)"), 0);
    WD_TEST_INT(TestConstant<int>("output = clamp(2, 0, 10)"), 2);
    WD_TEST_FLOAT(TestConstant<float>("output = clamp(-1.5, 0, 2)"), 0.0f, wdMath::DefaultEpsilon<float>());
    WD_TEST_FLOAT(TestConstant<float>("output = clamp(2.5, 0, 2)"), 2.0f, wdMath::DefaultEpsilon<float>());

    // Select
    WD_TEST_INT(TestInstruction("output = (a == 1) ? b : c", 1, 2, 3), 2);
    WD_TEST_INT(TestInstruction("output = a != 1 ? b : c", 1, 2, 3), 3);
    WD_TEST_FLOAT(TestInstruction("output = (a == 1) ? b : c", 1.0f, 2.4f, 3.5f), 2.4f, wdMath::DefaultEpsilon<float>());
    WD_TEST_FLOAT(TestInstruction("output = a != 1 ? b : c", 1.0f, 2.4f, 3.5f), 3.5f, wdMath::DefaultEpsilon<float>());
    WD_TEST_INT(TestInstruction("output = (a == 1) ? (b > 2) : (c > 2)", 1, 2, 3), 0);
    WD_TEST_INT(TestInstruction("output = a != 1 ? b > 2 : c > 2", 1, 2, 3), 1);

    WD_TEST_INT(TestConstant<int>("output = (1 == 1) ? 2 : 3"), 2);
    WD_TEST_INT(TestConstant<int>("output = 1 != 1 ? 2 : 3"), 3);
    WD_TEST_FLOAT(TestConstant<float>("output = (1.0 == 1.0) ? 2.4 : 3.5"), 2.4f, wdMath::DefaultEpsilon<float>());
    WD_TEST_FLOAT(TestConstant<float>("output = 1.0 != 1.0 ? 2.4 : 3.5"), 3.5f, wdMath::DefaultEpsilon<float>());
    WD_TEST_INT(TestConstant<int>("output = (1 == 1) ? false : true"), 0);
    WD_TEST_INT(TestConstant<int>("output = 1 != 1 ? false : true"), 1);

    // Lerp
    WD_TEST_FLOAT(TestInstruction("output = lerp(a, b, c)", 1.0f, 5.0f, 0.75f), 4.0f, wdMath::DefaultEpsilon<float>());
    WD_TEST_FLOAT(TestInstruction("output = lerp(a, b, c)", -1.0f, -11.0f, 0.1f), -2.0f, wdMath::DefaultEpsilon<float>());
    WD_TEST_FLOAT(TestConstant<float>("output = lerp(1, 5, 0.75)"), 4.0f, wdMath::DefaultEpsilon<float>());
    WD_TEST_FLOAT(TestConstant<float>("output = lerp(-1, -11, 0.1)"), -2.0f, wdMath::DefaultEpsilon<float>());
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "Local variables")
  {
    wdExpressionByteCode referenceByteCode;
    {
      wdStringView code = "output = (a + b) * 2";
      Compile<float>(code, referenceByteCode);
    }

    wdExpressionByteCode testByteCode;

    wdStringView code = "var e = a + b; output = e * 2";
    Compile<float>(code, testByteCode);
    WD_TEST_BOOL(CompareByteCode(testByteCode, referenceByteCode));

    code = "var e = a + b; e = e * 2; output = e";
    Compile<float>(code, testByteCode);
    WD_TEST_BOOL(CompareByteCode(testByteCode, referenceByteCode));

    code = "var e = a + b; e *= 2; output = e";
    Compile<float>(code, testByteCode);
    WD_TEST_BOOL(CompareByteCode(testByteCode, referenceByteCode));

    code = "var e = a + b; var f = e; e = 2; output = f * e";
    Compile<float>(code, testByteCode);
    WD_TEST_BOOL(CompareByteCode(testByteCode, referenceByteCode));

    WD_TEST_FLOAT(Execute(testByteCode, 2.0f, 3.0f), 10.0f, wdMath::DefaultEpsilon<float>());
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "Assignment")
  {
    {
      wdStringView testCode = "output = 40; output += 2";
      wdStringView referenceCode = "output = 42";

      wdExpressionByteCode testByteCode;
      WD_TEST_BOOL(CompareCode<float>(testCode, referenceCode, testByteCode));

      WD_TEST_FLOAT(Execute<float>(testByteCode), 42.0f, wdMath::DefaultEpsilon<float>());
    }
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "Integer arithmetic")
  {
    wdExpressionByteCode testByteCode;

    wdStringView code = "output = ((a & 0xFF) << 8) | (b & 0xFFFF >> 8)";
    Compile<int>(code, testByteCode);

    const int a = 0xABABABAB;
    const int b = 0xCDCDCDCD;
    WD_TEST_INT(Execute(testByteCode, a, b), 0xABCD);
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "Constant folding")
  {
    wdStringView testCode = "var x = abs(-7) + saturate(2) + 2\n"
                            "var v = (sqrt(25) - 4) * 5\n"
                            "var m = min(300, 1000) / max(1, 3);"
                            "var r = m - x * 5 - v - clamp(13, 1, 3);\n"
                            "output = r";

    wdStringView referenceCode = "output = 42";

    {
      wdExpressionByteCode testByteCode;
      WD_TEST_BOOL(CompareCode<float>(testCode, referenceCode, testByteCode));

      WD_TEST_FLOAT(Execute<float>(testByteCode), 42.0f, wdMath::DefaultEpsilon<float>());
    }

    {
      wdExpressionByteCode testByteCode;
      WD_TEST_BOOL(CompareCode<int>(testCode, referenceCode, testByteCode));

      WD_TEST_INT(Execute<int>(testByteCode), 42);
    }

    testCode = "";
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "Constant instructions")
  {
    // There are special instructions in the vm which take the constant as the first operand in place and
    // don't require an extra mov for the constant.
    // This test checks whether the compiler transforms operations with constants as second operands to the preferred form.

    wdStringView testCode = "output = (2 + a) + (-1 + b) + (2 * c) + (d / 5) + min(1, c) + max(2, d)";

    {
      wdStringView referenceCode = "output = (a + 2) + (b + -1) + (c * 2) + (d * 0.2) + min(c, 1) + max(d, 2)";

      wdExpressionByteCode testByteCode;
      WD_TEST_BOOL(CompareCode<float>(testCode, referenceCode, testByteCode));
      WD_TEST_INT(testByteCode.GetNumInstructions(), 16);
      WD_TEST_INT(testByteCode.GetNumTempRegisters(), 4);
      WD_TEST_FLOAT(Execute(testByteCode, 1.0f, 2.0f, 3.0f, 40.f), 59.0f, wdMath::DefaultEpsilon<float>());
    }

    {
      wdStringView referenceCode = "output = (a + 2) + (b + -1) + (c * 2) + (d / 5) + min(c, 1) + max(d, 2)";

      wdExpressionByteCode testByteCode;
      WD_TEST_BOOL(CompareCode<int>(testCode, referenceCode, testByteCode));
      WD_TEST_INT(testByteCode.GetNumInstructions(), 16);
      WD_TEST_INT(testByteCode.GetNumTempRegisters(), 4);
      WD_TEST_INT(Execute(testByteCode, 1, 2, 3, 40), 59);
    }
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "Integer and float conversions")
  {
    wdStringView testCode = "var x = 7; var y = 0.6\n"
                            "var e = a * x * b * y\n"
                            "int i = c * 2; i *= i; e += i\n"
                            "output = e";

    wdStringView referenceCode = "int i = (int(c) * 2); output = int((float(a * 7 * b) * 0.6) + float(i * i))";

    wdExpressionByteCode testByteCode;
    WD_TEST_BOOL(CompareCode<int>(testCode, referenceCode, testByteCode));
    WD_TEST_INT(Execute(testByteCode, 1, 2, 3), 44);
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "Bool conversions")
  {
    wdStringView testCode = "var x = true\n"
                            "bool y = a\n"
                            "output = x == y";

    {
      wdStringView referenceCode = "bool r = true == (a != 0); output = r ? 1 : 0";

      wdExpressionByteCode testByteCode;
      WD_TEST_BOOL(CompareCode<int>(testCode, referenceCode, testByteCode));
      WD_TEST_INT(Execute(testByteCode, 14), 1);
    }

    {
      wdStringView referenceCode = "bool r = true == (a != 0); output = r ? 1.0 : 0.0";

      wdExpressionByteCode testByteCode;
      WD_TEST_BOOL(CompareCode<float>(testCode, referenceCode, testByteCode));
      WD_TEST_FLOAT(Execute(testByteCode, 15.0f), 1.0f, wdMath::DefaultEpsilon<float>());
    }
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "Load Inputs/Store Outputs")
  {
    TestInputOutput<float>();
    TestInputOutput<wdFloat16>();

    TestInputOutput<int>();
    TestInputOutput<wdInt16>();
    TestInputOutput<wdInt8>();
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "Function overloads")
  {
    s_pParser->RegisterFunction(s_TestFunc1.m_Desc);
    s_pParser->RegisterFunction(s_TestFunc2.m_Desc);

    s_pVM->RegisterFunction(s_TestFunc1);
    s_pVM->RegisterFunction(s_TestFunc2);

    {
      // take TestFunc1 overload for all ints
      wdStringView testCode = "output = TestFunc(1, 2, 3)";
      wdExpressionByteCode testByteCode;
      Compile<int>(testCode, testByteCode);
      WD_TEST_INT(Execute<int>(testByteCode), 2);
    }

    {
      // take TestFunc1 overload for float, int
      wdStringView testCode = "output = TestFunc(1.0, 2, 3)";
      wdExpressionByteCode testByteCode;
      Compile<int>(testCode, testByteCode);
      WD_TEST_INT(Execute<int>(testByteCode), 2);
    }

    {
      // take TestFunc2 overload for int, float
      wdStringView testCode = "output = TestFunc(1, 2.0, 3)";
      wdExpressionByteCode testByteCode;
      Compile<int>(testCode, testByteCode);
      WD_TEST_INT(Execute<int>(testByteCode), 7);
    }

    {
      // take TestFunc2 overload for all float
      wdStringView testCode = "output = TestFunc(1.0, 2.0, 3)";
      wdExpressionByteCode testByteCode;
      Compile<int>(testCode, testByteCode);
      WD_TEST_INT(Execute<int>(testByteCode), 7);
    }

    {
      // take TestFunc1 overload when only two params are given
      wdStringView testCode = "output = TestFunc(1.0, 2.0)";
      wdExpressionByteCode testByteCode;
      Compile<int>(testCode, testByteCode);
      WD_TEST_INT(Execute<int>(testByteCode), 2);
    }

    s_pParser->UnregisterFunction(s_TestFunc1.m_Desc);
    s_pParser->UnregisterFunction(s_TestFunc2.m_Desc);

    s_pVM->UnregisterFunction(s_TestFunc1);
    s_pVM->UnregisterFunction(s_TestFunc2);
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "Common subexpression elimination")
  {
    wdStringView testCode = "var x1 = a * max(b, c)\n"
                            "var x2 = max(c, b) * a\n"
                            "var y1 = a * pow(2, 3)\n"
                            "var y2 = 8 * a\n"
                            "output = x1 + x2 + y1 + y2";

    wdStringView referenceCode = "var x = a * max(b, c); var y = a * 8; output = x + x + y + y";

    wdExpressionByteCode testByteCode;
    WD_TEST_BOOL(CompareCode<int>(testCode, referenceCode, testByteCode));
    WD_TEST_INT(Execute(testByteCode, 2, 4, 8), 64);
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "Vector constructors")
  {
    {
      wdStringView testCode = "var x = vec3(1, 2, 3)\n"
                              "var y = vec4(x, 4)\n"
                              "vec3 z = vec2(1, 2)\n"
                              "var w = vec4()\n"
                              "output = vec4(x) + y + vec4(z) + w";

      wdExpressionByteCode testByteCode;
      Compile<wdVec3>(testCode, testByteCode);
      WD_TEST_VEC3(Execute<wdVec3>(testByteCode), wdVec3(3, 6, 6), wdMath::DefaultEpsilon<float>());
    }

    {
      wdStringView testCode = "var x = vec4(a.xy, (vec2(6, 8) - vec2(3, 4)).xy)\n"
                              "var y = vec4(1, vec2(2, 3), 4)\n"
                              "var z = vec4(1, vec3(2, 3, 4))\n"
                              "var w = vec4(1, 2, a.zw)\n"
                              "var one = vec4(1)\n"
                              "output = vec4(x) + y + vec4(z) + w + one";

      wdExpressionByteCode testByteCode;
      Compile<wdVec3>(testCode, testByteCode);
      WD_TEST_VEC3(Execute(testByteCode, wdVec3(1, 2, 3)), wdVec3(5, 9, 13), wdMath::DefaultEpsilon<float>());
    }

    {
      wdStringView testCode = "var x = vec4(1, 2, 3, 4)\n"
                              "var y = x.z\n"
                              "x.yz = 7\n"
                              "x.xz = vec2(2, 7)\n"
                              "output = x * y";

      wdExpressionByteCode testByteCode;
      Compile<wdVec3>(testCode, testByteCode);
      WD_TEST_VEC3(Execute<wdVec3>(testByteCode), wdVec3(6, 21, 21), wdMath::DefaultEpsilon<float>());
    }

    {
      wdStringView testCode = "var x = 1\n"
                              "x.z = 7.5\n"
                              "output = x";

      wdExpressionByteCode testByteCode;
      Compile<wdVec3>(testCode, testByteCode);
      WD_TEST_VEC3(Execute<wdVec3>(testByteCode), wdVec3(1, 0, 7), wdMath::DefaultEpsilon<float>());
    }
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "Vector instructions")
  {
    // The VM does only support scalar data types.
    // This test checks whether the compiler transforms everything correctly to scalar operation.

    wdStringView testCode = "output = a * vec3(1, 2, 3) + sqrt(b)";

    wdStringView referenceCode = "output.x = a.x + sqrt(b.x)\n"
                                 "output.y = a.y * 2 + sqrt(b.y)\n"
                                 "output.z = a.z * 3 + sqrt(b.z)";

    wdExpressionByteCode testByteCode;
    WD_TEST_BOOL(CompareCode<wdVec3>(testCode, referenceCode, testByteCode));
    WD_TEST_VEC3(Execute(testByteCode, wdVec3(1, 3, 5), wdVec3(4, 9, 16)), wdVec3(3, 9, 19), wdMath::DefaultEpsilon<float>());
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "Vector swizzle")
  {
    wdStringView testCode = "var n = vec4(1, 2, 3, 4)\n"
                            "var m = vec4(5, 6, 7, 8)\n"
                            "var p = n.xxyy + m.zzww * m.abgr + n.w\n"
                            "output = p";

    // vec3(1, 1, 2) + vec3(7, 7, 8) * vec3(8, 7, 6) + 4
    // output.x = 1 + 7 * 8 + 4 = 61
    // output.y = 1 + 7 * 7 + 4 = 54
    // output.z = 2 + 8 * 6 + 4 = 54

    wdExpressionByteCode testByteCode;
    Compile<wdVec3>(testCode, testByteCode);
    WD_TEST_VEC3(Execute<wdVec3>(testByteCode), wdVec3(61, 54, 54), wdMath::DefaultEpsilon<float>());
  }
}
