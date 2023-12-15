#version 150

uniform vec4 AmbientProduct;
uniform vec4 DiffuseProduct;
uniform vec4 SpecularProduct;
uniform vec4 Light;
uniform float Shininess;

in vec4 pos;
in vec4 N;
in vec4 color;

out vec4 fragColor;

void main()
{
  
  // Ambient
  vec4 ambient = AmbientProduct;
  
  // Diffuse
  
  vec4 L = normalize(Light-pos);
  
  float D  = dot(L,N);
  if(D < 0){ D = -D; }
  vec4  diffuse = D * color;
  
  //Specular
  vec4 R = normalize(reflect(-L,N));
  vec4 V = normalize(vec4(0.0, 0.0, 0.0, 1.0) - pos);
  
  float S = pow(max(0.0,dot(R,V)), Shininess);
  vec4  specular = S * SpecularProduct;
  
  fragColor = ambient + diffuse + specular;
}

