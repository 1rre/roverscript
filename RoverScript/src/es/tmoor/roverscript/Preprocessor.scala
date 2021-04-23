package es.tmoor.roverscript

import util.parsing.combinator.RegexParsers

class Preprocessor extends RegexParsers {
  override def skipWhitespace: Boolean = false
  def ws: Parser[String] = """[\s\r\n\t\v]""".r
  def replacer = "(" ~> """[^)]*""".r <~ ")" ^^ (_.strip)
  def macroChar = ws.? ~> """[^()\s|:;`?]+""".r <~ ws.?
  def ppMacro: Parser[String] = "macro" ~> macroChar.+ ~ replacer ^^ {
    case a~b => macros += (
      a.foldLeft[Parser[_]](success(""))((a,v) => a ~ (ws.* ~> v)) ^^^ b
    )
  } ^^^ ""

  val macros = collection.mutable.ListBuffer[Parser[String]]()
  def validMacro = macros.fold(failure(""))(_|||_)

  def other: Parser[String] = ".".r

  def process = (ws | ppMacro | validMacro | other).* ^^ (s => s.mkString.strip)
} 