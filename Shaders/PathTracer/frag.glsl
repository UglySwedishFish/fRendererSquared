#version 330
in vec2 TexCoord; 
out vec3 Color; 

struct Model {
	sampler2DArray ModelData; 
	sampler2DArray MaterialData; 
	int VerticeCount; 
	ivec2 Resolution; 
}; 

uniform sampler2D PositionSpecular; 
uniform sampler2D NormalRefractive; 
uniform sampler2D IOREmmisisionRoughness; 

uniform Model Models[14]; 
uniform int TotalModels; 

uniform mat4 ProjectionMatrix; 
uniform mat4 ViewMatrix; 
uniform vec3 CameraPosition; 

struct Ray {
	vec3 Direction,Origin; 
}; 

const float EPSILON = 0.0001; 

bool RayTriangleIntersection(vec3 edge1, vec3 edge2, vec3 edge3, Ray ray, inout vec2 uv, inout float t,inout float tGlobal) {

	vec3 e0 = edge2 - edge1; 
	vec3 e1 = edge3 - edge1; 

	vec3 h = cross(ray.Direction, e1); 
	float a = dot(e0, h); 

	if(a > -EPSILON && a < EPSILON) 
		return false; 

	float f = 1.0 / a; 

	vec3 s = ray.Origin - edge1; 
	float u = f * dot(s,h); 

	if (u < 0.0 || u > 1.0)
		return false;

	vec3 q = cross(s, e0);
	float v = f * dot(ray.Direction, q);

	if (v < 0.0 || u + v > 1.0)
		return false;
	
	uv = vec2(u,v); 

	t = f * dot(e1, q);

	if (t < EPSILON)
		return false;

	return (t > 0.0) && (t < tGlobal);
	
} 

//^ converted from c++ from lighthouse3d (no idea if it's gpu friendly or not) 


ivec2 WrapTo2DTexture(int texel, int resolution) {
	float TexelRes = float(texel) / float(resolution); 
	int TexelResDivider = int(floor(TexelRes)); 
	return ivec2(texel - TexelResDivider*resolution, TexelResDivider); 
}

vec3 GetTriangleData(int index,int model, int dataWanted) {
	
	
	//compute 


	int Y = int(floor(float(index)/float(Models[model].Resolution.x))); 
	int XSub = Y*Models[model].Resolution.x; 

	ivec2 texel = ivec2(index-XSub,Y); 
	
	return texelFetch(Models[model].ModelData,ivec3(texel, dataWanted),0).rgb; 

}

const float zFar = 1000.0; 

float RayWorldIntersection(Ray ray,inout int meshHit,inout int TriangleHit,inout vec2 UvCord) {


	meshHit=-1; 
	TriangleHit=-1; 
	float tm = zFar; 
	float t = zFar; 
	float _t = zFar; 



	for (int Mesh = 0; Mesh < TotalModels; Mesh++) {

		for (int Triangle = 0; Triangle < Models[Mesh].VerticeCount; Triangle+=3) {

				
				
				vec3 Triangle1, Triangle2, Triangle3; 

				Triangle1 = GetTriangleData(Triangle,Mesh,0); 
				Triangle2 = GetTriangleData(Triangle+1,Mesh,0); 
				Triangle3 = GetTriangleData(Triangle+2,Mesh,0); 
				vec2 tuv; 

				if (RayTriangleIntersection(Triangle1,Triangle2,Triangle3, ray, tuv, t, tm)) {
					meshHit = 0; 
					TriangleHit = Triangle; 
					tm = t; 
					UvCord = tuv; 
				}
			
		}
	}
	return tm; 
}

float seed; 

vec2 hash2() {
    return fract(sin(vec2(seed+=0.1,seed+=0.1))*vec2(43758.5453123,22578.1459123));
}

vec3 cosWeightedRandomHemisphereDirection( const vec3 n) {
  	vec2 rv2 = hash2();
    
	vec3  uu = normalize( cross( n, vec3(0.0,1.0,1.0) ) );
	vec3  vv = normalize( cross( uu, n ) );
	
	float ra = sqrt(rv2.y);
	float rx = ra*cos(6.2831*rv2.x); 
	float ry = ra*sin(6.2831*rv2.x);
	float rz = sqrt( 1.0-rv2.y );
	vec3  rr = vec3( rx*uu + ry*vv + rz*n );
    
    return normalize( rr );
}

float Barycentric(vec2 A, vec2 B, vec2 C) {
	return abs(A.x * (B.y - C.y) + B.x * (C.y - A.y) + C.x * (A.y - B.y)); 
}

void main() {
	
	seed = ((TexCoord.x * TexCoord.y)*100.0); 

	vec4 PositionRaw = texture(PositionSpecular, TexCoord); 
	vec4 NormalRaw = texture(NormalRefractive, TexCoord); 

	vec3 RayDirection = reflect(normalize(PositionRaw.xyz - CameraPosition), NormalRaw.xyz); 

	Ray RAY; 
	//RAY.Direction = cosWeightedRandomHemisphereDirection(NormalRaw.xyz); 
	RAY.Direction = RayDirection; 

	RAY.Origin = PositionRaw.xyz; 

	vec3 Indirect = vec3(1.); 

	Color = vec3(0.); 

	for(int Depth = 0; Depth < 2; Depth++) {
			vec2 uv; 
			int m,t; 
		
			RayWorldIntersection(RAY, m, t, uv); 

				if(m >= 0) {
					vec2 StoredUVC[3] = vec2[](GetTriangleData(t,m,3).rg,GetTriangleData(t+1,m,3).rg,GetTriangleData(t+2,m,3).rg); 
					vec3 StoredNormals[3] = vec3[](GetTriangleData(t,m,1),GetTriangleData(t+1,m,1),GetTriangleData(t+2,m,1)); 
					vec3 StoredPositions[3] = vec3[](GetTriangleData(t,m,0),GetTriangleData(t+1,m,0),GetTriangleData(t+2,m,0)); 

					int Material = int(floor(GetTriangleData(t,m,3).z * 10.0+.01)); 

					float RatioP1 = Barycentric(vec2(1.0,0.0),vec2(0.0,1.0),uv); 
					float RatioP2 = Barycentric(vec2(0.0,0.0),vec2(0.0,1.0),uv); 
					float RatioP3 = Barycentric(vec2(0.0,0.0),vec2(1.0,0.0),uv);

					vec2 UV = StoredUVC[0] * RatioP1 + StoredUVC[1] * RatioP2 + StoredUVC[2] * RatioP3; 

					vec3 Normal = normalize(StoredNormals[0] * RatioP1 + StoredNormals[1] * RatioP2 + StoredNormals[2] * RatioP3); 
					
					vec3 Position = StoredPositions[0] * RatioP1 + StoredPositions[1] * RatioP2 + StoredPositions[2] * RatioP3; 
					
					Indirect *= texelFetch(Models[m].MaterialData,ivec3(0,Material,0),0).rgb; 

					RAY.Direction = cosWeightedRandomHemisphereDirection(Normal); 
					RAY.Origin = Position; 

				}
				
				else {
					
					Color += Indirect; 
					break; 
				}
	}
	


}