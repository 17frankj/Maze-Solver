/*
 * make_shapes.c
 *
 * 
 *  CS1566 Project 2: Maze Solver Created on: Octobor 29, 2025
 *      Author: Joshua Frank
 */


#ifdef __APPLE__  // include Mac OS X verions of headers

#include <OpenGL/OpenGL.h>
#include <GLUT/glut.h>

#else // non-Mac OS X operating systems

#include <GL/glew.h>
#include <GL/freeglut.h>
#include <GL/freeglut_ext.h>

#endif  // __APPLE__

#include "initShader.h"
#include "myLib.h"
#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <ctype.h>

#define BUFFER_OFFSET( offset )   ((GLvoid*) (offset))

// basic structs for the maze 
typedef struct {
    int north;
    int east;
    int south;
    int west;
    int visited; // for generation/solving
    int on_path;      // for marking the final solution
} cell;

typedef struct {
    int width;
    int height;
    cell **cells;
} Maze;

// inital decelaration of vertices
//      -inital values for debugging
vec4 vertices[110000] =
{{ 0.0,  0.5,  0.0, 1.0},	// top
 {-0.5, -0.5,  0.0, 1.0},	// bottom left
 { 0.5, -0.5,  0.0, 1.0},	// bottom right
 };	

vec4 colors[110000] =
{{1.0, 0.0, 0.0, 1.0},	// red   (for top)
 {0.0, 1.0, 0.0, 1.0},	// green (for bottom left)
 {0.0, 0.0, 1.0, 1.0},	// blue  (for bottom right)
 };	

 
// ------------global variables- --------------------------------------------------------------------------------------// 

int num_vertices = 1100000;   // max number of verts needed for stl
mat4 my_ctm = {{1,0,0,0},{0,1,0,0}, {0,0,1,0}, {0,0,0,1}};  // initial identity matrix for moving the objects

// converstions to open gl types
GLuint ctm_location;  
GLuint program;
GLuint vbo;

// maze global variables
int maze_height = 5;
int maze_width = 8; 

// msc global variables
int stl_value = 0;    // change this to swap between stl and basic object for memory allocation
float current_scale = 1;
float large_scale_value = 1.1;
float small_scale_value = 0.9;
float object_radius = 1.0;

// mouse/ motion global variables
float lastX;
float lastY;
int leftDown = 0;
float touching = 0;

// -------------end glob vars  -----------------------------------------------------------------------------------------// 

// ----------------- maze functions  ---------------------------------------------------------------------------------//

// initialize maze

Maze *create_maze(int width, int height) 
{
    Maze *m = malloc(sizeof(Maze));
    m->width = width;
    m->height = height;

    m->cells = malloc(height * sizeof(cell *));
    for (int y = 0; y < height; y++) 
    {
        m->cells[y] = malloc(width * sizeof(cell));
        for (int x = 0; x < width; x++) 
        {
            m->cells[y][x].north = 1;
            m->cells[y][x].east = 1;
            m->cells[y][x].south = 1;
            m->cells[y][x].west = 1;
            m->cells[y][x].visited = 0;
        }
    }

    return m;
}

// Recursivly backtrack to generate maze

void remove_wall(Maze *m, int x1, int y1, int x2, int y2) 
{
    if (x2 == x1 && y2 == y1 - 1) { // north
        m->cells[y1][x1].north = 0;
        m->cells[y2][x2].south = 0;
    } else if (x2 == x1 && y2 == y1 + 1) { // south
        m->cells[y1][x1].south = 0;
        m->cells[y2][x2].north = 0;
    } else if (x2 == x1 + 1 && y2 == y1) { // east
        m->cells[y1][x1].east = 0;
        m->cells[y2][x2].west = 0;
    } else if (x2 == x1 - 1 && y2 == y1) { // west
        m->cells[y1][x1].west = 0;
        m->cells[y2][x2].east = 0;
    }
}

void shuffle(int *arr, int n) 
{
    for (int i = 0; i < n - 1; i++) {
        int j = i + rand() / (RAND_MAX / (n - i) + 1);
        int t = arr[j];
        arr[j] = arr[i];
        arr[i] = t;
    }
}

void generate_maze_recursive(Maze *m, int x, int y) 
{
    m->cells[y][x].visited = 1;

    int dirs[4] = {0, 1, 2, 3}; // N, E, S, W
    shuffle(dirs, 4);

    for (int i = 0; i < 4; i++) {
        int nx = x, ny = y;
        switch (dirs[i]) {
            case 0: ny--; break;
            case 1: nx++; break;
            case 2: ny++; break;
            case 3: nx--; break;
        }

        if (nx >= 0 && ny >= 0 && nx < m->width && ny < m->height && !m->cells[ny][nx].visited) {
            remove_wall(m, x, y, nx, ny);
            generate_maze_recursive(m, nx, ny);
        }
    }
}

