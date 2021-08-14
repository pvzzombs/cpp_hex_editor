//C++ file
#include "sha256.hpp"

#ifdef _SHA256_HEADER_FILE_AC75F356_AFE1_4F4F_B2D6_10FC42BFA635_
#include <exception>
#include <sstream>
#include <vector>
#include <cstring>

//// let's define some things
//// to make it easy to use

std::string sha256::padStringZeroes(std::string Num, u32 padlength){
  u32 length = u32(Num.length());
  while(length < padlength){
    Num = "0" + Num;
    length++;
  }
  return Num;
}

std::string sha256::decToRadix(u32 num, u32 radix){
  std::string rem;
  u32 temp = 0;
  std::stringstream ss;
  std::string result = "";
  while(num > 0){
    temp = num % radix;
    result = mapping[temp] + result;
    num = num / radix;
  }
  if(result == "")
  {
    result = "0";
  }
  return result;
}

void sha256::resetH(){
  H[0] = 0x6a09e667;
  H[1] = 0xbb67ae85;
  H[2] = 0x3c6ef372;
  H[3] = 0xa54ff53a;
  H[4] = 0x510e527f;
  H[5] = 0x9b05688c;
  H[6] = 0x1f83d9ab;
  H[7] = 0x5be0cd19;
}

u64 sha256::mod(i64 a, i64 b){
  i64 temp = a % b;
  while(temp < 0){
    temp += b;
  }
  return (u64)(temp);
}

//all function needs below here
//needs to return uint32_t

u32 sha256::rotr(u32 n, u32 x){
  return (x >> n) | (x << (32 - n));
}

u32 sha256::ch(u32 x, u32 y, u32 z){
  return (x & y) ^ (~x & z);
}

u32 sha256::maj(u32 x, u32 y, u32 z){
  return (x & y) ^ (x & z) ^ (y & z);
}

u32 sha256::sigma0(u32 x){
  return ((x >> 2) | (x << 30)) ^ ((x >> 13) | (x << 19)) ^ ((x >> 22) | (x << 10));
  //return rotr(2,x) ^ rotr(13,x) ^ rotr(22,x);
}

u32 sha256::sigma1(u32 x){
  return ((x >> 6) | (x << 26)) ^ ((x >> 11) | (x << 21)) ^ ((x >> 25) | (x << 7));
  //return rotr(6,x) ^ rotr(11,x) ^ rotr(25,x);
}

u32 sha256::omega0(u32 x){
  return ((x >> 7) | (x << 25)) ^ ((x >> 18) | (x << 14)) ^ (x >> 3);
  //return rotr(7,x) ^ rotr(18,x) ^ (x >> 3);
}

u32 sha256::omega1(u32 x){
  return ((x >> 17) | (x << 15)) ^ ((x >> 19) | (x << 13)) ^ (x >> 10);
  //return rotr(17,x) ^ rotr(19,x) ^ (x >> 10);
}

////sha256 constructor
sha256::sha256(){
	char * _mapping = "0123456789abcdefghijklmnopqrstuvwxyz";
	memcpy(mapping, _mapping, strlen(_mapping));
	////this constructor initializes the sha256 hash function
	hash_error = false;

	u32 K_[] = {
    0x428a2f98, 0x71374491, 0xb5c0fbcf, 0xe9b5dba5, 0x3956c25b, 0x59f111f1, 0x923f82a4, 0xab1c5ed5,
    0xd807aa98, 0x12835b01, 0x243185be, 0x550c7dc3, 0x72be5d74, 0x80deb1fe, 0x9bdc06a7, 0xc19bf174,
    0xe49b69c1, 0xefbe4786, 0x0fc19dc6, 0x240ca1cc, 0x2de92c6f, 0x4a7484aa, 0x5cb0a9dc, 0x76f988da,
    0x983e5152, 0xa831c66d, 0xb00327c8, 0xbf597fc7, 0xc6e00bf3, 0xd5a79147, 0x06ca6351, 0x14292967,
    0x27b70a85, 0x2e1b2138, 0x4d2c6dfc, 0x53380d13, 0x650a7354, 0x766a0abb, 0x81c2c92e, 0x92722c85,
    0xa2bfe8a1, 0xa81a664b, 0xc24b8b70, 0xc76c51a3, 0xd192e819, 0xd6990624, 0xf40e3585, 0x106aa070,
    0x19a4c116, 0x1e376c08, 0x2748774c, 0x34b0bcb5, 0x391c0cb3, 0x4ed8aa4a, 0x5b9cca4f, 0x682e6ff3,
    0x748f82ee, 0x78a5636f, 0x84c87814, 0x8cc70208, 0x90befffa, 0xa4506ceb, 0xbef9a3f7, 0xc67178f2
  };

  for(u32 m=0; m<64; m++){
    K[m] = K_[m];
  }

  m_length_up = 0;
  blockOfBytes_count = 0;

  H_up[0] = 0x6a09e667;
  H_up[1] = 0xbb67ae85;
  H_up[2] = 0x3c6ef372;
  H_up[3] = 0xa54ff53a;
  H_up[4] = 0x510e527f;
  H_up[5] = 0x9b05688c;
  H_up[6] = 0x1f83d9ab;
  H_up[7] = 0x5be0cd19;
}
///================================================================================///
///                            SHA-256 IMPLEMENTATION                              ///
///================================================================================///

