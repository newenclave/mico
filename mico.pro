TEMPLATE = app
CONFIG += console c++11
CONFIG -= app_bundle
CONFIG -= qt

SOURCES += main.cpp \
    test.cpp

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
    include/mico/expressions/expressions.h \
    include/mico/statements.h \
    include/mico/eval/evaluator.h \
    include/mico/eval/tree_walking.h \
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
    include/mico/expressions/detail.h \
    include/mico/expressions/value.h

DISTFILES += \
    drafts/tail_recursion.md \
    tests.txt


#DEFINES += DEBUG=1
