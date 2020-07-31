using System;
using System.Collections.Generic;
using System.Linq;

namespace Element.AST
{
    public abstract class Struct : Value, IScope
    {
        private readonly ResolvedBlock? _associatedBlock;
        private readonly IScope _parent;

        protected Struct(Identifier identifier, IReadOnlyList<ResolvedPort> fields, ResolvedBlock? associatedBlock, IScope parent)
        {
            Identifier = identifier;
            _associatedBlock = associatedBlock;
            _parent = parent;
            Fields = fields;
        }

        public override Identifier? Identifier { get; }
        public override IReadOnlyList<ResolvedPort> InputPorts => Fields;
        public override IValue ReturnConstraint => this;
        public IReadOnlyList<ResolvedPort> Fields { get; }

        public abstract override Result<IValue> Call(IReadOnlyList<IValue> arguments, Context context);
        public abstract override Result<bool> MatchesConstraint(IValue value, Context context);
        public override Result<IValue> Index(Identifier id, Context context) => _associatedBlock?.Index(id, context)
                                                                                 ?? (Result<IValue>)context.Trace(MessageCode.InvalidExpression, $"'{this}' has no associated scope - it cannot be indexed");
        public Result<IValue> Lookup(Identifier id, Context context) => (_associatedBlock ?? _parent).Lookup(id, context);
        public override IReadOnlyList<Identifier> Members => _associatedBlock?.Members ?? Array.Empty<Identifier>();
        public abstract override Result<IValue> DefaultValue(Context context);
        public Result<bool> IsInstanceOfStruct(IValue value, Context context) => value.FullyResolveValue(context).Map(v => v is StructInstance instance && instance.DeclaringStruct == this);
        public Result<IValue> ResolveInstanceFunction(IValue instance, Identifier id, Context context) =>
            Index(id, context)
                .Bind(v => v switch
                {
                    {} when !v.IsFunction() => context.Trace(MessageCode.CannotBeUsedAsInstanceFunction, $"'{v}' found by indexing '{instance}' is not a function"),
                    {} when v.IsNullaryFunction() => context.Trace(MessageCode.CannotBeUsedAsInstanceFunction, $"Constant '{v}' cannot be accessed by indexing an instance"),
                    // ReSharper disable once PossibleUnintendedReferenceComparison
                    {} when v.InputPorts[0].ResolvedConstraint == this => v.PartiallyApply(new[] {instance}, context),
                    {} => context.Trace(MessageCode.CannotBeUsedAsInstanceFunction, $"Found function '{v}' <{v.InputPorts[0]}> must be of type <{Identifier}> to be used as an instance function"),
                    null => throw new InternalCompilerException($"Indexing '{instance}' with '{id}' returned null IValue - this should not occur from user input")
                });
    }
    
    public class IntrinsicStruct : Struct, IIntrinsicValue
    {
        IIntrinsicImplementation IIntrinsicValue.Implementation => _implementation;
        private readonly IIntrinsicStructImplementation _implementation;

        public IntrinsicStruct(IIntrinsicStructImplementation implementation, IReadOnlyList<ResolvedPort> fields, ResolvedBlock? associatedBlock, IScope parent)
            : base(implementation.Identifier, fields, associatedBlock, parent) =>
            _implementation = implementation;

        public override Result<IValue> Call(IReadOnlyList<IValue> arguments, Context context) => _implementation.Construct(this, arguments, context);
        public override Result<bool> MatchesConstraint(IValue value, Context context) => _implementation.MatchesConstraint(this, value, context);
        public override Result<IValue> DefaultValue(Context context) => _implementation.DefaultValue(context);
    }

    public class CustomStruct : Struct
    {
        public CustomStruct(Identifier identifier, IReadOnlyList<ResolvedPort> fields, ResolvedBlock? associatedBlock, IScope parent)
            : base(identifier, fields, associatedBlock, parent) { }

        public override Result<IValue> Call(IReadOnlyList<IValue> arguments, Context context) => StructInstance.Create(this, arguments, context).Cast<IValue>(context);
        public override Result<bool> MatchesConstraint(IValue value, Context context) => IsInstanceOfStruct(value, context);
        public override Result<IValue> DefaultValue(Context context) =>
            Fields.Select(field => field.DefaultValue(context))
                  .BindEnumerable(defaults => StructInstance.Create(this, defaults.ToArray(), context)
                                                            .Cast<IValue>(context));
    }
    
    public sealed class StructInstance : Value
    {
        public Struct DeclaringStruct { get; }

        private readonly ResolvedBlock _resolvedBlock;

        public static Result<StructInstance> Create(Struct declaringStruct, IReadOnlyList<IValue> fieldValues, Context context) =>
            declaringStruct.VerifyArgumentsAndApplyFunction(fieldValues, () => new StructInstance(declaringStruct, fieldValues), context)
                           .Cast<StructInstance>(context);
        
        private StructInstance(Struct declaringStruct, IEnumerable<IValue> fieldValues)
        {
            DeclaringStruct = declaringStruct;
            _resolvedBlock = new ResolvedBlock(null, declaringStruct.Fields.Zip(fieldValues, (port, value) => (port.Identifier!.Value, value)).ToArray(), null);
        }

        public override Identifier? Identifier => null;
        public override string TypeOf => DeclaringStruct.Identifier?.String ?? "<unknown>";
        public override Result<IValue> Index(Identifier id, Context context) =>
            _resolvedBlock.Index(id, context)
                          .Else(() => DeclaringStruct.ResolveInstanceFunction(this, id, context));

        public override IReadOnlyList<Identifier> Members => _resolvedBlock.Members;

        public override void Serialize(ResultBuilder<List<Element.Expression>> resultBuilder, Context context)
        {
            if (DeclaringStruct.IsIntrinsic<ListStruct>())
            {
                // TODO: List serialization
                resultBuilder.Append(MessageCode.SerializationError, "List serialization not supported yet");
                return;
            }
            
            _resolvedBlock.Serialize(resultBuilder, context);
        }

        public override Result<IValue> Deserialize(Func<Element.Expression> nextValue, Context context) =>
            _resolvedBlock.DeserializeMembers(nextValue, context)
                          .Map(deserializedFields => (IValue) new StructInstance(DeclaringStruct, deserializedFields));
    }
}