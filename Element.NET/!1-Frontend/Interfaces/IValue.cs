using System;
using System.Collections.Generic;
using System.Linq;

namespace Element.AST
{
    public interface IValue
    {
        IType Type { get; }
    }

    public static class ValueExtensions
    {
        public static IEnumerable<(Identifier Identifier, IValue Value)> WithoutDiscardedArguments(this IValue[] arguments, Port[] ports)
        {
            var result = new List<(Identifier Identifier, IValue Value)>();
            var variadicArgNumber = 0;
            for (var i = 0; i < Math.Max(arguments.Length, ports.Length); i++)
            {
                var arg = i < arguments.Length ? arguments[i] : null;
                var port = i < ports.Length ? ports[i] : null;
                if (port == Port.VariadicPort || variadicArgNumber > 0)
                {
                    result.Add((new Identifier($"varg{variadicArgNumber}"), arg));
                    variadicArgNumber++;
                }
                else if ((port?.Identifier.HasValue ?? false) && arg != null)
                {
                    result.Add((port.Identifier.Value, arg));
                }
            }

            return result;
        }

        public static int? GetSerializedSize(this IValue value, CompilationContext compilationContext) => value switch
        {
            Element.Expression _ => 1,
            IFunctionSignature fn when fn.IsNullary() => fn.ResolveNullaryFunction(compilationContext).GetSerializedSize(compilationContext),
            StructInstance structInstance when structInstance.Type == ListType.Instance => ListType.GetListCount(structInstance, compilationContext) switch
            {
                (ListType.CountType t, int count) when t == ListType.CountType.Constant => count,
                _ => compilationContext.LogError(1, "List instance is not serializable - lists must have a constant count to be serializable").Return((int?)null)
            },
            IEnumerable<IValue> values => values.Select(v => v.GetSerializedSize(compilationContext)).Aggregate((int?)0, (a, b) => a == null || b == null ? null : a + b),
            _ => compilationContext.LogError(1, $"'{value}' is not serializable").Return((int?)null)
        };

        public static Element.Expression[] Serialize(this IValue value, CompilationContext compilationContext)
        {
            var size = value.GetSerializedSize(compilationContext);
            if (!size.HasValue || size == 0)
            {
                return Array.Empty<Element.Expression>();
            }
            
            var serialized = new Element.Expression[size.Value];
            var position = 0;

            bool AsExpression(object serializable, Element.Expression[] output)
            {
                switch(serializable)
                {
                    case Element.Expression expr:
                        output[position++] = expr;
                        return true;
                    case IFunctionSignature fn when fn.IsNullary():
                        return AsExpression(fn.ResolveNullaryFunction(compilationContext), output);
                    case StructInstance structInstance when structInstance.Type == ListType.Instance:
                        return AsExpression(ListType.EvaluateElements(structInstance, compilationContext), output);
                    case IEnumerable<IValue> values:
                        return values.Aggregate(true, (current, element) => current & AsExpression(element, output));
                    default:
                        compilationContext.LogError(1, $"'{serializable}' is not serializable");
                        return false;
                };
            }

            return AsExpression(value, serialized) ? serialized : Array.Empty<Element.Expression>();
        }

        public static bool TrySerialize(this IValue value, out Element.Expression[] serialized, CompilationContext compilationContext)
            => (serialized = value.Serialize(compilationContext)).Length > 0;

        public static bool TrySerialize(this IValue value, out float[] serialized, CompilationContext compilationContext)
        {
            if (!value.TrySerialize(out Element.Expression[] expressions, compilationContext))
            {
                serialized = null;
                return false;
            }

            serialized = new float[expressions.Length];
            var position = 0;
            var success = true;
            foreach (var expr in expressions)
            {
                if (expr is Constant constant) serialized[position++] = constant.Value;
                else
                {
                    compilationContext.LogError(1, $"Non-constant expression '{expr}' cannot be serialized");
                    position++;
                    success = false;
                }
            }

            return success;
        }
        
        public static IValue Deserialize(this IType type, IEnumerable<Element.Expression> expressions, CompilationContext compilationContext) =>
            type switch
            {
                {} t when t == ListType.Instance => ListType.Instance.MakeList(expressions.ToArray(), compilationContext),
                IntrinsicType t  => t.Call(expressions.ToArray(), compilationContext),
                _ => compilationContext.LogError(1, $"'{type}' cannot be deserialized")
            };
        
        public static bool TryDeserialize(this IType type, IEnumerable<Element.Expression> expressions, out IValue value, CompilationContext compilationContext) =>
            (value = type.Deserialize(expressions, compilationContext)) != CompilationErr.Instance;
    }
}