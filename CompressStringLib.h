/*
 * CompressStringLib.h
 *
 *  Created on: Mar 5, 2022
 *      Author: tugrul
 */

#ifndef COMPRESSSTRINGLIB_H_
#define COMPRESSSTRINGLIB_H_

#include <algorithm>
#include <vector>
#include <map>
#include <iostream>
#include <functional>
#include <string>
#include <memory>
#include<chrono>
#include <cstring>
#include<mutex>


namespace CompressedStringLib
{

	class Bench
	{
	public:
		Bench(size_t * targetPtr)
		{
			target=targetPtr;
			t1 =  std::chrono::duration_cast< std::chrono::nanoseconds >(std::chrono::high_resolution_clock::now().time_since_epoch());
		}

		~Bench()
		{
			t2 =  std::chrono::duration_cast< std::chrono::nanoseconds >(std::chrono::high_resolution_clock::now().time_since_epoch());
			*target= t2.count() - t1.count();
		}
	private:
		size_t * target;
		std::chrono::nanoseconds t1,t2;
	};

    // stores a bit in a byte at a position
    inline void storeBit(unsigned char & data, const unsigned char value, const int pos) noexcept
    {
    	data = (value << pos) | (data & ~(1 << pos));
    }

    // stores a bit in a size_t at a position
    inline void storeBit(size_t & data, size_t value, const int pos) noexcept
    {
    	data = (value << pos) | (data & ~(1ull << pos));
    }

    // stores a bit in a size_t at a position
    inline void storeBit(uint32_t & data, uint32_t value, const int pos) noexcept
    {
    	data = (value << pos) | (data & ~(((uint32_t)1) << pos));
    }

    // stores a bit in a size_t at a position
    inline void storeBit(uint16_t & data, uint16_t value, const int pos) noexcept
    {
    	data = (value << pos) | (data & ~(((uint16_t)1) << pos));
    }

    // loads a bit from a byte at a position
    inline unsigned char loadBit(const unsigned char & data, const int pos) noexcept
    {
    	return (data>>pos)&1;
    }

    // loads a bit from a size_t at a position
    inline size_t loadBit(const size_t & data, const int pos) noexcept
    {
    	return (data>>pos)&1;
    }

    // loads a bit from a size_t at a position
    inline uint32_t loadBit(const uint32_t & data, const int pos) noexcept
    {
    	return (data>>pos)&1;
    }

    // loads a bit from a size_t at a position
    inline uint16_t loadBit(const uint16_t & data, const int pos) noexcept
    {
    	return (data>>pos)&1;
    }

    // Node structure for Huffman Tree On a std::vector
    struct Node
    {
    	size_t count;
    	int self;
    	int leaf1;
    	int leaf2;
    	unsigned char data;
    	unsigned char isLeaf;
    };

    // Huffman Tree that works on std::vector for more cached reads

    class HuffmanTree
    {


    public:
    	HuffmanTree(){
    		for(int i=0;i<256;i++)
    		{
    			referenceMapDirect[i]=0;
    			encodeDirectSize[i]=0;
    		}

    	}

    	void add(std::string str)
    	{
    		const int sz = str.size();
    		const int sz8= sz-(sz%8);

    		size_t parallelAccumulator[4][256];
    		for(int i=0;i<4;i++)
    			for(int j=0;j<256;j++)
    				parallelAccumulator[i][j]=0;
    		for(int i=0;i<sz8;i+=8)
    		{
    			parallelAccumulator[0][str[i]]++;
    			parallelAccumulator[1][str[i+1]]++;
    			parallelAccumulator[2][str[i+2]]++;
    			parallelAccumulator[3][str[i+3]]++;
    			parallelAccumulator[0][str[i+4]]++;
    			parallelAccumulator[1][str[i+5]]++;
    			parallelAccumulator[2][str[i+6]]++;
    			parallelAccumulator[3][str[i+7]]++;
    		}
    		for(int i=0;i<4;i++)
    			for(int j=0;j<256;j++)
    				referenceMapDirect[j]+=parallelAccumulator[i][j];
    		for(int i=sz8;i<sz;i++)
    		{
    			referenceMapDirect[str[i]]++;
    		}
    	}

    	void add(unsigned char data)
    	{
    		referenceMapDirect[data]++;
    	}

