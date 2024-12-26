rm -rf bin
rm -rf lib
mkdir bin
mkdir lib
clang -o lib/parser.o -c source/parser.c
clang -o lib/analyzer.o -c source/analyzer.c
ar rcs lib/libbrv.so lib/parser.o lib/analyzer.o
clang tests/main.c -Llib -lbrv -o bin/main
