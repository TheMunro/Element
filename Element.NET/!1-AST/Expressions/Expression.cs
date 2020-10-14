using Lexico;

namespace Element.AST
{
    [WhitespaceSurrounded, MultiLine]
    // ReSharper disable once ClassNeverInstantiated.Global
    public abstract class Expression : AstNode
    {
        public Result<IValue> ResolveExpression(IScope parentScope, Context context)
        {
            context.TraceStack.Push(this.MakeTraceSite($"{GetType().Name} '{ToString()}'"));
            var result = ExpressionImpl(parentScope, context);
            context.TraceStack.Pop();
            return result;
        }

        protected abstract Result<IValue> ExpressionImpl(IScope parentScope, Context context);
    }
}