    	void generateTree(const unsigned char debug=false)
    	{
    		std::vector<Node> sortedNodes;


    		int ctr=0;
    		int singleCharDetected = 0;
    		int detectedNodeIndex = -1;
    		for(int i=0;i<256;i++)
    		{
    			size_t ct = referenceMapDirect[i];
    			if(ct>0)
    			{
    				detectedNodeIndex=i;
    				singleCharDetected++;
    				Node node;

    				node.data=i;
    				node.count=ct;

    				node.self=ctr;
    				node.leaf1=-1;
    				node.leaf2=-1;
    				node.isLeaf=true;
    				referenceVec.push_back(node);
    				sortedNodes.push_back(node);
    				ctr++;
    			}
    		}
    		if(singleCharDetected == 1)
    		{
				Node node;
				referenceMapDirect[(detectedNodeIndex+1)&255]++;
				node.data=(detectedNodeIndex+1)&255;
				node.count=referenceMapDirect[(detectedNodeIndex+1)&255];

				node.self=ctr;
				node.leaf1=-1;
				node.leaf2=-1;
				node.isLeaf=true;
				referenceVec.push_back(node);
				sortedNodes.push_back(node);
				ctr++;
    		}

    		std::sort(sortedNodes.begin(), sortedNodes.end(),[](const Node & n1, const Node & n2){ return n1.count<n2.count;});

    		while(sortedNodes.size()>1)
    		{
    			Node node1 = sortedNodes[0];
    			Node node2 = sortedNodes[1];
    			Node newNode;
    			newNode.count = node1.count + node2.count;
    			newNode.data=0;
    			newNode.leaf1 = node1.self;
    			newNode.leaf2 = node2.self;
    			newNode.self = ctr;
    			newNode.isLeaf=false;
    			sortedNodes.erase(sortedNodes.begin());
    			sortedNodes.erase(sortedNodes.begin());
    			sortedNodes.push_back(newNode);

    			referenceVec.push_back(newNode);
    			std::sort(sortedNodes.begin(), sortedNodes.end(),[](const Node & n1, const Node & n2){ return n1.count<n2.count;});
    			ctr++;
    		}

    		root = sortedNodes[0];


    		std::function<void(Node,std::vector<unsigned char>)> g = [&](Node node, std::vector<unsigned char> path){
    			if(node.leaf1!=-1)
    			{
    				std::vector<unsigned char> path1 = path;
    				path1.push_back(false);
    				g(referenceVec[node.leaf1],path1);
    			}

    			if(node.leaf2!=-1)
    			{
    				std::vector<unsigned char> path2 = path;
    				path2.push_back(true);
    				g(referenceVec[node.leaf2],path2);
    			}

    			if((node.leaf1 == -1) && (node.leaf2 == -1))
    			{
    				encodeDirect[node.data]=path;
    				encodeDirectSize[node.data]=path.size();
    			}

    		};

    		std::vector<unsigned char> rootPath;
    		g(root,rootPath);

    		if(debug)
    		{
    			std::cout<<"-------------------------------------"<<std::endl;

    			for(int i=0;i<256;i++)
    			{
    				if(encodeDirect[i].size()>0)
    				{
    					std::cout<<(unsigned char)i<<": ";
    					for(const auto & f: encodeDirect[i])
    					{
    						std::cout<<(bool)f<<" ";
    					}
    					std::cout<<std::endl;
    				}
    			}
    			std::cout<<"-------------------------------------"<<std::endl;
    		}
    	}

    	size_t getCount(unsigned char data)
    	{
    		return referenceMapDirect[data];
    	}

    	inline
    	const std::vector<unsigned char> & generateBits(unsigned char data) const noexcept
    	{
    		return encodeDirect[data];
    	}

    	inline
    	const int & generateBitsSize(unsigned char data) const noexcept
    	{
    		return encodeDirectSize[data];
    	}


    	inline
    	const unsigned char followBitsDirect(const Node * __restrict__ const refVec, const unsigned char * __restrict__ const path, size_t & idx, const size_t & ofs) const
    	{
    		unsigned char result;
    		const Node * curNode=&root;
    		unsigned char work=true;

    		while(work)
    		{
    			int p = loadBit(path[(idx>>3)-ofs],idx&7);
    			if(curNode->isLeaf)
    			{
    				result=curNode->data;
    				work=false;
    			}
    			else
    			{
    				curNode = refVec+(curNode->leaf2*p + curNode->leaf1*(1-p));
    				idx++;
    			}
    		}
    		return result;
    	}

    	const Node * getRefData() const { return referenceVec.data(); }



    	/*
    	 * changes checkpoint vector of this object to be used for consume() later
    	 * (same object must use consume on the produced data)
    	 */
        std::vector<unsigned char> produce(std::string str, size_t& szTotal, const size_t checkResolutionPrm=256)
        {
        	checkPointResolution = checkResolutionPrm;
        	checkPoint.clear();
        	const size_t checkResolutionM1 = checkPointResolution-1;
            szTotal=0;
            std::vector<unsigned char> result;

            unsigned char data = 0;
            unsigned char needWrite = false;
            int pos = 0;
            const size_t szStr = str.size();
            checkPoint.reserve(szStr/checkPointResolution);

            std::vector<unsigned char> vec;
            size_t sz=0;

            {

				for(size_t s = 0;s<szStr;s++ )
				{
					sz+=generateBitsSize(str[s]);
				}
            }

            vec.reserve(sz);
            size_t szCur=0;
            {

            	const size_t szStr8 = szStr-szStr%8;

            	// todo: fix 256-bit long stream


				for(size_t s = 0;s<szStr;s++ )
				{
					const size_t & szTmp = generateBitsSize(str[s]);
					if((s&checkResolutionM1) == 0)
					{
						checkPoint.push_back(szCur);
					}
					const auto & vecTmp =  generateBits(str[s]);
					for(size_t i=0;i<szTmp;i++)
					{
						vec[szCur+i]=vecTmp[i];
					}
					szCur+=szTmp;
				}
            }

            szTotal+=sz;
            result.reserve(szTotal);
            const size_t sz8 = sz-sz%8;
            {

				for(size_t i=0;i<sz8;i+=8)
				{
					unsigned char data = 0;

					storeBit(data,vec[i], 0);
					storeBit(data,vec[i+1], 1);
					storeBit(data,vec[i+2], 2);
					storeBit(data,vec[i+3], 3);
					storeBit(data,vec[i+4], 4);
					storeBit(data,vec[i+5], 5);
					storeBit(data,vec[i+6], 6);
					storeBit(data,vec[i+7], 7);

					result.push_back(data);

				}
            }

            {

				for(size_t i=sz8;i<sz;i++)
				{
					storeBit(data,vec[i], pos++);
					needWrite=true;
					if(pos==8)
					{
						result.push_back(data);
						pos=0;
						data=0;
						needWrite=false;

					}
				}
            }

            if(needWrite)
                result.push_back(data);


            return result;
        }

