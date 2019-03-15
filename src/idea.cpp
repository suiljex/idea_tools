#include "idea.hpp"

#include <string>
#include <vector>
#include <bitset>

#include "defines.hpp"

int RotateBits(std::bitset<KEY_LEN> &d_key, int i_number)
{
  d_key = d_key << i_number | d_key >> (d_key.size() - i_number);
  return 0;
}

//Опасная ф-я
int CopyBits(std::bitset<KEY_ROUND_LEN> &o_key, std::bitset<KEY_LEN> &i_key, int i_byte_pair_number)
{
  if ((i_byte_pair_number * BIT_BYTE_PAIR + BIT_BYTE_PAIR) > i_key.size())
  {
    return 1;
  }

  for (int i = 0; i < BIT_BYTE_PAIR; ++i)
  {
    o_key[i] = i_key[i + i_byte_pair_number * BIT_BYTE_PAIR];
  }
  return 0;
}

int GenerateRoundKeys(std::bitset<KEY_LEN> i_key)
{
  std::vector<std::bitset<KEY_ROUND_LEN>> round_keys;
  std::bitset<KEY_ROUND_LEN> temp_key;

  for (int key = 0; key < KEY_ROUND_AMM; ++key)
  {
    CopyBits(temp_key, i_key, key % 8);
    round_keys.push_back(temp_key);

    if ((key + 1) % 8 == 0)
    {
      RotateBits(i_key, 25);
    }
  }

  return 0;
}