std::string sha256::hash(std::string input){
	////resetH
  resetH();
  ////the actual message
  std::vector<u8> message_blocks;
  ////storage of length of message
  u64 a_length = input.length();
  u64 m_length = a_length * 8;
  ////cout << m_length;

  ////convert characters into group
  ////of bytes
  std::vector<u8> blockOfBytes;
  for(u64 ia=0; ia<a_length; ia++){
    blockOfBytes.push_back(input[ia]);
  }
  ////this is known to be 8 bytes
  u8 blockOfLength[8];

  ////we store the 64 bit length
  ////into 8 bytes using big endian order
  for(u32 lb=0; lb < 8; lb++){
    blockOfLength[lb] = (m_length >> ((7-lb)*8)) & 255;
    ////cout << " " << (u32)blockOfLength[lb];
  }

  ////the number of zeroes between the message
  ////and the length in bytes
  std::vector<u8> blockOfZeroes;
  u64 numberOfBlockOfZeroes = (mod((448-(m_length+1)), 512) + 1) / 8;

  ////store the zeroes in bytes
  ////note that the first block of zero
  ////will contain 128
  ////see below
  for(u64 nb=0; nb<numberOfBlockOfZeroes; nb++){
    blockOfZeroes.push_back(0);
  }

  ////add 1 bit plus 7 zeroes
  blockOfZeroes[0] = 128;

  ////Now concatenate the message, zeros and length
  ////in bytes, respectively
  for(u32 ij = 0; ij < blockOfBytes.size(); ij++){
    message_blocks.push_back(blockOfBytes[ij]);
  }

  for(u32 ij = 0; ij < blockOfZeroes.size(); ij++){
    message_blocks.push_back(blockOfZeroes[ij]);
  }

  for(u32 ij = 0; ij < 8; ij++){
    message_blocks.push_back(blockOfLength[ij]);
  }

  ////should be a multiple of 64
  ////cout << "Sizeof message " << message_blocks.size() << endl;

  ////create the main message block which
  ////is the M block
  ////note that this a 2d array,
  ////where each item in M[] will contain
  ////x16 32bits which is the padded message
  std::vector<std::vector<u32>> M;
  ////this will be always 16 in .size()
  std::vector<u32> tempa;
  u32 message_blocks_length = message_blocks.size() * 8;
  u32 lastIndex = 0;

  ////a strategy for storing 32bits padded message
  ////inside a 2d array
  ////will be re-factored soon...
  for(u32 ib=0; ib<message_blocks_length; ib+=512){
    tempa.clear();
    for(u32 j=0; j<64; j+=4){
      u32 word = (message_blocks[lastIndex+j] << 24) | (message_blocks[lastIndex+j+1] << 16) | (message_blocks[lastIndex+j+2] << 8) | (message_blocks[lastIndex+j+3]);
      tempa.push_back(word);
    }
    lastIndex += 64;
    M.push_back(tempa);
  }

  ////cout << M[0].size() << endl;
  ////THE MAIN PROCEDURE

  u32 a=0,b=0,c=0,d=0,e=0,f=0,g=0,h=0,t1=0,t2 =0,M_length = M.size();

  ////THE MAIN LOOP
  for(u32 i = 0; i < M_length; i++){
    ////Initialize the working schedule
    ////This will use the M[], which is
    ////the 2d array being stored while
    ////extending it's content to 64,
    ////that means, W[] total size is 64
    std::vector<u32> W;
    u32 temp = 0;

    ////the working schedule loop
    for(u32 tb = 0; tb<64; tb++){
      if(tb < 16){
        W.push_back(M[i][tb]);
      }else{
        temp = (omega1(W[tb-2]) + W[tb-7] + omega0(W[tb-15]) + W[tb-16]);
        W.push_back(temp);
      }
    }

    ////initialize the 8 variables
    ////using the H[] table
    a = H[0];
    b = H[1];
    c = H[2];
    d = H[3];
    e = H[4];
    f = H[5];
    g = H[6];
    h = H[7];

    ////the compression function, which will be done
    ////64 times for each working schedule
    for(u32 t=0; t<64; t++){
      t1 = h + sigma1(e) + ch(e,f,g) + K[t] + W[t];
      t2 = sigma0(a) + maj(a,b,c);
      h = g;
      g = f;
      f = e;
      e = d + t1;
      d = c;
      c = b;
      b = a;
      a = t1 + t2;
    }

    ////modify the content of the H[] table
    H[0] = (a + H[0]);
    H[1] = (b + H[1]);
    H[2] = (c + H[2]);
    H[3] = (d + H[3]);
    H[4] = (e + H[4]);
    H[5] = (f + H[5]);
    H[6] = (g + H[6]);
    H[7] = (h + H[7]);
  }

  ////after  the main loop, output
  ////the contents of the H[] table
  std::string output = "";
  for(u32 ic=0; ic<8; ic++){
    std::string tempb = decToRadix(H[ic], 16);
    output += padStringZeroes(tempb, 8);
  }

  return output;
}

