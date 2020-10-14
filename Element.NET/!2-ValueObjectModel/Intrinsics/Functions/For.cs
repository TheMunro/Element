using System.Collections.Generic;
using System.Linq;

namespace Element.AST
{
    public sealed class For : IntrinsicValue, IIntrinsicFunctionImplementation
    {
        private For()
        {
            Identifier = new Identifier("for");
        }
        
        public static For Instance { get; } = new For();
        public override Identifier Identifier { get; }
        public bool IsVariadic => false;
        public override Result<IValue> Call(IReadOnlyList<IValue> arguments, Context context)
        {
            var initial = arguments[0];
            var condition = arguments[1];
            var body = arguments[2];

            Result<Element.Instruction> Condition(IReadOnlyCollection<Element.Instruction> state) =>
                initial.Deserialize(state, context)
                       .Bind(value => condition.Call(new[] {value}, context))
                       .Cast<Element.Instruction>(context);
            
            Result<IEnumerable<Element.Instruction>> Body(IReadOnlyCollection<Element.Instruction> state) =>
                initial.Deserialize(state, context)
                       .Bind(value => body.Call(new[] {value}, context))
                       .Bind(result => result.Serialize(context))
                       .Cast<IEnumerable<Element.Instruction>>(context);

            return initial.Serialize(context)
                          .Bind(initialSerialized => Loop.CreateAndOptimize(initialSerialized, Condition, Body, context))
                          .Bind(expressionGroup => initial.Deserialize(Enumerable.Range(0, expressionGroup.Size).Select(i => new InstructionGroupElement(expressionGroup, i)), context))
                          .Cast<IValue>(context);
        }
    }
}