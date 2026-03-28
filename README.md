# Лабораторная Работа 6: анализатор ANTLR + TREE

## Почему тут есть Docker

Сначала я поднимал всё локально, но было неудобно из-за проблем с:
- несовпадения версий ANTLR tool/runtime
- кривого окружения на Windows :)
- проблем с путями/кодировкой (у меня имя юзера ОС на кириллице :)) и воспроизводимостью

Обернул всё в Docker, чтобы запуск был стабильный.

## Что внутри проекта

- `MiniLangLexer.g4` — лексер
- `MiniLangParser.g4` — парсер
- `src/Ast.h`, `src/Ast.cpp` — AST-узлы и печать дерева
- `src/AstBuilderVisitor.h`, `src/AstBuilderVisitor.cpp` — построение AST из parse tree
- `src/Interpreter.h`, `src/Interpreter.cpp` — интерпретатор: обходит AST и исполняет программу
- `src/SyntaxErrorListener.h` — красивый вывод синтаксических ошибок
- `src/main.cpp` — входная точка: чтение файла, парсинг, вывод parse tree, AST и результата выполнения
- `tests/*.txt` — тестовые примеры
- `Makefile` — сборка и тесты
- `Dockerfile` — контейнеризированный запуск

## Как запустить

### Docker (рекомендуется)

```bash
docker build -t minilang-lab6 .
docker run --rm minilang-lab6
```

В контейнере автоматически выполняется `make test` — все 8 тестов.

Запуск одного файла:

```bash
docker run --rm -v "$(pwd)/tests:/app/tests" minilang-final build/minilang tests/positive_basic.txt

# Или свой файл:
docker run --rm -v "$(pwd)/myfile.txt:/app/myfile.txt" minilang-final build/minilang myfile.txt
```

### Локально на Windows

Нужно:
- `mingw32-make`, `g++`, `java` в PATH;
- в папке `antlr4` лежат: `antlr-4.13.1-complete.jar`, runtime include/lib (и DLL для Windows).

```bash
mingw32-make all
mingw32-make test

# Один файл:
build/minilang.exe tests/positive_basic.txt
```

## Что выводит программа

Для каждого входного файла выводится три секции:

```
=== PARSE TREE ===
(program ...)          # дерево разбора ANTLR

=== AST ===
Program                # компактное абстрактное синтаксическое дерево
  ...

=== OUTPUT ===
3                      # фактический вывод программы (результаты print)
2
```

## Какие конструкции поддерживаются

- объявления функций (`void`/`int`);
- параметры `int`;
- `print(...)`;
- `return` и `return expr`;
- вызовы функций с аргументами;
- выражения с `+` и `-`, `ID`, `NUM`, скобки;
- объявления локальных переменных (`int x;`).

## Тесты

| Файл | Описание | Ожидаемый результат |
|---|---|---|
| `positive_basic.txt` | вызов функции с print | `3`, `2` |
| `positive_nested_calls.txt` | функции, вызывающие друг друга | `15`, `15` |
| `positive_arithmetic.txt` | цепочка арифметики, несколько вызовов | `16`, `9` |
| `positive_var_decl.txt` | объявление переменных, вложенные вызовы | `12`, `6`, `20`, `10` |
| `negative_missing_semicolon.txt` | синтаксическая ошибка: нет `;` | ошибка парсинга |
| `negative_bad_token.txt` | синтаксическая ошибка: символ `@` | ошибка парсинга |
| `negative_undefined_function.txt` | семантическая ошибка: вызов несуществующей функции | `Semantic error: call to undefined function` |
| `negative_wrong_argc.txt` | семантическая ошибка: неверное число аргументов | `Semantic error: function expects N argument(s)` |

## Нюансы

- Для негативных тестов в `make test` ошибки ожидаемы (это нормально, Makefile игнорирует их через `-`).
- Если локально что-то ломается, лучше сразу запускать через Docker.
