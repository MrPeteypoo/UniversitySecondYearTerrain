Triangulate My Terrain
======================
The second task in my second year 3D Graphics Programming module. We were tasked to generate and tesselate terrain from a given heightmap, this included using a Bézier surface algorithm to upscale the low resolution heightmap and some noise functionality to create a more realisitic surface.

My implementation contained a crude implementation of the Bézier surface algorithm utilising the quadratic Bézier curve algorithm. Bumpiness was given to the terrain in the form of fractional brownian motion and improve perlin noise. The only thing lacking from my implementation was frustum culling and spatial partitioning, hence why the application runs slowly. I ended up with a grade of 73% which I'm pleased with, though I was aiming for higher.
