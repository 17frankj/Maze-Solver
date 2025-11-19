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
#define TEX_BRICK 1
#define TEX_STONE 2
#define TEX_GRASS 3
#define MAX_VERTS 110000
#define TWO_PI (2.0 * M_PI)

vec2 tex_face[6] = {
    {0.0f, 1.0f}, // bottom-left
    {1.0f, 1.0f}, // bottom-right
    {0.0f, 0.0f}, // top-left

    {0.0f, 0.0f}, // top-left
    {1.0f, 1.0f}, // bottom-right
    {1.0f, 0.0f}  // top-right
};

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

// basic cube primitive (1x1x1 centered) to scale/translate later
vec4 unitCubeVertices[36] = {
    // Front
    {-0.5, -0.5, 0.5, 1}, {0.5, -0.5, 0.5, 1}, {0.5, 0.5, 0.5, 1},
    {-0.5, -0.5, 0.5, 1}, {0.5, 0.5, 0.5, 1}, {-0.5, 0.5, 0.5, 1},
    // Back
    {-0.5, -0.5, -0.5, 1}, {0.5, 0.5, -0.5, 1}, {0.5, -0.5, -0.5, 1},
    {-0.5, -0.5, -0.5, 1}, {-0.5, 0.5, -0.5, 1}, {0.5, 0.5, -0.5, 1},
    // Left
    {-0.5, -0.5, -0.5, 1}, {-0.5, -0.5, 0.5, 1}, {-0.5, 0.5, 0.5, 1},
    {-0.5, -0.5, -0.5, 1}, {-0.5, 0.5, 0.5, 1}, {-0.5, 0.5, -0.5, 1},
    // Right
    {0.5, -0.5, -0.5, 1}, {0.5, 0.5, 0.5, 1}, {0.5, -0.5, 0.5, 1},
    {0.5, -0.5, -0.5, 1}, {0.5, 0.5, -0.5, 1}, {0.5, 0.5, 0.5, 1},
    // Top
    {-0.5, 0.5, -0.5, 1}, {-0.5, 0.5, 0.5, 1}, {0.5, 0.5, 0.5, 1},
    {-0.5, 0.5, -0.5, 1}, {0.5, 0.5, 0.5, 1}, {0.5, 0.5, -0.5, 1},
    // Bottom
    {-0.5, -0.5, -0.5, 1}, {0.5, -0.5, 0.5, 1}, {-0.5, -0.5, 0.5, 1},
    {-0.5, -0.5, -0.5, 1}, {0.5, -0.5, -0.5, 1}, {0.5, -0.5, 0.5, 1}
};

// basic colors
vec4 wallColor = {0.6, 0.2, 0.2, 1}; // brown walls
vec4 poleColor = {0.5, 0.5, 0.5, 1}; // gray poles
vec4 floorColor = {0.3, 0.3, 0.3, 1}; // dark gray floor

vec2 tex_coords[110000] =
{{ 0.0,  0.5},	// top
 {-0.5, -0.5},	// bottom left
 { 0.5, -0.5},	// bottom right
 };	

int vertIndex = 0;
 
// ------------global variables- --------------------------------------------------------------------------------------// 

int num_vertices = 0;   // max number of verts needed 
int tex_width = 1200;
int tex_height = 1200;

mat4 my_ctm = {{1,0,0,0},{0,1,0,0}, {0,0,1,0}, {0,0,0,1}};  // initial identity matrix for moving the objects

// converstions to open gl types
GLuint ctm_location;  
GLuint mv_loc;
GLuint pr_loc;

GLuint program;
GLuint vbo;

// maze global variables
int maze_height = 8;
int maze_width = 8; 

// camera global variables
vec4 eye;
vec4 at;
vec4 up;

// animation global variables
typedef enum
{
    NONE = 0,
    ORBIT_ONCE,
    FLYING_UP,
    FLYING_DOWN,
    WALK_FORWARD,
    WALK_BACKWARD,
    TURN_LEFT,
    TURN_RIGHT,
    LOOK_UP,
    LOOK_DOWN,
} state;
state currentState = NONE;
int isAnimating = NONE; // checks for animation running
int current_step = 0;
int max_steps;

// flying around global variables
vec4 starting_eye;
vec4 changing_eye;

float starting_angle;
float changing_angle;

float orbit_radius;
float orbit_height;

const float mazeW = 8.0f;
const float mazeH = 8.0f;
vec4 maze_center = {mazeW/2.0f, 0.0f, mazeH/2.0f, 1.0f};

// inital fustrum values
float left   = -6;
float right  = 6;
float bottom = -6;
float top    = 6;
float near   = 8.0f;
float far    = 20.0f;

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