        std::string consume(std::vector<unsigned char> bits, size_t szTotal, const size_t& index=0, const size_t& numChars=0)
        {
            const Node * const nodePtr = getRefData();
            const size_t checkM1 = checkPointResolution-1;
            std::string resultStr;
            size_t idx = 0;
            size_t ofs = 0;
            size_t checkPointResolutionClosestLeft = checkPoint[index / checkPointResolution];


            const size_t szLast = szTotal + idx;
            const unsigned char * const ptr = bits.data();
            if(numChars == 0)
            {
            	resultStr.reserve((checkPoint.size()+1)*checkPointResolution);
				while(idx<szLast)
				{
					   resultStr += followBitsDirect(nodePtr, ptr,  idx,ofs);
				}
            }
            else
            {
            	resultStr.reserve(numChars);
                size_t charCtr = index%checkPointResolution;
                while(charCtr>0)
                {
                       followBitsDirect(nodePtr, ptr,  checkPointResolutionClosestLeft,ofs);
                       charCtr--;
                }
                charCtr=0;
				while(charCtr<numChars)
				{
					   resultStr += followBitsDirect(nodePtr,ptr,  checkPointResolutionClosestLeft,ofs);
					   charCtr++;
				}
            }
            return resultStr;

        }

        inline
        const std::vector<unsigned char> serializeReferenceMapDirect()
		{
        	std::vector<unsigned char> result(256*sizeof(size_t));
        	std::memcpy(result.data(),&referenceMapDirect[0],256*sizeof(size_t));
        	return result;
		}

        inline
        const std::vector<unsigned char> serializeEncodeDirectSize()
		{
        	std::vector<unsigned char> result(256*sizeof(int));
        	std::memcpy(result.data(),&encodeDirectSize[0],256*sizeof(int));
        	return result;
		}

        inline
        const std::vector<unsigned char> serializeEncodeDirect()
		{
        	std::vector<unsigned char> result;
        	size_t totSz = 0;
        	for(int i=0;i<256;i++)
        	{
        		unsigned char sz = encodeDirect[i].size();
        		totSz+=(sz+1);
        	}

        	result.resize(totSz);
        	size_t ctr=0;
        	for(int i=0;i<256;i++)
			{
				unsigned char sz = encodeDirect[i].size();
				std::memcpy(result.data()+ctr,&sz,1);
				ctr++;
				if(sz>0)
				{
					std::memcpy(result.data()+ctr,encodeDirect[i].data(),sz);
					ctr+=sz;
				}
			}
        	return result;
		}

        inline
        const std::vector<unsigned char> serializeRoot()
		{
        	std::vector<unsigned char> result(sizeof(Node));
        	std::memcpy(result.data(),&root,sizeof(Node));
        	return result;
		}

        inline
        const std::vector<unsigned char> serializeReferenceVec()
		{
        	std::vector<unsigned char> result(sizeof(size_t) + (referenceVec.size()*sizeof(Node)));
        	size_t sz = referenceVec.size();
        	std::memcpy(result.data(),&sz,sizeof(size_t));
        	std::memcpy(result.data()+sizeof(size_t),referenceVec.data(),referenceVec.size()*sizeof(Node));
        	return result;
		}

        inline
        const std::vector<unsigned char> serializeCheckpoint()
		{
        	size_t sz = checkPoint.size();
        	std::vector<unsigned char> result(sizeof(size_t)*(sz+2));

        	std::memcpy(result.data(),&sz,sizeof(size_t));
        	std::memcpy(result.data()+sizeof(size_t),&checkPointResolution,sizeof(size_t));
        	std::memcpy(result.data()+(2*sizeof(size_t)),checkPoint.data(),sz*sizeof(size_t));
        	return result;
		}

        inline
        void deserializeReferenceMapDirect(const std::vector<unsigned char>& data, size_t& ofs)
        {
        	std::memcpy(&referenceMapDirect[0],data.data()+ofs,256*sizeof(size_t));
        	ofs+=256*sizeof(size_t);
        }

        inline
        void deserializeEncodeDirectSize(const std::vector<unsigned char>& data, size_t& ofs)
        {
        	std::memcpy(&encodeDirectSize[0],data.data()+ofs,256*sizeof(int));
        	ofs+=256*sizeof(int);
        }

        inline
        void deserializeEncodeDirect(const std::vector<unsigned char>& data, size_t& ofs)
        {

        	for(int i=0;i<256;i++)
        	{
        		unsigned char sz = data[ofs];

        		ofs++;
        		encodeDirect[i].resize(sz);
        		if(sz>0)
        		{
        			std::memcpy(encodeDirect[i].data(),data.data()+ofs,sz);
        			ofs+=sz;

        		}
        	}
        }

        inline
        void deserializeRoot(const std::vector<unsigned char>& data, size_t& ofs)
		{
        	std::memcpy(&root,data.data()+ofs,sizeof(Node));
        	ofs+=sizeof(Node);
		}

        inline
        void deserializeReferenceVec(const std::vector<unsigned char>& data, size_t& ofs)
		{
        	size_t sz;
        	std::memcpy(&sz,data.data()+ofs,sizeof(size_t));
        	referenceVec.resize(sz);
        	std::memcpy(&referenceVec[0],data.data()+ofs+sizeof(size_t),sz*sizeof(Node));
        	ofs+=(sizeof(size_t) + (sz*sizeof(Node)));
		}

        inline
        void deserializeCheckpoint(const std::vector<unsigned char>& data, size_t& ofs)
		{
        	size_t sz;
        	std::memcpy(&sz,data.data()+ofs,sizeof(size_t));
        	ofs+=sizeof(size_t);
        	checkPoint.resize(sz);
        	std::memcpy(&checkPointResolution,data.data()+ofs,sizeof(size_t));
        	ofs+=sizeof(size_t);
        	std::memcpy(checkPoint.data(),data.data()+ofs+sizeof(size_t),sz*sizeof(size_t));
        	ofs+=(sz*sizeof(size_t));
		}

