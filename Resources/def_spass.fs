#version 330 core

layout (location = 0) out vec4 fColor;

uniform sampler2D u_diffuseMap;
uniform sampler2D u_normalMap;
uniform sampler2D u_vertexMap;
uniform mat4 viewMatrix;

const int g_numLights = 20;
vec4 g_ambientColor = vec4(0.1);
vec4 g_lightTestPos = vec4(0, 6, 4, 1);


in vec2 vTexCoord;

layout (std140) uniform LightSource
{
  vec4 lights[g_numLights];
};

void main()
{
  vec3 normal_camera = (texture(u_normalMap, vTexCoord)).xyz;
  if(length(normal_camera) == 0)
    fColor = vec4(0.8);
  else {
    fColor = vec4(0.f);
    vec4 diff_color = texture(u_diffuseMap, vTexCoord);
    vec3 vertex_camera = (texture(u_vertexMap, vTexCoord)).xyz;
	
    for(int i = 12; i < g_numLights; i++) {
      vec3 light_camera = vec3(viewMatrix * g_lightTestPos);//(viewMatrix * lights[i]).xyz;
      vec3 light_dir = light_camera - vertex_camera;
      float dist = length(light_dir);
      light_dir = normalize(light_dir);
      
      float diff = max(dot(normal_camera, light_dir), 0.f);
      fColor.rgb += diff * diff_color.rgb / (dist);
      
      if(diff > 0.f) {
        vec3 V = normalize(-vertex_camera);
        vec3 R = normalize(reflect(-light_dir, normal_camera));
        //vec3 H = normalize(V + normal_camera);
        
        float angle = max(dot(R, V), 0.f);
        float spec = pow(angle, 64.f); // (dist * dist);
        vec3 specColor = vec3(spec);
        fColor.rgb += specColor;
      }
      //fColor = vec4(light_dir, 1.f);
      //fColor = vec4(-vertex_camera, 1.f);
      //fColor = vec4(normal_camera, 1.f);
    }
    //fColor += g_ambientColor;
    
  }
  
  
}
