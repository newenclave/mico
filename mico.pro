TEMPLATE = app
CONFIG += console c++11
CONFIG -= app_bundle
CONFIG -= qt

SOURCES += mico.cpp

INCLUDEPATH += etool/include \
               catch         \
               include

DISTFILES += \
    drafts/tail_recursion.md \
    tests.txt \
    README2.md \
    examples/t002.mico \
    examples/t001.mico


#DEFINES += DEBUG=1

DEFINES += CHECK_CASTS=1
DEFINES += DISABLE_SWITCH_WARNINGS=1
DEFINES += DISABLE_MACRO=0

#CONFIG += c++14

HEADERS += \
    etool/include/etool/details/byte_order.h \
    etool/include/etool/details/byte_swap.h \
    etool/include/etool/details/dummy_mutex.h \
    etool/include/etool/details/operators.h \
    etool/include/etool/details/result.h \
    etool/include/etool/details/type_uid.h \
    etool/include/etool/intervals/traits/array_map.h \
    etool/include/etool/intervals/traits/array_set.h \
    etool/include/etool/intervals/traits/std_map.h \
    etool/include/etool/intervals/traits/std_set.h \
    etool/include/etool/intervals/attributes.h \
    etool/include/etool/intervals/endpoint_type.h \
    etool/include/etool/intervals/interval.h \
    etool/include/etool/intervals/map.h \
    etool/include/etool/intervals/set.h \
    etool/include/etool/intervals/tree.h \
    etool/include/etool/trees/trie/nodes/array.h \
    etool/include/etool/trees/trie/nodes/map.h \
    etool/include/etool/trees/trie/base.h \
    include/mico/builtin/common.h \
    include/mico/eval/operations/arrays.h \
    include/mico/eval/operations/boolean.h \
    include/mico/eval/operations/common.h \
    include/mico/eval/operations/float.h \
    include/mico/eval/operations/function.h \
    include/mico/eval/operations/integer.h \
    include/mico/eval/operations/module.h \
    include/mico/eval/operations/string.h \
    include/mico/eval/operations/tables.h \
    include/mico/eval/evaluator.h \
    include/mico/eval/operation.h \
    include/mico/eval/tree_walking.h \
    include/mico/expressions/array.h \
    include/mico/expressions/call.h \
    include/mico/expressions/elipsis.h \
    include/mico/expressions/fn.h \
    include/mico/expressions/ident.h \
    include/mico/expressions/ifelse.h \
    include/mico/expressions/impl.h \
    include/mico/expressions/index.h \
    include/mico/expressions/infix.h \
    include/mico/expressions/list.h \
    include/mico/expressions/macro.h \
    include/mico/expressions/module.h \
    include/mico/expressions/none.h \
    include/mico/expressions/prefix.h \
    include/mico/expressions/quote.h \
    include/mico/expressions/registry.h \
    include/mico/expressions/string.h \
    include/mico/expressions/table.h \
    include/mico/expressions/unquote.h \
    include/mico/expressions/value.h \
    include/mico/macro/processor.h \
    include/mico/modules/debug.h \
    include/mico/modules/io.h \
    include/mico/modules/string.h \
    include/mico/objects/array.h \
    include/mico/objects/base.h \
    include/mico/objects/boolean.h \
    include/mico/objects/collectable.h \
    include/mico/objects/error.h \
    include/mico/objects/functions.h \
    include/mico/objects/generator.h \
    include/mico/objects/interval.h \
    include/mico/objects/module.h \
    include/mico/objects/null.h \
    include/mico/objects/numbers.h \
    include/mico/objects/quote.h \
    include/mico/objects/reference.h \
    include/mico/objects/return.h \
    include/mico/objects/string.h \
    include/mico/objects/table.h \
    include/mico/ast.h \
    include/mico/builtin.h \
    include/mico/environment.h \
    include/mico/expressions.h \
    include/mico/idents.h \
    include/mico/lexer.h \
    include/mico/numeric.h \
    include/mico/objects.h \
    include/mico/operations.h \
    include/mico/parser.h \
    include/mico/repl.h \
    include/mico/state.h \
    include/mico/statements.h \
    include/mico/tokens.h \
    include/mico/expressions/interval.h \
    include/mico/expressions/forin.h \
    include/mico/objects/break.h \
    include/mico/objects/continue.h \
    include/mico/eval/operations/interval.h
