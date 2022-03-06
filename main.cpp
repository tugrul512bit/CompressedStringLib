
#include "CompressStringLib.h"
#include<fstream>
#include<sstream>
    int main()
    {

    	std::string s="a";

    	std::getline(std::ifstream("./bigfile.txt"), s, '\0');


    	{
    		CompressedStringLib::PredictorString<size_t> prh(s,0,CompressedStringLib::PredictorString<size_t>::OPTIMIZE_WITH_HUFFMAN_RUNLENGTH_ENCODING);
    		std::cout<<s.size()<<" --> "<<(prh.bits().size() + prh.bits2().size())<<" --> "<<prh.string().size()<<std::endl;
    		std::cout<<s.size()<<" --> "<<(prh.bits().size() + prh.bits2().size())<<" --> "<<prh.string().size()<<std::endl;
    		if(s!=prh.string())
    		{
    			std::cout<<"ERROR!"<<std::endl;
    		}
    	}

    	{
    		CompressedStringLib::PredictorString<size_t> pr(s,0,CompressedStringLib::PredictorString<size_t>::OPTIMIZE_WITH_RUNLENGTH_ENCODING);
    		std::cout<<s.size()<<" --> "<<(pr.bits().size() + pr.bits2().size())<<" --> "<<pr.string().size()<<std::endl;
    		std::cout<<s.size()<<" --> "<<(pr.bits().size() + pr.bits2().size())<<" --> "<<pr.string().size()<<std::endl;
    		if(s!=pr.string())
    		{
    			std::cout<<"ERROR!"<<std::endl;
    		}
    	}

    	{
    		CompressedStringLib::PredictorString<size_t> ph(s,0,CompressedStringLib::PredictorString<size_t>::OPTIMIZE_WITH_HUFFMAN_ENCODING);
    		std::cout<<s.size()<<" --> "<<(ph.bits().size() + ph.bits2().size())<<" --> "<<ph.string().size()<<std::endl;
    		std::cout<<s.size()<<" --> "<<(ph.bits().size() + ph.bits2().size())<<" --> "<<ph.string().size()<<std::endl;
    		if(s!=ph.string())
    		{
    			std::cout<<"ERROR!"<<std::endl;
    		}
    	}

    	{
    		CompressedStringLib::PredictorString<size_t> str(s,0,CompressedStringLib::PredictorString<size_t>::OPTIMIZE_NONE);
    		std::cout<<s.size()<<" --> "<<(str.bits().size() + str.bits2().size())<<" --> "<<str.string().size()<<std::endl;
    		std::cout<<s.size()<<" --> "<<(str.bits().size() + str.bits2().size())<<" --> "<<str.string().size()<<std::endl;
    		if(s!=str.string())
    		{
    			std::cout<<"ERROR!"<<std::endl;
    		}
    	}

    	{
    		CompressedStringLib::HuffmanString h(s);
    		std::cout<<s.size()<<" --> "<<(h.serialize().size())<<" --> "<<h.string().size()<<std::endl;
    		std::cout<<s.size()<<" --> "<<(h.serialize().size())<<" --> "<<h.string().size()<<std::endl;
    		if(s!=h.string())
    		{
    			std::cout<<"ERROR!"<<std::endl;
    		}
    	}
    	return 0;
    }

// output
/*

this is 334kB C++ header file content
333774 --> 227444 --> 333774 (Predictor + RLE*2 + Huffman)
333774 --> 227444 --> 333774 (Predictor + RLE*2 + Huffman)
333774 --> 273092 --> 333774 (Predictor + RLE*2)
333774 --> 273092 --> 333774 (Predictor + RLE*2)
333774 --> 173757 --> 333774 (Predictor + Huffman)
333774 --> 173757 --> 333774 (Predictor + Huffman)
333774 --> 226377 --> 333774 (Predictor)
333774 --> 226377 --> 333774 (Predictor)
333774 --> 227382 --> 333774 (Huffman encoding)
333774 --> 227382 --> 333774 (Huffman encoding)

this is 334kB C++ header file content appended to 1MB of "a" string
1382350 --> 227408 --> 1382350 (Predictor + RLE*2 + Huffman)
1382350 --> 227408 --> 1382350 (Predictor + RLE*2 + Huffman) compressed size increased by 36 bytes!
1382350 --> 273130 --> 1382350 (Predictor + RLE*2)
1382350 --> 273130 --> 1382350 (Predictor + RLE*2) compressed size increased by 40 bytes!
1382350 --> 190053 --> 1382350 (Predictor + Huffman)
1382350 --> 190053 --> 1382350 (Predictor + Huffman) compressed size increased by 9.8%!
1382350 --> 243289 --> 1382350 (Predictor)
1382350 --> 243289 --> 1382350 (Predictor) compressed size increased by 7.5%!
1382350 --> 425350 --> 1382350 (Huffman encoding) 
1382350 --> 425350 --> 1382350 (Huffman encoding) compressed size increased by 87%!

*/
