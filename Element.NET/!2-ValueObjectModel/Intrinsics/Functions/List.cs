using System;
using System.Collections.Generic;
using System.Linq;

namespace Element.AST
{
	public class List : IntrinsicValue, IIntrinsicFunctionImplementation
	{
		private List()
		{
			_identifier = new Identifier("list");
		}
		
		public static List Instance { get; } = new List();
		protected override Identifier _identifier { get; }
		public bool IsVariadic => true;

		public override Result<IValue> Call(IReadOnlyList<IValue> arguments, Context context) =>
			context.RootScope.Lookup(ListStruct.Instance.Identifier, context)
			       .Cast<IntrinsicStruct>(context)
			       .Accumulate(() => context.RootScope.Lookup(NumStruct.Instance.Identifier, context))
			       .Bind(t =>
			       {
				       var (listStruct, numStruct) = t;
				       return listStruct.Call(new IValue[]
				       {
					       new ListIndexer(arguments, new[]{new ResolvedPort(numStruct)}, AnyConstraint.Instance),
					       new Constant(arguments.Count)
				       }, context);
			       });
		
		private class ListIndexer : Function
        {
            private readonly IReadOnlyList<IValue> _elements;

            public ListIndexer(IReadOnlyList<IValue> elements, IReadOnlyList<ResolvedPort> inputPorts, IValue output)
            {
	            _elements = elements;
	            InputPorts = inputPorts;
	            ReturnConstraint = output;
            }

            public override Identifier? Identifier => ListStruct.IndexerId;
            public override IReadOnlyList<ResolvedPort> InputPorts { get; }
            public override IValue ReturnConstraint { get; }

            protected override Result<IValue> ResolveCall(IReadOnlyList<IValue> arguments, Context context) =>
	            arguments[0] is Element.Expression index
		                               ? new Result<IValue>(ListElement.Create(index,
		                                                                       _elements,
		                                                                       _elements[0].IsFunction() ? _elements[0].InputPorts : new[] {ResolvedPort.VariadicPort},
		                                                                       _elements[0].IsFunction() ? _elements[0].ReturnConstraint : AnyConstraint.Instance))
		                               : context.Trace(MessageCode.ConstraintNotSatisfied, "List Index must be a Num");
        }

        private class ListElement : Function
        {
            public static IValue Create(Element.Expression index, IReadOnlyList<IValue> elements, IReadOnlyList<ResolvedPort> inputConstraints, IValue outputConstraint) =>
                index switch
                {
	                Constant constantIndex => elements[(int) constantIndex.Value],
	                {} indexExpr => elements.All(e => e is Element.Expression)
		                                ? (IValue) Mux.CreateAndOptimize(indexExpr, elements.Cast<Element.Expression>())
		                                : new ListElement(index, elements, inputConstraints, outputConstraint),
	                _ => throw new ArgumentNullException(nameof(index))
                };

            private ListElement(Element.Expression index, IReadOnlyList<IValue> elements, IReadOnlyList<ResolvedPort> inputPorts, IValue output)
            {
                _index = index;
                _elements = elements;
                InputPorts = inputPorts;
                ReturnConstraint = output;
            }

            private readonly Element.Expression _index;
            private readonly IReadOnlyList<IValue> _elements;
            public override Identifier? Identifier => null;
            public override string SummaryString => $"List[{_index}]";
            public override IReadOnlyList<ResolvedPort> InputPorts { get; }
            public override IValue ReturnConstraint { get; }

            public override Result<IValue> Index(Identifier id, Context context) =>
	            _elements.Select(e => e.Index(id, context))
	                     .MapEnumerable(elements => Create(_index, elements.ToList(), InputPorts, ReturnConstraint));
            
            protected override Result<IValue> ResolveCall(IReadOnlyList<IValue> arguments, Context context) =>
	            _elements.Select(e => e.Call(arguments.ToArray(), context))
	                     .MapEnumerable(v => Create(_index, v.ToList(), InputPorts, ReturnConstraint));
        }
	}
}