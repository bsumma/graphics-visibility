#include "common.h"
#include "SourcePath.h"


using namespace Angel;

typedef vec4 color4;

// Initialize shader lighting parameters
vec4 light(   0.0, 2.0, 0.0, 1.0 );
color4 light_ambient(  0.1, 0.1, 0.1, 1.0 );
color4 light_diffuse(  1.0, 1.0, 1.0, 1.0 );
color4 light_specular( 1.0, 1.0, 1.0, 1.0 );

// Initialize shader material parameters
color4 material_ambient( 0.1, 0.1, 0.1, 1.0 );
color4 material_diffuse( 0.9, 0.9, 0.9, 1.0 );
color4 material_specular( 0.8, 0.8, 0.8, 1.0 );
float  material_shininess = 10;


GLuint buffer;
GLuint vao;
GLuint ModelView_loc, NormalMatrix_loc, Projection_loc;
GLuint program;


vec4 viewer_pos( 0.0, 0.0, 0, 1.0);

vec4 initial_look(0.0, 0.0, 1.0, 0.0);
vec4 look(0.0, 0.0, 1.0, 0.0);

vec4 initial_right(1.0, 0.0, 0.0, 0.0);
vec4 right(1.0, 0.0, 0.0, 0.0);

BSPTree *tree;
unsigned int numVertices;
float angle;

//==========Trackball Variables==========
static float curquat[4],lastquat[4];
/* current transformation matrix */
static float curmat[4][4];
mat4 curmat_a;
/* actual operation  */
bool scaling;
bool moving;
bool panning;
/* starting "moving" coordinates */
static int beginx, beginy;
/* ortho */
float ortho_x, ortho_y;
bool lbutton_down;

void BSP2GPU();

static void error_callback(int error, const char* description)
{
  fprintf(stderr, "Error: %s\n", description);
}

//User interaction handler
static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods){
  if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
    glfwSetWindowShouldClose(window, GLFW_TRUE);
  if (key == GLFW_KEY_W && (action == GLFW_REPEAT || action == GLFW_PRESS)){
    viewer_pos -= 0.2*look;
    BSP2GPU();
  }
  if (key == GLFW_KEY_S && (action == GLFW_REPEAT || action == GLFW_PRESS)){
    viewer_pos += 0.2*look;
    BSP2GPU();
  }
  if (key == GLFW_KEY_A && (action == GLFW_REPEAT || action == GLFW_PRESS)){
    viewer_pos -= 0.2*right;
    BSP2GPU();
  }
  if (key == GLFW_KEY_D && (action == GLFW_REPEAT || action == GLFW_PRESS)){
    viewer_pos += 0.2*right;
    BSP2GPU();
  }
  
  if (key == GLFW_KEY_Q && (action == GLFW_REPEAT || action == GLFW_PRESS)){
    angle -= 10;
    look = RotateY(-angle)*initial_look;
    right = RotateY(-angle)*initial_right;
  }
  
  if (key == GLFW_KEY_E && (action == GLFW_REPEAT || action == GLFW_PRESS)){
    angle += 10;
    look = RotateY(-angle)*initial_look;
    right = RotateY(-angle)*initial_right;
  }
  
}

void BSP2GPU(){
  //Per vertex attributes
  GLuint vPosition = glGetAttribLocation( program, "vPosition" );
  GLuint vNormal = glGetAttribLocation( program, "vNormal" );
  GLuint vColor = glGetAttribLocation( program, "vColor" );

  glBindVertexArray( vao);
  glBindBuffer( GL_ARRAY_BUFFER, buffer);
  
  std::vector < vec4 > vertices;
  std::vector < vec3 > normals;
  std::vector < vec4 > colors;
  
  
  tree->getOpenGLvectors(vec2(viewer_pos.x, viewer_pos.z), vertices, normals, colors);
  numVertices = vertices.size();

  
  unsigned long vertices_bytes = vertices.size()*sizeof(vec4);
  unsigned long normals_bytes  = normals.size()*sizeof(vec3);
  unsigned long colors_bytes  = normals.size()*sizeof(vec4);

  
  glBufferData( GL_ARRAY_BUFFER, vertices_bytes + normals_bytes + colors_bytes, NULL, GL_STATIC_DRAW );
  unsigned int offset = 0;
  glBufferSubData( GL_ARRAY_BUFFER, offset, vertices_bytes, &vertices[0] );
  offset += vertices_bytes;
  glBufferSubData( GL_ARRAY_BUFFER, offset, normals_bytes,  &normals[0] );
  offset += normals_bytes;
  glBufferSubData( GL_ARRAY_BUFFER, offset, colors_bytes,  &colors[0] );

  
  glEnableVertexAttribArray( vNormal );
  glEnableVertexAttribArray( vPosition );
  glEnableVertexAttribArray( vColor);

  
  glVertexAttribPointer( vPosition, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0) );
  glVertexAttribPointer( vNormal, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(vertices_bytes) );
  glVertexAttribPointer( vColor, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(vertices_bytes+normals_bytes));

  glBindVertexArray(0);
  
}

