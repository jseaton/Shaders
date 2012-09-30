#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>
#include <GL/glew.h>
#include <GL/glut.h>

GLuint program, fs_program;
GLuint vbo_quad;
GLint attribute_position, fs_attribute_position;
GLint uniform_texture, fs_uniform_texture;
GLint uniform_width, uniform_height;
GLuint fbo1, fbo2, texture1, texture2;
int buf;


#define TEXTURE_WIDTH 2048
#define TEXTURE_HEIGHT 2048

int init_resources() {
  GLfloat quad_vertices[] = {
    -1.0, -1.0,
    -1.0,  1.0,
     1.0,  1.0,
     1.0, -1.0
  };

  glGenBuffers(1, &vbo_quad);
  glBindBuffer(GL_ARRAY_BUFFER, vbo_quad);
  glBufferData(GL_ARRAY_BUFFER, sizeof(quad_vertices), quad_vertices, GL_STATIC_DRAW);

  GLubyte *data = (GLubyte *)malloc(TEXTURE_WIDTH*TEXTURE_HEIGHT*3*sizeof(GLubyte));
  for (int i=0;i<TEXTURE_WIDTH*TEXTURE_HEIGHT*3;i+=3) {
    if (rand() > RAND_MAX/2.0)
      data[i] = 255;
    else
      data[i] = 0;
    data[i+1] = data[i+2] = 0;
  }
  glGenTextures(1, &texture1);
  glBindTexture(GL_TEXTURE_2D, texture1);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, TEXTURE_WIDTH, TEXTURE_HEIGHT, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
  glBindTexture(GL_TEXTURE_2D, 0);

  glGenTextures(1, &texture2);
  glBindTexture(GL_TEXTURE_2D, texture2);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, TEXTURE_WIDTH, TEXTURE_HEIGHT, 0, GL_RGB, GL_UNSIGNED_BYTE, 0);
  glBindTexture(GL_TEXTURE_2D, 0);

  glGenFramebuffers(1, &fbo1);
  glBindFramebuffer(GL_FRAMEBUFFER, fbo1);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texture1, 0);
  glBindFramebuffer(GL_FRAMEBUFFER, 0);

  glGenFramebuffers(1, &fbo2);
  glBindFramebuffer(GL_FRAMEBUFFER, fbo2);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texture2, 0);
  glBindFramebuffer(GL_FRAMEBUFFER, 0);

  GLint link_ok = GL_FALSE;

  GLuint vs, fs;
  if ((vs = create_shader("life.v.glsl", GL_VERTEX_SHADER))   == 0) return 0;
  if ((fs = create_shader("life.f.glsl", GL_FRAGMENT_SHADER)) == 0) return 0;

  program = glCreateProgram();
  glAttachShader(program, vs);
  glAttachShader(program, fs);
  glLinkProgram(program);

  glGetProgramiv(program, GL_LINK_STATUS, &link_ok);
  if (!link_ok) {
    fprintf(stderr, "glLinkProgram:");
    print_log(program);
    return 0;
  }

  const char *attribute_name = "position";
  attribute_position = glGetAttribLocation(program, attribute_name);
  if (attribute_position == -1) {
    fprintf(stderr, "Could not bind attribute %s\n", attribute_name);
    return 0;
  }

  const char *uniform_name = "texture";
  uniform_texture = glGetUniformLocation(program, uniform_name);
  if (uniform_texture == -1) {
    fprintf(stderr, "Could not bind uniform %s\n", uniform_name);
    return 0;
  }

  /*uniform_name = "width";
  uniform_width = glGetUniformLocation(program, uniform_name);
  if (uniform_width == -1) {
    fprintf(stderr, "Could not bind uniform %s\n", uniform_name);
    return 0;
  }

  uniform_name = "height";
  uniform_height = glGetUniformLocation(program, uniform_name);
  if (uniform_height == -1) {
    fprintf(stderr, "Could not bind uniform %s\n", uniform_name);
    return 0;
    }*/

  ////////////////////////////////////////////////////
  //Passthrough

  link_ok = GL_FALSE;

  GLuint fs_vs, fs_fs;
  if ((fs_vs = create_shader("pass.v.glsl", GL_VERTEX_SHADER))   == 0) return 0;
  if ((fs_fs = create_shader("pass.f.glsl", GL_FRAGMENT_SHADER)) == 0) return 0;

  fs_program = glCreateProgram();
  glAttachShader(fs_program, fs_vs);
  glAttachShader(fs_program, fs_fs);
  glLinkProgram(fs_program);

  glGetProgramiv(fs_program, GL_LINK_STATUS, &link_ok);
  if (!link_ok) {
    fprintf(stderr, "glLinkProgram:");
    print_log(fs_program);
    return 0;
  }

  attribute_name = "position";
  fs_attribute_position = glGetAttribLocation(fs_program, attribute_name);
  if (fs_attribute_position == -1) {
    fprintf(stderr, "Could not bind attribute %s (fs)\n", attribute_name);
    return 0;
  }

  uniform_name = "texture";
  fs_uniform_texture = glGetUniformLocation(fs_program, uniform_name);
  if (fs_uniform_texture == -1) {
    fprintf(stderr, "Could not bind uniform %s (fs)\n", uniform_name);
    return 0;
  }

  return 1;
}

