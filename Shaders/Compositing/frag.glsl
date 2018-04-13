#version 330
in vec2 TexCoord; 
out vec3 Color; 

uniform sampler2D Albedo; 
uniform sampler2D Lighting; 

void main() {
	Color = pow(texture(Albedo,TexCoord).rgb * texture(Lighting,TexCoord).rgb,vec3(0.45454545)); 
}