#ifndef IDEA_HPP
#define IDEA_HPP 1

#include <string>
#include <vector>
#include <bitset>

#include "defines.hpp"

int RotateBits(std::bitset<KEY_LEN>& d_key, int i_number);

int CopyBits(std::bitset<KEY_ROUND_LEN>& o_key, std::bitset<KEY_LEN>& i_key, int i_byte_pair_number);

int GenerateRoundKeys(std::bitset<KEY_LEN> i_key);

int RoundEncrypt(std::bitset<BLOCK_LEN>& d_data
                 , std::vector<std::bitset<KEY_ROUND_LEN>> i_round_keys)
{

  return 0;
}

#endif //IDEA_HPP
