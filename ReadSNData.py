'''
Created on Oct 2, 2012

@author: virup
'''
import xml.etree.cElementTree as ET
import haversine
import numpy as np
from array import array

INVALID_LOC = -200
EARTH_RADIUS = 6367

nodePosMap = {}
wayMap = {} # wayID -> (name, nodeList, length)
posMap = {} # segment -> name
junctionMap = {}

def read_data():
    tree = ET.parse('map.osm')
    root = tree.getroot()
    print (root.tag)


    for child in root.findall('node'):
        lon = child.get("lon")
        lat = child.get("lat")
        nodeID = child.get("id")
        nodePosMap[nodeID] = [lat, lon]

    wayID = -1
    name = "DEFAULT"
    for child in root.findall('way'):
        wayID = -1
        name = "DEFAULT"
        uid = child.get("uid")
        wayID = child.get("id")
        for tag in child.getiterator('tag'):
            k = tag.get('k')
            if('name' ==  k):
                name = (tag.get('v'))

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
                posMap[(x1,y1,x2,y2)] = wayID
        wayMap[wayID] = (name, posMap, length)

def countData():
    print(len(wayMap))
    for i in wayMap:
        print (i, wayMap[i][0], wayMap[i][2])


def perp( a ) :
    b = np.empty_like(a)
    b[0] = -a[1]
    b[1] = a[0]
    return b

# line segment a given by endpoints a1, a2
# line segment b given by endpoints b1, b2
# return
def seg_intersect(a1,a2, b1,b2) :
    da = a2-a1
    db = b2-b1
    dp = a1-b1
    dap = perp(da)
    denom = np.dot( dap, db)
    num = np.dot( dap, dp )
    if (denom == 0):
        return None
    return (num / denom)*db + b1

def isJunction(seg1, seg2):
    p1 = np.array([seg1[0], seg1[1]], dtype=np.float32)
    p2 = np.array([seg1[2], seg1[3]], dtype=np.float32)

    p3 = np.array([seg2[0], seg2[1]], dtype=np.float32)
    p4 = np.array([seg2[2], seg2[3]], dtype=np.float32)

    return (seg_intersect(p1, p2, p3, p4))

def findJunctions():
    junctionCount = 0
    print len(posMap)

    for segment in posMap:
        for seg2 in posMap:

            if(segment == seg2):
                continue
            if(isJunction(segment, seg2)!= None):
                junctionMap[junctionCount] = (posMap[segment][0],
                        segment, posMap[seg2][0], seg2)
                junctionCount = junctionCount + 1
                if(junctionCount%1000000==0):
                    print junctionCount 


if __name__ == '__main__':
    read_data()
    countData()
    findJunctions()
 # seg1 = [0,0,10,10]
 # seg2 = [1,1,11,11]
 # isJunction(seg1, seg2)
