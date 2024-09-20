Frac-KMC
=
Frac-KMC is a FracMinHash sketch generator tool from FASTA/FASTQ files. This tool is a modified version of the k-mer counting tool KMC (hence the name). 

Quick start
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

## Computing the sketches
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