void sha256::process_block(){
  ////cout << "DEBUG - PROCESSING M BLOCK" << endl;
  u32 a=0,b=0,c=0,d=0,e=0,f=0,g=0,h=0,t1=0,t2 =0;
  u32 W[64];

  W[0] = (blockOfBytes_up[0] << 24) | (blockOfBytes_up[1] << 16) | (blockOfBytes_up[2] << 8) | (blockOfBytes_up[3]);
  W[1] = (blockOfBytes_up[4] << 24) | (blockOfBytes_up[5] << 16) | (blockOfBytes_up[6] << 8) | (blockOfBytes_up[7]);
  W[2] = (blockOfBytes_up[8] << 24) | (blockOfBytes_up[9] << 16) | (blockOfBytes_up[10] << 8) | (blockOfBytes_up[11]);
  W[3] = (blockOfBytes_up[12] << 24) | (blockOfBytes_up[13] << 16) | (blockOfBytes_up[14] << 8) | (blockOfBytes_up[15]);

  W[4] = (blockOfBytes_up[16] << 24) | (blockOfBytes_up[17] << 16) | (blockOfBytes_up[18] << 8) | (blockOfBytes_up[19]);
  W[5] = (blockOfBytes_up[20] << 24) | (blockOfBytes_up[21] << 16) | (blockOfBytes_up[22] << 8) | (blockOfBytes_up[23]);
  W[6] = (blockOfBytes_up[24] << 24) | (blockOfBytes_up[25] << 16) | (blockOfBytes_up[26] << 8) | (blockOfBytes_up[27]);
  W[7] = (blockOfBytes_up[28] << 24) | (blockOfBytes_up[29] << 16) | (blockOfBytes_up[30] << 8) | (blockOfBytes_up[31]);

  W[8] = (blockOfBytes_up[32] << 24) | (blockOfBytes_up[33] << 16) | (blockOfBytes_up[34] << 8) | (blockOfBytes_up[35]);
  W[9] = (blockOfBytes_up[36] << 24) | (blockOfBytes_up[37] << 16) | (blockOfBytes_up[38] << 8) | (blockOfBytes_up[39]);
  W[10] = (blockOfBytes_up[40] << 24) | (blockOfBytes_up[41] << 16) | (blockOfBytes_up[42] << 8) | (blockOfBytes_up[43]);
  W[11] = (blockOfBytes_up[44] << 24) | (blockOfBytes_up[45] << 16) | (blockOfBytes_up[46] << 8) | (blockOfBytes_up[47]);

  W[12] = (blockOfBytes_up[48] << 24) | (blockOfBytes_up[49] << 16) | (blockOfBytes_up[50] << 8) | (blockOfBytes_up[51]);
  W[13] = (blockOfBytes_up[52] << 24) | (blockOfBytes_up[53] << 16) | (blockOfBytes_up[54] << 8) | (blockOfBytes_up[55]);
  W[14] = (blockOfBytes_up[56] << 24) | (blockOfBytes_up[57] << 16) | (blockOfBytes_up[58] << 8) | (blockOfBytes_up[59]);
  W[15] = (blockOfBytes_up[60] << 24) | (blockOfBytes_up[61] << 16) | (blockOfBytes_up[62] << 8) | (blockOfBytes_up[63]);

  for(u32 tb = 16; tb<64; tb++){
    W[tb] = (omega1(W[tb-2]) + W[tb-7] + omega0(W[tb-15]) + W[tb-16]);
  }

  a = H_up[0];
  b = H_up[1];
  c = H_up[2];
  d = H_up[3];
  e = H_up[4];
  f = H_up[5];
  g = H_up[6];
  h = H_up[7];

  ////cout << "DEBUG - MAIN LOOP STARTED" << endl;

  for(u32 t=0; t<64; t++){
    t1 = h + sigma1(e) + ch(e,f,g) + K[t] + W[t];
    t2 = sigma0(a) + maj(a,b,c);
    h = g;
    g = f;
    f = e;
    e = d + t1;
    d = c;
    c = b;
    b = a;
    a = t1 + t2;
  }

  H_up[0] += a;
  H_up[1] += b;
  H_up[2] += c;
  H_up[3] += d;
  H_up[4] += e;
  H_up[5] += f;
  H_up[6] += g;
  H_up[7] += h;
}

