from matplotlib.cm import ScalarMappable, get_cmap
from matplotlib.colors import Normalize
from matplotlib.pyplot import contour, show, imshow, scatter, triplot, plot, axis
from numpy import roll, linspace, meshgrid, gradient, sqrt, array, exp, argmax, clip
from numpy.core.multiarray import unravel_index
from numpy.fft.fftpack import fft2
from numpy.ma.core import absolute
from numpy.random.mtrand import rand, shuffle
from planar import Vec2
from rtree.index import Index
from scipy.cluster.vq import kmeans2
from scipy.interpolate import RectBivariateSpline
from scipy.ndimage import gaussian_filter
from scipy.spatial.qhull import Delaunay

size = 512


def in_bounds(x):
    try:
        return all(map(in_bounds, x))
    except TypeError:
        return 0 <= x < size


# Generate a heightmap using
def noise(res=size, lin=100000, exp=1.2):
    realnoise = 2 * rand(res, res) - 1
    imagnoise = 2 * rand(res, res) - 1
    complexnoise = realnoise + imagnoise * 1j
    dx, dy = meshgrid(linspace(-1, 1, size), linspace(-1, 1, size))
    dists = dx * dx + dy * dy
    nf = (lin * dists + 0.1) ** (-exp)
    complexnoise *= nf
    m = fft2(complexnoise)
    m = absolute(m)
    m -= m.min()
    m /= m.max()
    return m


height = noise()


def smooth(x):
    x = clip(x, 0, 1)
    return x * x * x * (x * (x * 6 - 15) + 10)


# Find the lowest point on the map and move it into the middle

def center_valley(m):
    offset = unravel_index(m.argmin(), m.shape)
    m = roll(m, m.shape[0] / 2 - offset[0], axis=0)
    m = roll(m, m.shape[1] / 2 - offset[1], axis=1)
    return m


height = center_valley(height)

# Flatten out the low areas
height = pow(height, 2)

dx, dy = meshgrid(linspace(-1, 1, size), linspace(-1, 1, size))
dists = dx * dx + dy * dy
smooth_clamp = smooth(1 - dists)

# Flatten the edges of the heightmap
height *= smooth_clamp

dx, dy = gradient(height)
steepness = sqrt(dx * dx + dy * dy)

# Generate a population map
population = noise(size, exp(20), 1.2)

# Flatten it around the edges too
population *= smooth_clamp

# Inhibit population in steep areas
population /= (1 + steepness * size)

# Smooth the map
population = gaussian_filter(population, 5)

population = pow(population, 2)
population /= population.max()
pop_spline = RectBivariateSpline(range(size), range(size), population)

pop_samples = []
while len(pop_samples) < size:
    x, y = rand() * size, rand() * size
    if population[int(y), int(x)] * 2 > rand():
        pop_samples.append((x, y))

pop_samples = array(pop_samples)
# Pick some population centers using k-means
res, idx = kmeans2(pop_samples, 5)
res = array(filter(in_bounds, list(res)))

# Triangulate the resulting points to get highway edges
triangles = Delaunay(res)

# Plot the population map, heightmap contour, population samples, and highway edges
imshow(population.tolist(), origin='lower', cmap='gray', interpolation='none')
CS = contour(range(size), range(size), height, levels=linspace(0, 1, 15), colors='w')
cm = get_cmap('gist_rainbow')
cNorm = Normalize(vmin=0, vmax=len(res) - 1)
scalarMap = ScalarMappable(norm=cNorm, cmap=cm)
axis('off')
scatter(pop_samples[:, 0], pop_samples[:, 1], c=map(scalarMap.to_rgba, idx), s=1, edgecolors='none')
triplot(res[:, 0], res[:, 1], triangles.simplices, c='y')

# Now generate roads

node_table = []
node_index = Index()


# Get the nearest node and intersection to the given point
def get_nearest(l, exclude=[]):
    n, n_i = None, None
    c = 100
    while 1:
        for e in node_index.nearest((l.x, l.y, l.x, l.y), c):
            if node_table[e] not in exclude:
                if not n_i and node_table[e].is_intersection:
                    n_i = node_table[e]
                if not n:
                    n = node_table[e]
            if n and n_i: return n, n_i
        c *= 2


