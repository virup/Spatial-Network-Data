#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <cstdlib>
using namespace std;


#define MAXLENGTH 100

struct segment
{
    double lptlat;
    double lptlong;
    double rptlat;
    double rptlong;
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


vector<struct channel *> *readData(string filename)
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

void printData(vector<struct channel *> network)
{
    for(int i =0 ; i < network.size(); i++)
    {
        struct channel *c = network[i];
        cout<<c->channelID<<", "<<c->channelName<<", "<<c->channelLength<<", "<<c->channelType<<", "<<c->noOfSegments<<endl;
    }
}


int main()
{
    vector<struct channel *>  *channelList = readData("networkdata");
    cout<<"vector size = "<<channelList->size();
    printData(*channelList);
}



