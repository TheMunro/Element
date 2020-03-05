using System.Collections.Generic;
using System.IO;
using System.Linq;

namespace Element.AST
{
    public sealed class GlobalScope : DeclaredScope
    {
        private readonly Dictionary<FileInfo, SourceScope> _sourceScopes = new Dictionary<FileInfo, SourceScope>();

        public SourceScope this[FileInfo file]
        {
            get => _sourceScopes[file];
            set => _sourceScopes[file] = value;
        }

        public override IValue? this[Identifier id, bool recurse, CompilationContext context] => IndexCache(id);

        protected override IEnumerable<Declaration> ItemsToCacheOnValidate => _sourceScopes.Values.SelectMany(s => s);
    }
}