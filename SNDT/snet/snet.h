#include "TSS.h"
#include "UDT.h"

#include <string.h>

class snet:public UDT
{
    private:
        class snetImpl;
        snetImpl *network;

    public:
        snet();
        void readNetworkDataFromFile(string filename);
        snet getChannel(string name);
        double length();
        void print();
        ~snet();
};