void init(){
  
  std::vector < Wall > walls;
  
  walls.push_back(Wall(vec2(4.0, 4.0),
                       vec2(4.0, 2.0),
                       vec2(-1.0,0.0),
                       vec4(1.0, 0.1, 1.0, 1.0),
                       "a"));
  walls.push_back(Wall(vec2(2.0, 4.0),
                       vec2(2.0, 2.0),
                       vec2(1.0,0.0),
                       vec4(1.0, 1.0, 0.5, 1.0),
                       "b"));
  walls.push_back(Wall(vec2(4.0, 4.0),
                       vec2(2.0, 4.0),
                       vec2(0.0,-1.0),
                       vec4(0.1, 1.0, 1.0, 1.0),
                        "c"));
  walls.push_back(Wall(vec2(4.0, 2.0),
                       vec2(2.0, 2.0),
                       vec2(0.0,1.0),
                       vec4(1.0, 0.1, 1.0, 1.0),
                       "d"));
  walls.push_back(Wall(vec2(-4.0, -4.0),
                       vec2(-4.0, -2.0),
                       vec2(-1.0,0.0),
                       vec4(1.0, 0.1, 0.1, 1.0),
                       "e"));
  walls.push_back(Wall(vec2(-2.0, -4.0),
                       vec2(-2.0, -2.0),
                       vec2(1.0,0.0),
                       vec4(0.1, 1.0, 0.1, 1.0),
                       "f"));
  walls.push_back(Wall(vec2(-4.0, -4.0),
                       vec2(-2.0, -4.0),
                       vec2(0.0,-1.0),
                       vec4(0.1, 0.1, 1.0, 1.0),
                       "g"));
  walls.push_back(Wall(vec2(-4.0, -2.0),
                       vec2(-2.0, -2.0),
                       vec2(0.0,1.0),
                       vec4(0.1, 0.8, 1.0, 1.0),
                       "h"));
  walls.push_back(Wall(vec2(-6.0, -6.0),
                       vec2(-6.0, 6.0),
                       vec2(1.0,0.0),
                       vec4(1.0, 0.1, 0.1, 1.0),
                       "i"));
  walls.push_back(Wall(vec2(6.0, -6.0),
                       vec2(6.0, 6.0),
                       vec2(-1.0,0.0),
                       vec4(0.1, 1.0, 0.1, 1.0),
                       "j"));
  walls.push_back(Wall(vec2(-6.0, -6.0),
                       vec2(6.0, -6.0),
                       vec2(0.0,1.0),
                       vec4(0.1, 0.1, 1.0, 1.0),
                       "k"));
  walls.push_back(Wall(vec2(-6.0, 6.0),
                       vec2(6.0, 6.0),
                       vec2(0.0,-1.0),
                       vec4(0.1, 0.8, 1.0, 1.0),
                       "l"));

  
  tree = new BSPTree(walls);
  
  std::string vshader = source_path + "/shaders/vshader.glsl";
  std::string fshader = source_path + "/shaders/fshader.glsl";
  
  GLchar* vertex_shader_source = readShaderSource(vshader.c_str());
  GLchar* fragment_shader_source = readShaderSource(fshader.c_str());

  GLuint vertex_shader = glCreateShader(GL_VERTEX_SHADER);
  glShaderSource(vertex_shader, 1, (const GLchar**) &vertex_shader_source, NULL);
  glCompileShader(vertex_shader);
  check_shader_compilation(vshader, vertex_shader);
  
  GLuint fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
  glShaderSource(fragment_shader, 1, (const GLchar**) &fragment_shader_source, NULL);
  glCompileShader(fragment_shader);
  check_shader_compilation(fshader, fragment_shader);
  
  program = glCreateProgram();
  glAttachShader(program, vertex_shader);
  glAttachShader(program, fragment_shader);
  
  glLinkProgram(program);
  check_program_link(program);
  
  glUseProgram(program);
  
  glBindFragDataLocation(program, 0, "fragColor");


  //Compute ambient, diffuse, and specular terms
  color4 ambient_product  = light_ambient * material_ambient;
  color4 diffuse_product  = light_diffuse * material_diffuse;
  color4 specular_product = light_specular * material_specular;
  
  //Retrieve and set uniform variables
  glUniform4fv( glGetUniformLocation(program, "Light"), 1, light);
  glUniform4fv( glGetUniformLocation(program, "AmbientProduct"), 1, ambient_product );
  glUniform4fv( glGetUniformLocation(program, "DiffuseProduct"), 1, diffuse_product );
  glUniform4fv( glGetUniformLocation(program, "SpecularProduct"), 1, specular_product );
  glUniform1f(  glGetUniformLocation(program, "Shininess"), material_shininess );
  
  //Matrix uniform variable locations
  ModelView_loc = glGetUniformLocation( program, "ModelView" );
  NormalMatrix_loc = glGetUniformLocation( program, "NormalMatrix" );
  Projection_loc = glGetUniformLocation( program, "Projection" );
  
  //===== Send data to GPU ======
  glGenVertexArrays( 1, &vao );
  
  glGenBuffers( 1, &buffer );

  BSP2GPU();
  
  //===== End: Send data to GPU ======


  // ====== Enable some opengl capabilitions ======
  glShadeModel(GL_SMOOTH);

  glClearColor( 0.2, 0.2, 0.2, 1.0 );
  
  //===== Initalize some program state variables ======

  //Quaternion trackball variables, you can ignore
  scaling  = 0;
  moving   = 0;
  panning  = 0;
  beginx   = 0;
  beginy   = 0;
  
  matident(curmat);
  trackball(curquat , 0.0f, 0.0f, 0.0f, 0.0f);
  trackball(lastquat, 0.0f, 0.0f, 0.0f, 0.0f);
  add_quats(lastquat, curquat, curquat);
  build_rotmatrix(curmat, curquat);
    
  angle = 0.0;
  
  lbutton_down = false;


  //===== End: Initalize some program state variables ======

}

