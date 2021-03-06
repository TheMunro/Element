using System;
using System.Collections.Generic;
using System.Linq;
using Lexico;

namespace Element.AST
{
    [WhitespaceSurrounded, MultiLine]
    // ReSharper disable once ClassNeverInstantiated.Global
    public class Scope : DeclaredScope, IDeclared
    {
#pragma warning disable 649, 169
        [Literal("{")] private Unnamed _open;
        [Optional] private List<Declaration>? _items;
        [Literal("}")] private Unnamed _close;
#pragma warning restore 649, 169

        protected override IEnumerable<Declaration> ItemsToCacheOnValidate => _items ?? Enumerable.Empty<Declaration>();

        public Declaration Declarer { get; private set; }

        public override IValue? this[Identifier id, bool recurse, CompilationContext context] =>
            IndexCache(id) ?? (recurse ? Declarer.ParentScope[id, true, context] : null);

        public void Initialize(Declaration declarer)
        {
            Declarer = declarer ?? throw new ArgumentNullException(nameof(declarer));
            InitializeItems();
        }

        private class ClonedScope : ScopeBase, IDeclared
        {
            private readonly IScope _parentScope;

            public ClonedScope(Declaration declarer, IEnumerable<Declaration> items, IScope parentScope)
            {
                Declarer = declarer;
                _parentScope = parentScope;
                SetRange(items.Select(item => (item.Identifier, (IValue)item.Clone(this))));
            }

            public override IValue? this[Identifier id, bool recurse, CompilationContext context] =>
                IndexCache(id) ?? (recurse ? _parentScope[id, true, context] : null);

            public Declaration Declarer { get; }
        }

        public IScope Clone(IScope cloneParent) => new ClonedScope(Declarer, _items, cloneParent);
    }
}