// ----------------- cube (maze wall) functions ------------------------------------------------------------------------------------//

void addCube(mat4 T, int texType)
{
    float u0, u1, v0, v1;

    // Select quadrant based on texture type
    switch (texType)
    {
        case TEX_GRASS: // floor
            u0 = 0.0f; u1 = 0.5f;
            v0 = 0.5f; v1 = 1.0f;
            break;

        case TEX_STONE: // columns
            u0 = 0.5f; u1 = 1.0f;
            v0 = 0.0f; v1 = 0.5f;
            break;

        case TEX_BRICK: // wall
            u0 = 0.0f; u1 = 0.5f;
            v0 = 0.0f; v1 = 0.5f;
            break;

        default: // fallback
            u0 = 0.0f; u1 = 0.5f;
            v0 = 0.5f; v1 = 1.0f;
            break;
    }

    for (int i = 0; i < 36; i++)
    {
        vec4 v = unitCubeVertices[i];
        vec4 r = matrix_vector_multi(T, v);
        vertices[vertIndex] = r;

        int faceVert = i % 6;

        // Map unit square texcoords to the chosen quadrant
        tex_coords[vertIndex].x = u0 + tex_face[faceVert].x * (u1 - u0);
        tex_coords[vertIndex].y = v0 + tex_face[faceVert].y * (v1 - v0);

        vertIndex++;
    }
}

void addFloorTile(float x, float z)
{
    // place a flat cube at y = -0.5 (same height as poles)
    // scale Y very small so it's a flat tile
    mat4 T = matrix_multi(
                matrix_translation(x, -0.5f, z),
                matrix_scaling(1.0f, 0.05f, 1.0f) // thin plane
            );

    addCube(T, TEX_GRASS);
}

void buildMazeGeometry(Maze*m)
{
    //vertIndex = 0;
    float y = -0.5f; // floor plane

    // ---- Floor tiles ----
    for(int j = 0; j < m->height; j++)
    {
        for(int i = 0; i < m->width; i++)
        {
            addFloorTile(i, j);
        }
    }

    // place poles at grid intersections (width+1 x height+1)
    for(int j=0;j<=m->height;j++)
    {
        for(int i=0;i<=m->width;i++)
        {
            float x = (float)i;
            float z = (float)j;
            mat4 poleT = matrix_multi(matrix_translation(x-0.5f, y, z-0.5f), matrix_scaling(0.2,1,0.2));
            addCube(poleT, TEX_STONE);
        }
    }

    // place walls
    for(int j=0;j<m->height;j++)
    {
        for(int i=0;i<m->width;i++)
        {
            float cx = (float)i;
            float cz = (float)j;
            // north wall
            if(m->cells[j][i].north && !(i==0 && j==0))
            {
                mat4 nT = matrix_multi(matrix_translation(cx, y, cz-0.5),matrix_scaling(1,1,0.1));
                addCube(nT, TEX_BRICK);
            }
            // west wall
            if(m->cells[j][i].west)
            {
                mat4 wT = matrix_multi(matrix_translation(cx-0.5, y, cz), matrix_scaling(0.1,1,1));
                addCube(wT, TEX_BRICK);
            }
            // last row south
            if(j==m->height-1 && m->cells[j][i].south && !(i==m->width-1 && j==m->height-1))
            {
                mat4 sT = matrix_multi(matrix_translation(cx, y, cz+0.5), matrix_scaling(1,1,0.1));
                addCube(sT, TEX_BRICK);
            }
            // last col east
            if(i==m->width-1 && m->cells[j][i].east)
            {
                mat4 eT = matrix_multi(matrix_translation(cx+0.5, y, cz), matrix_scaling(0.1,1,1));
                addCube(eT, TEX_BRICK);
            }
        }
    }
    num_vertices = vertIndex;
}


// ----------------- end cube functions --------------------------------------------------------------------------------//

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

    // build the 3D maze geometry once
    buildMazeGeometry(m);

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


// -------------------------- viewing functions -------------------------------------------------------------------------------------//

mat4 look_at(vec4 eyePoint, vec4 atPoint, vec4 upVector)
{
    // 1. Compute the camera axes
    vec4 f = vec_Normalized((vec4){atPoint.x - eyePoint.x,
                                   atPoint.y - eyePoint.y,
                                   atPoint.z - eyePoint.z,
                                   0.0}); // forward
    vec4 s = vec_Normalized(vec_Cross_Product(f, upVector));  // right
    vec4 u = vec_Cross_Product(s, f);                   // true up

    // 2. Build column-major look-at matrix
    mat4 M = {
        { s.x,  u.x, -f.x, 0.0 },
        { s.y,  u.y, -f.y, 0.0 },
        { s.z,  u.z, -f.z, 0.0 },
        { -vec_Dot_product(s, eyePoint),
          -vec_Dot_product(u, eyePoint),
           vec_Dot_product(f, eyePoint),
           1.0 }
    };

    return M;
}

