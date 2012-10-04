'''
Created on Oct 2, 2012

@author: virup
'''
import xml.etree.cElementTree as ET

INVALID_LOC = -200

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

        for nodeRef in child.getiterator('nd'):
            internalNodeID = nodeRef.get('ref')
            position = nodePosMap.get(internalNodeID, INVALID_LOC)

if __name__ == '__main__':
    read_data()
