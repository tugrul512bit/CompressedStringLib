# CompressedStringLib
Heavy-weight std::string wrapper with compression for big files.

```C++
std::string s1;
std::getline(std::ifstream("opencl.hpp"), s1, '\0'); // 326kB (a big header file from an api)

CompressedStringLib::PredictorString<ttype> pstr(s1); // 246kB

std::string output = pstr.string(); // 326 kB again
```

Encoding in FX8150 3.6GHz CPU + single channel 1333MHz RAM: 110 MB/s single core

Decoding in FX8150 3.6GHz CPU + single channel 1333MHz RAM: 130 MB/s single core