mat4 ortho(GLfloat left, GLfloat right, GLfloat bottom, GLfloat top, GLfloat near, GLfloat far)
{
    mat4 M = 
    {
        {2.0f / (right - left), 0.0, 0.0, 0.0},
        {0.0, 2.0f / (top - bottom), 0.0, 0.0},
        {0.0, 0.0, -2.0f / (far - near), 0.0},
        {-(right + left) / (right - left),
         -(top + bottom) / (top - bottom),
         -(far + near) / (far - near),
         1.0}
    };
    return M;
}

mat4 frustum(GLfloat left, GLfloat right, GLfloat bottom, GLfloat top, GLfloat near, GLfloat far)
{
    /*
    mat4 P = {
        {(-2.0f * near) / (right - left), 0.0f, 0.0f, 0.0f},
        {0.0f, (-2.0f * near) / (top - bottom), 0.0f, 0.0f},
        {(right + left) / (right - left), (top + bottom) / (top - bottom), (far + near) / (far - near), -1.0f},
        {0.0f, 0.0f, -((2.0f * near * far) / (far - near)), 0.0f}
    };

    return P;
    */
   mat4 P = {
        // column 0
        { (2.0f*near)/(right-left),
          0.0f,
          0.0f,
          0.0f },
        // column 1
        { 0.0f,
          (2.0f*near)/(top-bottom),
          0.0f,
          0.0f },
        // column 2
        { (right+left)/(right-left),
          (top+bottom)/(top-bottom),
          -(far+near)/(far-near),
          -1.0f },
        // column 3
        { 0.0f,
          0.0f,
          -(2.0f*near*far)/(far-near),
          0.0f }
    };
    return P;

}


// -------------------------- end viewing functions -------------------------------------------------------------------------------------//


// ---------------Open Gl Functions  --------------------------------------------------------------------------------- // 
void init(void)
{
    // set sizes for verts from STL before drawing
    my_ctm = identity(); // clear garbage in memory

    tex_width = 800;
    tex_height = 800;

    GLubyte my_texels[tex_width][tex_height][3];
    
    FILE *fp = fopen("p2texture04.raw", "r");
    if (!fp) {
        fprintf(stderr, "ERROR: Cannot open p2texture04.raw\n");
        exit(1);
    }
    fread(my_texels, tex_width * tex_height * 3, 1, fp);
    fclose(fp);

    GLuint program = initShader("vshader.glsl", "fshader.glsl");
    glUseProgram(program);

    GLuint mytex[1];
    glGenTextures(1, mytex);
    glBindTexture(GL_TEXTURE_2D, mytex[0]);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, tex_width, tex_height, 0, GL_RGB, GL_UNSIGNED_BYTE, my_texels);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

    int param;
    glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_WIDTH, &param);

    GLuint vao;
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);
    
    GLuint buffer;
    glGenBuffers(1, &buffer);
    glBindBuffer(GL_ARRAY_BUFFER, buffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vec4) * num_vertices + sizeof(vec2) * num_vertices, NULL, GL_STATIC_DRAW);

    // Upload only actual data (vertIndex == num_vertices)
    glBufferSubData(GL_ARRAY_BUFFER, 0, num_vertices * sizeof(vec4), vertices);
    glBufferSubData(GL_ARRAY_BUFFER, sizeof(vec4) * num_vertices, sizeof(vec2) * num_vertices, tex_coords);

    GLuint vPosition = glGetAttribLocation(program, "vPosition");
    glEnableVertexAttribArray(vPosition);
    glVertexAttribPointer(vPosition, 4, GL_FLOAT, GL_FALSE, 0, (GLvoid *) 0);

    GLuint vTexCoord = glGetAttribLocation(program, "vTexCoord");
    glEnableVertexAttribArray(vTexCoord);
    glVertexAttribPointer(vTexCoord, 2, GL_FLOAT, GL_FALSE, 0, (GLvoid *) (sizeof(vec4) * num_vertices));

    GLuint texture_location = glGetUniformLocation(program, "texture");
    glUniform1i(texture_location, 0);

    ctm_location = glGetUniformLocation(program, "ctm");
    mv_loc = glGetUniformLocation(program, "model_view_matrix");
    pr_loc = glGetUniformLocation(program, "projection_matrix");

    glEnable(GL_DEPTH_TEST);
    glClearColor(0.0, 0.0, 0.0, 1.0);
    glDepthRange(1,0);
}

