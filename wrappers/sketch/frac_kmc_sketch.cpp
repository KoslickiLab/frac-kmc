#include <iostream>
#include <string>
#include <cstdlib>
#include <random>
#include <thread>

#define RANDSTRLEN 10

std::string generateRandomString(int length) {
    std::string characters = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
    std::string randomString;

    std::random_device rd;
    std::mt19937 generator(rd());
    std::uniform_int_distribution<int> distribution(0, characters.size() - 1);

    for (int i = 0; i < length; ++i) {
        randomString += characters[distribution(generator)];
    }

    return randomString;
}

int main(int argc, char* argv[]) {
    int ksize = 21;
    int scaled = 1000;
    std::string infilename;
    int seed = 42;
    std::string outfilename;
    bool isFasta = false;
    bool isFastq = false;
    int num_threads = -1;

    if (argc < 3) {
        std::cerr << "Usage: " << argv[0] << " <infilename> <outfilename> [options]" << std::endl;
        std::cerr << "Options:" << std::endl;
        std::cerr << "  --ksize <int>      kmer size (default: 21)" << std::endl;
        std::cerr << "  --scaled <int>     Scaled value (default: 1000)" << std::endl;
        std::cerr << "  --seed <int>       Random seed (default: 42)" << std::endl;
        std::cerr << "  --fa               Input file is in fasta format" << std::endl;
        std::cerr << "  --fq               Input file is in fastq format" << std::endl;
        std::cerr << "  --a                Write abundances" << std::endl;
        std::cerr << "  --n <int>          Number of threads (default: 1)" << std::endl;
        return 1;
    }

    infilename = argv[1];
    outfilename = argv[2];
    bool use_abundance = false;

    for (int i = 3; i < argc; i++) {
        if (std::string(argv[i]) == "--ksize" && i + 1 < argc) {
            ksize = std::atoi(argv[i + 1]);
        } else if (std::string(argv[i]) == "--scaled" && i + 1 < argc) {
            scaled = std::atoi(argv[i + 1]);
        } else if (std::string(argv[i]) == "--seed" && i + 1 < argc) {
            seed = std::atoi(argv[i + 1]);
        } else if (std::string(argv[i]) == "--fa") {
            isFasta = true;
        } else if (std::string(argv[i]) == "--fq") {
            isFastq = true;
        } else if (std::string(argv[i]) == "--a") {
            use_abundance = true;
        } else if (std::string(argv[i]) == "--n" && i + 1 < argc) {
            num_threads = std::atoi(argv[i + 1]);
        }
    }

    if ( !isFasta && !isFastq ) {
        std::cerr << "Input file format: Not specified" << std::endl;
        return 1;
    }

    // if number of threads is not specified, use the default value
    if (num_threads == -1) {
        num_threads = 1;
    }

    //std::cout << "ksize: " << ksize << std::endl;
    //std::cout << "scaled: " << scaled << std::endl;
    //std::cout << "infilename: " << infilename << std::endl;
    //std::cout << "seed: " << seed << std::endl;
    //std::cout << "outfilename: " << outfilename << std::endl;

    std::string random_string = generateRandomString(RANDSTRLEN);
    std::string kmers_dbname = infilename + "_kmers_" + random_string;
    std::string cmd1;

    // mkdir random_string
    std::string cmd0 = "mkdir ./" + random_string;
    std::cout << cmd0.c_str() << std::endl;
    int result0 = std::system(cmd0.c_str());
    if (result0 != 0) {
        std::cout << "Directory creation failed! Exiting..." << std::endl;
        return -1;
    }

    
    if (isFasta) {
        cmd1 = "frackmc -ci1 -cs35565 -scaled" + std::to_string(scaled)
                            + " -S" + std::to_string(seed)
                            + " -k" + std::to_string(ksize)
                            + " -t" + std::to_string(num_threads)
                            + " -fm " + infilename
                            + " " + kmers_dbname
                            + " ./" + random_string;
    } else {
        cmd1 = "frackmc -ci1 -cs35565 -scaled" + std::to_string(scaled)
                            + " -S" + std::to_string(seed)
                            + " -k" + std::to_string(ksize)
                            + " -t" + std::to_string(num_threads)
                            + " -fq " + infilename
                            + " " + kmers_dbname
                            + " ./" + random_string;
    }

    std::cout << cmd1.c_str() << std::endl;
    int result1 = std::system(cmd1.c_str());

    if (result1 != 0) {
        std::cout << "kmc database build failed! Exiting..." << std::endl;
        // remove the directory random_string
        std::string cmd5 = "rm -rf ./" + random_string;
        std::cout << cmd5.c_str() << std::endl;
        int result5 = std::system(cmd5.c_str());
        return 1;
    }

    std::string cmd2 = "frackmcdump";
    if (use_abundance) {
        cmd2 += " -a";
    }

    cmd2 += " -ci1 -scaled" + std::to_string(scaled)
                            + " -S" + std::to_string(seed)
                            + " -ksize" + std::to_string(ksize)
                            + " -filename" + infilename
                            + " " + kmers_dbname
                            + " " + outfilename;
    
    std::cout << cmd2.c_str() << std::endl;
    int result2 = std::system(cmd2.c_str());

    if (result2 != 0) {
        std::cout << "sketch writing failed! Exiting..." << std::endl;
        return -1;
    }

    std::string cmd3 = "rm " + kmers_dbname+"*";
    std::cout << cmd3.c_str() << std::endl;
    int result3 = std::system(cmd3.c_str());    
    if (result3 != 0) {
        std::cout << "kmc database removal failed! Exiting..." << std::endl;
        return -1;
    }

    std::string cmd4 = "rm -r ./" + random_string;
    std::cout << cmd4.c_str() << std::endl;
    int result4 = std::system(cmd4.c_str());
    if (result4 != 0) {
        std::cout << "Directory removal failed! Exiting..." << std::endl;
        return -1;
    }

    return 0;
}
