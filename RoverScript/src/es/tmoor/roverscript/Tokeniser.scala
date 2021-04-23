package es.tmoor.roverscript
// Haha you get it it's like JavaScript but RoverScript
// I'll show myself out

import util.parsing.combinator.RegexParsers
import scala.collection.mutable.HashMap

class Tokeniser extends RegexParsers {
  override def skipWhitespace: Boolean = true

  sealed abstract trait Token
  type OToken[T] = Option[T] with Token
  type ValueParser = Parser[Value[Type]]
  private implicit class Option2Token[T] (option: Option[T]) {
    def toToken: OToken[T] = option.asInstanceOf[OToken[T]]
  }
  sealed abstract class Construct[T <: Token] extends Parser[T] {
    def parser: Parser[T]
    def apply(in: Input): ParseResult[T] = parser(in)
  }
  
  sealed abstract trait Value[-T <: Type] extends Token {
    def _t: Type
  }
  sealed trait Matchable[-T <: Type] extends Value[T]
  sealed trait Assignable[-T <: Type] extends Matchable[T]

  // LITERALS
  trait Literal[T <: Type] extends Matchable[T]
  object Literal extends Construct[Literal[Type]] {
    def parser: Parser[Literal[Type]] = IntLiteral.parser.asInstanceOf[Parser[Literal[Type]]] ||| Ident.parser.asInstanceOf[Parser[Literal[Type]]] ||| Atom.parser.asInstanceOf[Parser[Literal[Type]]]
  }
  case class IntLiteral(value: Int) extends Literal[IntType] {
    def _t: IntType = IntType(32)
  }
  object IntLiteral extends Construct[IntLiteral] {
    def parser: Parser[IntLiteral] = "[0-9]+".r ^^ (i => IntLiteral(i.toInt))
  }
  case class BooleanLiteral(value: Boolean) extends Literal[BooleanType.type] {
    def _t: Type = BooleanType
  }
  object BooleanLiteral extends Construct[BooleanLiteral] {
    def parser: Parser[BooleanLiteral] = "'true'" ^^^ BooleanLiteral(true) | "'false'" ^^^ BooleanLiteral(false)
  }
  case class Ident[T <: Type](name: String, identType: T) extends Literal[T] with Assignable[Type] {
    def _t: Type = identType
  }
  object Ident extends Construct[Ident[Type]] {
    def parser: Parser[Ident[Type]] = identTypes.keys.foldLeft[Parser[String]](failure(""))(_|||_) ^^ (i => Ident(i, identTypes(i))) | failure("Declared identifier expected")
  }
  

  case class Atom(value: String) extends Literal[AtomType.type] {
    def _t: Type = AtomType
  }
  object Atom extends Construct[Literal[AtomType.type]] {
    def parser: Parser[Literal[AtomType.type]] = """'([^']|\\')*'""".r ^^ (Atom(_))
  }

  abstract class LiteralList[T <: Type] extends Literal[ListType[T]] with Assignable[T with ListType[T]]

  case class CommaList[T <: Type](items: Seq[Literal[T]]) extends LiteralList[ListType[T]] {
    def _t: Type = new ListType[T]
  }
  object CommaList extends Construct[CommaList[Type]] {
    // TODO: Resolve cons for [a|b] where b is an ident holding a list
    def parser: Parser[CommaList[Type]] = "[" ~> repsep(Literal,",") <~ "]" ^^ (CommaList(_))
  }
  // This assumes we knw
  case class ConsList[T <: Type](prepend: Seq[Literal[T]], variable: Ident[ListType[T]])
  

  // TYPES
  val identTypes = collection.mutable.HashMap[String,Type]()
  abstract trait Type extends Token
  object Type extends Construct[Type] {
    def parser: Parser[Type] = BuiltInType
  }

  object BuiltInType extends Construct[Type] {
    def parser: Parser[Type] = BooleanType ||| IntType ||| FloatType ||| BitsType
  }

  object BooleanType extends Construct[Type] with Type {
    def parser: Parser[Type] = "boolean" ^^^ this
  }

  object AtomType extends Construct[Type] with Type {
    def parser: Parser[Type] = "atom" ^^^ this
  }

  class ListType[T <: Type] extends Type

  abstract class SizedType(size: Int) extends Type

