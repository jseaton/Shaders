#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>
#include <GL/glew.h>
#include <GL/glut.h>

GLuint program;
GLuint vbo_quad;
GLint attribute_position, uniform_scale, uniform_offset;
GLfloat scale;
GLfloat offsetX, offsetY;

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

  GLint link_ok = GL_FALSE;

  GLuint vs, fs;
  if ((vs = create_shader("mandelbrot.v.glsl", GL_VERTEX_SHADER))   == 0) return 0;
  if ((fs = create_shader("mandelbrot.f.glsl", GL_FRAGMENT_SHADER)) == 0) return 0;

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

  const char *uniform_name = "scale";
  uniform_scale = glGetUniformLocation(program, uniform_name);
  if (uniform_scale == -1) {
    fprintf(stderr, "Could not bind attribute %s\n", uniform_name);
    return 0;
  }

  uniform_name = "offset";
  uniform_offset = glGetUniformLocation(program, uniform_name);
  if (uniform_offset == -1) {
    fprintf(stderr, "Could not bind attribute %s\n", uniform_name);
    return 0;
  }

  return 1;
}

void onDisplay() {
  glClearColor(0.0, 0.0, 0.0, 1.0);
  glClear(GL_COLOR_BUFFER_BIT);

  glUseProgram(program);

  glUniform1f(uniform_scale, scale);
  glUniform2f(uniform_offset, offsetX, offsetY);

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

  glutSwapBuffers();
}

void onMouse(int button, int state, int x, int y) {
  if (state == GLUT_DOWN) {
    if (button == GLUT_LEFT_BUTTON) {
      //offsetX += ((float)x)/scale;
      //offsetY += ((float)y)/scale;
      scale /= 2.0;
    } else {
      scale *= 2.0;
    }
  }
  glutPostRedisplay();
}

void free_resources() {
  glDeleteProgram(program);
}

int main(int argc, char* argv[]) {
  glutInit(&argc, argv);
  glutInitDisplayMode(GLUT_RGBA|GLUT_ALPHA|GLUT_DOUBLE|GLUT_DEPTH);
  glutInitWindowSize(800, 800);
  glutCreateWindow("Mandelbrot");
  scale = 1.0;
  offsetX = 0.0;
  offsetY = 0.0;

  GLenum glew_status = glewInit();
  if (glew_status != GLEW_OK) {
    fprintf(stderr, "Error: %s\n", glewGetErrorString(glew_status));
    return EXIT_FAILURE;
  }
  
  if (init_resources() == 1) {
    glutDisplayFunc(onDisplay);
    glutMouseFunc(onMouse);
    glutMainLoop();
  }

  free_resources();
  return EXIT_SUCCESS;
}
