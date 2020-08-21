using System.Linq;

namespace Element.AST
{
    // ReSharper disable once ClassNeverInstantiated.Global
    public class CustomStructDeclaration : StructDeclaration
    {
        protected override string IntrinsicQualifier => string.Empty;

        protected override bool AdditionalValidation(SourceContext sourceContext)
        {
            var success = true;

            if (DeclaredType != null)
            {
                sourceContext.LogError(19, $"Struct '{Identifier}' cannot have declared return type");
                success = false;
            }

            if (!HasDeclaredInputs)
            {
                sourceContext.LogError(13, $"Non intrinsic '{Location}' must have ports");
                success = false;
            }

            return success;
        }

        public override bool MatchesConstraint(IValue value, CompilationContext compilationContext) => value is StructInstance instance && instance.DeclaringStruct == this;
        public override ISerializableValue DefaultValue(CompilationContext context) =>
            CreateInstance(Fields.Select(f => (f.ResolveConstraint(context) as IType ??
                                               context.LogError(14, $"'{f}' is not a type - only types can produce a default value"))
                                             .DefaultValue(context)).ToArray());

        public override IValue Call(IValue[] arguments, CompilationContext compilationContext) => CreateInstance(arguments);
    }
}