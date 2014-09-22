#version 330 core

struct Light{
	vec3 direction;
	float Si;
	float Di;
	float Ai;
	float Intensity;
};

uniform Light light;

uniform vec3 diffColor;

smooth in vec3 pass_Normal;
smooth in vec3 pass_Position;

out vec3 outColor;

void main(void){
    vec3 Normal           = normalize(pass_Normal);
    vec3 light_Direction  = normalize(-light.direction);
    vec3 camera_Direction = normalize(-pass_Position);
    vec3 half_vector      = normalize(camera_Direction + light_Direction);

    float nlddot    = dot(Normal, light_Direction);
    float diffuse   = vec3(max(0.0, nlddot) + 0.2);
    float specular  = max(0.0, dot(Normal, half_vector));
    float fspecular = vec3(pow(specular, 128.0));

	outColor = light.Intensity * (light.Di * diffuse + light.Si * fspecular) * diffColor;
}
