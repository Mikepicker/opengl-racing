#version 330 core
out vec4 fragColor;

uniform float time;
uniform sampler2D texture1;

float radius = .5;

in vec2 uv;

void main()
{
	vec2 coords = uv.st;
	vec2 dir = coords - vec2(.5);
	
	float dist = distance(coords, vec2(.5));
	vec2 offset = dir * (sin(dist * 80. - time*15.) + .5) / 30.;

	vec2 texCoord = coords + offset;
	vec4 diffuse = texture2D(texture1, texCoord);

 	fragColor = diffuse;
}