        inline void clearRef(){ referenceVec.clear();}
    	~HuffmanTree(){}
    private:
    	size_t referenceMapDirect[256];
    	int encodeDirectSize[256];
    	std::vector<unsigned char> encodeDirect[256];
    	Node root;
    	std::vector<Node> referenceVec;

    	// check-point data for multiple-entrance support into compressed string
    	// resolution=256 means every 256th character's offset is retained in a vector
    	std::vector<size_t> checkPoint;
    	size_t checkPointResolution;
    };




    /*
     * DirectMappedCache.h
     *
     *  Created on: Oct 8, 2021
     *      Author: tugrul
     */

    #ifndef DIRECTMAPPEDMULTITHREADCACHE_H_
    #define DIRECTMAPPEDMULTITHREADCACHE_H_




    /* Direct-mapped cache implementation with granular locking (per-tag)
     *       Only usable for integer type keys in range [0,maxPositive-1]  (if key is int then "-1" not usable, if key is uint16_t then "65535" not usable)
     *       since locking protects only items/keys, also the user should make cache-miss functions thread-safe (i.e. adding a lock-guard)
     *       unless backing-store is thread-safe already (or has multi-thread support already)
     * Intended to be used as LLC(last level cache) for CacheThreader instances
     * 															to optimize contentions out in multithreaded read-only scenarios
     * Can be used alone, as a read+write multi-threaded cache using getThreadSafe setThreadSafe methods but cache-hit ratio will not be good
     * CacheKey: type of key (only integers: int, char, size_t)
     * CacheValue: type of value that is bound to key (same as above)
     * InternalKeyTypeInteger: type of tag found after modulo operationa (is important for maximum cache size. unsigned char = 255, unsigned int=1024*1024*1024*4)
     */
    template<	typename CacheKey, typename CacheValue, typename InternalKeyTypeInteger=size_t>
    class DirectMappedMultiThreadCache
    {
    public:
    	// allocates buffers for numElements number of cache slots/lanes
    	// readMiss: 	cache-miss for read operations. User needs to give this function
    	// 				to let the cache automatically get data from backing-store
    	//				example: [&](MyClass key){ return redis.get(key); }
    	//				takes a CacheKey as key, returns CacheValue as value
    	// writeMiss: 	cache-miss for write operations. User needs to give this function
    	// 				to let the cache automatically set data to backing-store
    	//				example: [&](MyClass key, MyAnotherClass value){ redis.set(key,value); }
    	//				takes a CacheKey as key and CacheValue as value
    	// numElements: has to be integer-power of 2 (e.g. 2,4,8,16,...)
    	// prepareForMultithreading: by default (true) it allocates an array of structs each with its own mutex to evade false-sharing during getThreadSafe/setThreadSafe calls
    	//          with a given "false" value, it does not allocate mutex array and getThreadSafe/setThreadSafe methods become undefined behavior under multithreaded-use
    	//          true: allocates at least extra 64 bytes per cache tag
    	DirectMappedMultiThreadCache(CacheKey numElements,
    				const std::function<CacheValue(CacheKey)> & readMiss,
    				const std::function<void(CacheKey,CacheValue)> & writeMiss,
    				const bool prepareForMultithreading = true):size(numElements),sizeM1(numElements-1),loadData(readMiss),saveData(writeMiss)
    	{
    		if(prepareForMultithreading)
    			mut = std::vector<MutexWithoutFalseSharing>(numElements);
    		// initialize buffers
    		for(size_t i=0;i<numElements;i++)
    		{
    			valueBuffer.push_back(CacheValue());
    			isEditedBuffer.push_back(0);
    			keyBuffer.push_back(CacheKey()-1);
    		}
    	}



    	// get element from cache
    	// if cache doesn't find it in buffers,
    	// then cache gets data from backing-store
    	// then returns the result to user
    	// then cache is available from RAM on next get/set access with same key
    	inline
    	const CacheValue get(const CacheKey & key)  noexcept
    	{
    		return accessDirect(key,nullptr);
    	}

    	// only syntactic difference
    	inline
    	const std::vector<CacheValue> getMultiple(const std::vector<CacheKey> & key)  noexcept
    	{
    		const int n = key.size();
    		std::vector<CacheValue> result(n);

    		for(int i=0;i<n;i++)
    		{
    			result[i]=accessDirect(key[i],nullptr);
    		}
    		return result;
    	}


    	// thread-safe but slower version of get()
    	inline
    	const CacheValue getThreadSafe(const CacheKey & key)  noexcept
    	{
    		return accessDirectLocked(key,nullptr);
    	}

    	// set element to cache
    	// if cache doesn't find it in buffers,
    	// then cache sets data on just cache
    	// writing to backing-store only happens when
    	// 					another access evicts the cache slot containing this key/value
    	//					or when cache is flushed by flush() method
    	// then returns the given value back
    	// then cache is available from RAM on next get/set access with same key
    	inline
    	void set(const CacheKey & key, const CacheValue & val) noexcept
    	{
    		accessDirect(key,&val,1);
    	}

    	// thread-safe but slower version of set()
    	inline
    	void setThreadSafe(const CacheKey & key, const CacheValue & val)  noexcept
    	{
    		accessDirectLocked(key,&val,1);
    	}

    	// use this before closing the backing-store to store the latest bits of data
    	void flush()
    	{
    		try
    		{
    			if(mut.size()>0)
    			{
    				for (size_t i=0;i<size;i++)
    				{
    					std::lock_guard<std::mutex> lg(mut[i].mut);
    					if (isEditedBuffer[i] == 1)
    					{
    						isEditedBuffer[i]=0;
    						auto oldKey = keyBuffer[i];
    						auto oldValue = valueBuffer[i];
    						saveData(oldKey,oldValue);
    					}
    				}
    			}
    			else
    			{
    				for (size_t i=0;i<size;i++)
    				{
    					if (isEditedBuffer[i] == 1)
    					{
    						isEditedBuffer[i]=0;
    						auto oldKey = keyBuffer[i];
    						auto oldValue = valueBuffer[i];
    						saveData(oldKey,oldValue);
    					}
    				}
    			}

    		}catch(std::exception &ex){ std::cout<<ex.what()<<std::endl; }
    	}

