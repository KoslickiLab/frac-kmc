Frac-KMC
=
Frac-KMC is a FracMinHash sketch generator tool from FASTA/FASTQ files. This tool is a modified version of the k-mer counting tool KMC (hence the name). 

### Why use Frac-KMC?
KMC is an extremely fast k-mer couting tool. KMC is also very low-memory. It uses minimizers to count kmers very fast, using multiple bins and threads. Therefore, KMC that has been modified to compute FracMinHash sketches should be able to compute the sketches extremely fast. People traditionally use the software `sourmash` (the command `sourmash sketch`) to compute FracMinHash sketches. Frac-KMC is an attemp to make the sketching faster.

##### Frac-KMC is faster than `sourmash sketch`

Initial investigations have revealed following results: we ran `sourmash sktech` and `fracKmcSketch` to generate FracMinHash sketch of Human reference genome (GrCh38) for various scaled values and kmer sizes. The running time is compared below.

![Running time comparison of sourmash and Frac-KMC in generating sketches](res.png)

It shows that Frac-KMC is **up to 5.7 times faster** to compute the same sketch.

##### Frac-KMC sketches are compatible with `sourmash`

Frac-KMC has been written to compute the FracMinHash sketches that are compatible with `sourmash`. This means that after computing a FracMinHash sketch using Frac-KMC, you can use the sketch as an input to `sourmash compare`. We have not tested compatibility with other `sourmash` commands yet, but they will hopefully work too.

Quick start
=
For now, Frac-KMC has only been compiled for linux systems. We are working on other versions too, to see if we can make a release.
#### Getting the executable
Easiest wat to obtain the executables is by
1. downloading the three executables in `wrappers/sketch/bin`,
1. adding execution permission to these three executables, and
1. adding the directory where the executables are in your `PATH` variable
#### Computing the sketches
```
fracKmcSketch <fasta/fastq_filename> <sketch_name> --ksize 21 --scaled 1000 --seed 42
```
This command with create a sketch from the fasta/fastq file using 21-mers, a scaled value of 1000, and use 42 as the seed for the hash function. The resulting sketch should be compatible with a sketch computed using `sourmash sketch dna input_filename -p k=21,scaled=1000 -o sketch_name`.


## Citing

Frac-KMC is not associated with any manuscript yet, but if you use Frac-KMC, make sure to cite original KMC:

[Marek Kokot, Maciej Długosz, Sebastian Deorowicz, KMC 3: counting and manipulating k-mer statistics, Bioinformatics, Volume 33, Issue 17, 01 September 2017, Pages 2759–2761, https://doi.org/10.1093/bioinformatics/btx304](https://academic.oup.com/bioinformatics/article/33/17/2759/3796399)

[Sebastian Deorowicz, Marek Kokot, Szymon Grabowski, Agnieszka Debudaj-Grabysz, KMC 2: fast and resource-frugal k-mer counting, Bioinformatics, Volume 31, Issue 10, 15 May 2015, Pages 1569–1576, https://doi.org/10.1093/bioinformatics/btv022](https://academic.oup.com/bioinformatics/article/31/10/1569/177467)

[Deorowicz, S., Debudaj-Grabysz, A. & Grabowski, S. Disk-based k-mer counting on a PC. BMC Bioinformatics 14, 160 (2013). https://doi.org/10.1186/1471-2105-14-160](https://bmcbioinformatics.biomedcentral.com/articles/10.1186/1471-2105-14-160)