  object SizeSpecs extends Construct[IntLiteral] {
    def parser: Parser[IntLiteral] = (":" ~> IntLiteral ^^ (i=> i))
  }

  case class IntType(size: Int) extends SizedType(size)
  object IntType extends Construct[IntType] {
    def parser: Parser[IntType] = "int" ~> SizeSpecs.? ^^ {
      case Some(IntLiteral(s)) => IntType(s)
      case None => IntType(32)
    }
  }
  case class FloatType(size: Int) extends SizedType(size)
  object FloatType extends Construct[FloatType] {
    def parser: Parser[FloatType] = "float" ~> SizeSpecs.?  ^^ {
      case Some(IntLiteral(s)) => FloatType(s)
      case None => FloatType(32)
  }
  }
  // Bits requires size
  case class BitsType(size: Int) extends SizedType(size)
  object BitsType extends Construct[BitsType] {
    def parser: Parser[BitsType] = "bits" ~> SizeSpecs ^^ (s => BitsType(s.value))
  }

  // INSTRUCTIONS
  def validIdent[T<:Type](t: T): Parser[Ident[Type]] =
    identTypes.filter(_._2 == t).keys.foldLeft[Parser[String]](failure("expected an identifier"))(_|||_)^^(i => Ident(i,identTypes(i)))
  def validCall[T<:Type](t:T): Parser[LinkCall] =
    declFunctions.filter(_._2 == t).values.foldLeft[Parser[LinkCall]](failure("expected a function call"))(_|||_.signature)
  trait Instruction[-T <: Type] extends Token with Value[T]
  object Instruction extends Construct[Instruction[Type]] {
    def parser: Parser[Instruction[Type]] = Assignment ||| Update ||| LinkedFnCall ||| Signature
  }
  object Value extends Construct[Value[Type]] {
    def parser: ValueParser = Literal
    def apply[T <: Type](t: T): Parser[Value[Type]] = t match {
      case IntType(size) => validCall(t) ||| validIdent(t) ||| IntLiteral.parser.asInstanceOf[Parser[Value[Type]]]
      case FloatType(size) => validCall(t) ||| validIdent(t) // or float literal
      case BooleanType => validCall(t) ||| validIdent(t) ||| BooleanLiteral.parser.asInstanceOf[Parser[Value[Type]]]
      case t => validCall(t) ||| validIdent(t)
    }
  }
  object Assignable extends Construct[Assignable[Type]] {
    def parser: Parser[Assignable[Type]] = Ident.parser ||| CommaList.asInstanceOf[Parser[Assignable[Type]]]
  }
  object Matchable extends Construct[Matchable[Type]] {
    def parser: Parser[Matchable[Type]] = Literal
  }

  case class Assignment[T1 <: Type,T2 <: Type](lhs: Matchable[T2], rhs: Value[T2]) extends Instruction[T2] {
    def _t = lhs._t
  }
  object Assignment extends Construct[Assignment[Type,Type]] {
    def assignment[T1<:Type,T2<:Type]: Parser[Assignment[T1,T2]] = (Literal ~ ("<-" ~> Value) ^^ {
      case l ~ r => Assignment[T1,T2](l,r)
    })
    def parser: Parser[Assignment[Type,Type]] = {
      assignment[Type,Type] |
      ("[$@]?[_a-zA-Z0-9]+".r ~ ("<-" ~> Value) ^^ {case l ~ r =>
        identTypes += ((l,r._t))
        Assignment(Ident(l,r._t),r)})
    }
  }
  abstract class FnCall[T <: Type](args: Seq[Value[Type]]) {
    def lhs: Value[T] = args.head
    def _t: Type
  }
  case class UnlinkedFnCall(args: Value[Type]*) extends FnCall(args) {
    // Will return 'error' for now
    def _t: Type = AtomType
  }
  case class LazyFnCall(sig: Signature, call: UnlinkedFnCall) extends FnCall(call.args) with Instruction[Type] {
    def _t: Type = sig.returns
  }

  object Signature extends Construct[LazyFnCall] {
    def parser: Parser[LazyFnCall] = funSigs.keys.foldLeft[Parser[LazyFnCall]](failure(""))((a,v) => a|(v.value^^(f => LazyFnCall(v,f))))
  }

  
  case class LinkedFnCall[T1 <: Type,T2 <: Type](fun: FunctionDef[T1], args: Seq[Value[Type]]) extends FnCall[T2](args) with Instruction[T1] {
    def _t: Type = fun.sig.returns
  }
  type LinkCall = LinkedFnCall[Type,Type]