    	// direct mapped cache element access, locked
    	// opType=0: get
    	// opType=1: set
    	CacheValue const accessDirectLocked(const CacheKey & key,const CacheValue * value, const bool opType = 0)
    	{

    		// find tag mapped to the key
    		CacheKey tag = key & sizeM1;
    		std::lock_guard<std::mutex> lg(mut[tag].mut); // N parallel locks in-flight = less contention in multi-threading

    		// compare keys
    		if(keyBuffer[tag] == key)
    		{
    			// cache-hit

    			// "set"
    			if(opType == 1)
    			{
    				isEditedBuffer[tag]=1;
    				valueBuffer[tag]=*value;
    			}

    			// cache hit value
    			return valueBuffer[tag];
    		}
    		else // cache-miss
    		{
    			CacheValue oldValue = valueBuffer[tag];
    			CacheKey oldKey = keyBuffer[tag];

    			// eviction algorithm start
    			if(isEditedBuffer[tag] == 1)
    			{
    				// if it is "get"
    				if(opType==0)
    				{
    					isEditedBuffer[tag]=0;
    				}

    				saveData(oldKey,oldValue);

    				// "get"
    				if(opType==0)
    				{
    					const CacheValue && loadedData = loadData(key);
    					valueBuffer[tag]=loadedData;
    					keyBuffer[tag]=key;
    					return loadedData;
    				}
    				else /* "set" */
    				{
    					valueBuffer[tag]=*value;
    					keyBuffer[tag]=key;
    					return *value;
    				}
    			}
    			else // not edited
    			{
    				// "set"
    				if(opType == 1)
    				{
    					isEditedBuffer[tag]=1;
    				}

    				// "get"
    				if(opType == 0)
    				{
    					const CacheValue && loadedData = loadData(key);
    					valueBuffer[tag]=loadedData;
    					keyBuffer[tag]=key;
    					return loadedData;
    				}
    				else // "set"
    				{
    					valueBuffer[tag]=*value;
    					keyBuffer[tag]=key;
    					return *value;
    				}
    			}

    		}
    	}

    	// direct mapped cache element access
    	// opType 0 = get
    	// opType 1 = set
    	CacheValue const accessDirect(const CacheKey & key,const CacheValue * value, const bool opType = 0)
    	{

    		// find tag mapped to the key
    		CacheKey tag = key & sizeM1;

    		// compare keys
    		if(keyBuffer[tag] == key)
    		{
    			// cache-hit

    			// "set"
    			if(opType == 1)
    			{
    				isEditedBuffer[tag]=1;
    				valueBuffer[tag]=*value;
    			}

    			// cache hit value
    			return valueBuffer[tag];
    		}
    		else // cache-miss
    		{
    			CacheValue oldValue = valueBuffer[tag];
    			CacheKey oldKey = keyBuffer[tag];

    			// eviction algorithm start
    			if(isEditedBuffer[tag] == 1)
    			{
    				// if it is "get"
    				if(opType==0)
    				{
    					isEditedBuffer[tag]=0;
    				}

    				saveData(oldKey,oldValue);

    				// "get"
    				if(opType==0)
    				{
    					const CacheValue && loadedData = loadData(key);
    					valueBuffer[tag]=loadedData;
    					keyBuffer[tag]=key;
    					return loadedData;
    				}
    				else /* "set" */
    				{
    					valueBuffer[tag]=*value;
    					keyBuffer[tag]=key;
    					return *value;
    				}
    			}
    			else // not edited
    			{
    				// "set"
    				if(opType == 1)
    				{
    					isEditedBuffer[tag]=1;
    				}

    				// "get"
    				if(opType == 0)
    				{
    					const CacheValue && loadedData = loadData(key);
    					valueBuffer[tag]=loadedData;
    					keyBuffer[tag]=key;
    					return loadedData;
    				}
    				else // "set"
    				{
    					valueBuffer[tag]=*value;
    					keyBuffer[tag]=key;
    					return *value;
    				}
    			}

    		}
    	}


    private:
    	struct MutexWithoutFalseSharing
    	{
    		std::mutex mut;
    		char padding[64-sizeof(std::mutex) <= 0 ? 4:64-sizeof(std::mutex)];
    	};
    	const CacheKey size;
    	const CacheKey sizeM1;
    	std::vector<MutexWithoutFalseSharing> mut;

    	std::vector<CacheValue> valueBuffer;
    	std::vector<unsigned char> isEditedBuffer;
    	std::vector<CacheKey> keyBuffer;
    	const std::function<CacheValue(CacheKey)>  loadData;
    	const std::function<void(CacheKey,CacheValue)>  saveData;

    };


    #endif /* DIRECTMAPPEDMULTITHREADCACHE_H_ */


    class HuffmanFields
    {
    public:
    	HuffmanTree tree;
    	unsigned char useMultiThread;
    	size_t bitLength;
    	std::vector<unsigned char> bits; // compacted bits

        inline
        const std::vector<unsigned char> serializeBitLength()
		{
        	std::vector<unsigned char> result(sizeof(size_t));
        	std::memcpy(result.data(),&bitLength,sizeof(size_t));
        	return result;
		}

        inline
        const std::vector<unsigned char> serializeBits()
		{
        	std::vector<unsigned char> result(bits.size()+sizeof(size_t));
        	size_t sz=bits.size();

        	std::memcpy(result.data(),&sz,sizeof(size_t));
        	std::memcpy(result.data()+sizeof(size_t),bits.data(),bits.size());
        	return result;
		}

