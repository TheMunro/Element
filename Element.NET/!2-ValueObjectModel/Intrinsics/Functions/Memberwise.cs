using System.Collections.Generic;
using System.Linq;

namespace Element.AST
{
    public sealed class Memberwise : IntrinsicValue, IIntrinsicFunctionImplementation
    {
        private Memberwise()
        {
            Identifier =  new Identifier("memberwise");
        }
        
        public static Memberwise Instance { get; } = new Memberwise();
        public override Identifier Identifier { get; }
        public bool IsVariadic => true;
        public override Result<IValue> Call(IReadOnlyList<IValue> arguments, Context context)
        {
            var func = arguments[0];
            var structs = arguments.Skip(1).Select(arg => arg as StructInstance).ToArray();
            if (structs.Any(s => s == null)) return context.Trace(EleMessageCode.ConstraintNotSatisfied, "Memberwise can only be applied to struct instances");
            if (structs.Length < 1) return context.Trace(EleMessageCode.ConstraintNotSatisfied, "Memberwise requires at least 1 struct to apply the given function");
            
            var structType = structs[0]!.DeclaringStruct;
            if (structs.Any(s => s!.DeclaringStruct != structType)) return context.Trace(EleMessageCode.ConstraintNotSatisfied, "Memberwise can only be applied to struct instances of the same type");

            Result<IValue> ApplyFuncToMemberPair(ResolvedPort p) =>
                structs.Select(inst => inst!.Index(p.Identifier!.Value, context))
                       .BindEnumerable(fields => func.Call(fields.ToArray(), context));

            return structType.Fields
                             .Select(ApplyFuncToMemberPair)
                             .BindEnumerable(resultFields => StructInstance.Create(structType, resultFields.ToArray(), context)
                                                                           .Cast<IValue>(context));
        }
    }
}