#version 330 core
out vec4 FragColor;

in VS_OUT {
    vec3 FragPos;
    vec2 TexCoords;
    vec3 Normal;
    mat3 TBN;
} fs_in;

uniform sampler2D texture1;
uniform sampler2D normalMap;
uniform bool hasNormal;

uniform vec3 lightPos;
uniform vec3 viewPos;

vec3 CalcLight()
{
    vec3 color = texture(texture1, fs_in.TexCoords).rgb;
    vec3 lightColor = vec3(1.0);

    vec3 normal;
    if (hasNormal)
    {
        normal = texture(normalMap, fs_in.TexCoords).rgb;
        normal = normal * 2.0 - 1.0;
        normal = normalize(fs_in.TBN * normal);
    }
    else
    {
        normal = normalize(fs_in.Normal);
    }


    // ambient
    vec3 ambient = 0.15 * lightColor;

    // diffuse
    vec3 lightDir = normalize(lightPos - fs_in.FragPos);
    float diff = max(dot(lightDir, normal), 0.0);
    vec3 diffuse = diff * lightColor;

    // specular
    vec3 viewDir = normalize(viewPos - fs_in.FragPos);
    float spec = 0.0;
    vec3 halfwayDir = normalize(lightDir + viewDir);
    spec = pow(max(dot(normal, halfwayDir), 0.0), 64.0);
    vec3 specular = spec * lightColor;

    // calculate shadow
    vec3 lighting = (ambient + diffuse + specular) * color;

    return lighting;
}

void main()
{    
    vec3 lighting = CalcLight();
    FragColor = vec4(lighting, 1.0);
}
