namespace Element.CLR
{
    using System;
    
    /// <summary>
    /// Something that can be compiled to a linq expression.
    /// </summary>
    public interface ICLRExpression
    {
        System.Linq.Expressions.Expression Compile(Func<Instruction, System.Linq.Expressions.Expression> compileOther);
    }
}