        inline
        void deserializeBitLength(const std::vector<unsigned char>& data, size_t& ofs)
		{
        	std::vector<unsigned char> result(sizeof(size_t));
        	std::memcpy(&bitLength,data.data()+ofs,sizeof(size_t));
        	ofs+=sizeof(size_t);
		}

        /*
         * output is packed binary
         */
        inline
        const void deserializeBits(const std::vector<unsigned char>& data, size_t& ofs)
		{
        	size_t sz;
        	std::memcpy(&sz,data.data()+ofs,sizeof(size_t));

        	bits.resize(sz);
        	std::memcpy(bits.data(),data.data()+ofs+sizeof(size_t),sz);
        	ofs+=(sizeof(size_t)+sz);
		}

        /*
         * from packed binary
         */
    	inline
    	const std::vector<unsigned char> serialize()
		{
    		std::vector<unsigned char> result;
    		auto v1 = tree.serializeEncodeDirect();
    		auto v2 = tree.serializeEncodeDirectSize();
    		auto v3 = tree.serializeReferenceMapDirect();
    		auto v4 = tree.serializeRoot();
    		auto v5 = tree.serializeReferenceVec();
    		auto v6 = tree.serializeCheckpoint();
    		auto v7 = serializeBitLength();
    		auto v8 = serializeBits();
    		std::copy(v1.begin(),v1.end(),std::back_inserter(result));
    		std::copy(v2.begin(),v2.end(),std::back_inserter(result));
    		std::copy(v3.begin(),v3.end(),std::back_inserter(result));
    		std::copy(v4.begin(),v4.end(),std::back_inserter(result));
    		std::copy(v5.begin(),v5.end(),std::back_inserter(result));
    		std::copy(v6.begin(),v6.end(),std::back_inserter(result));
    		std::copy(v7.begin(),v7.end(),std::back_inserter(result));
    		std::copy(v8.begin(),v8.end(),std::back_inserter(result));
    		return result;
		}

    	inline
    	void deserialize(const std::vector<unsigned char>& data)
    	{
    		size_t ofs=0;

    		tree.deserializeEncodeDirect(data,ofs);
    		tree.deserializeEncodeDirectSize(data,ofs);
    		tree.deserializeReferenceMapDirect(data,ofs);
       		tree.deserializeRoot(data,ofs);
       		tree.deserializeReferenceVec(data,ofs);
       		tree.deserializeCheckpoint(data,ofs);
       		deserializeBitLength(data,ofs);
       		deserializeBits(data,ofs);
    	}
    };

    /*
     * Huffman-Encoded string wrapper for fast+compressed access to text
     * subString() calls are cached (LRU approximation + direct mapped)
     * decoding has parallelism support (including subString())
     * encoding
     */
    class HuffmanString
    {
public:

    	/*
    	 * str: initializer string data
    	 * useMultiThreading: optional decoding speedup by multiple threads for long strings
    	 * cacheSizePrm: direct-mapped cache size for increasing reduntant indexed access performance
    	 * checkPointResolutionPrm: resolution of entrance points within compressed bits to optimize subString decoding performance (and multi-threaded decoding performance)
    	 * 					checkPointResolutionPrm=256 means there is an entrance for every 256th character in string
    	 * 					(also means worst-case of O(256) complexity to extract a character)
    	 */
    	HuffmanString(	const std::string & str=std::string(""),
    					const bool useMultiThreading=false,
    					const size_t cacheSizePrm=0,
    					const size_t checkPointResolutionPrm=256):fields(std::make_shared<HuffmanFields>())
		{
    		cacheSize = std::make_shared<size_t>(cacheSizePrm);
			size_t pickSizeCheck = 1;
			while(pickSizeCheck < checkPointResolutionPrm)
			{
				pickSizeCheck *= 2;
			}
    		checkPointResolution = std::make_shared<size_t>(pickSizeCheck);
    		if(cacheSize>0)
    		{
    			size_t pickSize = 1;
    			while(pickSize < *cacheSize)
    			{
    				pickSize *= 2;
    			}
    			*cacheSize = pickSize;
    			cacheL1 = std::make_shared<DirectMappedMultiThreadCache<size_t,unsigned char>>(
    					pickSize,
    					[&,this](size_t index)
    					{
    						return this->fields->tree.consume(this->fields->bits,this->fields->bitLength,index,1)[0];
    					},
    					[&](size_t index, unsigned char val){   } /* string is immutable, so no cache-write */
    					);
    		}
    		else
    		{
    			*cacheSize=0;
    		}

    		if(str.size()>0)
    		{
    			fields->useMultiThread = useMultiThreading;
				fields->tree.add(str);
				fields->tree.generateTree();
				size_t nano;
				{
					Bench bench(&nano);
					fields->bits = fields->tree.produce(str,fields->bitLength, *checkPointResolution);
				}
				//std::cout<<"produce:"<<nano<<"ns"<<std::endl;
				//fields->tree.clearRef();
    		}
    		else
    		{
    			fields->bitLength=0;
    		}
		}

    	HuffmanString operator +=(const std::string & str)
		{
    		HuffmanString result(this->string() + str);
    		*this=result;
    		return *this;
		}


    	HuffmanString operator +=(HuffmanString & str)
		{
    		HuffmanString result(this->string() + str.string());
    		*this=result;
    		return *this;
		}

    	// get full string without caching
    	const std::string string()
		{
    		return fields->tree.consume(fields->bits,fields->bitLength);
		}

