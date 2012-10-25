'''
Created on Oct 2, 2012
@author: virup
'''
import xml.etree.cElementTree as ET
import haversine

INVALID_LOC = -200
EARTH_RADIUS = 6367
NOT_PRESENT = -9999

nodePosMap = {}
nodeToWayMap = {} # nodeID->wayID
wayMap = {} # wayID -> (name, nodeList, length)
posMap = {} # segment -> name
junctionMap = {}

def read_data():
    tree = ET.parse('map.osm.xml')
    root = tree.getroot()

    # Get all nodes
    for child in root.findall('node'):
        lon = child.get("lon")
        lat = child.get("lat")
        nodeID = child.get("id")
        nodePosMap[nodeID] = [lat, lon]


    # Get all ways
    wayID = -1
    name = "DEFAULT"
    jCount = 0
    for child in root.findall('way'):
        wayID = -1
        name = "DEFAULT"
        tigerName = "DEFAULT"
        isHighway = False
        roadType = "DEFAULT"
        isNamePresent = False
        uid = child.get("uid")
        wayID = child.get("id")
        for tag in child.getiterator('tag'):
            k = tag.get('k')
            if('name' ==  k):
                name = (tag.get('v'))
                isNamePresent = True
            if('highway' ==  k):
                isHighway = True
                roadType = tag.get('v')
            if('tiger:name_base' ==  k):
                tigerName = (tag.get('v'))
                isNamePresent = True

        if(isHighway == False or isNamePresent == False):
            continue

        if(name == "DEFAULT"):
            name = tigerName

        length = 0
        firstNode = True;
        earlierPos = [0,0]
        segmentList = []

        for nodeRef in child.getiterator('nd'):
            internalNodeID = nodeRef.get('ref')
            position = nodePosMap.get(internalNodeID, INVALID_LOC)
            if(nodeToWayMap.get(internalNodeID, NOT_PRESENT) == NOT_PRESENT):
                nodeToWayMap[internalNodeID] = wayID
            else:
                junctionMap[jCount] = (wayID, nodeToWayMap[internalNodeID], internalNodeID)
                jCount = jCount + 1

            if firstNode == True:
                earlierPos = position
                firstNode = False
            else:
                x1 = float(earlierPos[0])
                y1 = float(earlierPos[1])
                x2 = float(position[0])
                y2 = float(position[1])
                length = length + haversine.distance([x1,y1], [x2,y2])
                segmentList.append([x1,y1, x2,y2])
                earlierPos = position
        wayMap[wayID] = (name, segmentList, length, roadType)

def countData():
    print("No of ways = " + str(len(wayMap)))
    datafile = open('networkdata', 'w')
    for i in wayMap:
        #print (i, wayMap[i][0], len(wayMap[i][1]), wayMap[i][2], wayMap[i][3])
        strData = "{0},{1},{2},{3},{4},\n".format(i, wayMap[i][0],wayMap[i][2],
                wayMap[i][3],len(wayMap[i][1]))
        datafile.write (strData)
    datafile.close()

    junctionFile = open('junctiondata', 'w')
    print(" =======================")
    for i in junctionMap:
        print(i, junctionMap[i][0], junctionMap[i][1])
        strData = "{0},{1},{2},{3},{4},\n".format(i, junctionMap[i][0],
                junctionMap[i][1], nodePosMap[junctionMap[i][2]][0],
                nodePosMap[junctionMap[i][2]][1])
        junctionFile.write(strData)
    junctionFile.close()

def main():
    read_data()
    countData()
    #showData("SW 20th Ave")


if __name__ == '__main__':
    main()
