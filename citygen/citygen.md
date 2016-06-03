## Procedural City Generation

We'll be placing a procedurally generated moon base on our generated moon terrain.

The moon has rough terrain, as you can see from the screenshots below, but we can only situate parts of the moon base on flat terrain.

We're generating two heightmaps - the larger rough terrain is generated using (???) method, and the patch of terrain that the moon base sits on is generated using spectral synthesis. The edges of the patch are flattened so they coincide smoothly with the surrounding terrain.

![A sample generated heightmap](https://cryptarch.net/img/jt2bh.png)

After generating the heightmaps, we generate a population density map using a similar procedure, attenuating it where the terrain is steepest (it's difficult to build on the side of a hill!). Then we sample the density map and place population centers using k-means clustering. Finally, we triangulate the set of population centers to generate a connectivity graph for road generation.

![A sample heightmap, steepness map, clustered population density map, and population mask](https://cryptarch.net/img/2QQgL.png)

Once we have a connectivity graph, we draw adaptive highways between population centers using the technique described in [Citygen: An Interactive System for Procedural City Generation](http://www.citygen.net/files/citygen_gdtw07.pdf), using a heuristic combining Least Elevation Difference, population density, and the degree to which the road would deviate from its current direction. As these are drawn, we create additional perpendicular roads in areas of high population density. These roads are extended using the same heuristic until they reach a less populated area.

![A sample road network](https://cryptarch.net/img/z0UJb.png)

So far, we've implemented the above in Python so we can leverage the performant array operations from NumPy and SciPy, in addition to the spatial indexing functionality from Rtree. We'll need to use the Python C API to call into our Python code and store the resulting road graph in a buffer for rendering.

The next task is to turn the road graph into a set of buildable cells to be subdivided and populated with generated buildings. I'm [Andrew] hoping to implement that on Saturday. By Sunday night I hope to have the road network and very simple rectangular buildings rendering from C++. On Monday I'll implement more elaborate procedurally generated buildings, and have more screenshots to show in our next blog post. 