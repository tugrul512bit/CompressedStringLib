# CompressedStringLib
Heavy-weight std::string wrapper that compresses its content and decompresses when used, accelerated with caching.

## PredictorString

Uses single-char transition dictionary prediction to compress data. Fast decompression.

```C++
std::string s1;
std::getline(std::ifstream("opencl.hpp"), s1, '\0'); // 326kB (a big header file from an api)
using UnsignedIntegerPrefixType = size_t;
CompressedStringLib::PredictorString<UnsignedIntegerPrefixType> pstr(s1); // 246kB

std::string output = pstr.string(); // 326 kB again

// 126kB of aaaaaa (and 2x decoding speed because of predictable data)
CompressedStringLib::PredictorString<UnsignedIntegerPrefixType> pstr2(std::string(1024*1024,'a'));  

// ~5 nanoseconds sampling latency with caching
// 14 miliseconds without caching (or with cache-miss), not optimized yet
unsigned char c = pstr2[1024*1024*2]; 

```

Tests on compiler g++ with flags  -march=native -mavx -m64 -O3 -std=c++1y:

### FX8150 3.6GHz CPU + single channel 1333MHz RAM + Ubuntu 18.04LTS + turbo disabled

Encoding: 110 MB/s single core

Decoding: 130 MB/s single core


## 1MB string of "aaaaa"

### FX8150 3.6GHz CPU + single channel 1333MHz RAM + Ubuntu 18.04LTS + turbo disabled

Encoding: 144 MB/s single core

Decoding: 140 MB/s single core


### Intel(R) Xeon(R) CPU E3-1270 V2 @ 3.50GHz  (https://ideone.com/PFlyVj)

Encoding: 280 MB/s

Decoding: 270 MB/s



### Intel(R) Xeon(R) E-2286G CPU @ 4.00GHz (https://wandbox.org/)

Encoding: 516MB/s

Decoding: 426MB/s



### AMD Opteron(tm) Processor 4332 HE (http://coliru.stacked-crooked.com/a/5ecec4a2fcf7d6e9)

Encoding: 280 MB/s

Decoding: 290 MB/s



### Intel(R) Xeon(R) Gold 5215 CPU @ 2.50GHz (https://rextester.com/SJPGP78370)

Encoding: 340 MB/s

Decoding: 240 MB/s



### AMD EPYC 7601 32-Core Processor (unknown frequency, unknown compilation flags, https://csacademy.com/code/S5TBVCoB/)

Encoding: 212 MB/s (if each thread does a different string encoding, then 12GB/s total)

Decoding: 194 MB/s

----

## HuffmanString

Uses Huffman-Encoding to compress data. Fast indexing into compressed data.

```C++
std::string stri;
std::getline(std::ifstream("opencl.hpp"), stri, '\0'); // 326kB (a big header file from an api)

// keeps Huffman Tree to decode later
CompressedStringLib::HuffmanString test(stri,false,256); // 212kB

// makes internal Huffman tree data serialized to save on disk or other storage
auto ser = test.serialize();

// later loaded from another instance
CompressedStringLib::HuffmanString test2;
test2.deserializeFrom(ser);


std::cout<<test2.string().size()<<std::endl; // 326 kB again
std::cout<<ser.size()<<std::endl; // 212 kB


// 640 kB and 4x decoding speed due to having too many "a" compared to just 1 "b"
// 256 chars cached
CompressedStringLib::HuffmanString test3(std::string(1024*1024*4,'a') + std::string("b"),false,256);

 // bigFile: 4MB of "a"
CompressedStringLib::HuffmanString test5(bigFile,false,256); // 540kB

// ~5 nanoseconds sampling latency with caching
// 35 microseconds without caching (or with cache-miss), optimized for indexing
unsigned char c = test5[1024*1024*2]; 
```

Encoding: 53 MB/s single core

Decoding: 30 MB/s single core

(FX8150 3.6GHz CPU + single channel 1333MHz RAM  + Ubuntu 18.04LTS + turbo disabled)

