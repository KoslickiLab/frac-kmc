- [Frac-KMC](#frac-kmc)
- [Installation](#installation)
  - [Building from the source](#building-from-the-source)
  - [Pre-built executable](#pre-built-executable)
  - [Running by docker](#running-by-docker)
    - [Input/output in docker](#inputoutput-in-docker)
- [Computing the sketches](#computing-the-sketches)
- [Citing](#citing)


Frac-KMC
=
Frac-KMC is a FracMinHash sketch generator tool from FASTA/FASTQ files. This tool is a modified version of the k-mer counting tool KMC (hence the name). 

Installation
=

## Building from the source
After downloading the repository:
```
make
```
Make sure to add the bin directory to your PATH variable.

## Pre-built executable
For now, Frac-KMC has only been compiled for linux on an x64 machine. Easiest wat to obtain the executables is by
1. downloading the three executables in `bin`,
1. adding execution permission to these three executables, and
1. adding the directory where the executables are in your `PATH` variable

## Running by docker
frac-kmc can be run using docker.
```
docker run --platform linux/amd64 mahmudhera/frackmc:x86-64
```

This will download the docker image of frac-kmc into your local machine. The image is about 550 MB. After downloading the image, docker will run frac-kmc, and you should see the following outout:

```
Usage: /usr/src/app/bin/fracKmcSketch <infilename> <outfilename> [options]
Options:
  --ksize <int>      kmer size (default: 21)
  --scaled <int>     Scaled value (default: 1000)
  --seed <int>       Random seed (default: 42)
  --fa               Input file is in fasta format
  --fq               Input file is in fastq format
  --a                Write abundances
  --n <int>          Number of threads (default: 1)
```

### Input/output in docker

You can mount a local directory when invoking the `docker run` command by using the `-v` flag. 

For example, you can ask docker to mount a local directory to `/data` using the following command. This allows you to provide the input/output arguments in your local directory as if they are in `/data` in the docker container.

```
docker run -v <your_local_directory>:/data --platform linux/amd64 frackmc:x86-64 /data/<input_filename> /data/<output_filename> <options>
```

Computing the sketches
=
```
fracKmcSketch <fasta/fastq_filename> <sketch_name> --ksize 21 --scaled 1000 --seed 42 --n 32
```
This command with create a sketch from the fasta/fastq file using 21-mers, a scaled value of 1000, and use 42 as the seed for the hash function. It will also use 32 parallel threads to compute the sketch. The resulting sketch should be compatible with a sketch computed using `sourmash sketch dna input_filename -p k=21,scaled=1000 -o sketch_name`.


Citing
=

Please cite the following to credit use of frac-kmc:

[Mahmudur Rahman Hera, David Koslicki, Cosine Similarity Estimation Using FracMinHash: Theoretical Analysis, Safety Conditions, and Implementation, In 24th International Workshop on Algorithms in Bioinformatics (WABI 2024), Schloss Dagstuhl–Leibniz-Zentrum für Informatik
](https://drops.dagstuhl.de/entities/document/10.4230/LIPIcs.WABI.2024.6)

Additionally, you may also cite original KMC:

[Marek Kokot, Maciej Długosz, Sebastian Deorowicz, KMC 3: counting and manipulating k-mer statistics, Bioinformatics, Volume 33, Issue 17, 01 September 2017, Pages 2759–2761, https://doi.org/10.1093/bioinformatics/btx304](https://academic.oup.com/bioinformatics/article/33/17/2759/3796399)