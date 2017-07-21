TEMPLATE = app
CONFIG += console c++11
CONFIG -= app_bundle
CONFIG -= qt

SOURCES += main.cpp

INCLUDEPATH += etool/include \
               catch         \
               include

HEADERS += \
    include/mico/tokens.h \
    include/mico/lexer.h \
    include/mico/numeric.h \
    include/mico/idents.h \
    include/mico/parser.h \
    include/mico/ast.h \
    include/mico/operations.h \
    include/mico/expressions.h \
    include/mico/statements.h \
    include/mico/repl.h \
    include/mico/environment.h \
    include/mico/builtin.h \
    include/mico/objects.h \
    include/mico/objects/base.h \
    include/mico/objects/reference.h \
    include/mico/objects/numbers.h \
    include/mico/objects/table.h \
    include/mico/objects/null.h \
    include/mico/objects/array.h \
    include/mico/objects/functions.h \
    include/mico/objects/string.h \
    include/mico/objects/error.h \
    include/mico/objects/return.h \
    include/mico/objects/boolean.h \
    include/mico/objects/collectable.h \
    include/mico/expressions/fn.h \
    include/mico/expressions/impl.h \
    include/mico/expressions/value.h \
    include/mico/eval/evaluator.h \
    include/mico/eval/tree_walking.h \
    include/mico/eval/operations/boolean.h \
    include/mico/eval/operations/float.h \
    include/mico/eval/operations/integer.h \
    include/mico/eval/operations/string.h \
    include/mico/eval/operation.h \
    include/mico/eval/operations/common.h \
    include/mico/eval/operations/tables.h \
    include/mico/eval/operations/arrays.h \
    include/mico/eval/operations/function.h \
    include/mico/expressions/array.h \
    include/mico/expressions/ident.h \
    include/mico/expressions/string.h \
    include/mico/expressions/prefix.h \
    include/mico/expressions/infix.h \
    include/mico/expressions/none.h \
    include/mico/expressions/index.h \
    include/mico/expressions/call.h \
    include/mico/expressions/ifelse.h \
    include/mico/expressions/table.h \
    include/mico/state.h \
    etool/include/etool/trees/trie/nodes/array.h \
    etool/include/etool/trees/trie/nodes/map.h \
    etool/include/etool/trees/trie/base.h \
    etool/include/etool/impls/result.h \
    include/mico/expressions/registry.h \
    include/mico/expressions/list.h \
    etool/include/etool/details/result.h

DISTFILES += \
    drafts/tail_recursion.md \
    tests.txt


#DEFINES += DEBUG=1

DEFINES += CHECK_CASTS=1
DEFINES += DISABLE_SWITCH_WARNINGS=1
