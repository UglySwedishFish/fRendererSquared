/*

fRenderer² is a hybrid path-tracer built for one purpose - highly realistic rendering in real time*

TODO: 
	-	the core engine (done) 
	-	deffered rasterizing (aka the part that makes it hybrid, and not entirely path tracing) (done) 
	-	The tracing function (done) 
	-	Github page? (doneOkay hum Hello , I'm going to present my C++/SFML project : Xenorbax.    Xenorbax is a Tamagotchi game in isometric view in developpement . In my game you'll have to take care of a pet which will fight versus others pets around the world with an isometric turn by turn mecanic. if you wanna see more come on my serv : https://discord.gg/YBkf2 ^^)
	-	Spatial split BVH (similar aproach to the production-ready cycles renderer) 
	-	Woop ray/triangle intersection 
	-	Some basic hand optimization (hand-written kernels etc) 
	-	logo (I know I have great priorities) 
	-	everything else (who cares) 
	-	potential integration with blender? (no lol) 

Planned features
	-	Ridiciously fast path tracing (hopefully) 
	-	Spatiotemporal filtering (simple at first but I have a paper stored somewhere) (really needed since I have a budget of 1 sample per pixel) 
	-	Non-path traced giant data sets (essentially designed for smaller entities that are going to be instanced, like grass for instance) 
	-	Dynamic sky with volumetric clouds (ported over from fEngine) 
	-	Fast & simple volumetric lighting (helps with realism) (does not utilize path tracing) 
	-	Physically based rendering (would be a shame if I didn't use it with pathtracing lol) 


References: 
	https://www.nvidia.com/object/nvidia_research_pub_012.html
	http://www.sven-woop.de/papers/2004-GH-SaarCOR.pdf (simplified aproach because every triangle consists of the verticies (0,0,0) (1,0,0) and (0,1,0) while the ray is transformed to
	//unit triangle space
	http://blog.hvidtfeldts.net/index.php/2012/10/image-based-lighting/ fantastic tutorial about equirectangular maps (i;e most hdrs) 
	
	* 30 samples per pixel at 720p, 150k tris. (1 sample per pixel) 
*/

#include <Pipeline\Pipeline.h> 
#include <iostream> 

int main() {
	Window Screen;
	Screen.SetTitle("Test"); 
	Screen = Window(Vector2i(1280, 720), false);
	
	
	fRenderer::Control::Camera Camera = fRenderer::Control::Camera(70.0,0.01,1000.0,Vector3f(0.), Vector3f(0.), Screen); //fov, znear, zfar, position, rotation, display 


	
	std::vector<fRenderer::Rendering::Mesh::Model> Models;
	Models.push_back(fRenderer::Rendering::Mesh::Model("checkerboard.obj")); //you can of course have multiple models and that should work just fine 

	fRenderer::Rendering::Combined::Pipeline Pipeline; //the pipeline itself 
	Pipeline.Prepare(Models, Screen, Camera); 
	Pipeline.Run(Screen, Camera); 
	//todo: something along the lines of Pipeline.free(); 
	

}