void sha256::hash_update(std::string input){
  ////get the input
  u64 lx = input.length();
  for(u32 jx=0; jx<lx; jx++){
    blockOfBytes_up[blockOfBytes_count++] = input[jx];
    if(blockOfBytes_count == 64){
      process_block();
      blockOfBytes_count = 0;
    }
  }
  ////compute new length
  m_length_up = m_length_up + (lx * 8);
}

void sha256::hash_update(char input){
  blockOfBytes_up[blockOfBytes_count++] = input;
  if(blockOfBytes_count == 64){
    process_block();
    blockOfBytes_count = 0;
  }
  m_length_up += 8;
}

void sha256::hash_update_array(char input[], u32 input_length){
  try{
    for(u32 jx=0; jx<input_length; jx++){
      blockOfBytes_up[blockOfBytes_count++] = input[jx];
      if(blockOfBytes_count == 64){
        process_block();
        blockOfBytes_count = 0;
      }
    }

    m_length_up = m_length_up + (input_length * 8);
  }catch(std::exception& e){
    ////cerr << "An error occured while updating. Error type: " << e.what() << endl;
    hashResult_up = "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx";
    hash_error = true;

    //clean up
    //blockOfBytes_up.clear();
    blockOfBytes_count = 0;
    m_length_up = 0;
  }
}

void sha256::hash_update(char * input, u64 input_length){
  try{
    for(u64 jx=0; jx<input_length; jx++){
      blockOfBytes_up[blockOfBytes_count++] = input[jx];
      if(blockOfBytes_count == 64){
        process_block();
        blockOfBytes_count = 0;
      }
    }

    m_length_up = m_length_up + (input_length * 8);
  }catch(std::exception& e){
    ////cerr << "An error occured while updating. Error type: " << e.what() << endl;
    hashResult_up = "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx";
    hash_error = true;

    //clean up
        //blockOfBytes_up.clear();
    blockOfBytes_count = 0;
    m_length_up = 0;
  }
}

