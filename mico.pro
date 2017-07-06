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
    include/mico/objects.h \
    include/mico/operations.h \
    include/mico/expressions.h \
    include/mico/statements.h \
    include/mico/eval/evaluator.h \
    include/mico/eval/tree_walking.h \
    include/mico/repl.h \
    include/mico/environment.h \
    include/mico/builtin.h \
    include/mico/memory.h

DISTFILES += \
    drafts/tail_recursion.md \
    tests.txt


#DEFINES += DEBUG=1
