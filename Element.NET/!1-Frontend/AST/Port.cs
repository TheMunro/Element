using Lexico;

namespace Element.AST
{
    [WhitespaceSurrounded, MultiLine]
    // ReSharper disable once ClassNeverInstantiated.Global
    public class Port
    {
        // ReSharper disable once UnusedMember.Global - Used by Lexico to generate instances
        // ReSharper disable once MemberCanBePrivate.Global
        public Port() {}

        public Port(string identifier, TypeAnnotation? typeAnnotation)
        {
            _identifier = new Identifier(identifier);
            _type = typeAnnotation;
        }
        
        public Port(string identifier, IConstraint constraint)
        {
            _identifier = new Identifier(identifier);
            _cachedConstraint = constraint;
        }
        
        internal void Initialize(Declaration declarer) => _type?.Initialize(declarer);

        public static Port VariadicPort { get; } = new Port();
        public static Port ReturnPort(TypeAnnotation? annotation) => new Port("return", annotation);
        public static Port ReturnPort(IConstraint constraint) => new Port("return", constraint);
        
#pragma warning disable 649
        // ReSharper disable FieldCanBeMadeReadOnly.Local
        [Alternative(typeof(Identifier), typeof(Unidentifier))] private object _identifier;
        [Optional] private TypeAnnotation? _type;
        // ReSharper restore FieldCanBeMadeReadOnly.Local
#pragma warning restore 649

        public Identifier? Identifier => _identifier is Identifier id ? (Identifier?)id : null;
        private IConstraint? _cachedConstraint;

        public IConstraint ResolveConstraint(CompilationContext compilationContext) =>
            _cachedConstraint ?? (_type != null
                                      ? _cachedConstraint = _type.ResolveConstraint(compilationContext)
                                      : AnyConstraint.Instance);

        public override string ToString() => $"{_identifier}{_type}";
    }
    
    // ReSharper disable once ClassNeverInstantiated.Global
    public class PortList : ListOf<Port> // CallExpression uses ListOf because it looks like a list due to using brackets
    {
        public void Initialize(Declaration declarer)
        {
            foreach (var port in List)
            {
                port.Initialize(declarer);
            }
        }
    }
}