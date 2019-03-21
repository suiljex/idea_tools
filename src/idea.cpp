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

unsigned int IdeaInit(IdeaContext *context, const uint8_t *key, size_t keyLen)
{
  uint32_t i;
  uint16_t *ek;
  uint16_t *dk;

  //Проверка длины ключа
  if(keyLen != 16)
  {
    return 1;
  }

  //Получение указателей на ключи раундов
  ek = context->keys_encryption;
  dk = context->keys_decryption;

  //Деление 128 битного ключа на блоки по 16 бит
  for(i = 0; i < 8; i++)
  {
    ek[i] = LOAD16BE(key + i * 2);
  }

  //Получение ключей раундов для шифрования
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

  //Получение ключей раундов для дешифрования
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

  //Успешное завершение
  return 0;
}

void IdeaEncryptBlock(IdeaContext *context, const uint8_t *input, uint8_t *output)
{
  uint32_t i;
  uint16_t e;
  uint16_t f;
  uint16_t *k;

  //Текст делится на четыре 16 битные части
  uint16_t a = LOAD16BE(input + 0);
  uint16_t b = LOAD16BE(input + 2);
  uint16_t c = LOAD16BE(input + 4);
  uint16_t d = LOAD16BE(input + 6);

  //Указатель на ключи раундов шифрования
  k = context->keys_encryption;

  //Процесс шифрования состоит из 8 одинаковых частей
  for(i = 0; i < 8; i++)
  {
    //Провести раунд
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

    //Продвижение по списку ключей
    k += 6;
  }

  //Четыре 16 битных значения комбинируются с последними 4 ключами
  a = IdeaMul(a, k[0]);
  c += k[1];
  b += k[2];
  d = IdeaMul(d, k[3]);

  //Полученное значение является шифрованным текстом
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

  //Текст делится на четыре 16 битные части
  uint16_t a = LOAD16BE(input + 0);
  uint16_t b = LOAD16BE(input + 2);
  uint16_t c = LOAD16BE(input + 4);
  uint16_t d = LOAD16BE(input + 6);

  //Указатель на ключи раундов дешифрования
  k = context->keys_decryption;

  //Процесс шифрования состоит из 8 одинаковых частей
  for(i = 0; i < 8; i++)
  {
    //Провести раунд
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

    //Продвижение по списку ключей
    k += 6;
  }

  //Четыре 16 битных значения комбинируются с последними 4 ключами
  a = IdeaMul(a, k[0]);
  c += k[1];
  b += k[2];
  d = IdeaMul(d, k[3]);

  //Полученное значение является дешифрованным текстом
  STORE16BE(a, output + 0);
  STORE16BE(c, output + 2);
  STORE16BE(b, output + 4);
  STORE16BE(d, output + 6);
}
