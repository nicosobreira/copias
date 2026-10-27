# Cópias de programas do Linux em C

Cópias dos seguintes programas comuns do Linux:

- `cat`: concatena vários arquivos.
- `wc`: conta quantas linhas, palavras e bytes o texto possui.
- `mygit`: clone simplificado do [Git](https://git-scm.com/) (**Incompleto**).

## Executando

Garanta as seguintes dependências, ou use `nix develop` para baixá-las:

- [CMake](https://cmake.org/) como *Build System*.
- Um compilador de C (recomendo o [Clang](https://clang.llvm.org/)).
- E para o `mygit`, a biblioteca [OpenSSL](https://openssl-library.org/).

Para compilar e executar, faça:

> Considere:
> - `<nome da cópia>`: qual cópia deseja executar.
> - `<parâmetros>`: os parâmetros para a cópia.

``` bash
cmake --workflow --preset dev
./build/debug/bin/<nome da cópia> <parâmetros>
```

Ou usando o script `./tools/run.sh`:

``` bash
./tools/run.sh run <nome da cópia> <parâmetros>
```

## Explicações

Decidi focar **apenas na parte da manipulação de arquivos** ao longo do projeto, por isso espere uma qualidade menor nas outras áreas do código. Abaixo, listo alguns projetos que possuem uma melhor organização:

- [c-algorithms](https://github.com/nicosobreira/c-algorithms): Algoritmos e Estruturas de Dados.
- [c-matrix](https://github.com/nicosobreira/c-matrix): Biblioteca de Álgebra Linear.
- [Typetest](https://github.com/nicosobreira/Typetest): Jogo de digitação no terminal.

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

Em `cat.c`, separamos o fluxo do programa em dois: com a opção `-n` vamos para `with_numbers.c`, caso contrário, para `normal.c`.

#### `normal.c`

> Exemplo mais simples de leitura de arquivos em C.

[Código](#código-de-cat)

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

A documentação do [`fopen`](https://devdocs.io/c/io/fopen) diz que, em caso de **falha**, a função retornará um ponteiro `NULL`, e por isso precisamos agir adequadamente e abrir o próximo arquivo, se houver.

Depois vamos para a função `output_file`, que irá copiar o conteúdo do recém-aberto `file` para `stream`; como estamos exibindo o resultado para o terminal, passamos `stdout` para o parâmetro `stream`, e por isso o conteúdo será acrescentado (*append*) a `stdout`. A função é definida da seguinte forma:

``` c
static inline void output_file(FILE *file, FILE *stream)
{
    int c;
    while ((c = fgetc(file)) != EOF)
    {
        int did_write = fputc(c, stream);

        if (did_write == EOF && ferror(stream))
        {
            LOG_ERROR("I/O error while writing to stream");
            return;
        }
    }

    if (ferror(file))
    {
        LOG_ERROR("I/O error while reading from file");
    }
}
```

O *while loop* copia cada caractere `c` de `file` para `stream`, onde `fgetc` pega o caractere e `fputc` o escreve em `stream`. As documentações das funções [`fgetc`](https://devdocs.io/c/io/fgetc) e [`fputc`](https://devdocs.io/c/io/fputc) dizem que, caso o arquivo tenha acabado ou algum erro tenha acontecido, o valor retornado será `EOF`, sendo que a diferenciação entre os dois casos é feita pelas funções [`feof`](https://devdocs.io/c/io/feof) e [`ferror`](https://devdocs.io/c/io/ferror), respectivamente. Como nesse exemplo o `EOF` indica que podemos sair da função, temos apenas que tratar os erros.

Por fim, temos que fechar o arquivo `file` que foi aberto em `show_files` com o `fclose`.

#### `with_numbers.c`

> Lendo linha por linha.

[Código](#código-de-cat)

As funções `show_files_number` e `show_file_number` fazem a mesma coisa que `show_files` no exemplo anterior; a diferença está em `output_file_number`.

``` c
static inline int output_file_number(FILE *file, FILE *stream, int total_lines)
{
    size_t buffer_size = BUFFER_SIZE;
    char *buffer = malloc(buffer_size);
    if (buffer == NULL)
    {
        LOG_ERROR("Failed to allocate memory");
        return total_lines;
    }

    while (get_line(&buffer, &buffer_size, file) == 0)
    {
        fprintf(stream, "%8d  ", total_lines);
        fputs(buffer, stream);

        total_lines += 1;
    }

    free(buffer);

    return total_lines;
}
```

Como é preciso contar cada linha do arquivo, não basta copiar caractere por caractere: precisamos saber onde cada linha termina. Por isso, `output_file_number` aloca um `buffer` com `malloc` (verificando se a alocação não falhou) e chama `get_line` em um *while loop*. Cada chamada lê **uma** linha de `file` para o `buffer` e retorna `0` quando conseguiu. O tamanho atual do `buffer` fica na variável `buffer_size`, inicializada com `BUFFER_SIZE`: é por ela que `get_line` sabe quanto espaço já existe. Enquanto as chamadas têm sucesso, imprimimos o número da linha com `fprintf` e o conteúdo com `fputs`.

O formato `"%8d  "` reserva 8 colunas para o número, alinhado à direita, seguido de dois espaços; é esse alinhamento que imita a saída do `cat -n`. Depois de cada linha, incrementamos `total_lines`. Não precisamos limpar o `buffer` entre as chamadas, já que `get_line` sempre escreve o `'\0'` ao final da linha lida. Quando `get_line` avisa que não há mais linhas (ou que houve um erro), liberamos o `buffer` com `free` e devolvemos `total_lines`; assim, a numeração **continua** de onde parou quando passamos vários arquivos.

Agora vamos para `get_line`:

``` c
static inline int get_line(char **buffer, size_t *len, FILE *file)
{
    const char delimiter = '\n';

    size_t current_index = 0;

    int c;
    while ((c = fgetc(file)) != EOF)
    {
        // Keeps room for the current character and for the final '\0'
        if (current_index + 1 >= (*len))
        {
            *len += 1;

            char *new_buffer = realloc(*buffer, *len);
            if (new_buffer == NULL)
            {
                LOG_ERROR("Failed to allocate memory");
                return 2;
            }

            *buffer = new_buffer;
        }

        (*buffer)[current_index] = (char)c;
        current_index += 1;

        if (c == delimiter)
        {
            break;
        }
    }

    if (ferror(file))
    {
        LOG_ERROR("I/O error while reading file");
        return 2;
    }

    // Reached the end of the file without reading anything
    if (current_index == 0)
    {
        return 1;
    }

    // Need to add the '\0' at the end of buffer
    (*buffer)[current_index] = '\0';

    return 0;
}
```

A ideia é a mesma do `normal.c`: ler com `fgetc` até o `EOF`. A diferença é que, aqui, o loop também **para** quando encontra o delimitador `'\n'`, de modo que cada chamada leia exatamente uma linha.

Como não sabemos o tamanho da linha de antemão, o `buffer` precisa crescer conforme necessário. Sempre que não há espaço para o próximo caractere **e** para o `'\0'` final, aumentamos `*len` e chamamos `realloc`, que redimensiona o bloco de memória preservando o que já foi escrito. Guardamos o retorno em `new_buffer`, e não direto em `*buffer`, para não perder o bloco original caso `realloc` falhe e devolva `NULL`. Note que `len` e `buffer` são ponteiros (`size_t *` e `char **`): como os argumentos são copiados, é assim que `get_line` consegue atualizar o tamanho e o endereço do bloco que `output_file_number` enxerga, já que `realloc` pode movê-lo para outro lugar da memória.

Ao sair do loop, precisamos descobrir **por que** ele terminou, e é aí que `ferror` entra, como no exemplo anterior. A função retorna `2` se houve um erro de leitura (`ferror`), `1` se o arquivo acabou sem que nenhum caractere novo fosse lido e `0` se uma linha foi lida com sucesso. Isso permite que `output_file_number` use apenas `get_line(...) == 0` como condição do loop. Por fim, como `fputs` espera uma *string* de C, adicionamos o terminador `'\0'` ao final do `buffer`. Repare que a última linha de um arquivo sem `'\n'` final também é lida normalmente: o fim do arquivo só é reportado na chamada seguinte.

### Wc

[Código](#código-de-wc)

- **Estrutura do Projeto**:

```
./project/wc/
├── CMakeLists.txt
└── src
    ├── main.c    # Passando argumentos
    ├── wc.c      # Lógica do comando wc
    └── wc.h
```

- **Exemplos de Uso**:

Com um arquivo. Cada coluna indica, respectivamente, o número de linhas, palavras e bytes.

```
$ ./build/debug/bin/wc .envrc
   1     2    10  .envrc
   1     2    10    total
```

Sem argumentos, lendo da entrada padrão:

```
$ echo "use flake" | ./build/debug/bin/wc
   1     2    10
```

---

#### `count_file`

> Uma única passagem pelo arquivo para contar linhas, palavras e bytes.

Os três contadores ficam agrupados na estrutura `Data`. Assim, a função devolve todos os resultados de uma vez e `Data_Sum` consegue acumulá-los entre vários arquivos.

``` c
Data count_file(FILE *file)
{
    Data data = {0};
    int previous_c = fgetc(file);

    if (previous_c == EOF)
    {
        return data;
    }

    data.bytes = 1;

    int c;
    while ((c = fgetc(file)) != EOF)
    {
        if (c == '\n')
        {
            data.lines += 1;
        }

        if (is_character(previous_c) && is_delimiter(c))
        {
            data.words += 1;
        }

        data.bytes += 1;

        previous_c = c;
    }

    return data;
}
```

A função recebe um `FILE *` já aberto e não sabe se ele aponta para um arquivo em disco ou para o terminal. É isso que permite reutilizá-la tanto em `do_files` quanto em `do_stdin`.

O primeiro caractere é lido antes do laço para que `previous_c` já exista na primeira iteração, e por isso `bytes` começa em `1`. Se esse `fgetc` retornar `EOF`, o arquivo está vazio e saímos da função.

Dentro do laço, olhamos sempre para **dois** caracteres: o anterior e o atual. Cada `'\n'` incrementa `lines`; uma palavra só é contada quando um caractere comum (`is_character`) é seguido de um delimitador (`is_delimiter`). Em outras palavras, contamos o **fim** de cada palavra, não o início. Aqui, os delimitadores são o espaço e a quebra de linha.

#### `do_files`

``` c
void do_files(Parameters param, FILE *stream)
{
    Data total = {0};

    for (int i = 0; i < param.total; i++)
    {
        const char *file_path = param.args[i];
        FILE *file = fopen(file_path, "r");
        if (file == NULL)
        {
            LOG_ERROR("No such file \"%s\"", file_path);

            continue;
        }

        Data data = do_file_path(file, file_path, stream);

        Data_Sum(&total, data);

        fclose(file);
    }

    Data_Print(stream, total, "  total");
}
```

Para cada arquivo aberto, `do_file_path` conta e imprime a linha correspondente, o resultado é somado em `total` com `Data_Sum` e o arquivo é fechado com `fclose`. Ao final do laço, imprimimos o total acumulado.

#### Entrada padrão

Sem argumentos, `do_stdin` chama `do_file(stdin, stdout)`. Como `count_file` só depende de um `FILE *`, não existe código específico para esse caso. A única diferença está em `Data_Print`: ao receber `NULL` no lugar do nome do arquivo, ela omite essa coluna.

### Mygit

O `mygit` não está concluído: o comando `commit` ainda não faz nada e o cálculo do hash tem limitações conhecidas (veja [O que falta](#o-que-falta)). Ele está aqui para demonstrar a **escrita** de arquivos: criação de diretórios, anexação a arquivos existentes e cópia de conteúdo.

- **Estrutura do Projeto**:

```
./project/mygit/
├── CMakeLists.txt
├── app
│   ├── main.c                # Escolhe o comando pelo primeiro argumento
│   └── commands
│       ├── init.h            # Cria o repositório
│       ├── add.h             # Prepara arquivos para o commit
│       ├── hash_object.h     # Exibe o hash de um arquivo
│       └── commit.h          # Ainda não implementado
├── include/mygit
│   ├── directory.h           # Caminhos e criação de diretórios
│   ├── hash.h
│   └── stage.h               # Escrita dos objetos e do index
└── src/hash                  # Cálculo do hash (OpenSSL)
```

- **Exemplos de Uso**:

```
$ ./build/debug/bin/mygit init
The folder .mygit was created!
$ ./build/debug/bin/mygit add main.c
```

O resultado fica no disco assim:

```
.mygit/
├── index                     # Lista dos arquivos preparados
└── objects
    └── ab                    # Dois primeiros caracteres do hash
        └── cdef...           # Restante do hash: o objeto em si
```

---

#### Git: o mínimo necessário

Para entender o que o programa escreve, precisamos de apenas quatro ideias do Git:

- **Repositório**: uma pasta oculta (`.git` no Git original, `.mygit` aqui) onde tudo é guardado. O comando `init` a cria.
- **Objeto *blob***: uma cópia do conteúdo de um arquivo, cujo nome é o **hash** do conteúdo. Dois arquivos com o mesmo conteúdo geram o mesmo nome, e por isso não são guardados duas vezes. Os dois primeiros caracteres do hash viram uma subpasta, para que `objects` não acumule milhares de arquivos em um único lugar.
- ***Index***: a lista do que será incluído no próximo *commit*. No Git original ele é um arquivo binário; aqui, para simplificar, é um arquivo de texto com uma linha por arquivo.
- **`add`**: grava o objeto em `objects` **e** registra uma linha no `index`.

O *commit* depende de todas essas partes, mas ainda não foi implementado.

#### `init`

> Criando os diretórios.

O comando `init` precisa criar duas pastas: `.mygit` e, dentro dela, `objects`. Como a função de criação de diretórios muda de acordo com o sistema operacional (`mkdir` em sistemas POSIX e `_mkdir` no Windows), `directory.h` esconde essa diferença atrás da macro `CREATE_DIR`.

Também em `directory.h`, o caminho de `objects` é montado a partir de `GIT_DIR_PATH`. Duas *strings* literais lado a lado são concatenadas pelo compilador, então `OBJECT_DIR_PATH` vale `"./.mygit/objects"` e, se o nome do repositório mudar, o ajuste é feito em um único lugar.

#### `copy_file`

> Escrevendo o conteúdo do arquivo em outro, em blocos.

No Cat, copiamos o conteúdo caractere por caractere com `fgetc` e `fputc`. Aqui o objetivo é gravar arquivos inteiros, então usamos [`fread`](https://devdocs.io/c/io/fread) e [`fwrite`](https://devdocs.io/c/io/fwrite), que operam sobre **blocos** de bytes: cada chamada lê até `sizeof(buffer)` bytes (`BUFFER_SIZE_LARGE`, 4096) de `src` e os grava em `dst`, em vez de uma chamada por caractere.

``` c
while ((bytes = fread(buffer, sizeof(char), sizeof(buffer), src)) > 0)
{
    if (fwrite(buffer, sizeof(char), bytes, dst) != bytes)
    {
        // ...
    }
}
```

Ambas retornam o **número de itens** transferidos, e não um código de erro. Por isso gravamos exatamente `bytes` itens, e não `sizeof(buffer)`: o último bloco de um arquivo quase nunca está cheio. Pelo mesmo motivo, comparamos o retorno de `fwrite` com `bytes`: um valor menor indica que a escrita falhou ou foi parcial. O laço termina quando `fread` retorna `0`, o que acontece tanto no fim do arquivo quanto em um erro de leitura. A diferenciação é feita depois, com `ferror(src)`, como já fizemos no Cat.

O comentário acima da função (`dst have to be in append mode!`) registra uma exigência: `dst` deve ter sido aberto em modo de anexação, que é o que `save_blob` faz.

#### `save_blob`

``` c
const char dir[2 + 1] = {hash[0], hash[1], '\0'};

char buffer[BUFFER_SIZE];
snprintf(buffer, sizeof(buffer), "%s/%s", OBJECT_DIR_PATH, dir);

CREATE_DIR(buffer);
```

Primeiro separamos os dois primeiros caracteres do hash em `dir`, com o `'\0'` ao final para formar uma *string* válida. Depois `snprintf` monta `./.mygit/objects/ab` em `buffer`. Aqui, o retorno de `CREATE_DIR` é ignorado de propósito: se a subpasta já existe, porque outro objeto com o mesmo prefixo foi guardado antes, isso não é um erro.

Em seguida, o restante do hash (`hash + 2`) é anexado ao caminho com um segundo `snprintf`, escrevendo a partir do fim da *string* atual, em `buffer + strlen(buffer)`. O tamanho restante é calculado com `sizeof(buffer) - strlen(buffer)` para que a escrita nunca ultrapasse o `buffer`.

Por fim, abrimos o arquivo de saída com `fopen(buffer, "a")`. O modo `"a"` (*append*) cria o arquivo se ele não existir e posiciona a escrita sempre no fim. Caso `fopen` retorne `NULL`, reportamos o erro e saímos. Se o arquivo foi aberto, chamamos `copy_file` e fechamos com `fclose` **antes** de verificar o resultado, para que o arquivo seja liberado mesmo em caso de falha.

#### `cmd_add` e o index

> Registrando o que foi preparado.

``` c
char buffer[BUFFER_SIZE];
snprintf(buffer, sizeof(buffer), "%s/index", GIT_DIR_PATH);
FILE *index = fopen(buffer, "a");
```

O `index` é aberto **uma única vez**, antes do laço, e compartilhado por todos os arquivos passados na linha de comando. O modo `"a"` é o que torna isso possível: cada execução de `add` acrescenta linhas ao final, sem apagar o que já havia.

Para cada arquivo, `cmd_add` o abre para leitura e chama `stage_file`, que calcula o hash, grava o objeto com `save_blob` e, se tudo der certo, registra uma linha no `index`:

``` c
fprintf(index, "blob %.*s\n", (int)sizeof(hash), hash);
```

O formato `%.*s` recebe a quantidade de caracteres como argumento (`(int)sizeof(hash)`, ou seja, 64). Isso é necessário porque `hash` é um vetor de `char` **sem `'\0'` ao final**: sem o limite explícito, `fprintf` continuaria lendo além do vetor. O mesmo formato é usado em `save_blob`. Antes de copiar o arquivo, `stage_file` usa `rewind` para voltá-lo ao início: o cálculo do hash já o percorreu e, sem isso, `copy_file` começaria a ler do ponto em que o hash parou.

Se o `index` não puder ser aberto, `cmd_add` encerra com erro. Se `fopen` falhar para um arquivo, ou se `stage_file` falhar, o erro é reportado e o laço continua com o próximo, como em `show_files`. Ao final, `fclose(index)` descarrega o que ainda estava no *buffer* e libera o arquivo.

#### O que falta

- O `commit` apenas valida os argumentos; não grava nada.
- O hash considera apenas os primeiros `BUFFER_SIZE` bytes do arquivo (comentário `FIX` em `hash.c`). Arquivos maiores precisam ser lidos em blocos, como em `copy_file`.
- Se `copy_file` falhar no meio da escrita, o objeto parcial permanece no disco (comentário `FIX` em `stage.h`).
- O `add .` ainda não foi implementado.
- O *index* não evita linhas repetidas se o mesmo arquivo for adicionado duas vezes.

## Apêndice

### Código de [Cat](#cat)

- **`normal.c`**:

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
            LOG_ERROR("I/O error while writing to stream");
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

- **`with_numbers.c`**:

``` c
#include "with_numbers.h"

#include <stdio.h>
#include <stdlib.h>

#include "commom/buffer.h"
#include "commom/error.h"

static inline int get_line(char **buffer, size_t *len, FILE *file)
{
    const char delimiter = '\n';

    size_t current_index = 0;

    int c;
    while ((c = fgetc(file)) != EOF)
    {
        // Keeps room for the current character and for the final '\0'
        if (current_index + 1 >= (*len))
        {
            *len += 1;

            char *new_buffer = realloc(*buffer, *len);
            if (new_buffer == NULL)
            {
                LOG_ERROR("Failed to allocate memory");
                return 2;
            }

            *buffer = new_buffer;
        }

        (*buffer)[current_index] = (char)c;
        current_index += 1;

        if (c == delimiter)
        {
            break;
        }
    }

    if (ferror(file))
    {
        LOG_ERROR("I/O error while reading file");
        return 2;
    }

    // Reached the end of the file without reading anything
    if (current_index == 0)
    {
        return 1;
    }

    // Need to add the '\0' at the end of buffer
    (*buffer)[current_index] = '\0';

    return 0;
}

static inline int output_file_number(FILE *file, FILE *stream, int total_lines)
{
    size_t buffer_size = BUFFER_SIZE;
    char *buffer = malloc(buffer_size);
    if (buffer == NULL)
    {
        LOG_ERROR("Failed to allocate memory");
        return total_lines;
    }

    while (get_line(&buffer, &buffer_size, file) == 0)
    {
        fprintf(stream, "%8d  ", total_lines);
        fputs(buffer, stream);

        total_lines += 1;
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

### Código de [Wc](#wc)

``` c
#include "wc.h"

#include <stdbool.h>
#include <stdio.h>
#include <string.h>

#include "commom/error.h"

typedef struct Data
{
    int lines;
    int words;
    int bytes;
} Data;

void Data_Sum(Data *result, Data plus)
{
    result->lines += plus.lines;
    result->words += plus.words;
    result->bytes += plus.bytes;
}

static inline void wc_help(void)
{
    printf("Usage: wc [OPTION] [FILE(s)]\n"
           "Show the number of new lines, words and bytes.\n"
           "  --help    display this help page and exit\n");
}

void Data_Print(FILE *stream, Data data, const char *file_path)
{
    if (file_path == NULL)
    {
        fprintf(stream, "%4d  %4d  %4d\n", data.lines, data.words, data.bytes);
    }
    else
    {
        fprintf(stream, "%4d  %4d  %4d  %s\n", data.lines, data.words, data.bytes, file_path);
    }
}

static inline bool is_delimiter(int c)
{
    return c == ' ' || c == '\n';
}

static inline bool is_character(int c)
{
    return !is_delimiter(c);
}

Data count_file(FILE *file)
{
    Data data = {0};
    int previous_c = fgetc(file);

    if (previous_c == EOF)
    {
        return data;
    }

    data.bytes = 1;

    int c;
    while ((c = fgetc(file)) != EOF)
    {
        if (c == '\n')
        {
            data.lines += 1;
        }

        if (is_character(previous_c) && is_delimiter(c))
        {
            data.words += 1;
        }

        data.bytes += 1;

        previous_c = c;
    }

    return data;
}

Data do_file_path(FILE *file, const char *path, FILE *stream)
{
    Data data = count_file(file);

    Data_Print(stream, data, path);

    return data;
}

void do_file(FILE *file, FILE *stream)
{
    Data data = count_file(file);

    Data_Print(stream, data, NULL);
}

void do_files(Parameters param, FILE *stream)
{
    Data total = {0};

    for (int i = 0; i < param.total; i++)
    {
        const char *file_path = param.args[i];
        FILE *file = fopen(file_path, "r");
        if (file == NULL)
        {
            LOG_ERROR("No such file \"%s\"", file_path);

            continue;
        }

        Data data = do_file_path(file, file_path, stream);

        Data_Sum(&total, data);

        fclose(file);
    }

    Data_Print(stream, total, "  total");
}

void do_stdin(void)
{
    do_file(stdin, stdout);
}

void wc(Parameters *param)
{
    if (Parameters_IsEmpty(param))
    {
        do_stdin();

        return;
    }

    if (strcmp(param->args[0], "--help") == 0)
    {
        wc_help();

        return;
    }
    if (strcmp(param->args[0], "--bytes") == 0 || strcmp(param->args[0], "-c") == 0)
    {
        return;
    }
    if (strcmp(param->args[0], "--lines") == 0 || strcmp(param->args[0], "-l") == 0)
    {
        return;
    }
    if (strcmp(param->args[0], "--words") == 0 || strcmp(param->args[0], "-w") == 0)
    {
        return;
    }

    do_files(*param, stdout);
}
```

### Código de [Mygit](#mygit)

`init.h`:

``` c
#pragma once

#include "mygit/directory.h"

#include "commom/error.h"
#include "commom/parameters.h"

static inline int cmd_init(Parameters param)
{
    if (param.total != 0)
    {
        LOG_ERROR("Type no arguments, please.");
        return 1;
    }

    if (CREATE_DIR(GIT_DIR_PATH) != 0)
    {
        LOG_ERROR("This folder already exists!");
        return 1;
    }

    if (CREATE_DIR(OBJECT_DIR_PATH) != 0)
    {
        LOG_ERROR("This folder already exists!");
        return 1;
    }

    printf("The folder .mygit was created!\n");

    return 0;
}
```

`directory.h`:

``` c
#pragma once

#define GIT_DIR_PATH "./.mygit"
#define OBJECT_DIR_PATH \
    GIT_DIR_PATH "/"    \
                 "objects"

#ifdef _WIN32

#include <direct.h>
#define CREATE_DIR(path) _mkdir(path)

#else

#include <sys/stat.h>
#define CREATE_DIR(path) mkdir(path, 0755)

#endif
```

`stage.h`:

``` c
#pragma once

#include <stdio.h>
#include <string.h>

#include "commom/buffer.h"
#include "commom/error.h"
#include "mygit/directory.h"
#include "mygit/hash.h"

/* dst have to be in append mode! */
static inline int copy_file(FILE *src, FILE *dst)
{
    char buffer[BUFFER_SIZE_LARGE];

    size_t bytes;
    while ((bytes = fread(buffer, sizeof(char), sizeof(buffer), src)) > 0)
    {
        if (fwrite(buffer, sizeof(char), bytes, dst) != bytes)
        {
            LOG_ERROR("Error while writing the file.");
            // FIX: Clear all changes made by `fwrite`.
            return 1;
        }
    }

    if (ferror(src))
    {
        LOG_ERROR("Error on src.");
        return 1;
    }

    return 0;
}

static inline int save_blob(const char *hash, size_t len, FILE *src)
{
    const char dir[2 + 1] = {hash[0], hash[1], '\0'};

    char buffer[BUFFER_SIZE];
    snprintf(buffer, sizeof(buffer), "%s/%s", OBJECT_DIR_PATH, dir);

    CREATE_DIR(buffer);

    const char *out_file = hash + 2;

    snprintf(buffer + strlen(buffer), sizeof(buffer) - strlen(buffer), "/%.*s", (int)(len - 2), out_file);

    FILE *out = fopen(buffer, "a");
    if (out == NULL)
    {
        LOG_ERROR("Can't create file '%s'", buffer);
        return 1;
    }

    int result = copy_file(src, out);
    fclose(out);

    return result;
}

static inline int stage_file(FILE *index, FILE *file)
{
    char hash[HASH_LEN_HEX];
    if (hash_blob(hash, sizeof(hash), file) != 0)
    {
        LOG_ERROR("Couldn't hash the file");
        return 1;
    }

    // `hash_blob` already read the file, so go back to its beginning
    rewind(file);

    if (save_blob(hash, sizeof(hash), file) != 0)
    {
        return 1;
    }

    fprintf(index, "blob %.*s\n", (int)sizeof(hash), hash);

    return 0;
}
```

`add.h`:

``` c
#pragma once

#include <string.h>

#include "commom/error.h"
#include "commom/parameters.h"

#include "mygit/stage.h"

static inline int cmd_add(Parameters param)
{
    if (param.total == 0)
    {
        LOG_ERROR("Nothing specified, nothing added.");
        return 1;
    }

    if (strcmp(param.args[0], ".") == 0)
    {
        LOG_ERROR("Not implemented yet :(");
        return 2;
    }

    char buffer[BUFFER_SIZE];
    snprintf(buffer, sizeof(buffer), "%s/index", GIT_DIR_PATH);

    FILE *index = fopen(buffer, "a");
    if (index == NULL)
    {
        LOG_ERROR("Unable to open '%s'", buffer);
        return 1;
    }

    for (int i = 0; i < param.total; i++)
    {
        const char *file_name = param.args[i];
        FILE *file = fopen(file_name, "r");
        if (file == NULL)
        {
            LOG_ERROR("Unable to open '%s'", file_name);
            continue;
        }

        if (stage_file(index, file) != 0)
        {
            LOG_ERROR("Unable to stage file '%s'", file_name);
        }

        fclose(file);
    }

    fclose(index);

    return 0;
}
```