void sha256::hash_update(char * input, u64 input_length, void (*func)(u32 percentage)){
  u64 m = input_length / 100;
  m = (m > 0) ? m : 1;
  double ab = 0, cd = input_length;
  try{
    for(u64 jx=0; jx<input_length; jx++){
      blockOfBytes_up[blockOfBytes_count++] = input[jx];
      if(blockOfBytes_count == 64){
        process_block();
        blockOfBytes_count = 0;
      }
	  if( (jx+1) % m == 0 ){
		ab = (jx+1);
		ab = ab / cd * 100;
		(*func)((u32)ab);
	  }
    }

    m_length_up = m_length_up + (input_length * 8);
  }catch(std::exception& e){
    ////cerr << "An error occured while updating. Error type: " << e.what() << endl;
    hashResult_up = "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx";
    hash_error = true;

    //clean up
        //blockOfBytes_up.clear();
    blockOfBytes_count = 0;
    m_length_up = 0;
  }
}

void sha256::hash_finalize(){
  if(hash_error){
    hash_error = false;
    ////cerr << "DEBUG - ERROR HASHING" << endl;
    return ;
  }
  ////cout << "DEBUG - FINALIZING" << endl;
  u32 end_count = (blockOfBytes_count < 56) ? 56 : 64;
  u32 start_zero = blockOfBytes_count;
  ////cout << "Start of zero is " << start_zero << endl;

  ////cout << "Block of bytes is " << start_zero << endl;
  //START OF PADDING ZEROES
  blockOfBytes_up[start_zero] = 128;

  for(u32 sz = (start_zero + 1); sz < end_count; sz++){
    blockOfBytes_up[sz] = 0;
  }
  //END OF PADDING

  if(blockOfBytes_count >= 56){
    ////cout << "Verifying" << endl;
    process_block();
    //CLEANING LEFT OVER
    //This is only done if blockOfBytes_count < 56
    //or starting from index 0 to 55
    for(u32 ch=0; ch < 56; ch++){
      blockOfBytes_up[ch] = 0;
    }
    //END OF CLEANING LEFT OVER
  }

  blockOfBytes_up[56] = (m_length_up >> 56) & 255;
  blockOfBytes_up[57] = (m_length_up >> 48) & 255;
  blockOfBytes_up[58] = (m_length_up >> 40) & 255;
  blockOfBytes_up[59] = (m_length_up >> 32) & 255;

  blockOfBytes_up[60] = (m_length_up >> 24) & 255;
  blockOfBytes_up[61] = (m_length_up >> 16) & 255;
  blockOfBytes_up[62] = (m_length_up >> 8) & 255;
  blockOfBytes_up[63] = m_length_up & 255;

  process_block();


  ////cout << "DEBUG - COLLECTING HASH STARTED" << endl;
  std::string tempb;
  hashResult_up = "";

  tempb = decToRadix(H_up[0], 16);
  hashResult_up += padStringZeroes(tempb, 8);

  tempb = decToRadix(H_up[1], 16);
  hashResult_up += padStringZeroes(tempb, 8);

  tempb = decToRadix(H_up[2], 16);
  hashResult_up += padStringZeroes(tempb, 8);

  tempb = decToRadix(H_up[3], 16);
  hashResult_up += padStringZeroes(tempb, 8);

  tempb = decToRadix(H_up[4], 16);
  hashResult_up += padStringZeroes(tempb, 8);

  tempb = decToRadix(H_up[5], 16);
  hashResult_up += padStringZeroes(tempb, 8);

  tempb = decToRadix(H_up[6], 16);
  hashResult_up += padStringZeroes(tempb, 8);

  tempb = decToRadix(H_up[7], 16);
  hashResult_up += padStringZeroes(tempb, 8);

  ////cout << "DEBUG - HASHING DONE" << endl;
  //reset H_tables
  H_up[0] = 0x6a09e667;
  H_up[1] = 0xbb67ae85;
  H_up[2] = 0x3c6ef372;
  H_up[3] = 0xa54ff53a;
  H_up[4] = 0x510e527f;
  H_up[5] = 0x9b05688c;
  H_up[6] = 0x1f83d9ab;
  H_up[7] = 0x5be0cd19;
  //reset blockOfbytes
  ////cout << "DEBUG - PERFORMING CLEAN UP" << endl;
  //blockOfBytes_up.clear();
  blockOfBytes_count = 0;
  m_length_up = 0;
}

std::string sha256::hash_result(){
  return hashResult_up;
}

#endif //_SHA256_HEADER_FILE_AC75F356_AFE1_4F4F_B2D6_10FC42BFA635_
