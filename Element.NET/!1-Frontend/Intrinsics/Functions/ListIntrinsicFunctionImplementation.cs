using System.Collections.Generic;
using System.Linq;

namespace Element.AST
{
	public class ListIntrinsicFunctionImplementation : IntrinsicFunctionImplementation
	{
		private ListIntrinsicFunctionImplementation()
		{
			Identifier = new Identifier("list");
		}
		
		public static ListIntrinsicFunctionImplementation Instance { get; } = new ListIntrinsicFunctionImplementation();

		public override Identifier Identifier { get; }

		public override Result<IValue> Call(IReadOnlyList<IValue> arguments, CompilationContext context) =>
			context.SourceContext.GlobalScope.Lookup(ListStructImplementation.Instance.Identifier, context)
			       .Cast<IntrinsicStruct>(context)
			       .Accumulate(() => context.SourceContext.GlobalScope.Lookup(AnyConstraintImplementation.Instance.Identifier, context),
			                   () => context.SourceContext.GlobalScope.Lookup(NumStructImplementation.Instance.Identifier, context))
			       .Bind(t =>
			       {
				       var (listStruct, anyConstraint, numStruct) = t;
				       return listStruct.Call(new IValue[]
				       {
					       new ListIndexer(arguments, new[]{new ResolvedPort(numStruct)}, anyConstraint),
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

            public override IReadOnlyList<ResolvedPort> InputPorts { get; }
            public override IValue ReturnConstraint { get; }

            protected override Result<IValue> ResolveFunctionBody(IReadOnlyList<IValue> arguments, CompilationContext context) =>
	            new Result<IValue>(ListElement.Create(arguments[0],
	                               _elements,
	                               _elements[0] is IFunctionSignature f
		                               ? f.InputPorts
		                               : new[] {ResolvedPort.VariadicPort},
	                               _elements[0] is IFunctionSignature fn
		                               ? fn.ReturnConstraint
		                               : new IntrinsicConstraint(AnyConstraintImplementation.Instance)));
        }

        private class ListElement : Function
        {
            public static IValue Create(IValue index, IReadOnlyList<IValue> elements, IReadOnlyList<ResolvedPort> inputConstraints, IValue outputConstraint) =>
                index switch
                {
                    Element.Expression indexExpr when elements.All(e => e is Element.Expression) => new Mux(indexExpr, elements.Cast<Element.Expression>()),
                    Constant constantIndex => elements[(int)constantIndex.Value],
                    _ => new ListElement(index, elements, inputConstraints, outputConstraint)
                };

            private ListElement(IValue index, IReadOnlyList<IValue> elements, IReadOnlyList<ResolvedPort> inputPorts, IValue output)
            {
                _index = index;
                _elements = elements;
                InputPorts = inputPorts;
                ReturnConstraint = output;
            }

            private readonly IValue _index;
            private readonly IReadOnlyList<IValue> _elements;
            public override IReadOnlyList<ResolvedPort> InputPorts { get; }
            public override IValue ReturnConstraint { get; }
            
            public override Result<IValue> Index(Identifier id, CompilationContext context) =>
	            _elements.Select(e => e.Index(id, context))
	                     .MapEnumerable(elements => Create(_index, elements.ToList(), InputPorts, ReturnConstraint));
            
            protected override Result<IValue> ResolveFunctionBody(IReadOnlyList<IValue> arguments, CompilationContext context) =>
	            _elements.Select(e => e.Call(arguments.ToArray(), context))
	                     .MapEnumerable(v => Create(_index, v.ToList(), InputPorts, ReturnConstraint));
        }
	}
}