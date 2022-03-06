
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

