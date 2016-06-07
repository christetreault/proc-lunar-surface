# cse167-final

Chris Tetrault, Andrew Buss

Blog: https://doingmyprogramming.com/category/projects/procedural-moonbase/

Features:
 - Procedurally generated terrain
   - press L to generate a new heightmap
   - press O to swap to an imported heightmap of the Moon
 - Procedurally generated plants with L-systems
   - press D to generate new plants
 - Shadow mapping
   - press M to toggle shadow mapping and the shadow map display in the left corner
 - Procedural city generation (incomplete)
   - press Y to regenerate the smaller heightmap and a new road network

Right click + drag orbits the world

Scroll zooms in and out

These features are briefly shown in a video (1m20s): https://cryptarch.net/proj5-3.ogv

## Dependencies

This project uses the SOIL library to load textures, and GLFW/GLEW/OpenGL to display them.

The Python city generator uses NumPy, SciPy, Rtree, and Planar.
