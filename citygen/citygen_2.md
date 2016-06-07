## Procedural City Generation

This weekend, I added functionality to render the smaller heightmap for the city on top of the
buildable area of the larger heightmap.

The main C++ program invokes a Python script to generate the heightmap and road network, then reads
the data into a new LandscapeModel at the appropriate location. I render the road network using a
solid color shader for now. By rendering only a portion of the road network at a time, it's possible
to animate the process, showing how the network grows from the population centers and intersects
with itself.

![The growth of the road network](https://cryptarch.net/img/4iofX.webm)

The more significant challenge has been extracting buildable areas from the road network. Implementing
the road network growth required hacking together a spatially indexed graph database. The generated
graphs are subtly broken - some roads cross without an intersection, so the graph is not necessarily planar.

I need to resolve this issue before I can reliably extract cycles, and I need to extract buildable plots
before I can place and generate buildings. I'll be working on that tonight.

Where the graph is sensible, I'm able to extract a cycle:

![A successfully extracted cycle](https://cryptarch.net/img/Kq4VF.png)

But in a case where the edges intersect somewhere besides an intersection point, this fails:

![An incorrect attempt at extracting a cycle](https://cryptarch.net/img/A7Dl3.png)