void display(void)
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    //float mazeW = 8;
    //float mazeH = 8;

    if(!isAnimating &&currentState == NONE)
    {
        // set initial center camera eye values
        eye = (vec4){mazeW/2.0f, 10.0f, mazeH/2.0f, 1.0f};   // above center
        at  = (vec4){mazeW/2.0f, 0.0f, mazeH/2.0f, 1.0f};    // look at center on ground
        up  = (vec4){0.0f, 0.0f, -1.0f, 0.0f};
                            //    ^ changes if looking at front or back of maze with -1 or 1
    }
    //printf("Eye: (%f, %f, %f)\n", eye.x, eye.y, eye.z);
    mat4 model_view = look_at(eye, at, up);
    mat4 projection = frustum(left, right, bottom, top, near, far);
    //mat4 projection = ortho(left, right, bottom, top, near, far); // debugging

    glUniformMatrix4fv(mv_loc, 1, GL_FALSE, (GLfloat*)&model_view);
    glUniformMatrix4fv(pr_loc, 1, GL_FALSE, (GLfloat*)&projection);
    glUniformMatrix4fv(ctm_location, 1, GL_FALSE, (GLfloat*)&my_ctm);

    // --- DRAW ---
    glPolygonMode(GL_FRONT, GL_FILL);
    glPolygonMode(GL_BACK, GL_LINE);

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
        case 's': // start intro animation
            isAnimating = 1;
            currentState = FLYING_UP;  // start flying animation

            max_steps = 2000;          // set max steps for flying up
            current_step = 0;        // reset current step

            // Maze center:
            maze_center = (vec4){mazeW/2.0f, 0.0f, mazeH/2.0f, 1.0f};
            at = maze_center; 
            
            up = (vec4){0.0f, 1.0f, 0.0f, 0.0f}; 

            orbit_radius = 15.0f;
            starting_angle = -1.57f; //  Start at -PI/2
            changing_angle = 6.28f; //  Full circle 2*PI


            // --- FLY UP ANIMATION SETUP ---
            starting_eye = (vec4){0.0f, 1.0f, 0.0f, 1.0f}; // Start height at Y=2
            changing_eye = (vec4){0.0f, 15.0f, 0.0f, 0.0f}; // We will add 15 to height by the end

            break;
    }
    glutPostRedisplay();
}

// call idle function to do animation
void idle(void)
{
    if (isAnimating)
    {
        current_step++;

        if(currentState == NONE) 
        {
            
        }

        else if(currentState == FLYING_UP) 
        {
            float alpha; // current_state / max_steps

            if(current_step > max_steps) // reached end of animation
            {
                isAnimating = 0; // turn off animation
                currentState = NONE; // Reset state
                alpha = 1.0; // set to final value

            }
            else
            {
                alpha = (float)current_step / (float)max_steps;

                // current_something = (alpha * changing_vector) + starting; 
                // calculate new transformation(s)
            }

            // Current Angle = Start + (PercentComplete * TotalRotation)
            float current_theta = starting_angle + (alpha * changing_angle);

            // Current Height = StartHeight + (PercentComplete * HeightChange)
            float current_height = starting_eye.y + (alpha * changing_eye.y);

            // --- 2. Calculate New Eye Position using Polar Coordinates ---
            // x = center_x + radius * cos(theta)
            // z = center_z + radius * sin(theta)
            
            eye.x = maze_center.x + (orbit_radius * cosf(current_theta));
            eye.z = maze_center.z + (orbit_radius * sinf(current_theta));
            eye.y = current_height;
            eye.w = 1.0f;

            // --- 3. Ensure we keep looking at the center ---
            at = maze_center;
        }
    
        else if(currentState == FLYING_DOWN) 
        {
        
        }
        else if(currentState == WALK_FORWARD) 
        {
        
        }
        else if(currentState == WALK_BACKWARD) 
        {
        
        }
        else if(currentState == TURN_LEFT) 
        {
        
        }
        else if(currentState == TURN_RIGHT) 
        {
        
        }
        else if(currentState == LOOK_UP) 
        {
        
        }
        else if(currentState == LOOK_DOWN) 
        {
        
        }
        // printf("Animating...\n");
        glutPostRedisplay();
    }
    //glutPostRedisplay();
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
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
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
    glutIdleFunc(idle);
    glutMainLoop();

    return 0;
}