  object LinkedFnCall extends Construct[LinkCall] {
    def parser: Parser[LinkCall] = declFunctions.values.foldLeft[Parser[LinkCall]](failure(""))(_|_.signature)
  }
  /*
  case class IntAddOperation(lhs: Value[Type], rhs: Value[Type]) extends FnCall {
    def _t: Type = {
      val (IntType(s1),IntType(s2)) = (lhs._t,rhs._t)
      IntType(math.max(s1,s2))
    }
  }
  object IntAddOperation extends Construct[IntAddOperation] {
    def validInt: ValueParser = (identTypes.filter(_._2.isInstanceOf[IntType]).keys.foldLeft[Parser[String]](failure(""))(_|_) ^^ (s => Ident(s,identTypes(s))) | failure("Declared Identifier Expected")) ||| IntLiteral.parser.asInstanceOf[ValueParser]
    def parser: Parser[IntAddOperation] = (validInt <~ "+") ~ (validInt ||| Operation) ^^ {case a ~ b => IntAddOperation(a,b)}
  }
  */
  // TODO: Update in the form: `=x + 3` to assign x to x+3
  case class Update[T1<:Type,T2<:Type](lhs: Assignable[T1], rhs: Value[T2]) extends Instruction[T1] {
    def _t: Type = lhs._t
  }
  object Update extends Construct[Update[Type,Type]] {
    object isAssignable extends Parser[Any] {
      val grd = guard(LinkedFnCall)
      def apply(in: Input): ParseResult[Any] = grd(in) match {
        case Success(result, next) => result match {
          case op: LinkCall if op.lhs.isInstanceOf[Assignable[Type]] => Success(result,next)
          case op => Failure(s"Expected ${op.lhs} to be assignable", next)
        }
        case x => x
      }
    }
    def parser: Parser[Update[Type,Type]] = "=" ~ isAssignable ~> LinkedFnCall ^^ (op => Update(op.lhs.asInstanceOf[Assignable[Type]],op))
  }

  //def getAssign(in: Input) = Assignment(in)
  type CallParser = Parser[UnlinkedFnCall]
  
  
  val declFunctions = collection.mutable.HashMap[Signature,FunctionDef[Type]]()
  val funSigs = collection.mutable.HashMap[Signature,Type]()

  trait Function extends Token
  case class AnonFun() extends Function

  object AnonFun {

  }

  case class FunPredicate(variable: Matchable[Type], predicate: Seq[Value[Type]]) extends Token
  def newIdentwType: Parser[Ident[Type]] = "[$@]?[_a-zA-Z0-9]+".r ~ ("|" ~> Type) ^^ {
    case a ~ b => {
      identTypes += a -> b
      Ident(a,b)
    }
  }
  // May need to be class so that we have type
  object FunPredicate extends Construct[FunPredicate] {
    def parser: Parser[FunPredicate] = {
      (Matchable ||| newIdentwType) ~ ("," ~> rep1sep(Value(BooleanType),",")).? ^^ {
        case m ~ f => FunPredicate(m,f.getOrElse(Seq()))
      }
    }
  }

  case class AtomicFun(instructions: Array[Instruction[Type]]) extends Function
  object AtomicFun extends Construct[AtomicFun] {
    def parser: Parser[AtomicFun] = "(" ~> rep1sep(Instruction,";")<~"."~")" ^^ (l => AtomicFun(l.toArray))
  }

  case class Signature(value: CallParser, returns: Type, args: Seq[Type]) {
    def _t: Type = returns
    lazy val fn = funSigs(this)
  }

  // TODO: Find out if we need to track arg types here
  case class FunctionDef[+T <: Type](sig: Signature, fun: Function) extends Token {
    declFunctions += (sig.asInstanceOf[Signature] -> this.asInstanceOf[FunctionDef[Type]])
    def arity = sig.args.length
    def signature: Parser[LinkCall] = sig.value ^^ (f => LinkedFnCall(this,f.args))
    fun match {
      case AtomicFun(instructions) => instructions.mapInPlace {
        case s: LazyFnCall => LinkedFnCall(this,s.call.args)
        case i => i
      }
      case i => i
    }
  }

