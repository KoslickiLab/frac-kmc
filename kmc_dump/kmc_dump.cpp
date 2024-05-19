/*
  This file is a part of KMC software distributed under GNU GPL 3 licence.
  The homepage of the KMC project is http://sun.aei.polsl.pl/kmc

  This file demonstrates the example usage of kmc_api software.
  It reads kmer_counter's output and prints kmers to an output file.

  Authors: Sebastian Deorowicz, Agnieszka Debudaj-Grabysz, Marek Kokot

  Version: 3.2.2
  Date   : 2023-03-10
*/

#include <iostream>
#include <string>
#include <cstring>
#include <cstdlib>
#include <sstream>
#include <vector>
#include <algorithm>
#include <cmath>
#include "../kmc_api/kmc_file.h"
#include "nc_utils.h"
#include <map>

using namespace std;

#define ROTL64(x,y)	rotl64(x,y)
#define BIG_CONSTANT(x) (x)

inline uint64_t getblock64 ( const uint64_t * p, int i )
{
  return p[i];
}

inline uint64_t rotl64 ( uint64_t x, int8_t r )
{
  return (x << r) | (x >> (64 - r));
}

inline uint64_t fmix64 ( uint64_t k )
{
  k ^= k >> 33;
  k *= BIG_CONSTANT(0xff51afd7ed558ccd);
  k ^= k >> 33;
  k *= BIG_CONSTANT(0xc4ceb9fe1a85ec53);
  k ^= k >> 33;

  return k;
}

void MurmurHash3_x64_128 ( const void * key, const int len,
                           const uint32_t seed, void * out )
{
  const uint8_t * data = (const uint8_t*)key;
  const int nblocks = len / 16;

  uint64_t h1 = seed;
  uint64_t h2 = seed;

  const uint64_t c1 = BIG_CONSTANT(0x87c37b91114253d5);
  const uint64_t c2 = BIG_CONSTANT(0x4cf5ad432745937f);

  //----------
  // body

  const uint64_t * blocks = (const uint64_t *)(data);

  for(int i = 0; i < nblocks; i++)
  {
    uint64_t k1 = getblock64(blocks,i*2+0);
    uint64_t k2 = getblock64(blocks,i*2+1);

    k1 *= c1; k1  = ROTL64(k1,31); k1 *= c2; h1 ^= k1;

    h1 = ROTL64(h1,27); h1 += h2; h1 = h1*5+0x52dce729;

    k2 *= c2; k2  = ROTL64(k2,33); k2 *= c1; h2 ^= k2;

    h2 = ROTL64(h2,31); h2 += h1; h2 = h2*5+0x38495ab5;
  }

  //----------
  // tail

  const uint8_t * tail = (const uint8_t*)(data + nblocks*16);

  uint64_t k1 = 0;
  uint64_t k2 = 0;

  switch(len & 15)
  {
  case 15: k2 ^= ((uint64_t)tail[14]) << 48;
  case 14: k2 ^= ((uint64_t)tail[13]) << 40;
  case 13: k2 ^= ((uint64_t)tail[12]) << 32;
  case 12: k2 ^= ((uint64_t)tail[11]) << 24;
  case 11: k2 ^= ((uint64_t)tail[10]) << 16;
  case 10: k2 ^= ((uint64_t)tail[ 9]) << 8;
  case  9: k2 ^= ((uint64_t)tail[ 8]) << 0;
           k2 *= c2; k2  = ROTL64(k2,33); k2 *= c1; h2 ^= k2;

  case  8: k1 ^= ((uint64_t)tail[ 7]) << 56;
  case  7: k1 ^= ((uint64_t)tail[ 6]) << 48;
  case  6: k1 ^= ((uint64_t)tail[ 5]) << 40;
  case  5: k1 ^= ((uint64_t)tail[ 4]) << 32;
  case  4: k1 ^= ((uint64_t)tail[ 3]) << 24;
  case  3: k1 ^= ((uint64_t)tail[ 2]) << 16;
  case  2: k1 ^= ((uint64_t)tail[ 1]) << 8;
  case  1: k1 ^= ((uint64_t)tail[ 0]) << 0;
           k1 *= c1; k1  = ROTL64(k1,31); k1 *= c2; h1 ^= k1;
  };

  h1 ^= len; h2 ^= len;

  h1 += h2;
  h2 += h1;

  h1 = fmix64(h1);
  h2 = fmix64(h2);

  h1 += h2;
  h2 += h1;

  ((uint64_t*)out)[0] = h1;
  ((uint64_t*)out)[1] = h2;
}


