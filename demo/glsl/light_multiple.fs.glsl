#version 300 es
precision mediump float;

out vec4 FragColor;

struct Material {
    sampler2D diffuse;
    sampler2D specular;

    float shininess;
};

struct DirectLight {
    vec3 direction;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

struct PointLight {
    vec3 position;

    float constant;
    float linear;
    float quadratic;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

// flashlight
struct SpotLight {
    vec3 position;
    vec3 direction;
    float cut_off;
    float outer_cut_off;

    float constant;
    float linear;
    float quadratic;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

#define NR_POINT_LIGHTS 4
#define NR_MATERIALS 16

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoords;

uniform vec3 viewPos;

uniform PointLight point_lights[NR_POINT_LIGHTS];
uniform Material material[NR_MATERIALS];

uniform SpotLight spot_light;
uniform DirectLight direct_light;
uniform bool spot_light_enabled;
uniform int material_number;

vec3 calc_dir_light(DirectLight light, vec3 normal, vec3 viewDir);
vec3 calc_point_light(
    PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir);
vec3 calc_spot_light(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir);

vec4 material_diffuse;
vec4 material_specular;
float shininess;

void main()
{
    material_diffuse = texture(material[material_number].diffuse, TexCoords);
    material_specular = texture(material[material_number].specular, TexCoords);
    shininess = material[material_number].shininess;

    if (material_diffuse.a < 0.001) {
        discard;
    }

    // properties
    vec3 norm = normalize(Normal);
    vec3 viewDir = normalize(viewPos - FragPos);

    // directional lighting
    vec3 result = calc_dir_light(direct_light, norm, viewDir);
    // point lights
    for(int i = 0; i < NR_POINT_LIGHTS; i++) {
        result += calc_point_light(point_lights[i], norm, FragPos, viewDir);
    }
    // spot light
    if (spot_light_enabled) {
        result += calc_spot_light(spot_light, norm, FragPos, viewDir);
    }
    FragColor = vec4(result, material_diffuse.a);
}

// calculates the color when using a directional light.
vec3 calc_dir_light(DirectLight light, vec3 normal, vec3 viewDir)
{
    vec3 light_direction = normalize(-light.direction);
    // diffuse shading
    float diff = max(dot(normal, light_direction), 0.0);
    // specular shading
    vec3 reflectDir = reflect(-light_direction, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), shininess);

    // combine results
    vec3 ambient = light.ambient * material_diffuse.xyz;
    vec3 diffuse = light.diffuse * diff * material_diffuse.xyz;
    vec3 specular = light.specular * spec * material_specular.xyz;

    return ambient + diffuse + specular;
}

// calculates the color when using a point light.
vec3 calc_point_light(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir)
{
    vec3 light_direction = normalize(light.position - fragPos);
    // diffuse shading
    float diff = max(dot(normal, light_direction), 0.0);
    // specular shading
    vec3 reflectDir = reflect(-light_direction, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), shininess);
    // attenuation
    float distance = length(light.position - fragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance
                        + light.quadratic * (distance * distance));

    // combine results
    vec3 ambient = light.ambient * material_diffuse.xyz;
    vec3 diffuse = light.diffuse * diff * material_diffuse.xyz;
    vec3 specular = light.specular * spec * material_specular.xyz;

    ambient *= attenuation;
    diffuse *= attenuation;
    specular *= attenuation;

    return (ambient + diffuse + specular);
}

// calculates the color when using a spot light.
vec3 calc_spot_light(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir)
{
    vec3 light_direction = normalize(light.position - fragPos);
    // diffuse shading
    float diff = max(dot(normal, light_direction), 0.0);
    // specular shading
    vec3 reflectDir = reflect(-light_direction, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), shininess);
    // attenuation
    float distance = length(light.position - fragPos);
    float attenuation = 1.0 / (light.constant
        + light.linear * distance + light.quadratic * (distance * distance));
    // spotlight intensity
    float theta = dot(light_direction, normalize(-light.direction));
    float epsilon = light.cut_off - light.outer_cut_off;
    float intensity = clamp((theta - light.outer_cut_off) / epsilon, 0.0, 1.0);

    // combine results
    vec3 ambient = light.ambient * material_diffuse.xyz;
    vec3 diffuse = light.diffuse * diff * material_diffuse.xyz;
    vec3 specular = light.specular * spec * material_specular.xyz;
    attenuation = min(1.0, attenuation);

    ambient *= attenuation * intensity;
    diffuse *= attenuation * intensity;
    specular *= attenuation * intensity;

    return (ambient + diffuse + specular);
}