def intersection_density(l):
    n = 0
    for i in node_index.nearest((l.x, l.y, l.x, l.y), 1000):
        e = node_table[i]
        if not e.is_intersection: continue
        if l.distance_to(e.loc) < 40:
            n += 1
    return n


def register_item(o, l):
    node_index.insert(len(node_table), (l.x, l.y, l.x, l.y))
    node_table.append(o)


class RoadNode(object):
    nodes = {}

    def __init__(self, loc):
        self.loc = loc
        self.segments = []
        self.roads = set()
        register_item(self, loc)

    @classmethod
    def build(cls, loc):
        l = loc.x, loc.y
        if l not in cls.nodes:
            cls.nodes[l] = cls(loc)
        return cls.nodes[l]

    is_intersection = property(lambda self: len(self.segments) != 2)


class PopulationCenter(RoadNode):
    def __init__(self, loc):
        super(PopulationCenter, self).__init__(loc)


class RoadSegment(object):
    segs = {}

    def __init__(self, a, b):
        self.a, self.b = a, b
        self.roads = set()
        a.segments.append(self)
        b.segments.append(self)

    def render(self):
        plot([self.a.loc.x, self.b.loc.x], [self.a.loc.y, self.b.loc.y], 'r-', zorder=1)

    @classmethod
    def build(cls, a, b):
        s = cls.segs.get((a, b))
        if not s:
            s = cls(a, b)
            cls.segs[(a, b)] = s
            cls.segs[(b, a)] = s
        return s


def extend(a, b_loc, p):
    new_node = RoadNode.build(b_loc)
    new_seg = RoadSegment.build(a, new_node)
    new_seg.roads.add(p)
    new_node.roads.add(p)
    p.nodes.append(new_node)
    p.node_set.add(new_node)
    p.segs.append(new_seg)


def pop_at_loc(l):
    if not in_bounds(l): return exp(-10)
    return pop_spline(l.y, l.x)


class Road(object):
    roads = []

    def __init__(self, a):
        self.nodes = [a]
        # Keep a set too for fast membership checking
        self.node_set = {a}
        self.segs = []
        self.offshoots = []
        self.closed = False
        self.roads.append(self)

    tip = property(lambda self: self.nodes[-1])
    base = property(lambda self: self.nodes[0])

    @property
    def prev_dir(self):
        if not self.segs:
            return Vec2(0, 0)
        return (self.tip.loc - self.nodes[-2].loc).normalized()

    @property
    def last_offshoot(self):
        if not self.offshoots: return self.nodes[0].loc
        return self.offshoots[-1].base.loc

    # Find the best new direction for a new points
    def best_candidate(self, forward):
        candidates = map(forward.rotated, linspace(-45, 45, 3))

        # by maximizing an objective function
        def objective(c):
            c = self.tip.loc + c
            if not in_bounds(c.x) or not in_bounds(c.y): return -9999999
            s = steepness[c.y % size, c.x % size] * size
            p = pop_at_loc(c)
            # p = population[c.x, c.y] * size
            dir = (c - self.tip.loc).normalized()
            d = dir.dot(forward)
            # d2 = dir.dot(self.prev_dir())
            # print p*10, s, d*4
            # return -s
            return p * 10 - s + d * 6
            # return - (pow(dpopx[c.x, c.y], 2) + pow(dpopy[c.x, c.y], 2))

        i = argmax(map(objective, candidates))
        return self.tip.loc + candidates[i]

    def step(self, forward):

        nearest, nearest_intersection = get_nearest(self.tip.loc, self.node_set)
        dir = (nearest.loc - self.tip.loc).normalized()
        if nearest_intersection and (self.tip.loc + forward).distance_to(nearest_intersection.loc) < 5 and \
                        forward.dot((nearest_intersection.loc - self.tip.loc).normalized()) > 0:
            extend(self.tip, nearest_intersection.loc, self)
            return True
        if (self.tip.loc + forward).distance_to(nearest.loc) < 3 and forward.dot(dir) > 0:
            extend(self.tip, nearest.loc, self)
            return True
        else:
            extend(self.tip, self.best_candidate(forward), self)
            if self.tip.loc.distance_to(self.last_offshoot) > 10.00 / pow(pop_at_loc(self.tip.loc), 0.5):
                # and intersection_density(self.tip.loc) < 100\
                self.offshoots.append(SecondaryRoad(self.tip, self.prev_dir.rotated(90)))
                self.offshoots.append(SecondaryRoad(self.tip, self.prev_dir.rotated(-90)))
            return False

    done = property(lambda self: self.closed and all([o.done for o in self.offshoots]))

    def render(self):
        for seg in self.segs:
            seg.render()

    # Average each node's position with the position of its neighbors
    def smooth(self):
        for i in range(1, len(self.nodes) - 1):
            self.nodes[i].loc += self.nodes[i - 1].loc + self.nodes[i + 1].loc
            self.nodes[i].loc *= (1.0 / 3.0)
        if len(self.segs) and len(self.tip.segments) == 1:
            self.node_set.remove(self.tip)
            self.nodes.pop(-1)
            self.segs.pop(-1)



