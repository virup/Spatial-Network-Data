#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <cstdlib>
using namespace std;


#define MAXLENGTH 100

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
    point left;
    point right;

    segment(double lat1, double longitude1, double lat2, double longitude2):left(lat1, longitude1), right(lat2, longitude2)
    {
    }

    segment(point l, point r):left(l), right(r)
    {
    }
};

struct channel
{
    int channelID;
    string channelName;
    double channelLength;
    string channelType;
    int noOfSegments;
    vector<segment> segments;
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

        //file>>channelData->channelID>>channelData->channelName>>channelData->channelLength>>channelData->channelType>>noOfSegments;
        file.getline(tempValue, MAXLENGTH,',');c->channelID = atoi(tempValue);
        file.getline(tempValue,MAXLENGTH, ',');c->channelName = tempValue;
        file.getline(tempValue, MAXLENGTH,',');c->channelLength = atof(tempValue);
        cout<<c->channelLength<<endl;
        file.getline(tempValue, MAXLENGTH,',');c->channelType = tempValue;
        file.getline(tempValue, MAXLENGTH,',');c->noOfSegments = atoi(tempValue);
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

void printData(vector<struct channel *> network, vector<junction *> junctionList)
{
    for(int i =0 ; i < network.size(); i++)
    {
        struct channel *c = network[i];
        cout<<c->channelID<<", "<<c->channelName<<", "<<c->channelLength<<", "<<c->channelType<<", "<<c->noOfSegments<<endl;
    }


    for(int i = 0; i < junctionList.size(); i++)
    {
        junction *j = junctionList[i];
        cout<<j->junctionID<<", "<<j->channel1ID<<", "<<j->channel2ID<<", "<<j->junctionLocation.latitude<<","<<j->junctionLocation.longitude<<endl;
    }
}


int main()
{
    vector<struct channel *>  *channelList = readChannel("networkdata");
    cout<<"vector size = "<<channelList->size();
    vector<junction *> *junctionList = readJunction("junctiondata");
    printData(*channelList, *junctionList);
}