(Tested on compiler g++ with flags  -march=native -mavx -m64 -O3 -std=c++1y)

---

## PredictorString Optimized With Huffman Encoding

By default, PredictorString does not enable Huffman Encoding for speed. With ```CompressedStringLib::PredictorString<UnsignedIntegerPrefixType>::OPTIMIZE_WITH_HUFFMAN_ENCODING``` parameter given to its constructor, it further compresses the internal char-literal data to save some more space (despite allocating some memory for Huffman Tree & its lookup tables).

```C++
std::string str;
std::getline(std::ifstream("opencl.hpp"), stri, '\0'); // 326kB


// internal data (only compressed char-literals, not prefixes) is compressed further by Huffman Encoding
// 173 kB
// 32 MB/s decoding bandwidth (just a bit higher than Huffman Encoding alone due to already compressed data size)
using UnsignedIntegerPrefixType = size_t;
CompressedStringLib::PredictorString<UnsignedIntegerPrefixType> pstr(
     str,
     2048,
     CompressedStringLib::PredictorString<size_t>::OPTIMIZE_WITH_HUFFMAN_ENCODING
);

 auto extracted = pstr.string(); // 326 kB
```

---

Some compression ratios for other test data:

- E.coli from https://corpus.canterbury.ac.nz/descriptions/: 4MB to 540kB
- CompressStringLib.h: 44kB to 35kB
- Executable binary file for benchmarks: 83kB to 80kB
- Png file: 180kB to 195kB (png is already in a compressed format)
- Minified JavaScript file (Jquery 1.4.2): 72kB to 60kB
- JavaScript file (Jquery 1.4.2): 163kB to 114kB
- English dictionary from http://www.math.sjsu.edu/~foster/dictionary.txt: 3.2MB to 1.8MB
- Dictionary from https://raw.githubusercontent.com/dwyl/english-words/master/words_alpha.txt: 3.8MB to 2.1MB
- Html text from https://flint.cs.yale.edu/cs421/papers/x86-asm/asm.html: 45kB to 42kB 

Htop mem measurement:

```C++

#include "CompressStringLib.h"
#include<fstream>

int main()
{
   using UnsignedIntegerPrefixType = size_t;
   CompressedStringLib::PredictorString<UnsignedIntegerPrefixType> pstrArr[2000];
   {
        std::string s1; //str holds the content of the file
        std::getline(std::ifstream("opencl.hpp"), s1, '\0'); // 326kB 



        for(int i=0;i<2000;i++)
        {
           // disabling caching (0) as it increases memory footprint per instance!
           pstrArr[i]=CompressedStringLib::PredictorString<UnsignedIntegerPrefixType>(s1,0,CompressedStringLib::PredictorString<UnsignedIntegerPrefixType>::OPTIMIZE_WITH_HUFFMAN_ENCODING);
        }
   }
   int ii;
   std::cin>>ii;
   return 0;
}

```
According to Htop, 11.6% of 4GB RAM is consumed by the app. This is 464 megabytes. 464 MB / 2000 = 232 kB

---

## How Does Compression Work?

In PredictorString, algorithm uses a dynamically built boolean dictionary to guess the next character in the input. Depending on true/false predictions, it edits dictionary and on every failed prediction it carries character to output and on every successful prediction it just updates dictionary. Depending on combination of failures/successes, it builds a mask (1=success, 0=fail) and writes it to a prefix output. Then in decompression part, it does exact opposite as same dictionary is dynamically built again. This has the extra memory footprint only during encoding and decoding.

In HuffmanString, there is Huffman Tree building -> encoding of string -> decoding of string phases. Huffman Tree is built by frequencies of character codes. The most frequent characters are placed on top nodes while lower-frequent characters are placed on deeper nodes. Then they are traversed top-down approach to build tags for them. On every "left" path taken, a "0" is added to tag, on every "right" path taken, "1" is added to path, until target char's node is reached. This effectively encodes plain text with non-random content into much shorter names, as low as single bit per char.
