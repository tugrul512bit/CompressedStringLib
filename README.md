# CompressedStringLib
Heavy-weight std::string wrapper with compression for big files.

## PredictorString

Uses single-char transition dictionary prediction to compress data.

```C++
std::string s1;
std::getline(std::ifstream("opencl.hpp"), s1, '\0'); // 326kB (a big header file from an api)

CompressedStringLib::PredictorString<ttype> pstr(s1); // 246kB

std::string output = pstr.string(); // 326 kB again
```

Encoding in FX8150 3.6GHz CPU + single channel 1333MHz RAM: 110 MB/s single core

Decoding in FX8150 3.6GHz CPU + single channel 1333MHz RAM: 130 MB/s single core

----

## HuffmanString

Uses Huffman-Encoding to compress data.

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
```
