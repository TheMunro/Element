using NUnit.Framework;

namespace Laboratory.Tests.L2.Semantics
{
    internal class _6_NestedConstructs : SemanticsFixture
    {
        public _6_NestedConstructs() : base("_6_NestedConstructs") { }

        [Test]
        public void AddUsingLocal() => AssertApproxEqual(CompilationInput, "12", "addUsingLocal(6, 6)");

        [Test]
        public void AddUsingLocalWithCaptures() => AssertApproxEqual(CompilationInput, "12", "addUsingLocalCapture(6, 6)");
        
        [Test]
        public void AddUsingLocalWithCapturesAsFirstClassFunction() => AssertApproxEqual(CompilationInput, "12", "addUsingLocalCaptureAsFirstClassFunction(6, 6)");

        [Test]
        public void AddUsingLocalWithCapturesCalledMultipleTimes() => AssertApproxEqual(CompilationInput, "18", "addBTwice(6, 6)");

        [Test]
        public void AddUsingLocalWithShadowing() => AssertApproxEqual(CompilationInput, "12", "addUsingLocalWithShadowing(6, 6)");

        [Test]
        public void LocalStructInstance() => AssertTypeof(CompilationInput, "returnLocalStructInstance(5)", "Vector2");

        [Test]
        public void LocalStructInstanceFunction() => AssertApproxEqual(CompilationInput, "15", "returnLocalStructInstance(5).add(10).x");

        [Test]
        public void ClosureWithinAnonymousFunction() => AssertApproxEqual(CompilationInput, "13", "addUsingLambdaWithCapture(5, 8)");
        
        [Test]
        public void DeepClosureWithinAnonymousFunction() => AssertApproxEqual(CompilationInput, "19", "deepNestedLambdaWithCapture(5, 8, 6)");
        
        [Test]
        public void AddUsingDeepNestedCapture() => AssertApproxEqual(CompilationInput, "13", "addUsingDeepNestedCapture(5, 8)");
        
        [Test]
        public void AddUsingDeepNestedCaptureWithLambda() => AssertApproxEqual(CompilationInput, "13", "addUsingDeepNestedCaptureWithLambda(5, 8)");
        
        [Test]
        public void NestedNamespacesInStructBody() => AssertTypeof(CompilationInput, "rootStruct.nestedNamespace", "Namespace");
    }
}