void onDisplay() {
  if (buf)
    glBindFramebuffer(GL_FRAMEBUFFER, fbo2);
  else
    glBindFramebuffer(GL_FRAMEBUFFER, fbo1);

  glViewport(0.0, 0.0, TEXTURE_WIDTH*2, TEXTURE_HEIGHT*2);
  glClearColor(0.0, 0.0, 0.0, 1.0);
  glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

  glUseProgram(program);

  glActiveTexture(GL_TEXTURE0);
  if (buf)
    glBindTexture(GL_TEXTURE_2D, texture1);
  else
    glBindTexture(GL_TEXTURE_2D, texture2);
  
  glUniform1i(uniform_texture, 0);

  //glUniform1f(uniform_width, 1.0/TEXTURE_WIDTH);
  //glUniform1f(uniform_height, 1.0/TEXTURE_HEIGHT);

  glEnableVertexAttribArray(attribute_position);
  glBindBuffer(GL_ARRAY_BUFFER, vbo_quad);
  glVertexAttribPointer(
			attribute_position,
			2,
			GL_FLOAT,
			GL_FALSE,
			0,
			0
			);
  glDrawArrays(GL_QUADS, 0, 4);
  glDisableVertexAttribArray(attribute_position);

  glBindFramebuffer(GL_FRAMEBUFFER, 0);

  ////////////////////////////////////////////////////
  //Passthrough

  glViewport(0.0, 0.0, TEXTURE_WIDTH, TEXTURE_HEIGHT);
  glClearColor(0.0, 0.0, 0.0, 1.0);
  glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

  glUseProgram(fs_program);

  glActiveTexture(GL_TEXTURE1);
  if (buf)
    glBindTexture(GL_TEXTURE_2D, texture2);
  else
    glBindTexture(GL_TEXTURE_2D, texture1);
  glUniform1i(fs_uniform_texture, 1);

  glEnableVertexAttribArray(fs_attribute_position);
  glBindBuffer(GL_ARRAY_BUFFER, vbo_quad);
  glVertexAttribPointer(
			fs_attribute_position,
			2,
			GL_FLOAT,
			GL_FALSE,
			0,
			0
			);
  glDrawArrays(GL_QUADS, 0, 4);
  glDisableVertexAttribArray(fs_attribute_position);
  glutSwapBuffers();
}

void step() {
  buf = !buf;
  glutPostRedisplay();
}

void onKey(unsigned char key, int x, int y) {
  if (key == 'a')
    glutIdleFunc(step);
  else {
    glutIdleFunc(NULL);
    step();
  }
}

void free_resources() {
  glDeleteProgram(fs_program);
  glDeleteTextures(1, &texture1);
}

int main(int argc, char* argv[]) {
  glutInit(&argc, argv);
  glutInitDisplayMode(GLUT_RGBA|GLUT_ALPHA|GLUT_DOUBLE|GLUT_DEPTH);
  glutInitWindowSize(TEXTURE_WIDTH, TEXTURE_HEIGHT);
  glutCreateWindow("Life");

  GLenum glew_status = glewInit();
  if (glew_status != GLEW_OK) {
    fprintf(stderr, "Error: %s\n", glewGetErrorString(glew_status));
    return EXIT_FAILURE;
  }
  
  if (init_resources() == 1) {
    glutDisplayFunc(onDisplay);
    glutKeyboardFunc(onKey);
    glEnable(GL_BLEND);
    glEnable(GL_DEPTH_TEST);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    buf = 1;
    glutMainLoop();
  }

  free_resources();
  return EXIT_SUCCESS;
}
