// Thanks to:
//  https://eli.thegreenplace.net/2012/08/02/parsing-expressions-by-precedence-climbing

#include "parser.h"
#include "core/parser/regexs.h"

namespace teditor {
namespace calc {

Parser::Parser():
  OpInfoMap({
    {Assignment, {1, true}},
    {Plus,       {2, true}},
    {Minus,      {2, true}},
    {Multiply,   {3, true}},
    {Divide,     {3, true}},
    {Power,      {4, false}}
  }),
  Tokens({
    {Float,       parser::Regexs::FloatingPt},
    {Int,         parser::Regexs::Integer},
    {BrktOpen,    "\\("},
    {BrktClose,   "\\)"},
    {SemiColon,   ";"},
    {Symbol,      parser::Regexs::Variable},
    {Assignment,  "="},
    {Plus,        "[+]"},
    {Minus,       "[-]"},
    {Multiply,    "[*]"},
    {Divide,      "/"},
    {Power,       "^"},
    {Sq,          "sq("},
    {Cube,        "cube("},
    {Abs,         "abs("},
    {Sine,        "sin("},
    {Cosine,      "cos("},
    {Tangent,     "tan("},
    {ArcSince,    "asin("},
    {ArcCosine,   "acos("},
    {ArcTangent,  "atan("},
    {SineH,       "sinh("},
    {CosineH,     "cosh("},
    {TangentH,    "tanh("},
    {ArcSineH,    "asinh("},
    {ArcCosineH,  "acosh("},
    {ArcTangentH, "atanh("},
    {Sqrt,        "sqrt("},
    {Cbrt,        "cbrt("},
    {Log,         "log("},
    {Log10,       "log10("},
    {Exp,         "exp("},
    {Floor,       "floor("},
    {Ceil,        "ceil("},
    {Round,       "round("},
    {ToInt,       "int("},
    {ToFloat,     "float("},
    {WhiteSpace,  "\\s+"}
  }),
  lex(new parser::Lexer(Tokens)) {}

bool Parser::lexingDone(const parser::Token& tok) {
  return tok.type == parser::Token::End || tok.type == parser::Token::Unknown;
}

void Parser::evaluate(const std::string& expr, VarMap& vars) {
  parser::StringScanner sc(expr);
  evaluate(&sc, vars);
}

void Parser::evaluate(parser::Scanner *sc, VarMap& vars) {
  auto tok = lex->next(sc);
  if (lexingDone(tok)) return;
  if (tok.type == WhiteSpace) {
    tok = lex->next(sc);
    if (lexingDone(tok)) return;
  }
  ///@todo:
}

Num64 Parser::computeBinaryOp(const Num64& a, const Num64& b, TokenIds op) {
  switch(op) {
  case Plus:     return a + b;
  case Minus:    return a - b;
  case Multiply: return a * b;
  case Divide:   return a / b;
  case Power:    return pow(a, b);
  default:       return Num64();
  };
}

Num64 Parser::computeUnaryFunc(TokenIds funcId, const Num64& a) {
  switch(funcId) {
  case Sq:          return sq(a);
  case Cube:        return cube(a);
  case Abs:         return abs(a);
  case Sine:        return sin(a);
  case Cosine:      return cos(a);
  case Tangent:     return tan(a);
  case ArcSince:    return asin(a);
  case ArcCosine:   return acos(a);
  case ArcTangent:  return atan(a);
  case SineH:       return sinh(a);
  case CosineH:     return cosh(a);
  case TangentH:    return tanh(a);
  case ArcSineH:    return asinh(a);
  case ArcCosineH:  return acosh(a);
  case ArcTangentH: return atanh(a);
  case Sqrt:        return sqrt(a);
  case Cbrt:        return cbrt(a);
  case Log:         return log(a);
  case Log10:       return log10(a);
  case Exp:         return exp(a);
  case Floor:       return floor(a);
  case Ceil:        return ceil(a);
  case Round:       return round(a);
  case ToInt:       return a.toInt();
  case ToFloat:     return a.toFloat();
  default:          return Num64();
  };
}

}  // namespace calc
}  // namespace teditor
