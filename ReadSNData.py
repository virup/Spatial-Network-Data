'''
Created on Oct 2, 2012

@author: virup
'''
import xml.etree.cElementTree as ET
import haversine
INVALID_LOC = -200
EARTH_RADIUS = 6367 


def read_data():
    tree = ET.parse('map.osm')
    root = tree.getroot()
    print (root.tag)


    nodePosMap = {}
    for child in root.findall('node'):
        lon = child.get("lon")
        lat = child.get("lat")
        nodeID = child.get("id")
        nodePosMap[nodeID] = [lat, lon]

    print(len(nodePosMap))
    for child in root.findall('way'):
        uid = child.get("uid")
        wayID = child.get("id")
        print ("WayID  = "+ wayID)
        for tag in child.getiterator('tag'):
            k = tag.get('k')
            if('name' ==  k):
                print(tag.get('v'))

        length = 0
        firstNode = True;
        earlierPos = [0,0]

        for nodeRef in child.getiterator('nd'):
            internalNodeID = nodeRef.get('ref')
            position = nodePosMap.get(internalNodeID, INVALID_LOC)
            if firstNode == True:
                earlierPos = position
                firstNode = False
            else:
                x1 = float(earlierPos[0])
                y1 = float(earlierPos[1])
                x2 = float(position[0])
                y2 = float(position[1])
                length = length + haversine.distance([x1,y1], [x2,y2])
        print(length)

if __name__ == '__main__':
    read_data()
