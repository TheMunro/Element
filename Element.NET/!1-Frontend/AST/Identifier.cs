using Lexico;

namespace Element.AST
{
    public class Identifier : IExpressionListStart
    {
        // ReSharper disable once UnusedMember.Global
        public Identifier() {} // Need parameterless constructor for Lexico to construct instance
        public Identifier(string value) {Value = value;}

        // https://stackoverflow.com/questions/4400348/match-c-sharp-unicode-identifier-using-regex
        [field: Regex(@"[_\p{L}\p{Nl}][\p{L}\p{Nl}\p{Mn}\p{Mc}\p{Nd}\p{Pc}\p{Cf}]*")]
        public string Value { get; }
        public static implicit operator string(Identifier i) => i.Value;
        public override string ToString() => Value;
        public override int GetHashCode() => Value.GetHashCode();
        public override bool Equals(object obj) => obj?.Equals(Value) ?? (Value == null);
    }
}