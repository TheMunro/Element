using System;
using System.Linq;

namespace Element.AST
{
    public abstract class StructDeclaration : Declaration
    {
        protected override string IntrinsicQualifier => "intrinsic";
        protected override string Qualifier { get; } = "struct";
        protected override Type[] BodyAlternatives { get; } = {typeof(Block), typeof(Terminal)};
        
        protected override void AdditionalValidation(ResultBuilder builder, CompilationContext context)
        {
            if (ReturnConstraint != null)
            {
                builder.Append(MessageCode.StructCannotHaveReturnType, $"Struct '{context.CurrentDeclarationLocation}' cannot have declared return type");
            }
            
            if (PortList?.Ports.List.Any(port => !port.Identifier.HasValue) ?? false)
            {
                builder.Append(MessageCode.PortListCannotContainDiscards, $"Struct '{context.CurrentDeclarationLocation}' contains discards");
            }
        }
    }
    
    // ReSharper disable once UnusedType.Global
    public sealed class IntrinsicStructDeclaration : StructDeclaration
    {
        protected override Result<IValue> ResolveImpl(IScope scope, CompilationContext context) =>
            IntrinsicCache.Get<IntrinsicStructImplementation>(Identifier, context)
                          .Accumulate(() => PortList.ResolveInputConstraints(scope, context, true, true))
                          .Bind(t =>
                          {
                              var (structImpl, inputPorts) = t;
                              
                              Result<IValue> ToIntrinsicStructResult(IScope? associatedScope) => new Result<IValue>(new IntrinsicStruct(structImpl, inputPorts, associatedScope, scope, context.CurrentDeclarationLocation));
                              
                              return Body is Block b
                                         ? b.Resolve(scope, context).Bind(ToIntrinsicStructResult)
                                         : ToIntrinsicStructResult(null);
                          });
    }
    
    public class CustomStructDeclaration : StructDeclaration
    {
        protected override Result<IValue> ResolveImpl(IScope scope, CompilationContext context) =>
            PortList.ResolveInputConstraints(scope, context, true, false)
                    .Bind(inputPorts =>
                    {
                        Result<IValue> ToIntrinsicStructResult(IScope? associatedScope) => new Result<IValue>(new CustomStruct(inputPorts, associatedScope, scope, context.CurrentDeclarationLocation));
                              
                        return Body is Block b
                                   ? b.Resolve(scope, context).Bind(ToIntrinsicStructResult)
                                   : ToIntrinsicStructResult(null);
                    });

        protected override void AdditionalValidation(ResultBuilder builder, CompilationContext context)
        {
            base.AdditionalValidation(builder, context);
            if (PortList?.Ports.List.Count !> 0)
            {
                builder.Append(MessageCode.MissingPorts, $"Non intrinsic '{context.CurrentDeclarationLocation}' must have ports");
            }
        }
    }
}