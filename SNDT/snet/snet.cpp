#include "TSS.h"
#include "UDT.h"
#include "snet.h"
#include <string.h>
#include <cstdlib>


#define MAXLENGTH 100

class snet::snetImpl
{
    private:
        const std::string grammarFile;
        const TSS t;

    public:
        snetImpl():grammarFile("snet/snet.tss"), t(grammarFile, true){}
        void readNetworkDataFromFile(string filename, iBlob *);
        double getChannelLength(string name, iBlob *);
        double length(iBlob *);
        void print(iBlob *);
};

string itoa(int Number)
{
   return static_cast<ostringstream*>( &(ostringstream() << Number) )->str();
}

struct point
{
    double latitude;
    double longitude;

    point()
    {
    }

    point(double lat, double longitude)
    {
        this->latitude = lat;
        this->longitude = longitude;
    }

    point(point &p)
    {
        this->latitude = p.latitude;
        this->longitude = p.longitude;
    }
};

struct segment
{
    public:
    point left;
    point right;

    segment(double lat1, double longitude1, double lat2, double longitude2):left(lat1, longitude1), right(lat2, longitude2)
    {
    }

    segment(point l, point r):left(l), right(r)
    {
    }

    void print()
    {
        cout<<"("<<left.latitude<<","<<left.longitude<<")  ("<<right.latitude<<","<<right.longitude<<")  "<<endl;
    }
};

struct channel
{
    int channelID;
    string channelName;
    double channelLength;
    string channelType;
    int noOfSegments;
    vector<segment*> *segments;
};

struct junction
{
    int junctionID;
    int channel1ID;
    int channel2ID;
    point junctionLocation;
    void setPoint(double lat, double longitude)
    {
        this->junctionLocation.latitude = lat;
        this->junctionLocation.longitude = longitude;
    }
};

vector<struct channel *> *readChannel(string filename)
{
    ifstream file(filename.c_str());
    vector<struct channel *>  *channelList = new vector<struct channel *>();

    while(!file.eof())
    {
        struct channel *c = new channel();
        char tempValue[MAXLENGTH];

        file.getline(tempValue, MAXLENGTH,',');c->channelID = atoi(tempValue);
        file.getline(tempValue,MAXLENGTH, ',');c->channelName = tempValue;
        file.getline(tempValue, MAXLENGTH,',');c->channelLength = atof(tempValue);
        file.getline(tempValue, MAXLENGTH,',');c->channelType = tempValue;
        file.getline(tempValue, MAXLENGTH,',');c->noOfSegments = atoi(tempValue);

        c->segments = new vector<segment *>;

        for(int i =0; i < c->noOfSegments; i++)
        {
            file.getline(tempValue, MAXLENGTH,',');double latitude1 = atof(tempValue);
            file.getline(tempValue, MAXLENGTH,',');double longitude1 = atof(tempValue);
            file.getline(tempValue, MAXLENGTH,',');double latitude2 = atof(tempValue);
            file.getline(tempValue, MAXLENGTH,',');double longitude2 = atof(tempValue);
            segment *s = new segment(latitude1, longitude1, latitude2, longitude2);
            c->segments->push_back(s);
        }

        channelList->push_back(c);
    }
    file.close();
    return channelList;
}


vector<junction *> *readJunction(string filename)
{
    ifstream file(filename.c_str());
    vector<struct junction *>  *junctionList = new vector<struct junction *>();

    while(!file.eof())
    {
        junction *j = new junction();
        char tempValue[MAXLENGTH];

        double lat, longitude;

        file.getline(tempValue, MAXLENGTH, ','); j->junctionID = atoi(tempValue);
        file.getline(tempValue, MAXLENGTH, ','); j->channel1ID = atoi(tempValue);
        file.getline(tempValue, MAXLENGTH, ','); j->channel2ID = atoi(tempValue);
        file.getline(tempValue, MAXLENGTH, ','); lat = atof(tempValue);
        file.getline(tempValue, MAXLENGTH, ','); longitude = atof(tempValue);
        j->setPoint(lat, longitude);

        junctionList->push_back(j);
    }
    file.close();
    return junctionList;

}

snet::snet():network(new snetImpl)
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

double snet::getChannelLength(string name)
{
    if(!isStorageAllocated())
        return -1.0;
    return network->getChannelLength(name, store);
}

double snet::length()
{
    if(!isStorageAllocated())
        return -1;
    return network->length(store);
}

void snet::snetImpl::readNetworkDataFromFile(string filename, iBlob *store)
{
    vector<channel *>  *channelList = readChannel("networkdata");
    vector<junction *> *junctionList = readJunction("junctiondata");
    Path channel = t.createPath("snet.vchannel", store);
    channel.print();

    for(unsigned int i =0; i < channelList->size(); i++)
    {
        if(i%100==0)
            cout<<"Doing channel Number "<<i<<endl;
        string channelIndex = "[" + itoa(i) + "]";
        Path thisChannel = channel + channelIndex;
        Path thisChannelID = thisChannel+"id_attr";
        Path thisChannelName = thisChannel+"name";
        Path thisChannelLength = thisChannel+"length";
        Path thisChannelSegment = thisChannel+"vSegment";
        Path thisChannelJunction = thisChannel + "vJunctionPtr";

        ::channel* chnl = (*(channelList))[i];


        thisChannelID.set(chnl->channelID);
        char name[100];
        strcpy(name, chnl->channelName.c_str());
        thisChannelName.set(name, chnl->channelName.length()+1);
        thisChannelLength.set(chnl->channelLength);

        for(int segIdx = 0; segIdx<chnl->segments->size(); segIdx++)
        {
            string segmentIndex = "[" + itoa(segIdx) + "]";
            Path thisSegment = thisChannelSegment + segmentIndex;
            Path startPt = thisSegment+"startPt";
            Path startPtLat = startPt + "lat";
            Path startPtLong = startPt + "long";

            Path endPt = thisSegment + "endPt";
            Path endPtLat = endPt + "lat";
            Path endPtLong = endPt + "long";

            segment *s = (*(chnl->segments))[segIdx];
            startPtLat.set(s->left.latitude);
            startPtLong.set(s->left.longitude);

            endPtLat.set(s->right.latitude);
            endPtLong.set(s->right.longitude);
        }
    }
    cout<<"No of channels = " <<channel.count()<<endl;
    cout<<"Done inserting ! "<<endl;
}


double snet::snetImpl::length(iBlob* store)
{
   Path channelPath = t.createPath("snet.vchannel", store);
   int noOfChannels = channelPath.count();
   cout<<"No of channels = "<<noOfChannels<<endl;
   double totalLength = 0;

   channelPath.print();
   for(int i =0;i < noOfChannels; i++)
   {
       if(i%100 == 0)
           cout<<"Channel no : "<<i<<endl;
       string channelIdx = "[" + itoa(i)+ "]";
       Path thisChannel = channelPath + channelIdx;
       Path channelLength = thisChannel + "length";
       double thisChannelLength = 0;
       channelLength.read(thisChannelLength);
       totalLength += thisChannelLength;
   }
   return totalLength;
}

double snet::snetImpl::getChannelLength(string name, iBlob* store)
{
    return 0;
}
