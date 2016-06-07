from matplotlib.cm import get_cmap, ScalarMappable
from matplotlib.colors import Normalize
from matplotlib.pyplot import imshow, contour, axis, show, scatter
from numpy import linspace

from citygen import RoadNode, make_city, size

height, population, centers, segs, paths = make_city()
intersections = [(node.loc.x, node.loc.y) for node in RoadNode.nodes.values() if node.is_intersection]

imshow(population, origin='lower', cmap='gray', interpolation='none')
CS = contour(range(size), range(size), height, levels=linspace(0, 1, 15), colors='w')
cm = get_cmap('gist_rainbow')
cNorm = Normalize(vmin=0, vmax=len(centers) - 1)
scalarMap = ScalarMappable(norm=cNorm, cmap=cm)
axis('off')

for seg in segs:
    seg.render()

for path in paths:
    path[0].render(color='w-')
    for seg in path[1:]:
        seg.render(color='b-')

ix, iy = zip(*intersections)
scatter(ix, iy, c='w', s=25, zorder=2)
# scatter(res[:, 0], res[:, 1], c=map(scalarMap.to_rgba, range(len(res))), s=25, zorder=3)
show()
