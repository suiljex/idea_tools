#include "idea_demo.hpp"

#include "defines.hpp"

static uint16_t IdeaDemoMul(uint16_t a, uint16_t b)
{
  uint32_t c = a * b;

  if(c)
  {
    c = (ROL32(c, 16) - c) >> 16;
    return (c + 1) & 0xFFFF;
  }
  else
  {
    return (1 - a - b) & 0xFFFF;
  }
}

static uint16_t IdeaDemoInv(uint16_t a)
{
  uint32_t b;
  uint32_t q;
  uint32_t r;
  int32_t t;
  int32_t u;
  int32_t v;

  b = 0x10001;
  u = 0;
  v = 1;

  while(a > 0)
  {
    q = b / a;
    r = b % a;

    b = a;
    a = r;

    t = v;
    v = u - q * v;
    u = t;
  }

  if(u < 0)
  {
    u += 0x10001;
  }

  return u;
}

unsigned int IdeaDemoInit(IdeaDemoContext *context, const uint8_t *key, size_t keyLen)
{
  uint32_t i;
  uint16_t *ek;
  uint16_t *dk;

  //Invalid key length?
  if(keyLen != 16)
  {
    return 1;
  }

  //Point to the encryption and decryption subkeys
  ek = context->keys_encryption;
  dk = context->keys_decryption;

  //First, the 128-bit key is partitioned into eight 16-bit sub-blocks
  for(i = 0; i < 8; i++)
  {
    ek[i] = LOAD16BE(key + i * 2);
  }

  //Expand encryption subkeys
  for(i = 8; i < 52; i++)
  {
    if((i % 8) == 6)
    {
      ek[i] = (ek[i - 7] << 9) | (ek[i - 14] >> 7);
    }
    else if((i % 8) == 7)
    {
      ek[i] = (ek[i - 15] << 9) | (ek[i - 14] >> 7);
    }
    else
    {
      ek[i] = (ek[i - 7] << 9) | (ek[i - 6] >> 7);
    }
  }

  //Generate subkeys for decryption
  for(i = 0; i < 52; i += 6)
  {
    dk[i] = IdeaDemoInv(ek[48 - i]);

    if(i == 0 || i == 48)
    {
      dk[i + 1] = -ek[49 - i];
      dk[i + 2] = -ek[50 - i];
    }
    else
    {
      dk[i + 1] = -ek[50 - i];
      dk[i + 2] = -ek[49 - i];
    }

    dk[i + 3] = IdeaDemoInv(ek[51 - i]);

    if(i < 48)
    {
      dk[i + 4] = ek[46 - i];
      dk[i + 5] = ek[47 - i];
    }
  }

  //No error to report
  return 0;
}

std::vector<IdeaDemoRoundData> IdeaDemoEncryptBlock(IdeaDemoContext *context, const uint8_t *input, uint8_t *output)
{
  std::vector<IdeaDemoRoundData> ret_rounds_data;
  IdeaDemoRoundData temp_round_data;

  uint32_t i;
  uint16_t e;
  uint16_t f;
  uint16_t *k;

  //The plaintext is divided into four 16-bit registers
  uint16_t a = LOAD16BE(input + 0);
  uint16_t b = LOAD16BE(input + 2);
  uint16_t c = LOAD16BE(input + 4);
  uint16_t d = LOAD16BE(input + 6);

  //Point to the key schedule
  k = context->keys_encryption;

  //The process consists of eight identical encryption steps
  for(i = 0; i < 8; i++)
  {
    temp_round_data.round_number = i;

    temp_round_data.block_input[0] = a;
    temp_round_data.block_input[1] = b;
    temp_round_data.block_input[2] = c;
    temp_round_data.block_input[3] = d;

    temp_round_data.keys_used[0] = k[0];
    temp_round_data.keys_used[1] = k[1];
    temp_round_data.keys_used[2] = k[2];
    temp_round_data.keys_used[3] = k[3];
    temp_round_data.keys_used[4] = k[4];
    temp_round_data.keys_used[5] = k[5];

    //Apply a round
    a = IdeaDemoMul(a, k[0]);
    temp_round_data.inner_data[0] = a;
    b += k[1];
    temp_round_data.inner_data[1] = b;
    c += k[2];
    temp_round_data.inner_data[2] = c;
    d = IdeaDemoMul(d, k[3]);
    temp_round_data.inner_data[3] = d;

    e = a ^ c;
    temp_round_data.inner_data[4] = e;
    f = b ^ d;
    temp_round_data.inner_data[5] = f;

    e = IdeaDemoMul(e, k[4]);
    temp_round_data.inner_data[6] = e;
    f += e;
    temp_round_data.inner_data[7] = f;
    f = IdeaDemoMul(f, k[5]);
    temp_round_data.inner_data[8] = f;
    e += f;
    temp_round_data.inner_data[9] = e;

    a ^= f;
    temp_round_data.inner_data[10] = a;
    d ^= e;
    temp_round_data.inner_data[13] = d;
    e ^= b;
    temp_round_data.inner_data[12] = e;
    f ^= c;
    temp_round_data.inner_data[11] = f;

    b = f;
    c = e;

    //Advance current location in key schedule
    k += 6;

    ret_rounds_data.push_back(temp_round_data);
  }

  temp_round_data.round_number = i;

  temp_round_data.block_input[0] = a;
  temp_round_data.block_input[1] = b;
  temp_round_data.block_input[2] = c;
  temp_round_data.block_input[3] = d;

  temp_round_data.keys_used[0] = k[0];
  temp_round_data.keys_used[1] = k[1];
  temp_round_data.keys_used[2] = k[2];
  temp_round_data.keys_used[3] = k[3];
  temp_round_data.keys_used[4] = 0x0000;
  temp_round_data.keys_used[5] = 0x0000;

  //The four 16-bit values produced at the end of the 8th encryption
  //round are combined with the last four of the 52 key sub-blocks
  a = IdeaDemoMul(a, k[0]);
  temp_round_data.inner_data[0] = a;
  c += k[1];
  temp_round_data.inner_data[2] = c;
  b += k[2];
  temp_round_data.inner_data[1] = b;
  d = IdeaDemoMul(d, k[3]);
  temp_round_data.inner_data[3] = d;

  temp_round_data.inner_data[4] = 0x0000;
  temp_round_data.inner_data[5] = 0x0000;
  temp_round_data.inner_data[6] = 0x0000;
  temp_round_data.inner_data[7] = 0x0000;
  temp_round_data.inner_data[8] = 0x0000;
  temp_round_data.inner_data[9] = 0x0000;
  temp_round_data.inner_data[10] = 0x0000;
  temp_round_data.inner_data[11] = 0x0000;
  temp_round_data.inner_data[12] = 0x0000;
  temp_round_data.inner_data[13] = 0x0000;

  ret_rounds_data.push_back(temp_round_data);

  //The resulting value is the ciphertext
  STORE16BE(a, output + 0);
  STORE16BE(c, output + 2);
  STORE16BE(b, output + 4);
  STORE16BE(d, output + 6);

  return ret_rounds_data;
}

