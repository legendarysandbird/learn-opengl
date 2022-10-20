#version 330 core

struct Material {
  sampler2D diffuse;
  sampler2D specular;
  float shininess;

  sampler2D emission;
};

struct Light {
  vec3 position;
  vec3 direction;
  float cutOff;
  float outerCutOff;

  vec3 ambient;
  vec3 diffuse;
  vec3 specular;
};

out vec4 FragColor;

uniform vec3 viewPos;
uniform Material material;
uniform Light light;
uniform float time;

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoords;

void main()
{
  vec3 result;

  vec3 lightDir = normalize(light.position - FragPos);

  vec3 ambient = light.ambient * vec3(texture(material.diffuse, TexCoords));
  vec3 norm = normalize(Normal);
  float diff = max(dot(norm, lightDir), 0.0);
  vec3 diffuse = light.diffuse * diff * vec3(texture(material.diffuse, TexCoords));

  vec3 viewDir = normalize(viewPos - FragPos);
  vec3 reflectDir = reflect(-lightDir, norm);

  float spec;
  if (dot(norm, reflectDir) > 0.0f)
  {
    spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
  }
  else 
  {
    spec = 0.0f;
  }
  vec3 specular = light.specular * spec * vec3(texture(material.specular, TexCoords));

  vec3 emission;
  if (vec3(texture(material.specular, TexCoords)) == vec3(0.0f, 0.0f, 0.0f))
  {
    emission = vec3(texture(material.emission, TexCoords + vec2(0.0, time)));
  }
  else
  {
    emission = vec3(0.0f, 0.0f, 0.0f);
  }

  emission = vec3(0.0f, 0.0f, 0.0f);

  float theta = dot(lightDir, normalize(-light.direction));
  float epsilon = light.cutOff - light.outerCutOff;
  float intensity = clamp((theta - light.outerCutOff) / epsilon, 0.0f, 1.0f);

  diffuse *= intensity;
  specular *= intensity;

  result = (ambient + diffuse + specular + emission);

  FragColor = vec4(result, 1.0);
}

