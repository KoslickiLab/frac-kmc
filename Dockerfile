FROM ubuntu:20.04

# non-interactive
ENV DEBIAN_FRONTEND=noninteractive

# Update and install dependencies
RUN apt-get update && apt-get install -y \
    build-essential \
    g++ \
    gcc \
    make \
    cmake

# Copy all files in the current directory to the container
COPY . /usr/src/app
WORKDIR /usr/src/app

# Make
RUN make

# Add the bin directory to PATH
ENV PATH="/usr/src/app/bin:${PATH}"

# Run the app with any command line arguments
ENTRYPOINT [ "/usr/src/app/bin/fracKmcSketch" ]
CMD []


# docker run -v /Users/mbr5797/PSU/Research/frac-kmc-koslickilab/frac-kmc/test_frac_kmc:/data --platform linux/amd64 frackmc:x86-64 /data/ndl.fasta /data/test_output --fa