    	// get cached/uncached substring (constructor must take cacheSize>0 to work with cache)
    	const std::string subString(size_t offset, size_t length, const bool cachedRead = false)
    	{
    		std::string result;

    		result.reserve(length);
    		const size_t last = offset+length;
    		if(cachedRead)
    		{
				for(size_t i=offset;i<last;i++)
				{

						result += cacheL1->get(i);
				}
    		}
    		else
    		{
    			result += fields->tree.consume(fields->bits,fields->bitLength,offset,length);
    		}
    		return result;
    	}

    	// get char
    	const unsigned char operator[](size_t index)
    	{
    		if(*cacheSize>0)
    			return cacheL1->get(index);
    		else
    			return fields->tree.consume(fields->bits,fields->bitLength,index,1)[0];
    	}

    	// find position of character
    	const size_t find(const unsigned char character, const bool cachedSearch=false)
    	{
    		size_t result = -1;
    		return result;
    	}


    	/*
    	 * Serialize internal data into byte vector to save into a file to be loaded by deserializeFrom() later
    	 */
    	const std::vector<unsigned char> serialize()
		{
    		return fields->serialize();
		}

    	/*
    	 * To retain a HuffmanString instance from a serialized data
    	 */
    	void deserializeFrom(const std::vector<unsigned char>& data)
    	{
    		fields->deserialize(data);
    	}

    	size_t compressedBytes()
    	{
    		return fields->bits.size();
    	}

    	~HuffmanString(){}
private:
    	std::shared_ptr<HuffmanFields> fields;
    	std::shared_ptr<DirectMappedMultiThreadCache<size_t,unsigned char>> cacheL1;
    	std::shared_ptr<size_t> cacheSize;
    	std::shared_ptr<size_t> checkPointResolution;
    };


    template<typename PrefixType=size_t>
    class PredictorFields
    {
    public:
    	const std::string decompress(size_t offset=0,size_t length=0,bool cachedRead=false)
    	{
    		std::string result;
    		const int n = 256;
    		unsigned char dict[(n*n)/8]; // todo: 8bit->1byte compression
    		int predict[n];
    		std::memset(&dict[0],0,(n*n)/8);
    		std::memset(&predict[0],-1,n*sizeof(int));

    		const size_t sz = prefix.size();
    		constexpr int szTileIn = 64;

    		alignas(64)
    		unsigned char in[szTileIn];

    		unsigned char last = 0;

    		int inCtr = 0;
    		size_t ctr=0;
    		for(size_t i=0;i<sz;i++)
			{
    			PrefixType mask = prefix[i];


				int numWork = sizeof(PrefixType)*8 - 2 /* 8 bits x 8 bytes, saving 2 bits for control logic */;

				// if no mixed work happened

				if((mask&1) == 0)
				{
					numWork = mask >> 2;

					// if all prediction happened
					if(((mask>>1)&1) == 1)
					{
						for(int j=0;j<numWork;j++)
						{
							unsigned char cur = predict[last];
							//if(result.size()<szStr)
							in[inCtr++]=cur;
							if(inCtr == szTileIn)
							{

								result.append(in,in+szTileIn);
								inCtr=0;
							}
							last = cur;
						}
					}
					else // all stream happened
					{
						for(int j=0;j<numWork;j++)
						{
							unsigned char cur = compressed[ctr++];
							//if(result.size()<szStr)
							in[inCtr++]=cur;
							if(inCtr == szTileIn)
							{

								result.append(in,in+szTileIn);
								inCtr=0;
							}
							if(predict[last]!=-1)
							{
								storeBit(dict[(last*(unsigned int)256+predict[last])>>3],0,(last*(int)256+predict[last])&7);
							}

							predict[last]=cur;
							last = cur;
						}
					}

				}
				else
				{
					mask = mask>>2;
					for(int j=0;j<numWork;j++)
					{
						// if predicted
						if(loadBit(mask,j))
						{
							unsigned char cur = predict[last];
							//if(result.size()<szStr)
							in[inCtr++]=cur;
							if(inCtr == szTileIn)
							{

								result.append(in,in+szTileIn);
								inCtr=0;
							}
							last = cur;
						}
						else
						{
							unsigned char cur = compressed[ctr++];
							//if(result.size()<szStr)
							in[inCtr++]=cur;
							if(inCtr == szTileIn)
							{

								result.append(in,in+szTileIn);
								inCtr=0;
							}
							if(predict[last]!=-1)
							{
								storeBit(dict[(last*(unsigned int)256+predict[last])>>3],0,(last*(int)256+predict[last])&7);
							}

							predict[last]=cur;
							last = cur;
						}
					}
				}
			}

    		if(inCtr>0)
    		{
    			result.append(in,in+szTileIn);
    		}


    		return result.substr(0,szStr);
    	}

    	const std::string bits()
    	{
    		return std::string((const char * )compressed.data(),compressed.size());
    	}


    	const std::string bits2()
    	{
    		return std::string((const char * )prefix.data(),prefix.size());
    	}



