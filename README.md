# fRenderer² is a hybrid path-tracer built for one purpose - highly realistic rendering in real time*

# TODO: 
	-	the core engine (done) 
	-	deffered rasterizing (aka the part that makes it hybrid, and not entirely path tracing) (done) 
	-	The tracing function (done) 
	-	github page? (done)
	-	Spatial split BVH (similar aproach to the production-ready cycles renderer) 
	-	Woop ray/triangle intersection 
	-	Some basic hand optimization (hand-written kernels etc) 
	-	logo (I know I have great priorities) 
	-	everything else (who cares) 
	-	potential integration with blender? (no lol) 

# Planned features
	-	Ridiciously fast path tracing (hopefully) 
	-	Spatiotemporal filtering (simple to start but I have a paper somewhere, really needed since I have a budget of 1 sample per pixel) 
	-	Non-path traced giant data sets (essentially designed for smaller entities that are going to be instanced, like grass for instance) 
	-	Dynamic sky with volumetric clouds (ported over from fEngine) 
	-	Fast & simple volumetric lighting (helps with realism) (does not utilize path tracing) 
	-	Physically based rendering (would be a shame if I didn't use it with pathtracing lol) 


# References: 
	-	https://www.nvidia.com/object/nvidia_research_pub_012.html
	-	http://www.sven-woop.de/papers/2004-GH-SaarCOR.pdf 
  		(simplified aproach because every triangle consists of the verticies (0,0,0) (1,0,0) and (0,1,0) while the ray is 			transformed to unit triangle space) 
	-	http://blog.hvidtfeldts.net/index.php/2012/10/image-based-lighting/ 
		fantastic tutorial about equirectangular maps (i;e most hdrs) 
	
	* 30 samples per pixel at 720p, 150k tris. (1 sample per pixel) 