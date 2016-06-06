import struct

from citygen import RoadNode, make_city

height, population, centers, segs = make_city()
intersections = [(node.loc.x, node.loc.y) for node in RoadNode.nodes.values() if node.is_intersection]

flatten = lambda l: [x for v in l for x in v]

def export_floats(l, f):
    f = open(f, 'w+')
    for v in l:
        f.write(struct.pack('f', v))
    f.close()


# f.write("P6\n%d %d\n255\n" % (width, width))

export_floats(flatten(height), 'res/terrain/citygen_heightmap')

roads = []
for r in segs:
    roads += [r.a.loc.x, r.a.loc.y, r.b.loc.x, r.b.loc.y]

export_floats(roads, "res/terrain/citygen_roads")
