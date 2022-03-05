# CompressedStringLib
Heavy-weight std::string wrapper with compression for big files.

## PredictorString

Uses single-char transition dictionary prediction to compress data. Fast decompression.

```C++
std::string s1;
std::getline(std::ifstream("opencl.hpp"), s1, '\0'); // 326kB (a big header file from an api)

CompressedStringLib::PredictorString<size_t> pstr(s1); // 246kB

std::string output = pstr.string(); // 326 kB again

// 126kB of aaaaaa (and 2x decoding speed because of predictable data)
CompressedStringLib::PredictorString<size_t> pstr(std::string(1024*1024,'a'));  

```

Encoding: 110 MB/s single core

Decoding: 130 MB/s single core

(FX8150 3.6GHz CPU + single channel 1333MHz RAM + Ubuntu 18.04LTS + turbo disabled)

----

## HuffmanString

Uses Huffman-Encoding to compress data. Fast indexing into compressed data.

```C++
std::string stri;
std::getline(std::ifstream("opencl.hpp"), stri, '\0'); // 326kB (a big header file from an api)

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

// ~5 nanoseconds sampling latency with caching
// 35 microseconds without caching (or with cache-miss)
unsigned char c = test3[1024*1024*2]; 
```

Encoding: 53 MB/s single core

Decoding: 30 MB/s single core

(FX8150 3.6GHz CPU + single channel 1333MHz RAM  + Ubuntu 18.04LTS + turbo disabled)