void generate_maze(Maze *m) 
{
    srand((unsigned) time(NULL));
    generate_maze_recursive(m, 0, 0);
}

int solve_maze(Maze *m, int x, int y, int goal_x, int goal_y) 
{
    if (x == goal_x && y == goal_y) {
        m->cells[y][x].on_path = 1;
        return 1;
    }

    m->cells[y][x].visited = 1;

    // north
    if (!m->cells[y][x].north && y > 0 && !m->cells[y - 1][x].visited)
        if (solve_maze(m, x, y - 1, goal_x, goal_y)) { m->cells[y][x].on_path = 1; return 1; }
    // east
    if (!m->cells[y][x].east && x < m->width - 1 && !m->cells[y][x + 1].visited)
        if (solve_maze(m, x + 1, y, goal_x, goal_y)) { m->cells[y][x].on_path = 1; return 1; }
    // south
    if (!m->cells[y][x].south && y < m->height - 1 && !m->cells[y + 1][x].visited)
        if (solve_maze(m, x, y + 1, goal_x, goal_y)) { m->cells[y][x].on_path = 1; return 1; }
    // west
    if (!m->cells[y][x].west && x > 0 && !m->cells[y][x - 1].visited)
        if (solve_maze(m, x - 1, y, goal_x, goal_y)) { m->cells[y][x].on_path = 1; return 1; }

    return 0;
}

void free_maze(Maze *m) 
{
    for (int y = 0; y < m->height; y++)
        free(m->cells[y]);
    free(m->cells);
    free(m);
}

void print_maze(Maze *m) 
{
    // Top border (with opening at start)
    for (int x = 0; x < m->width; x++) 
    {
        if (x == 0)
            printf("+   "); // opening at top-left
        else
            printf("+---");
    }
    printf("+\n");

    for (int y = 0; y < m->height; y++) 
    {
        // Left wall and cells
        for (int x = 0; x < m->width; x++) 
        {
            if (m->cells[y][x].west)
                printf("|");
            else
                printf(" ");

            // mark visited cells with *
            if (m->cells[y][x].on_path)
                printf(" * ");
            else
                printf("   ");
        }

        // rightmost wall, with exit at bottom-right
        if (y == m->height - 1)
            printf(" \n"); // opening at bottom-right
        else
            printf("|\n");

        // Bottom walls
        for (int x = 0; x < m->width; x++) 
        {
            if (m->cells[y][x].south)
                printf("+---");
            else
                printf("+   ");
        }
        printf("+\n");
    }
}

void testmaze(void)
{
    Maze *m = create_maze(maze_width, maze_height);
    generate_maze(m);

    // reset visited flags for solver
    for (int y = 0; y < m->height; y++)
        for (int x = 0; x < m->width; x++)
        {
            m->cells[y][x].visited = 0;
            m->cells[y][x].on_path = 0;
        }
    if (solve_maze(m, 0, 0, maze_width-1, maze_height-1))
        printf("Maze solved!\n");
    else
        printf("No solution.\n");

    print_maze(m);

    free_maze(m);
}

// ----------------- end maze funcitons  ---------------------------------------------------------------------------------//

// ----------------- object functions  ---------------------------------------------------------------------------------//

// Generate "random" numberes
vec4 pick_color(int random_num)
{

    // red
    if(random_num <= 2)
    {
        return (vec4){1.0, 0.0, 0.0, 1.0};
        
    }
    // green
    else if(random_num <= 5)
    {
        return (vec4){0.0, 1.0, 0.0, 1.0};
    }
    // blue
    else if(random_num <= 7)
    {
        return (vec4){0.0, 0.0, 1.0, 1.0};
    }
    // yellow
    else if(random_num <= 9)
    {
        return (vec4){1.0, 1.0, 0.0, 1.0};
    }
    // cyan
    else if(random_num <= 10)
    {
        return (vec4){0.0, 1.0, 1.0, 0.5};
    }
}

