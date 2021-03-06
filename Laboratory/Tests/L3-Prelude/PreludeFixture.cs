using Element;

namespace Laboratory.Tests
{
    internal abstract class PreludeFixture : HostFixture
    {
        protected CompilationInput ValidatedCompilationInput { get; } = new CompilationInput(FailOnError);
        protected CompilationInput NonValidatedCompilationInput { get; } = new CompilationInput(FailOnError) {SkipValidation = true};
    }
}