class SecondaryRoad(Road):
    def __init__(self, base, dir):
        super(SecondaryRoad, self).__init__(base)
        self.dir = dir

    def step(self):
        if not self.closed:
            if not self.tip.is_intersection and rand() < 0.3:
                self.closed = True
            elif pop_at_loc(self.tip.loc) < 0.2 * rand():
                self.closed = True
            else:
                self.closed = super(SecondaryRoad, self).step(self.dir)


class Highway(object):
    highways = []
    highway_roads = set()

    def __init__(self, a, b):
        self.a = Road(a)
        self.b = Road(b)
        self.highways.append(self)
        self.highway_roads.add(self.a)
        self.highway_roads.add(self.b)

    def length(self):
        return self.a.nodes[0].loc.distance_to(self.b.nodes[0].loc)

    def step(self):
        if not self.a.closed:
            ab = self.b.tip.loc - self.a.tip.loc
            self.a.step(ab.normalized())
            if self.a.tip == self.b.tip:
                self.b.closed = True
                self.a.closed = True

        if not self.b.closed:
            ba = self.a.tip.loc - self.b.tip.loc
            self.b.step(ba.normalized())
            if self.a.tip == self.b.tip or self.a.tip.loc.distance_to(self.b.tip.loc) < 0.001:
                self.b.closed = True
                self.a.closed = True

        return self.a.closed and self.b.closed

    def gap_length(self):
        return self.a.nodes[-1].loc.distance_to(self.b.nodes[-1].loc)

    def render(self):
        self.a.render()
        self.b.render()


pop_centers = [PopulationCenter(Vec2(x, y)) for x, y in res]

for a, b, c in triangles.simplices:
    Highway(pop_centers[a], pop_centers[b])
    Highway(pop_centers[b], pop_centers[c])
    Highway(pop_centers[c], pop_centers[a])


def step_all_roads():
    n_open = 0
    for r in Road.roads:
        if r.closed or r in Highway.highway_roads: continue
        r.step()
        n_open += 1
    print n_open
    return n_open


while 1:
    gap = 0

    # Randomize the step order each time
    shuffle(Highway.highways)
    for h in Highway.highways:
        if not h.step():
            gap += h.gap_length()
    print "Gap:", int(gap),
    if step_all_roads() == 0 and gap == 0: break

for _ in range(5):
    for r in Road.roads:
        r.smooth()

intersections = [(node.loc.x, node.loc.y) for node in RoadNode.nodes.values() if node.is_intersection]

for seg in set(RoadSegment.segs.values()):
    seg.render()

ix, iy = zip(*intersections)
scatter(ix, iy, c='w', s=25, zorder=2)
scatter(res[:, 0], res[:, 1], c=map(scalarMap.to_rgba, range(len(res))), s=25, zorder=3)
show()