// tester functon
void make_first_triangle_sphere(void)
{
    vertices[0] = (vec4){-0.0625,0.0625,0.0,1.0};  // top
    vertices[1] = (vec4){-0.0625,-0.0625,0.0,1.0};  // bottom left
    vertices[2] = (vec4){0.0625,-0.0625,0.0,1.0};  // bottom right
    vertices[3] = (vec4){0.0625,0.0625,0.0,1.0};  // top right
    vertices[4] = (vec4){-0.0625,0.0625,0.0,1.0};  // top left
    vertices[5] = (vec4){0.0625,-0.0625,0.0,1.0};  // bottom right

    colors[0] = (vec4){1.0, 0.0, 0.0, 1.0}; // red
    colors[1] = (vec4){1.0, 0.0, 0.0, 1.0}; // red
    colors[2] = (vec4){1.0, 0.0, 0.0, 1.0}; // red
    colors[3] = (vec4){0.0, 0.0, 1.0, 1.0};	// blue
    colors[4] = (vec4){0.0, 0.0, 1.0, 1.0};	// blue
    colors[5] = (vec4){0.0, 0.0, 1.0, 1.0};	// blue
}



// Zoom out
void make_shape_larger(void)
{
    // increment the scale value to up the radious of the stored object
    current_scale += 0.1;
    my_ctm = matrix_multi(my_ctm, matrix_scaling(1.1, 1.1, 1.1));
    object_radius = current_scale; // updates value to keep track of virtual circle for mouse boundry
}

// Zoom in
void make_shape_smaller(void)
{
    // decrease the scale value to up the radious of the stored object
    current_scale -= 0.1;
    my_ctm = matrix_multi(my_ctm, matrix_scaling(0.9, 0.9, 0.9));
    object_radius = current_scale; // updates value to keep track of virtual circle for mouse boundry
}

// -------------------------- end object funcitons  ---------------------------------------------------------------------------------//


// ---------------Open Gl Functions  --------------------------------------------------------------------------------- // 
void init(void)
{
    // set sizes for verts from STL before drawing
    my_ctm = identity(); // clear garbage in memory

    GLuint program = initShader("vshader.glsl", "fshader.glsl");
    glUseProgram(program);

    GLuint vao;
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    // Single VBO
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices) + sizeof(colors), NULL, GL_STATIC_DRAW);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
    glBufferSubData(GL_ARRAY_BUFFER, sizeof(vertices), sizeof(colors), colors);

    GLuint vPosition = glGetAttribLocation(program, "vPosition");
    glEnableVertexAttribArray(vPosition);
    glVertexAttribPointer(vPosition, 4, GL_FLOAT, GL_FALSE, 0, (GLvoid *) 0);

    GLuint vColor = glGetAttribLocation(program, "vColor");
    glEnableVertexAttribArray(vColor);  

   
    glVertexAttribPointer(vColor, 4, GL_FLOAT, GL_FALSE, 0, (GLvoid *) sizeof(vertices));
   
    ctm_location = glGetUniformLocation(program, "ctm");

    glEnable(GL_DEPTH_TEST);
    glClearColor(0.0, 0.0, 0.0, 1.0);
    glDepthRange(1,0);
}

void display(void)
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glPolygonMode(GL_FRONT, GL_FILL);
    glPolygonMode(GL_BACK, GL_LINE);

    glUniformMatrix4fv(ctm_location, 1, GL_FALSE, (GLfloat *) &my_ctm);

    
    glDrawArrays(GL_TRIANGLES, 0, num_vertices);
  

    glutSwapBuffers();
}

void keyboard(unsigned char key, int mousex, int mousey)
{
    switch(key)
    {
        case 'e': // enlarge
            make_shape_larger();
            break;

        case 'r': // reduce
            make_shape_smaller();
            break;

        case 'q': // quit application
            glutLeaveMainLoop();
            break;
    }
    glutPostRedisplay();
}

// return 1 if pointer at (glx,gly) is over the object
// i.e the mouse is "out of bounds"
int is_pointer_on_object(float glx, float gly)
{
    // where mouse is pointing
    vec4 p_screen = (vec4){ glx, gly, 0.0, 1.0};

    // transform pointer into the object space: p_obj = inverse(my_ctm) * p_screen
    mat4 inv = matrix_inverse(my_ctm);
    vec4 p_obj = matrix_vector_multi(inv, p_screen);

    // distance from origin in object space 
    // create a vec4 direction with w = 0 to compute magnitude
    vec4 p_obj_dir = (vec4){ p_obj.x, p_obj.y, p_obj.z, 0.0};
    float dist = vec_Magnitude(p_obj_dir);

    return (dist <= object_radius); // returns true if dist from origin is less than or equal to the object's radious
                                    // otherwise false
}

// make a vector to aid in mouse location and motion
vec4 project_to_sphere(float x, float y)
{
    // calculate vector values 
    float z;

    // compute distance from center
    // gives how far the pointer is from the center of the virtual sphere
    float d = x * x + y * y; 

    // only compute if pointer is within the sphere
    if (d <= 1.0)
        z = sqrtf(1.0 - d);
    else // otherwise keep z 0
        z = 0.0f;
    return (vec4){x, y, z, 0.0};
}

