#include "TSS.h"
#include "UDT.h"
#include "snet.h"
#include <string.h>

class snet::snetImpl
{
    private:
        const std::string grammarFile;
        const TSS t;

    public:
        snetImpl():grammarFile("snet/snet.tss"), t(grammarFile, true){}
        void readNetworkDataFromFile(string filename, iBlob *);
        snet getChannel(string name, iBlob *);
        double length(string name, iBlob *);
        void print(iBlob *);
};

snet::snet():network(new snet)
{}


snet::~snet()
{
    delete network;
}

void snet::readNetworkDataFromFile(string filename)
{
    if(!isStorageAllocated())
        return;
    network->readNetworkDataFromFile(filename, store);
}

snet snet::getChannel(string name)
{
    if(!isStorageAllocated())
        return;
    return network->getChannel(string name, store);
}

double snet::length(string name)
{
    if(!isStorageAllocated())
        return -1;
    return network->length(name);
}

void snet::snetImpl::readNetworkDataFromFile(string filename, store)
{

}

double snet::snetImpl::length(string name)
{
    int noOfChannels = 
}
