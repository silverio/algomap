""" Parses OpenStreetMap *.osm file extracting points of interest and
    writing them into a simple text file
"""
import sys
import xml.etree.ElementTree as xml

POI_TYPES = {
    "amenity" : set(["cinema", "pub", "parking", "fuel", "cafe", "restaurant",\
                         "theatre", "post_office", "place_of_worship", "hotel", \
                         "fuel", "toilets", "bench", \
                         "taxi", "school", "pharmacy", \
                         "bar", "fountain", "fast_food", "cinema", "bicycle_rental",\
                         "atm", "gym"]),
    "natural" : set(["tree"]),
    "historic": set(["monument"]),
    "tourism" : set(["hotel"]),
    "leisure" : set(["playground"]),
    "railway" : set(["tram_stop", "subway_entrance"]),
    "highway" : set(["bus_stop", "traffic_signals", "crossing"]),
    "shop"    : set(["clothes", "kiosk", "supermarket", "bakery", "hairdresser"])
}

try:
    input_file = sys.argv[1]
    output_file = sys.argv[2]
except:
    print "Usage: %s <input osm file> <output file>" % sys.argv[0]
    exit(1)

try:
    fin = open(input_file, "r") 
    data = fin.read()
    fin.close()
except:
    print "Error loading file: %s" % input_file
    exit(1)

tree = xml.XML(data)

poi = {}

bounds = tree.find("bounds")
minlat = bounds.get("minlat")
minlon = bounds.get("minlon")
maxlat = bounds.get("maxlat")
maxlon = bounds.get("maxlon")

for node in tree.findall("node"):
    for tag in node.findall("tag"):
        for k, v in POI_TYPES.iteritems():
            if tag.get("k") == k and tag.get("v") in v:
                lat = node.get("lat")
                lon = node.get("lon")
                name = tag.get("v")
                if not name in poi:
                    poi[name] = []
                poi[name].append((lat, lon))

try:
    fout = open(output_file, "w") 
except:
    print "Error creating file: %s" % output_file
    exit(1)

fout.write("bounds: %s %s %s %s\n" % (minlat, minlon, maxlat, maxlon))
for name, points in poi.iteritems():
    fout.write("%s\n" % name)
    for lat, lon in points:
        fout.write("%s %s; " % (lat, lon))
    fout.write("\n")
    print name, len(points)

fout.close()