void draw(GLFWwindow* window, bool left){
  
  int width, height;
  glfwGetFramebufferSize(window, &width, &height);
  
  if(left){
    glViewport(0, 0, width/2, height);
    glEnable( GL_DEPTH_TEST );
  }else{
    glViewport(width/2, 0, width/2, height);
    glDisable( GL_DEPTH_TEST );
  }
  
  GLfloat aspect = GLfloat(width)/height;
  
  //Projection matrix
  mat4  projection = Perspective( 45.0, aspect, 0.001, 15.0 );
  
  
  //Track_ball rotation matrix
  mat4 track_ball =  mat4(curmat[0][0], curmat[1][0], curmat[2][0], curmat[3][0],
                          curmat[0][1], curmat[1][1], curmat[2][1], curmat[3][1],
                          curmat[0][2], curmat[1][2], curmat[2][2], curmat[3][2],
                          curmat[0][3], curmat[1][3], curmat[2][3], curmat[3][3]);

  //Modelview based on user interaction
  mat4 user_MV  =  RotateY(angle) * Translate( -viewer_pos );
                                                  //Rotate Camera
  

  // ====== Draw ======
  glBindVertexArray(vao);
  
  glUniformMatrix4fv( ModelView_loc, 1, GL_TRUE, user_MV);
  glUniformMatrix4fv( Projection_loc, 1, GL_TRUE, projection );
  glUniformMatrix4fv( NormalMatrix_loc, 1, GL_TRUE, transpose(Invert(user_MV)));

  glDrawArrays( GL_TRIANGLES, 0, numVertices);
  // ====== End: Draw ======

  
  
}


int main(void){
  
  GLFWwindow* window;
  
  glfwSetErrorCallback(error_callback);
  
  if (!glfwInit())
    exit(EXIT_FAILURE);
  
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  
  glfwWindowHint(GLFW_SAMPLES, 4);
  
  window = glfwCreateWindow(1024, 512, "BSP Tree", NULL, NULL);
  if (!window){
    glfwTerminate();
    exit(EXIT_FAILURE);
  }
  
  //Set key and mouse callback functions
  glfwSetKeyCallback(window, key_callback);
//  glfwSetMouseButtonCallback(window, mouse_click);
//  glfwSetCursorPosCallback(window, mouse_move);

  
  glfwMakeContextCurrent(window);
  gladLoadGL(glfwGetProcAddress);
  glfwSwapInterval(1);
  
  init();
  

  
  while (!glfwWindowShouldClose(window)){
    //Display as wirframe, boolean tied to keystoke 'w'
    glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


    
    draw(window, true);
    draw(window, false);
    
    glfwSwapBuffers(window);
    glfwPollEvents();
    
  }
  
  delete tree;
  
  glfwDestroyWindow(window);
  
  glfwTerminate();
  exit(EXIT_SUCCESS);
}
