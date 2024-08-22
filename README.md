# TLPI

This repository documents my self-study journey through the book
*The Linux Programming Interface* by Michael Kerrisk. See [the book website here](https://man7.org/tlpi/).

In particular, this repository contains my implementation code and answers to most of the exercises
found in the book. The language of choice was C to easily to follow along the book.

I embarked on this journey with the goal of developing a firm foundation on Linux/UNIX
concepts, and a conversant level in Linux/UNIX system programming. Though I've read
the book linearly, my journey has been anything but that. For example, I had to develop
a reading knowledge of C programming. Here are some turns I have taken or done simultaneously:

- I learned C by reading and completing the exercises in the book [The C Programming Language](https://github.com/segarciat/TCPL), by Dennis and Ritchie.

- I read [Modern 21st Century C](https://www.oreilly.com/library/view/21st-century-c/9781491904428/) to
learn basics of setting up a modern C development environment and learn basics of C11.

- I read the [GNU Make manual](https://www.gnu.org/software/make/manual/make.pdf) to make simple makefiles.

- I learned fundamnetals of computer architecture, x86-64 assembly, operating systems, and system programming
by reading and completing exercises in [CS:APP 3rd edition](https://github.com/segarciat/CSAPP) by Briant
and O'Hallaron.

In a world where it is difficult to get face-to-face mentorship, I am grateful to Michael Kerrisk
because reading his book made it feel like he was there all along ensuring I understood the material.
Feel free to look around and provide feedback.

## Environment

The following command outputs information about the environment I used throughout my journey:

```bash
cat <(lsb_release -dc) <(uname -rmo) <(/usr/lib/x86_64-linux-gnu/libc.so.6 | head -n 1)
```

```
Description:	Ubuntu 22.04.4 LTS
Codename:	jammy
6.8.0-40-generic x86_64 GNU/Linux
GNU C Library (Ubuntu GLIBC 2.35-0ubuntu3.8) stable release version 2.35.
```

## TLPI Source Code

Michael Kerrisk provides [source code accompanying TLPI, available through the book site](https://man7.org/tlpi/code/index.html). The source code is provided on the website as a compressed tarball:

```bash
# My repo directory
cd TLPI

# Download the tarball for Distributed Version (see website)
wget https://man7.org/tlpi/code/download/tlpi-240109-dist.tar.gz

# Decompress and extract tarball in current directory
tar -xvz -f tlpi-240109-dist.tar.gz 
```

The `README` and `BUILDING` in `tlpi-dist` explain how to proceed. Specifically,
after installing the necessary packages (explained in `BUILDING`), one can run
`make` on the top-level directory `tlpi-dist`:

```bash
cd tlpi-dist
# Read README and BUILDING files to learn what packages must be installed.

# For example, it says we need libcap-dev, among others.
sudo apt install libcap-dev
sudo apt install libacl1-dev
sudo apt install libcrypt-dev
sudo apt install libselinux1-dev
sudo apt install libseccomp-dev

# Install gcc and make
sudo apt install gcc
sudo apt install make

# Build tlpi-dist/lib and all examples.
make
```

The directory `tlpi-dist` contains folders with source code referenced through the code
listings in the book. Of interest is also `tlpi-dist/lib`, which includes important headers
and static archives such as `tlpi_hdr.h` and `libtlpi.a`, gracefully provided by Kerrisk.
These are developed throughout the book starting on chapter 3, and I use them in many exercises.

To use the sources and headers in the TLPI distribution, I created a file `Makefile.template`
that you will often see in my solutions. The template is meant to be used in the exercises,
and assumes `tlpi-dist` is in a directory `TLPI` above them. I did my best to stick to
a hierarchy such as `TLPI/chxxxxx/exercises/zz-zz/Makefile` and coded a path to the library
directory that assumes this.

## PThreads

Notice that the Pthreads man pages come in a separate package:

```bash
sudo apt install manpages-posix-dev
```