// get the mouse location and input from user
// then return a state based on the mouse pointer's location
void mouse(int button, int state, int x, int y)
{
    float glx = (x / 400.0) - 1.0;
    float gly = 1.0 - (y / 400.0);

    // makes sure left mouse button is pressed and is down
    if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN)
    {
        leftDown = 1;
        touching = is_pointer_on_object(glx, gly);  // check if pointer is on object's radius
        lastX = glx;
        lastY = gly;
    }
    else if (button == GLUT_LEFT_BUTTON && state == GLUT_UP)
    {
        leftDown = 0;  // sets bool down value if not currently pressed
    }

    glutPostRedisplay();
}

// find a matrix from an arbitrary angle
mat4 axis_angle_rotation(vec4 axis, float angle)
{
    float c = cosf(angle);  // cos
    float s = sinf(angle);  // sin
    float t = 1.0 - c;     // tan

    float x = axis.x;
    float y = axis.y;
    float z = axis.z;

    mat4 m;  // generic mat4

    // calculate the arbitrary matrix coordinates with cos, sin, tan of provided angle
    m.x = (vec4){t*x*x + c,     t*x*y - s*z,   t*x*z + s*y,   0.0}; 
    m.y = (vec4){t*x*y + s*z,   t*y*y + c,     t*y*z - s*x,   0.0};
    m.z = (vec4){t*x*z - s*y,   t*y*z + s*x,   t*z*z + c,     0.0};
    m.w = (vec4){0.0,           0.0,           0.0,           1.0}; // just assigns this struct as a mat4 (0) instead of a point(1)

    return m; // returns the arbitrary matrix 
}

// calculates object location based on the users actions
void motion(int x, int y)
{
    // make sure the user is pressing their keys and the pointer location is in bounds
    if (!leftDown)
        return;
    if (!touching)
        return;

    // get loaction of mouse pointer 
    // based on a 400 / 400 pixel display
    float glx = (x / 400.0f) - 1.0f;
    float gly = 1.0f - (y / 400.0f);

    // if pointer has moved off the object, stop rotating
    // this is checked every frame!
    if (!is_pointer_on_object(glx, gly))
    {
        touching = 0;    // stop the rotation session
        return;
    }

    // keep previous and current points on the "virtual sphere"
    vec4 v1 = project_to_sphere(lastX, lastY);
    vec4 v2 = project_to_sphere(glx, gly);

    // rotation axis = cross product(v2, v1)
    // cross product of the previous and current virtual spheres 
    vec4 axis = vec_Cross_Product(v2, v1);  // make this v1, v2 for inverse controls!!!!
    float axis_len = vec_Magnitude(axis);
    axis = vec_Normalized(axis);  

    // gets the angle of the two vectors using the inverse 
    // angle = arccos(dot(v1,v2))
    float dot = vec_Dot_product(v1, v2);
    if (dot > 1.0) 
    {
        dot = 1.0;
    }
    if (dot < -1.0) 
    {
        dot = -1.0;
    }
    float angle = acosf(dot);

    // rotation matrix about arbitrary axis
    mat4 rotation = axis_angle_rotation(axis, angle);

    // apply rotation about origin
    //my_ctm = matrix_multi(rotation, my_ctm);
    my_ctm = matrix_multi(my_ctm, rotation);
   
    lastX = glx;
    lastY = gly;
    glutPostRedisplay();
}

// ------------------------- end open Gl Functions --------------------------------------------------------------------------------- //

// ------------------------ MSC Functions ------------------------------------------------------------------------------------------ //

// checks if user's file ext matches stl
int has_extension(const char *filename, const char *ext) {
    const char *dot = strrchr(filename, '.');
    if (!dot || dot == filename) return 0;

    // compare case-insensitively
    while (*dot && *ext) {
        if (tolower((unsigned char)*dot) != tolower((unsigned char)*ext))
            return 0;
        dot++;
        ext++;
    }
    return *dot == '\0' && *ext == '\0';
}

// basic selection menu for shapes
void menu(void)
{
    char line[100];
    printf("Welcome to Maze Solver\n");

}

int main(int argc, char **argv)
{
    // inital parameters 
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);
    glutInitWindowSize(800, 800);
    glutInitWindowPosition(100,100);
    glutCreateWindow("Maze Solver");

    // do prompt before drawing anything
    menu();
    testmaze();

    // now draw the shapes
    glewInit();
    init();
    glutDisplayFunc(display);
    glutKeyboardFunc(keyboard);
    glutMouseFunc(mouse);
    glutMotionFunc(motion);
    glutMainLoop();

    return 0;
}
