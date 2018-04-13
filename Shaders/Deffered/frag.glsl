#version 330

layout(location = 0) out vec4 Albedo; 
layout(location = 1) out vec4 NormalRefractive;
layout(location = 2) out vec4 PositionSpecular;  
layout(location = 3) out vec4 IOREmmisisionRoughness; 

struct FMaterial { //all of the material properties 
	bool HasTexture; 
	sampler2D Texture; 

	bool HasNormalMap; 
	sampler2D NormalMap; 

	bool HasSpecularMap; 
	sampler2D SpecularMap; 

	bool HasRoughnessMap; 
	sampler2D RoughnessMap; 

	bool HasParallaxMap; 
	sampler2D ParallaxMap; 

	bool isShadowCatcher; 

		
	float Specularity; 
	float Emmision; 
	float Roughness; 
	float Refractivity; 
	float RefractiveIndex; 
	float ParallaxStrenght; 
	float NormalMapStrenght; 

	vec3 DiffColor; 

}; 



uniform FMaterial FragmentMaterial; 


uniform vec3 CamPos; 

uniform bool SSelfShadow; 

in VertexValues {
vec3 Position;
vec2 TexCoord;  
vec3 Normal; 
vec3 Tangent;

} FS_IN; 

vec3 CalcNormalMappedNormal(sampler2D Normalmap,vec3 Normal,vec2 TexCoord,float intensity,mat3 TBN)  { //for normal maps 
    return mix(Normal,(TBN * (texture(Normalmap,TexCoord).rgb * 2. -1.)),intensity);
}

uniform mat4 ViewMatrix; 
uniform mat4 ModelMatrix; 
uniform mat4 ProjectionMatrix;


struct VertexMaterial {

	bool Fluid; 
	float WaveDensity; 
	float WaveStrenght; 
	float WaveContrast; 

}; 


struct StandardParameters {
	float Time; 
	int Frame; 
	sampler2D GrayNoise; 
	sampler2D RGBNoise; 
}; 


uniform VertexMaterial Material; 
uniform StandardParameters Parameters; 

void main() {
	
	
	
	vec3 Normal = normalize(FS_IN.Normal);
    vec3 Tang = normalize(FS_IN.Tangent);
    vec3 Tangent = normalize(Tang - dot(Tang, Normal) * Normal);
    vec3 Bitangent = normalize(cross(Tang, Normal));
	mat3 TBN = mat3(Tangent,Bitangent,Normal); 
	
	vec4 Alb = (FragmentMaterial.HasTexture ? (texture(FragmentMaterial.Texture,FS_IN.TexCoord)) : vec4(FragmentMaterial.DiffColor,1.0)); 

	if(Alb.a < 0.5) 
	discard; 

	Albedo.rgb = Alb.rgb;
	

	
	mat3 normalMatrix = transpose(inverse((mat3(ViewMatrix))));

	NormalRefractive.xyz = normalize(FragmentMaterial.HasNormalMap ? CalcNormalMappedNormal(FragmentMaterial.NormalMap,FS_IN.Normal,FS_IN.TexCoord,0.35,TBN) : FS_IN.Normal); 
	
	PositionSpecular.xyz = FS_IN.Position;

	IOREmmisisionRoughness.rgb = vec3(
	FragmentMaterial.Specularity,
	FragmentMaterial.Emmision*4.,
	FragmentMaterial.Roughness

	); 

	IOREmmisisionRoughness.r = FragmentMaterial.HasSpecularMap ? pow(texture(FragmentMaterial.SpecularMap,FS_IN.TexCoord).r,1.0/IOREmmisisionRoughness.r) : IOREmmisisionRoughness.r; 

	
	
	//store additional values in the alpha component here: 
	Albedo.a = 1.0; 
	NormalRefractive.a = 1.0; 
	PositionSpecular.a = 1.0; 
	IOREmmisisionRoughness.a = 1.0; 

}