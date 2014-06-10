#version 330

layout (location = 0) out vec4 fColor;

uniform sampler2D u_diffuseMap;
uniform sampler2D u_normalMap;
uniform sampler2D u_vertexMap;
uniform sampler2D u_ssaoMap;
uniform mat4 viewMatrix;

in vec2 vTexCoord;

uniform vec4 u_lightPos;

void main()
{
  vec3 normal_camera = (texture(u_normalMap, vTexCoord)).xyz;
  if(length(normal_camera) == 0)
    fColor = vec4(0.8);
  else {    
    fColor = vec4(0.f);
    vec4 ambient_color = vec4(vec3(texture(u_ssaoMap, vTexCoord).r), 0.f);
    vec4 diff_color = texture(u_diffuseMap, vTexCoord);
    vec3 vertex_camera = (texture(u_vertexMap, vTexCoord)).xyz;

    vec3 light_camera = (viewMatrix * u_lightPos).xyz;
    vec3 light_dir = light_camera - vertex_camera;
    float dist = length(light_dir);
    light_dir = normalize(light_dir);
      
    float diff = max(dot(normal_camera, light_dir), 0.f);
    fColor.rgb += diff * diff_color.rgb;// / (dist);
      
    /*if(diff > 0.f) {
      vec3 V = normalize(-vertex_camera);
      vec3 R = normalize(reflect(-light_dir, normal_camera));
      
      float angle = max(dot(R, V), 0.f);
      float spec = pow(angle, 128.f);
      vec3 specColor = vec3(spec);
      fColor.rgb += specColor;
    }*/
      
    fColor *= ambient_color;
  }
}
