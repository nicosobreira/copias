# Copias de programas do Linux em C

Copia dos seguintes programas comuns no Linux:

- `cat`: concatena vários arquivos.
- `wc`: conta quantas letras, palavras e linhas o texto possui.
- `mygit`: clone simplificado do [Git](https://git-scm.com/) (**Incompleto**).

## Executando

Garanta as seguintes dependências, ou use `nix develop` para baixá-las:

- [CMake](https://cmake.org/) como *Build System*.
- Um compilador de C (recomendo o [Clang](https://clang.llvm.org/)).
- E para o `mygit`, a biblioteca [OpenSSL](https://openssl-library.org/).

Para compilar e executar faça:

> Considere.
> - `<nome da copia>`: qual copia deseja executar.
> - `<parâmetros>`: os parâmetros para a copia.

``` bash
cmake --workflow dev
./build/debug/bin/<nome da copia> <parâmetros>
```

Ou usando o script `./tools/run.sh`:

``` bash
./tools/run.sh run <nome da copia> <parâmetros>
```

## Explicações

Decidi focar **apenas na parte da manipulação de arquivos** ao longo do projeto, por isso espere uma qualidade menor nas outras áreas do código. Abaixo listo alguns projetos que possuem uma melhor organização:

- [c-algorithms](https://github.com/nicosobreira/c-algorithms): Algoritmos e Estruturas de Dados.
- [c-matrix](https://github.com/nicosobreira/c-matrix): Biblioteca de Álgebra Linear.
- [Typetest](https://github.com/nicosobreira/Typetest): Jogo de digitação no Terminal.

### Cat

- **Estrutura do Projeto**:

```
./project/cat/
├── CMakeLists.txt
└── src
    ├── main.c            # Passando argumentos
    ├── cat.c             # Lógica do comando cat
    ├── cat.h
    ├── normal.c          # Concatena arquivos
    ├── normal.h
    ├── with_numbers.c    # Concatena arquivos e exibe o número de linhas
    └── with_numbers.h
```

- **Exemplos de Uso**:

Com apenas um arquivo:

```
$ ./build/debug/bin/cat .envrc
use flake
```

Com dois arquivos:

```
$ ./build/debug/bin/cat -n .envrc .gitignore
       1  use flake
       2  # Custom
       3  build/
       4  perf.data*
       # ...
       # ...
       # ...
      58  Mkfile.old
      59  dkms.conf
```

---

Em `cat.c`, temos a lógica de separar o fluxo do programa em dois: com a opção `-n` vamos para `with_number.c`, caso contrário, para `normal.c`.

#### `normal.c`

> Exemplo mais simples de leitura de arquivos em C.

<details>
<summary>Clique aqui para ver o código completo.</summary>

``` c
#include "normal.h"

#include "commom/error.h"

#include <stdio.h>

static inline void output_file(FILE *file, FILE *stream)
{
    int c;
    while ((c = fgetc(file)) != EOF)
    {
        int did_write = fputc(c, stream);

        if (did_write == EOF && ferror(stream))
        {
            LOG_ERROR("I/O error while writing from stream");
            return;
        }
    }

    if (ferror(file))
    {
        LOG_ERROR("I/O error while reading from file");
    }
}

void show_files(Parameters param, FILE *stream)
{
    for (int i = 0; i < param.total; i++)
    {
        const char *file_path = param.args[i];
        FILE *file = fopen(file_path, "r");
        if (file == NULL)
        {
            LOG_ERROR("Failed to open the file \"%s\"", file_path);
            continue;
        }

        output_file(file, stream);

        fclose(file);
    }
}
```

</details>

Tudo começa na função `show_files`, que abre cada arquivo em `file_path` usando o `fopen(file_path, "r")`.

``` c
void show_files(Parameters param, FILE *stream)
{
    for (int i = 0; i < param.total; i++)
    {
        const char *file_path = param.args[i];
        FILE *file = fopen(file_path, "r");
        if (file == NULL)
        {
            LOG_ERROR("Failed to open the file \"%s\"", file_path);
            continue;
        }

        output_file(file, stream);

        fclose(file);
    }
}
```

A documentação do [`fopen`](https://devdocs.io/c/io/fopen) diz que em caso de **falha** a função retornará um ponteiro `NULL`, e por isso precisamos verificar esse caso e agir adequadamente.

Depois vamos para a função `output_file`, que irá copiar o conteúdo do recém aberto `file` para `stream`; como estamos exibindo o resultado para o terminal, passamos `stdout` para o parâmetro `stream`, e por isso conteúdo será acrescentado (*append*) a `stdout`. A função é declarada da seguinte forma:

``` c
static inline void output_file(FILE *file, FILE *stream)
{
    int c;
    while ((c = fgetc(file)) != EOF)
    {
        int did_write = fputc(c, stream);

        if (did_write == EOF && ferror(stream))
        {
            LOG_ERROR("I/O error while writing from stream");
            return;
        }
    }

    if (ferror(file))
    {
        LOG_ERROR("I/O error while reading from file");
    }
}
```

O *while loop* copia cada carácter `c` de `file` para `stream`, onde `fgetc` pega o carácter e `fputc` escreve-o para `stream`. As documentações das funções [`fgetc`](https://devdocs.io/c/io/fgetc) e [`fputc`](https://devdocs.io/c/io/fputc) dizem que caso o arquivo tenha acabado ou algum erro tenha acontecido, o valor retornado será `EOF`, sendo que a diferenciação entre os dois casos é feita pela funções [`feof`](https://devdocs.io/c/io/feof) e [`ferror`](https://devdocs.io/c/io/ferror), respectivamente. Como nesse exemplo o `EOF` indica que podemos sair da função, temos apenas que tratar os erros.

Por fim, temos que fechar o arquivo `file` que foi aberto em `show_files` com o `fclose`.

#### `with_numbers.c`

> Lendo linha por linha.

<details>
<summary>Clique aqui para ver o código completo.</summary>

``` c
#include "with_numbers.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "commom/buffer.h"
#include "commom/error.h"

static inline int get_line(char *buffer, size_t *len, FILE *file)
{
    const char delimiter = '\n';

    size_t current_index = 0;

    char c;
    while ((c = (char)fgetc(file)) != EOF)
    {
        if (current_index >= (*len))
        {
            *len += 1;
            buffer = realloc(buffer, *len);
        }

        buffer[current_index] = c;
        current_index += 1;

        if (c == delimiter)
        {
            break;
        }
    }

    if (feof(file))
    {
        return 1;
    }

    if (ferror(file))
    {
        LOG_ERROR("I/O error while reading file");
        return 2;
    }

    // Need to add the '\0' at the end of buffer
    current_index += 1;
    if (current_index >= (*len))
    {
        *len += 1;
        buffer = realloc(buffer, *len);
    }
    buffer[current_index] = '\0';

    return 0;
}

static inline int output_file_number(FILE *file, FILE *stream, int total_lines)
{
    char *buffer = malloc(BUFFER_SIZE);

    size_t buffer_size;
    while ((get_line(buffer, &buffer_size, file) == 0))
    {
        fprintf(stream, "%8d  ", total_lines);
        fputs(buffer, stream);

        total_lines += 1;

        memset(buffer, 0, BUFFER_SIZE);
    }

    free(buffer);

    return total_lines;
}

static inline int show_file_number(const char *file_path, FILE *stream, int total_lines)
{
    FILE *file = fopen(file_path, "r");
    if (file == NULL)
    {
        LOG_ERROR("Failed to open the file \"%s\"", file_path);

        return total_lines;
    }

    int new_total = output_file_number(file, stream, total_lines);

    fclose(file);

    return new_total;
}

void show_files_number(Parameters param, FILE *stream)
{
    int total_lines = 1;
    for (int i = 0; i < param.total; i++)
    {
        total_lines = show_file_number(param.args[i], stream, total_lines);
    }
}
```

</details>

As funções `show_files_number` e `show_file_number` fazem a mesma coisa que `show_files` no exemplo anterior, a diferença está em `output_file_number`.

``` c
static inline int output_file_number(FILE *file, FILE *stream, int total_lines)
{
    char *buffer = malloc(BUFFER_SIZE);

    size_t buffer_size;
    while ((get_line(buffer, &buffer_size, file) == 0))
    {
        fprintf(stream, "%8d  ", total_lines);
        fputs(buffer, stream);

        total_lines += 1;

        memset(buffer, 0, BUFFER_SIZE);
    }

    free(buffer);

    return total_lines;
}
```

Como é preciso contar cada linha do arquivo,
