# Лабораторная Работа 6: анализатор ANTLR + TREE

## Почему тут есть Docker

Сначала я поднимал всё локально, но было неудобно из-за проблем с:
- несовпадения версий ANTLR tool/runtime
- кривого окружение на Windows :)
- проблем с путями/кодировкой (у меня имя юзера ОС на кириллице :))  и воспроизводимостью

обернул всё в Docker, чтобы запуск был стабильный

## Что внутри проекта

- `MiniLangLexer.g4` - лексер
- `MiniLangParser.g4` - парсе
- `src/Ast.h`, `src/Ast.cpp` - AST-узлы и печать дерева
- `src/AstBuilderVisitor.h`, `src/AstBuilderVisitor.cpp` - построение AST из parse tree
- `src/SyntaxErrorListener.h` - красивый вывод синтаксических ошибок
- `src/main.cpp` - входная точка: чтение файла, парсинг, вывод parse tree и AST
- `tests/*.txt` - тестовые примеры
- `Makefile` - сборка и тесты (локально и в Docker)
- `Dockerfile` - контейнеризированный запуск

## Как запустить

### 1. Docker

Самый простой путь:

```bash
docker build -t minilang-lab6 .
docker run --rm minilang-lab6
```

В контейнере автоматически выполняется `make test`.

### 2. локально на Windows :)

У меня это тоже работает, но окружение должно быть настроено

Нужно:
- `mingw32-make`, `g++`, `java` в PATH;
- в папке `antlr4` лежат:
  - `antlr-4.13.1-complete.jar`,
  - runtime include/lib (и DLL для Windows).

Сборка:

```bash
mingw32-make -f Makefile.windows
```

Проверка:

```bash
mingw32-make -f Makefile.windows test
```

Запуск одного файла:

```bash
build/minilang.exe tests/positive_basic.txt
```

## Какие конструкции поддерживаются
Поддержал подмножество языка из прошлых лаб. работ:
- объявления функций (`void`/`int`);
- параметры `int`;
- `print(...)`;
- `return` и `return expr`;
- вызовы функций;
- выражения с `+` и `-`, `ID`, `NUM`, скобки.

## Нюансы
- Для негативных тестов в `make test` ошибки ожидаемы (это нормально).
- Если локально что-то ломается, лучше сразу запускать через Docker