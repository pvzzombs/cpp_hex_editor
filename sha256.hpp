//C++ header file
#ifndef _SHA256_HEADER_FILE_AC75F356_AFE1_4F4F_B2D6_10FC42BFA635_
#define _SHA256_HEADER_FILE_AC75F356_AFE1_4F4F_B2D6_10FC42BFA635_

#include <iostream>
#include <stdint.h>

#ifdef MEMLEAK
#define _CRTDBG_MAP_ALLOC 1
#include <stdlib.h>
#include <crtdbg.h>
#endif //MEMLEAK

//// let's define some things
//// to make it easy to use

typedef uint8_t u8;
typedef uint32_t u32;
typedef uint64_t u64;

typedef int8_t i8;
typedef int32_t i32;
typedef int64_t i64;

class sha256{
	bool hash_error;
	char mapping[36];

	u32 K[64];
	u32 H[8];

	//updatable
	u32 H_up[8];
	u64 m_length_up;
	u8 blockOfBytes_up[64];
	u32 blockOfBytes_count;
	//vector<u8> blockOfBytes_up;
	std::string hashResult_up;

	void resetH();
	std::string padStringZeroes(std::string Num, u32 padlength);
	std::string decToRadix(u32 num, u32 radix);

	u64 mod(i64 a, i64 b);

	//all function needs below here
	//needs to return uint32_t

	u32 rotr(u32 n, u32 x);
	u32 ch(u32 x, u32 y, u32 z);
	u32 maj(u32 x, u32 y, u32 z);

	u32 sigma0(u32 x);
	u32 sigma1(u32 x);
	u32 omega0(u32 x);
	u32 omega1(u32 x);

	void process_block();

	//file accessories
	//string fileHash;

public:
	/**
	\brief The default constructor of sha256.
	*/
	sha256();

	/**
	  \brief This function will take the input and hashes it immediately.
	  \param input The input to be hashed.
	  \return sha256 hash
	*/
	std::string hash(std::string input);

	/**
	  \brief Updates the message block. Note that this is part
	         of sha256 update and finalize function.
    \param input The string that will be added to the message block.
	*/
	void hash_update(std::string input);

	/**
	  \brief Updates the message block. Note that this is part
	         of sha256 update and finalize function.
    \param input The character to be added to the message block.
	*/
	void hash_update(char input);

	/**
	  \brief Updates the message block. Note that this is part
	         of sha256 update and finalize function.
    \param input[] The array of characters to be added to the message block.
    \param input_length The total length of the characters inside the array.
	*/
	void hash_update_array(char input[], u32 input_length);

	/**
	  \brief Updates the message block. Note that this is part
	         of sha256 update and finalize function.
    \param *input The pointer to character array or dynamic array or buffer to be added to the message block.
    \param input_length The total length of the characters inside the array or dynamic array or buffer.
	*/
	void hash_update(char * input, u64 input_length);

	/**
	  \brief Updates the message block. Note that this is part
	         of sha256 update and finalize function.
    \param *input The pointer to character array or dynamic array or buffer to be added to the message block.
    \param input_length The total length of the characters inside the array or dynamic array or buffer.
	\param (*func) The function that will be pass tah will accept parameter as unsigned integer
	*/
	void hash_update(char * input, u64 input_length, void (*func)(u32 percentage));

	/**
	  \brief Updates the message block. Note that this is part
	         of sha256 update and finalize function.

    The function that takes the message block inside and finishes everything inside of it.
    It hashes the message block's final content. Now the all you need to do is to collect the
    hash by calling the hash_result().
	*/
	void hash_finalize();

	/**
	  \brief After finalizing the hash, get the result as a string
	  \return sha256 hash
	*/
	std::string hash_result();
};


#endif //_SHA256_HEADER_FILE_AC75F356_AFE1_4F4F_B2D6_10FC42BFA635_
