#ifndef IDEA_DEMO_HPP
#define IDEA_DEMO_HPP 1

#include <stddef.h>
#include <stdint.h>
#include <vector>

#include "defines.hpp"
#include "crypto.hpp"

typedef struct
{
  uint16_t keys_encryption[52];
  uint16_t keys_decryption[52];
} IdeaDemoContext;

typedef struct
{
  uint16_t block_input[4];
  //uint16_t block_out[4];
  uint16_t keys_used[6];
  uint16_t inner_data[14];
} IdeaDemoRoundData;

unsigned int IdeaDemoInit(IdeaDemoContext *context, const uint8_t *key, size_t keyLen);

std::vector<IdeaDemoRoundData> IdeaDemoEncryptBlock(IdeaDemoContext *context, const uint8_t *input, uint8_t *output);

std::vector<IdeaDemoRoundData> IdeaDemoDecryptBlock(IdeaDemoContext *context, const uint8_t *input, uint8_t *output);

#endif //IDEA_DEMO_HPP