  object FunctionDef extends Construct[FunctionDef[Type]] {
    private def atomicHeader: Parser[Signature] = {
      "define" ~> """[^`\s|]+""".r.+ ~ ("|" ~> Type) ^^ {
        case a~b => 
          val sig = Signature(a.foldLeft[Parser[_]](success(""))(_~_) ^^^ UnlinkedFnCall(),b,Seq())
          funSigs += sig -> b
          sig
      }
    }
    private def atomicDef: Parser[FunctionDef[Type]] = atomicHeader ~ AtomicFun ^^ {
      case a~c => {
        FunctionDef(a,c)
      }
    }
    def parser: Parser[FunctionDef[Type]] = atomicDef
  }






  
  case class BIF(op: String) extends Function
  trait RootOps {
    def toBIF = this.asInstanceOf[BIF]
    def mkParser(t1: Type, t2: Type) = (Value(t1) <~ toBIF.op) ~ Value(t2) ^^ {
      case a~b => UnlinkedFnCall(a,b)
    }
  }
  abstract trait IntOps extends RootOps {
    private val iiCall: CallParser = mkParser(IntType(32),IntType(32))
    val iiSig = Signature(iiCall,IntType(32),Seq(IntType(32),IntType(32)))
    val iiDefn: FunctionDef[Type] = FunctionDef(iiSig,toBIF)
  }
  abstract trait FloatOps extends RootOps {
    private val ffCall: CallParser = mkParser(FloatType(32),FloatType(32))
    val ffSig = Signature(ffCall,FloatType(32),Seq(FloatType(32),FloatType(32)))
    val ffDefn: FunctionDef[Type] = FunctionDef(ffSig,toBIF)
    private val fiCall: CallParser = mkParser(FloatType(32),IntType(32))
    val fiSig = Signature(fiCall,FloatType(32),Seq(FloatType(32),IntType(32)))
    val fiDefn: FunctionDef[Type] = FunctionDef(fiSig,toBIF)
    private val ifCall: CallParser = mkParser(IntType(32),FloatType(32))
    val ifSig = Signature(ifCall,FloatType(32),Seq(IntType(32),FloatType(32)))
    val ifDefn: FunctionDef[Type] = FunctionDef(ifSig,toBIF)
  }

  object Add extends BIF("+") with IntOps with FloatOps
  declFunctions += (Add.ffSig->Add.ffDefn,Add.fiSig->Add.fiDefn,Add.ifSig->Add.ifDefn,Add.iiSig->Add.iiDefn)
  object Sub extends BIF("-") with IntOps with FloatOps
  declFunctions += (Sub.ffSig->Sub.ffDefn,Sub.fiSig->Sub.fiDefn,Sub.ifSig->Sub.ifDefn,Sub.iiSig->Sub.iiDefn)
  object Mul extends BIF("*") with IntOps with FloatOps
  declFunctions += (Mul.ffSig->Mul.ffDefn,Mul.fiSig->Mul.fiDefn,Mul.ifSig->Mul.ifDefn,Mul.iiSig->Mul.iiDefn)
  object IntDiv extends BIF("div") with IntOps
  declFunctions += IntDiv.iiSig -> IntDiv.iiDefn
  object FloatDiv extends BIF("/") with FloatOps {
    private def iiCall = mkParser(IntType(32),IntType(32))
    def iiSig = Signature(iiCall,FloatType(32),Seq(IntType(32),IntType(32)))
    def iiDefn: FunctionDef[Type] = FunctionDef(iiSig,toBIF)
  }
  declFunctions += (FloatDiv.ffSig->FloatDiv.ffDefn,FloatDiv.fiSig->FloatDiv.fiDefn,FloatDiv.ifSig->FloatDiv.ifDefn,FloatDiv.iiSig->FloatDiv.iiDefn)
  object Rem extends BIF("rem") with IntOps
  declFunctions += Rem.iiSig->Rem.iiDefn
  object Band extends BIF("band") with IntOps
  declFunctions += Band.iiSig->Band.iiDefn
  object Bor extends BIF("bor") with IntOps
  declFunctions += Bor.iiSig->Bor.iiDefn
  object Bxor extends BIF("bxor") with IntOps
  declFunctions += Bxor.iiSig->Bxor.iiDefn
  object Bnot extends BIF("bnot") with IntOps
  declFunctions += Bnot.iiSig->Bnot.iiDefn
}