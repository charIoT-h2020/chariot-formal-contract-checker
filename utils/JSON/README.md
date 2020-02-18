The JSON unit
=============

The JSON library contains two generic JSON parsers, both being
reactive to JSON events. The JSON lexer and the JSON parsers are
incremental and persistent. They can stop their parsing and start it
again in the future from the same internal state. This library is
complete, but it does not support unicode.

The most evolved parsers are based on the classes `CommonParser`
and `CommonWriter`. It is not useful to derive them, but they
require the methods `readJSon` and `writeJSon` (see their respective
contructors) in the objects to persist.

The `BasicParser` and `BasicWriter` classes are more basic. They
require nothing in the objects to persist but they need to be derived
in order to have an implementation for the methods
`BasicParser::openStartElement`, ..., `BasicParser::endDocument`
and `BasicWriter::getEvent`. This methods have to manage the internal
object changes in adequation with the persisted objects

