#include <stdint.h>
#include <errno.h>
#include <string.h>

#include "idea.hpp"

int main(int argc, char **argv)
{
  uint8_t key_idea128bit[16] = {0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11};
  uint8_t block_plain[8] = "suiljex";
  uint8_t block_cipher[8];
  uint8_t block_ret[8];

  IdeaContext idea_ctx;

  IdeaInit(&idea_ctx, key_idea128bit, 16);

  IdeaEncryptBlock(&idea_ctx, block_plain, block_cipher);
  IdeaDecryptBlock(&idea_ctx, block_cipher, block_ret);
  
  return 0;
}
