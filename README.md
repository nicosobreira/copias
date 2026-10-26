# Copias de programas do Linux em C

Copia dos seguintes programas comuns no Linux:

- `cat`: concatena vários arquivos.
- `wc`: conta quantas letras, palavras e linhas o texto possui.
- `mygit`: clone simplificado do [Git](https://git-scm.com/).

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
