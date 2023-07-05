#include <FoundationTest/FoundationTestPCH.h>

#include <Foundation/CodeUtils/MathExpression.h>

WD_CREATE_SIMPLE_TEST(CodeUtils, MathExpression)
{
  WD_TEST_BLOCK(wdTestBlock::Enabled, "Basics")
  {
    {
      wdMathExpression expr("");
      WD_TEST_BOOL(!expr.IsValid());

      expr.Reset("");
      WD_TEST_BOOL(!expr.IsValid());
    }
    {
      wdMathExpression expr(nullptr);
      WD_TEST_BOOL(!expr.IsValid());

      expr.Reset(nullptr);
      WD_TEST_BOOL(!expr.IsValid());
    }
    {
      wdMathExpression expr("1.5 + 2.5");
      WD_TEST_BOOL(expr.IsValid());
      WD_TEST_DOUBLE(expr.Evaluate(), 4.0, 0.0);
    }
    {
      wdMathExpression expr("1- 2");
      WD_TEST_BOOL(expr.IsValid());
      WD_TEST_DOUBLE(expr.Evaluate(), -1.0, 0.0);
    }
    {
      wdMathExpression expr("1 *2");
      WD_TEST_BOOL(expr.IsValid());
      WD_TEST_DOUBLE(expr.Evaluate(), 2.0, 0.0);
    }
    {
      wdMathExpression expr(" 1.0/2 ");
      WD_TEST_BOOL(expr.IsValid());
      WD_TEST_DOUBLE(expr.Evaluate(), 0.5, 0.0);
    }
    {
      wdMathExpression expr("1 - -1");
      WD_TEST_BOOL(expr.IsValid());
      WD_TEST_DOUBLE(expr.Evaluate(), 2.0, 0.0);
    }
    {
      wdMathExpression expr("abs(-3)");
      WD_TEST_BOOL(expr.IsValid());
      WD_TEST_DOUBLE(expr.Evaluate(), 3.0, 0.0);
    }
    {
      wdMathExpression expr("sqrt(4)");
      WD_TEST_BOOL(expr.IsValid());
      WD_TEST_DOUBLE(expr.Evaluate(), 2.0, 0.0);
    }
    {
      wdMathExpression expr("saturate(4)");
      WD_TEST_BOOL(expr.IsValid());
      WD_TEST_DOUBLE(expr.Evaluate(), 1.0, 0.0);
    }
    {
      wdMathExpression expr("min(3, 4)");
      WD_TEST_BOOL(expr.IsValid());
      WD_TEST_DOUBLE(expr.Evaluate(), 3.0, 0.0);
    }
    {
      wdMathExpression expr("max(3, 4)");
      WD_TEST_BOOL(expr.IsValid());
      WD_TEST_DOUBLE(expr.Evaluate(), 4.0, 0.0);
    }
    {
      wdMathExpression expr("clamp(2, 3, 4)");
      WD_TEST_BOOL(expr.IsValid());
      WD_TEST_DOUBLE(expr.Evaluate(), 3.0, 0.0);
    }
    {
      wdMathExpression expr("clamp(5, 3, 4)");
      WD_TEST_BOOL(expr.IsValid());
      WD_TEST_DOUBLE(expr.Evaluate(), 4.0, 0.0);
    }
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "Operator Priority")
  {
    {
      wdMathExpression expr("1 - 2 * 4");
      WD_TEST_BOOL(expr.IsValid());
      WD_TEST_DOUBLE(expr.Evaluate(), -7.0, 0.0);
    }
    {
      wdMathExpression expr("-1 - 2 * 4");
      WD_TEST_BOOL(expr.IsValid());
      WD_TEST_DOUBLE(expr.Evaluate(), -9.0, 0.0);
    }
    {
      wdMathExpression expr("1 - 2.0 / 4");
      WD_TEST_BOOL(expr.IsValid());
      WD_TEST_DOUBLE(expr.Evaluate(), 0.5, 0.0);
    }
    {
      wdMathExpression expr("abs (-4 + 2)");
      WD_TEST_BOOL(expr.IsValid());
      WD_TEST_DOUBLE(expr.Evaluate(), 2.0, 0.0);
    }
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "Braces")
  {
    {
      wdMathExpression expr("(1 - 2) * 4");
      WD_TEST_BOOL(expr.IsValid());
      WD_TEST_DOUBLE(expr.Evaluate(), -4.0, 0.0);
    }
    {
      wdMathExpression expr("(((((0)))))");
      WD_TEST_BOOL(expr.IsValid());
      WD_TEST_DOUBLE(expr.Evaluate(), 0.0, 0.0);
    }
    {
      wdMathExpression expr("(1 + 2) * (3 - 2)");
      WD_TEST_BOOL(expr.IsValid());
      WD_TEST_DOUBLE(expr.Evaluate(), 3.0, 0.0);
    }
  }

  WD_TEST_BLOCK(wdTestBlock::Enabled, "Variables")
  {
    wdHybridArray<wdMathExpression::Input, 4> inputs;
    inputs.SetCount(4);

    {
      wdMathExpression expr("_var1 + v2Ar");
      WD_TEST_BOOL(expr.IsValid());

      inputs[0] = {wdMakeHashedString("_var1"), 1.0};
      inputs[1] = {wdMakeHashedString("v2Ar"), 2.0};

      double result = expr.Evaluate(inputs);
      WD_TEST_DOUBLE(result, 3.0, 0.0);

      inputs[0].m_fValue = 2.0;
      inputs[1].m_fValue = 0.5;

      result = expr.Evaluate(inputs);
      WD_TEST_DOUBLE(result, 2.5, 0.0);
    }

    // Make sure we got the spaces right and don't count it as part of the variable.
    {
      wdMathExpression expr("  a +  b /c*d");
      WD_TEST_BOOL(expr.IsValid());

      inputs[0] = {wdMakeHashedString("a"), 1.0};
      inputs[1] = {wdMakeHashedString("b"), 4.0};
      inputs[2] = {wdMakeHashedString("c"), 2.0};
      inputs[3] = {wdMakeHashedString("d"), 3.0};

      double result = expr.Evaluate(inputs);
      WD_TEST_DOUBLE(result, 7.0, 0.0);
    }
  }


  WD_TEST_BLOCK(wdTestBlock::Enabled, "Invalid Expressions")
  {
    wdMuteLog logErrorSink;
    wdLogSystemScope ls(&logErrorSink);

    {
      wdMathExpression expr("1+");
      WD_TEST_BOOL(!expr.IsValid());
    }
    {
      wdMathExpression expr("1+/1");
      WD_TEST_BOOL(!expr.IsValid());
    }
    {
      wdMathExpression expr("(((((0))))");
      WD_TEST_BOOL(!expr.IsValid());
    }
    {
      wdMathExpression expr("_va£r + asdf");
      WD_TEST_BOOL(!expr.IsValid());
    }
    {
      wdMathExpression expr("sqrt(2, 4)");
      WD_TEST_BOOL(!expr.IsValid());
    }
  }
}
