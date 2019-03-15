#ifndef IDEA_HPP
#define IDEA_HPP 1

#include <stddef.h>
#include <errno.h>
#include <stdint.h>

#include "defines.hpp"
#include "crypto.hpp"

typedef struct
{
   uint16_t keys_encryption[52];
   uint16_t keys_decryption[52];
} IdeaContext;

error_t IdeaInit(IdeaContext *context, const uint8_t *key, size_t keyLen);

void IdeaEncryptBlock(IdeaContext *context, const uint8_t *input, uint8_t *output);

void IdeaDecryptBlock(IdeaContext *context, const uint8_t *input, uint8_t *output);

#endif //IDEA_HPP