void print_info(void);


//----------------------------------------------------------------------------------
// Check if --help or --version was used
bool help_or_version(int argc, char** argv)
{
	const std::string version = "--version";
	const std::string help = "--help";
	for (int i = 1; i < argc; ++i)
	{
		if (argv[i] == version || argv[i] == help)
			return true;
	}
	return false;
}

int main(int argc, char* argv[])
{
	if (argc == 1 || help_or_version(argc, argv))
	{
		print_info();
		return 0;
	}

	CKMCFile kmer_data_base;
	int32 i;
	uint32 min_count_to_set = 0;
	uint32 max_count_to_set = 0;
	std::string input_file_name;
	std::string output_file_name;
	uint32 seed = 0;
	uint32 scaled = 1;
	uint32 ksize = 0;
	string filename = "";
	bool output_abundances = false;

	FILE * out_file;
	//------------------------------------------------------------
	// Parse input parameters
	//------------------------------------------------------------
	if(argc < 5)
	{
		print_info();
		return EXIT_FAILURE;
	}

	for(i = 1; i < argc; ++i)
	{
		if(argv[i][0] == '-')
		{
			if(strncmp(argv[i], "-ci", 3) == 0)
				min_count_to_set = atoi(&argv[i][3]);
			else if(strncmp(argv[i], "-cx", 3) == 0)
					max_count_to_set = atoi(&argv[i][3]);
			else if(strncmp(argv[i], "-S", 2) == 0)
					seed = atoi(&argv[i][2]);
			else if(strncmp(argv[i], "-scaled", 7) == 0)
					scaled = atoi(&argv[i][7]);
			else if(strncmp(argv[i], "-ksize", 6) == 0)
					ksize = atoi(&argv[i][6]);
			else if(strncmp(argv[i], "-filename", 9) == 0)
					filename = string(&argv[i][9]);
			else if(strncmp(argv[i], "-a", 2) == 0)
					output_abundances = true;
			else
				break;
		}
		else
			break;
	}

	// print output abudances
	cout << "Output abundances " << output_abundances << endl;

	//cout << "Scaled " << scaled << endl;
	//cout << "Seed " << seed << endl;

	if(argc - i < 2)
	{
		print_info();
		return EXIT_FAILURE;
	}

	input_file_name = std::string(argv[i++]);
	output_file_name = std::string(argv[i]);

	if((out_file = fopen (output_file_name.c_str(),"wb")) == NULL)
	{
		print_info();
		return EXIT_FAILURE;
	}

	setvbuf(out_file, NULL ,_IOFBF, 1 << 24);

	//------------------------------------------------------------------------------
	// Open kmer database for listing and print kmers within min_count and max_count
	//------------------------------------------------------------------------------

	if (!kmer_data_base.OpenForListing(input_file_name))
	{
		print_info();
		return EXIT_FAILURE ;
	}
	else
	{
		uint32 _kmer_length;
		uint32 _mode;
		uint32 _counter_size;
		uint32 _lut_prefix_length;
		uint32 _signature_len;
		uint32 _min_count;
		uint64 _max_count;
		uint64 _total_kmers;

		kmer_data_base.Info(_kmer_length, _mode, _counter_size, _lut_prefix_length, _signature_len, _min_count, _max_count, _total_kmers);


		//std::string str;
		char str[1024];
		uint32 counter_len;

		CKmerAPI kmer_object(_kmer_length);

		if(min_count_to_set)
		if (!(kmer_data_base.SetMinCount(min_count_to_set)))
				return EXIT_FAILURE;
		if(max_count_to_set)
		if (!(kmer_data_base.SetMaxCount(max_count_to_set)))
				return EXIT_FAILURE;

		uint64 counter;

		// MRH code
		uint64_t largest_value = 0xFFFFFFFFFFFFFFFF;
		uint64_t threshold = std::round((long double)(largest_value)/(long double)(scaled));
		//cout << "threshold = " << threshold << endl;

		string output_string = "[{\"class\":\"frac_kmc_signature\",\"email\":\"\",\"hash_function\":\"0.murmur64\"";
		strcpy(str, output_string.c_str());
		fwrite(str, 1, output_string.length(), out_file);
		output_string = ",\"filename\":\"" + filename + "\"";
		strcpy(str, output_string.c_str());
		fwrite(str, 1, output_string.length(), out_file);
		output_string = ",\"license\":\"CC0\"";
		strcpy(str, output_string.c_str());
		fwrite(str, 1, output_string.length(), out_file);
		output_string = ",\"signatures\":[{\"num\":0";
		std::ostringstream ss1;
		ss1 << ksize;
		output_string = output_string + ",\"ksize\":" + string(ss1.str());
		std::ostringstream ss2;
		ss2 << seed;
		output_string = output_string + ",\"seed\":" + string(ss2.str());

		// MRH code
		vector<uint64_t> hashes;

		// store hash value to counter dictionary
		std::map<uint64_t, uint64_t> hash_to_counter;

		while (kmer_data_base.ReadNextKmer(kmer_object, counter))
		{
			kmer_object.to_string(str);
			str[_kmer_length] = '\t';
			counter_len = CNumericConversions::Int2PChar(counter, (uchar*)str + _kmer_length + 1);
			str[_kmer_length + 1 + counter_len] = '\n';

			// MRH code
			uint64_t out[2] = {0};
			MurmurHash3_x64_128 ( str, sizeof(char)*_kmer_length, seed, out);
			str[_kmer_length] = '\0';
			//std::cout << str << " hashed to " << out[0] << " " << (out[0]<threshold) << std::endl;
			str[_kmer_length] = '\t';
			if (out[0]<threshold)
			{
				//fwrite(str, 1, _kmer_length + counter_len + 2, out_file);
				hashes.push_back(out[0]);
			}

			// store hash value to counter dictionary
			hash_to_counter[out[0]] = counter;
		}

		std::sort(hashes.begin(), hashes.end());

		std::ostringstream ss;
    	ss << threshold;
		output_string = output_string + ",\"max_hash\":" + string(ss.str());
		output_string = output_string + ",\"mins\":[";
		strcpy(str, output_string.c_str());
		fwrite(str, 1, output_string.length(), out_file);

		for (int i=0; i<hashes.size(); i++)
		{
			std::ostringstream ss;
			ss << hashes[i];
			output_string = string(ss.str());
			strcpy(str, output_string.c_str());
			fwrite(str, 1, output_string.length(), out_file);
			if (i<hashes.size()-1)
			{
				output_string = ",";
				strcpy(str, output_string.c_str());
				fwrite(str, 1, output_string.length(), out_file);
			}
		}

		if (output_abundances) {

			output_string = "],\"abundances\":[";
			strcpy(str, output_string.c_str());
			fwrite(str, 1, output_string.length(), out_file);

			for (int i=0; i<hashes.size(); i++)
			{
				std::ostringstream ss;
				ss << hash_to_counter[hashes[i]];
				output_string = string(ss.str());
				strcpy(str, output_string.c_str());
				fwrite(str, 1, output_string.length(), out_file);
				if (i<hashes.size()-1)
				{
					output_string = ",";
					strcpy(str, output_string.c_str());
					fwrite(str, 1, output_string.length(), out_file);
				}
			}

		}

		output_string = "], \"molecule\":\"dna\", \"md5sum\":\"abcd\"}], \"version\":0.1}]\n";
		strcpy(str, output_string.c_str());
		fwrite(str, 1, output_string.length(), out_file);

		fclose(out_file);
		kmer_data_base.Close();
	}

	return EXIT_SUCCESS;
}
// -------------------------------------------------------------------------
// Print execution options
// -------------------------------------------------------------------------
void print_info(void)
{
	std::cout << "KMC dump ver. " << KMC_VER << " (" << KMC_DATE << ")\n"
			  << "\nUsage:\nkmc_dump [options] <kmc_database> <output_file>\n"
			  << "Parameters:\n"
			  << "<kmc_database> - kmer_counter's output\n"
			  << "Options:\n"
			  << "-ci<value> - exclude k-mers occurring less than <value> times\n"
			  << "-cx<value> - exclude k-mers occurring more of than <value> times\n"
			  << "-S<value>  - seed to be used by mmh3\n"
			  << "-scaled<value>  - scaled for FracMinHash\n"
			  << "-a - output abundances (default: false)\n";
}

// ***** EOF