std::vector<IdeaDemoRoundData> IdeaDemoDecryptBlock(IdeaDemoContext *context, const uint8_t *input, uint8_t *output)
{
  std::vector<IdeaDemoRoundData> ret_rounds_data;
  IdeaDemoRoundData temp_round_data;

  uint32_t i;
  uint16_t e;
  uint16_t f;
  uint16_t *k;

  //The plaintext is divided into four 16-bit registers
  uint16_t a = LOAD16BE(input + 0);
  uint16_t b = LOAD16BE(input + 2);
  uint16_t c = LOAD16BE(input + 4);
  uint16_t d = LOAD16BE(input + 6);

  //Point to the key schedule
  k = context->keys_decryption;

  //The process consists of eight identical encryption steps
  for(i = 0; i < 8; i++)
  {
    temp_round_data.round_number = i;

    temp_round_data.block_input[0] = a;
    temp_round_data.block_input[1] = b;
    temp_round_data.block_input[2] = c;
    temp_round_data.block_input[3] = d;

    temp_round_data.keys_used[0] = k[0];
    temp_round_data.keys_used[1] = k[1];
    temp_round_data.keys_used[2] = k[2];
    temp_round_data.keys_used[3] = k[3];
    temp_round_data.keys_used[4] = k[4];
    temp_round_data.keys_used[5] = k[5];

    //Apply a round
    a = IdeaDemoMul(a, k[0]);
    temp_round_data.inner_data[0] = a;
    b += k[1];
    temp_round_data.inner_data[1] = b;
    c += k[2];
    temp_round_data.inner_data[2] = c;
    d = IdeaDemoMul(d, k[3]);
    temp_round_data.inner_data[3] = d;

    e = a ^ c;
    temp_round_data.inner_data[4] = e;
    f = b ^ d;
    temp_round_data.inner_data[5] = f;

    e = IdeaDemoMul(e, k[4]);
    temp_round_data.inner_data[6] = e;
    f += e;
    temp_round_data.inner_data[7] = f;
    f = IdeaDemoMul(f, k[5]);
    temp_round_data.inner_data[8] = f;
    e += f;
    temp_round_data.inner_data[9] = e;

    a ^= f;
    temp_round_data.inner_data[10] = a;
    d ^= e;
    temp_round_data.inner_data[13] = d;
    e ^= b;
    temp_round_data.inner_data[12] = e;
    f ^= c;
    temp_round_data.inner_data[11] = f;

    b = f;
    c = e;

    //Advance current location in key schedule
    k += 6;

    ret_rounds_data.push_back(temp_round_data);
  }

  temp_round_data.round_number = i;

  temp_round_data.block_input[0] = a;
  temp_round_data.block_input[1] = b;
  temp_round_data.block_input[2] = c;
  temp_round_data.block_input[3] = d;

  temp_round_data.keys_used[0] = k[0];
  temp_round_data.keys_used[1] = k[1];
  temp_round_data.keys_used[2] = k[2];
  temp_round_data.keys_used[3] = k[3];
  temp_round_data.keys_used[4] = 0x0000;
  temp_round_data.keys_used[5] = 0x0000;

  //The four 16-bit values produced at the end of the 8th encryption
  //round are combined with the last four of the 52 key sub-blocks
  a = IdeaDemoMul(a, k[0]);
  temp_round_data.inner_data[0] = a;
  c += k[1];
  temp_round_data.inner_data[2] = c;
  b += k[2];
  temp_round_data.inner_data[1] = b;
  d = IdeaDemoMul(d, k[3]);
  temp_round_data.inner_data[3] = d;

  temp_round_data.inner_data[4] = 0x0000;
  temp_round_data.inner_data[5] = 0x0000;
  temp_round_data.inner_data[6] = 0x0000;
  temp_round_data.inner_data[7] = 0x0000;
  temp_round_data.inner_data[8] = 0x0000;
  temp_round_data.inner_data[9] = 0x0000;
  temp_round_data.inner_data[10] = 0x0000;
  temp_round_data.inner_data[11] = 0x0000;
  temp_round_data.inner_data[12] = 0x0000;
  temp_round_data.inner_data[13] = 0x0000;

  ret_rounds_data.push_back(temp_round_data);

  //The resulting value is the ciphertext
  STORE16BE(a, output + 0);
  STORE16BE(c, output + 2);
  STORE16BE(b, output + 4);
  STORE16BE(d, output + 6);

  return ret_rounds_data;
}
