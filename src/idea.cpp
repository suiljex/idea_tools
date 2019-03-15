#include "idea.hpp"

#include "defines.hpp"

static uint16_t IdeaMul(uint16_t a, uint16_t b)
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

static uint16_t IdeaInv(uint16_t a)
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

error_t IdeaInit(IdeaContext *context, const uint8_t *key, size_t keyLen)
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
    dk[i] = IdeaInv(ek[48 - i]);

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

    dk[i + 3] = IdeaInv(ek[51 - i]);

    if(i < 48)
    {
      dk[i + 4] = ek[46 - i];
      dk[i + 5] = ek[47 - i];
    }
  }

  //No error to report
  return 0;
}

void IdeaEncryptBlock(IdeaContext *context, const uint8_t *input, uint8_t *output)
{
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
    //Apply a round
    a = IdeaMul(a, k[0]);
    b += k[1];
    c += k[2];
    d = IdeaMul(d, k[3]);

    e = a ^ c;
    f = b ^ d;

    e = IdeaMul(e, k[4]);
    f += e;
    f = IdeaMul(f, k[5]);
    e += f;

    a ^= f;
    d ^= e;
    e ^= b;
    f ^= c;

    b = f;
    c = e;

    //Advance current location in key schedule
    k += 6;
  }

  //The four 16-bit values produced at the end of the 8th encryption
  //round are combined with the last four of the 52 key sub-blocks
  a = IdeaMul(a, k[0]);
  c += k[1];
  b += k[2];
  d = IdeaMul(d, k[3]);

  //The resulting value is the ciphertext
  STORE16BE(a, output + 0);
  STORE16BE(c, output + 2);
  STORE16BE(b, output + 4);
  STORE16BE(d, output + 6);
}

void IdeaDecryptBlock(IdeaContext *context, const uint8_t *input, uint8_t *output)
{
  uint32_t i;
  uint16_t e;
  uint16_t f;
  uint16_t *k;

  //The ciphertext is divided into four 16-bit registers
  uint16_t a = LOAD16BE(input + 0);
  uint16_t b = LOAD16BE(input + 2);
  uint16_t c = LOAD16BE(input + 4);
  uint16_t d = LOAD16BE(input + 6);

  //Point to the key schedule
  k = context->keys_decryption;

  //The computational process used for decryption of the ciphertext is
  //essentially the same as that used for encryption of the plaintext
  for(i = 0; i < 8; i++)
  {
    //Apply a round
    a = IdeaMul(a, k[0]);
    b += k[1];
    c += k[2];
    d = IdeaMul(d, k[3]);

    e = a ^ c;
    f = b ^ d;

    e = IdeaMul(e, k[4]);
    f += e;
    f = IdeaMul(f, k[5]);
    e += f;

    a ^= f;
    d ^= e;
    e ^= b;
    f ^= c;

    b = f;
    c = e;

    //Advance current location in key schedule
    k += 6;
  }

  //The four 16-bit values produced at the end of the 8th encryption
  //round are combined with the last four of the 52 key sub-blocks
  a = IdeaMul(a, k[0]);
  c += k[1];
  b += k[2];
  d = IdeaMul(d, k[3]);

  //The resulting value is the plaintext
  STORE16BE(a, output + 0);
  STORE16BE(c, output + 2);
  STORE16BE(b, output + 4);
  STORE16BE(d, output + 6);
}
