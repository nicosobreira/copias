#pragma once

#include <openssl/evp.h>
#include <stdio.h>

/* Por que HASH_LEN e HASH_LEN_HEX?
A função hash gera caracteres de 0 a 255 - incluindo os caracteres especiais.
Cada caracter possui 1 byte, ou seja, 8 bits, como representado a seguir:
    0000 0000
Podemos representar cada 4 bits como um hexadecimal, gerando uma hash de 64 bits no total.
*/

#define HASH_ALGO "SHA256"
#define HASH_LEN 32
#define HASH_LEN_HEX 64

int hash_blob(char *buffer, size_t len, FILE *file);