    	void compress(const std::string & str=std::string(""))
    	{
    		const int n = 256;

    		unsigned char dict[(n*n)/8]; // todo: 8bit->1byte compression
    		int predict[n];
    		std::memset(&dict[0],0,(n*n)/8);
    		std::memset(&predict[0],-1,n*sizeof(int));

    		const size_t sz = str.size();
    		szStr = sz;
    		unsigned char last = 0;


    		PrefixType mask = 0;
    		int pos = 0;


    		constexpr int szTileTmp = 64;
    		constexpr int szTileOut = 64;

    		alignas(64)
    		unsigned char tmp[szTileTmp];

    		alignas(64)
    		unsigned char out[szTileOut];

    		int outCtr = 0;

    		bool hasPrediction = false;
    		bool hasStream = false;
    		PrefixType countStream = 0;
    		PrefixType countPrediction = 0;



    		for(size_t i=0;i<sz;i+=szTileTmp)
    		{
    			for(int j=0;j<szTileTmp;j++)
    			{
    				tmp[j]=str[i+j];
    			}

    			for(int j=0;j<szTileTmp;j++)
    			{
    				const unsigned char cur = tmp[j];
					// if predicted, write 1 to bit
					if(loadBit(dict[(last*(unsigned int)256+cur)>>3],(last*(unsigned int)256+cur)&7))
					{
						storeBit(mask,1,pos++);
						hasPrediction=true;
						countPrediction++;
					}
					else
					{
						storeBit(mask,0,pos++);
						hasStream=true;
						countStream++;
						//compressed.push_back(cur);
						out[outCtr++]=cur;
						if(outCtr==szTileOut)
						{
							compressed.insert(compressed.end(),out,out+szTileOut);
							outCtr=0;
						}

						if(predict[last]!=-1)
						{
							storeBit(dict[(last*(unsigned int)256+predict[last])>>3],0,(last*(unsigned int)256+predict[last])&7);
						}

						predict[last]=cur;
						storeBit(dict[(last*(unsigned int)256+cur)>>3],1,(last*(unsigned int)256+cur)&7);
					}


					last = cur;


					if(pos==(sizeof(PrefixType)*8 - 2)) // -2: saving 2 bits for control logic
					{

						if(hasPrediction && !hasStream)
						{
							// only prediction happened, save only number of predictions
							mask=(countPrediction<<2)|2; // lsb+1 = 1 means all predictions
							countPrediction=0;
						}
						else if(!hasPrediction && hasStream)
						{
							// only stream happened, save only number of streams
							mask=(countStream<<2); // lsb+1 = 0 means all streams
							countStream=0;
						}
						else
						{

							// no possibility to have nothing
							// both prediction & stream happened
							// save normally
							countStream=0;
							countPrediction=0;
							mask = (mask<<2)|1; // lsb=1 means mixed work
						}

						prefix.push_back(mask);
						pos=0;
					}


    			}

    		}

			if(outCtr>0)
			{
				compressed.insert(compressed.end(),out,out+szTileOut);
			}

    		if(pos>0)
    		{

				if(hasPrediction && !hasStream)
				{
					// only prediction happened, save only number of predictions
					mask=(countPrediction<<2)|2; // lsb+1 = 1 means all predictions
					countPrediction=0;
				}
				else if(!hasPrediction && hasStream)
				{
					// only stream happened, save only number of streams
					mask=(countStream<<2); // lsb+1 = 0 means all streams
					countStream=0;
				}
				else
				{

					// no possibility to have nothing
					// both prediction & stream happened
					// save normally
					countStream=0;
					countPrediction=0;
					mask = (mask<<2)|1; // lsb=1 means mixed work
				}

				prefix.push_back(mask);
				pos=0;

    		}
    		prefix.shrink_to_fit();
    		compressed.shrink_to_fit();

    	}

private:
    	std::vector<PrefixType> prefix;
    	std::vector<unsigned char> compressed;
    	size_t szStr;
    };

    template<typename PrefixType=size_t>
    class PredictorString
    {
    public:
    	PredictorString(const std::string & str=std::string("")):fields(std::make_shared<PredictorFields<PrefixType>>())
    	{
    		fields->compress(str);
    	}

    	// gets compressed character stream without prefixes
    	const std::string bits()
    	{
    		return fields->bits();
    	}

    	// gets prefix stream for decompression
    	const std::string bits2()
    	{
    		return fields->bits2();
    	}

    	const std::string string()
    	{
    		return fields->decompress();
    	}


    	// do not use this to add small strings. inefficient
    	PredictorString operator +=(const std::string & str)
		{
    		PredictorString result(this->string() + str);
    		*this=result;
    		return *this;
		}

    	// do not use this to add small strings. inefficient
    	PredictorString operator +=(PredictorString & str)
		{
    		PredictorString result(this->string() + str.string());
    		*this=result;
    		return *this;
		}


    	// todo: implement cached load & multithreading
    	const std::string subString(size_t offset, size_t length, const bool cachedRead = true)
    	{
    		std::string result=this->fields->decompress(offset,length,cachedRead).substr(offset,length);
    		return result;
    	}

    	// todo: implement cached load & multithreading
    	const unsigned char operator[](size_t index)
    	{
    		return this->fields->decompress(index,1,true)[index];
    	}

    	// todo: implement multithreaded find
    	// returns size_t max if not found
    	const size_t find(const unsigned char character, const bool cachedSearch=false)
    	{
    		const std::string v = this->fields->decompress(0,0,true);
    		auto test = v.find(character);
    		if(test == v.npos)
    			return (size_t)-1;
    		else
    			return test;
    	}


    private:
    	std::shared_ptr<PredictorFields<PrefixType>> fields;
    };
}

class RunLengthEncoderString
{
public:
	RunLengthEncoderString(std::string str=std::string(""))
	{
		const size_t sz = str.size();


		for(size_t i=0;i<sz-2;i++)
		{
			if(sub.find(str.substr(i,2)) == sub.end())
			{
				sub[str.substr(i,2)] = 0;
			}
			else
			{
				sub[str.substr(i,2)]++;
			}
		}
	}

	std::vector<unsigned char> string()
	{
		std::map<std::string,size_t> tmp;
		size_t ct=0;
		for(auto & it:sub)
		{


			if(it.second>=2)
			{
				tmp[it.first]=it.second;
				std::cout<<it.first<<":"<<it.second<<std::endl;
				ct++;
			}
		}
		std::cout<<"="<<ct<<std::endl;
		return rle;
	}
private:
	std::vector<unsigned char> rle;
	std::map<std::string,size_t> sub;
};




#endif /* COMPRESSSTRINGLIB_H_ */
