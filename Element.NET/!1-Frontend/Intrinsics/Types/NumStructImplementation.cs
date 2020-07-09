using System.Collections.Generic;

namespace Element.AST
{
    /// <summary>
    /// The type for a single number.
    /// </summary>
    public sealed class NumStructImplementation : IntrinsicStructImplementation
    {
        private NumStructImplementation() { }
        public static NumStructImplementation Instance { get; } = new NumStructImplementation();
        public override Result<IValue> Construct(Struct @struct, IReadOnlyList<IValue> arguments, CompilationContext context) => new Result<IValue>(arguments[0]);
        public override Result<IValue> DefaultValue(CompilationContext _) => Constant.Zero;
        public override Result<bool> MatchesConstraint(Struct @struct, IValue value, CompilationContext context) => value.FullyResolveValue(context).Map(v => v is Element.Expression expr && expr.StructImplementation == Instance);
        public override Identifier Identifier { get; } = new Identifier("Num");
    }
}