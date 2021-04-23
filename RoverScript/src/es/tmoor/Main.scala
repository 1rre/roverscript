package es.tmoor.roverscript

import util.parsing.input.CharSequenceReader

object Main extends App {
  val preprocessor = new Preprocessor
  val tokeniser = new Tokeniser

  val in = new CharSequenceReader("""
    x|int:32, 'true'
  """)
  
  val res = tokeniser.FunPredicate(in)
  println(s"$res")
  //\nLines:\n${res.get.fun.asInstanceOf[tokeniser.AtomicFun].instructions.toList.mkString("\n")}
}
