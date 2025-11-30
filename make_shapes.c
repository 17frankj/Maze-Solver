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
#define CUBE_VERTICES 36
#define DEG_TO_RAD (M_PI / 180.0)

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
 
vec4 normals[110000] =
{{0.0, 0.0, 1.0, 0.0},	// facing z+
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

const vec4 cubeNormals[36] = {
    // Face 1: Right (+X)
    {1.0f, 0.0f, 0.0f, 0.0f}, {1.0f, 0.0f, 0.0f, 0.0f}, {1.0f, 0.0f, 0.0f, 0.0f},
    {1.0f, 0.0f, 0.0f, 0.0f}, {1.0f, 0.0f, 0.0f, 0.0f}, {1.0f, 0.0f, 0.0f, 0.0f},
    // Face 2: Left (-X)
    {-1.0f, 0.0f, 0.0f, 0.0f}, {-1.0f, 0.0f, 0.0f, 0.0f}, {-1.0f, 0.0f, 0.0f, 0.0f},
    {-1.0f, 0.0f, 0.0f, 0.0f}, {-1.0f, 0.0f, 0.0f, 0.0f}, {-1.0f, 0.0f, 0.0f, 0.0f},
    // Face 3: Top (+Y)
    {0.0f, 1.0f, 0.0f, 0.0f}, {0.0f, 1.0f, 0.0f, 0.0f}, {0.0f, 1.0f, 0.0f, 0.0f},
    {0.0f, 1.0f, 0.0f, 0.0f}, {0.0f, 1.0f, 0.0f, 0.0f}, {0.0f, 1.0f, 0.0f, 0.0f},
    // Face 4: Bottom (-Y)
    {0.0f, -1.0f, 0.0f, 0.0f}, {0.0f, -1.0f, 0.0f, 0.0f}, {0.0f, -1.0f, 0.0f, 0.0f},
    {0.0f, -1.0f, 0.0f, 0.0f}, {0.0f, -1.0f, 0.0f, 0.0f}, {0.0f, -1.0f, 0.0f, 0.0f},
    // Face 5: Front (+Z)
    {0.0f, 0.0f, 1.0f, 0.0f}, {0.0f, 0.0f, 1.0f, 0.0f}, {0.0f, 0.0f, 1.0f, 0.0f},
    {0.0f, 0.0f, 1.0f, 0.0f}, {0.0f, 0.0f, 1.0f, 0.0f}, {0.0f, 0.0f, 1.0f, 0.0f},
    // Face 6: Back (-Z)
    {0.0f, 0.0f, -1.0f, 0.0f}, {0.0f, 0.0f, -1.0f, 0.0f}, {0.0f, 0.0f, -1.0f, 0.0f},
    {0.0f, 0.0f, -1.0f, 0.0f}, {0.0f, 0.0f, -1.0f, 0.0f}, {0.0f, 0.0f, -1.0f, 0.0f}
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

// entrence and exit squares
mat4 entrance_ctm;
mat4 exit_ctm;
float startX;
float startY;
float goalX;
float goalY;

// converstions to open gl types
GLuint ctm_location;  
GLuint mv_loc;
GLuint pr_loc;

GLuint program;
GLuint vbo;

// maze global variables
int maze_height = 8;
int maze_width = 8; 

typedef struct { 
    int x; 
    int y; 
} PathNode;

PathNode solution_path[2048]; // Array to store the steps
int solution_length = 0;      // How many steps in the path
int solve_index = 0;          // Which step we are currently animating

// IMPORTANT: You must update your 'testmaze' function to assign the maze to this pointer!
// Remove 'free_maze(m)' from testmaze so the data persists.
Maze *global_maze_ptr = NULL;

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
    SIDE_STEP_LEFT,
    SIDE_STEP_RIGHT,
    SOLVE_WALK,
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
int maze_num_vertices = 0; // number of vertices in the maze (without sun)

// flying down global variables
vec4 starting_at;
vec4 changing_at;
vec4 target_at;

// moving animation global variables
float dx;
float dz;
float magnitude;
float step_size;

// inital fustrum values
float near   = 0.2f;   // Moved closer so objects 2.5 units away are visible
float far    = 2000.0f;

// Maintain FOV ratio: (0.3 / 0.5) = 0.6
// New value: 0.1 * 0.6 = 0.06
float left   = -0.12f;  
float right  = 0.12f;   
float bottom = -0.12f;  
float top    = 0.12f;

// Sun variables
vec4 light_pos = {0.0f, 10.0f, 0.0f, 1.0f}; // initial light position
float sun_radius = 10.0f;
float sun_azimuth = 45.0f;
float sun_elevation = 35.0f;
vec4 light_source_color = {1.0f, 0.9f, 0.6f, 1.0f}; // warm sunlight color
int inital_sun = 1; // flag to indicate if sun position needs initial update
int sun_cube_start_index; // Start index of the sun cube's 36 vertices
mat4 sun_ctm; // Holds the sun's calculated translation/scale matrix

GLuint sun_mode_loc;
int sun_mode_toggle = 1;
GLuint mv_loc;
GLuint pr_loc;
GLuint light_pos_loc;
GLuint shininess_loc;
GLuint light_color_loc;
GLuint spot_cutoff_loc;
GLuint spot_exponent_loc;

GLuint amb_intensity_loc;
GLuint diff_intensity_loc;
GLuint spec_color_loc;

vec4 ambient_intensity = {0.15f, 0.15f, 0.15f, 1.0f};  // Medium-low ambient light for visible shadows
vec4 diffuse_intensity = {1.0, 1.0, 1.0, 1.0};  // Reduced diffuse contribution to prevent overexposure
vec4 specular_color = {1.0f, 1.0f, 1.0f, 1.0f};      // Specular color remains white

// GLOBAL LIGHTING/MATERIAL PROPERTIES (Example values)
vec4 sun_position = {0.0f, 10.0f, 0.0f, 1.0f}; // Position of sun light
float material_shininess = 10.0f;               // Example shininess value

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

        // normals
        vec4 n = cubeNormals[i];
        n.w = 0.0f;
        vec4 transformed_normal = matrix_vector_multi(T, n);
        normals[vertIndex] = transformed_normal;

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
                matrix_translation(x, -1.0f, z),
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
    maze_num_vertices = vertIndex; // store maze vertex count
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

// Helper to reverse the path array so it matches the format expected by idle()
// (Goal at index 0, Start at index Length-1)
void reverse_path_array() {
    int start = 0;
    int end = solution_length - 1;
    while (start < end) {
        PathNode temp = solution_path[start];
        solution_path[start] = solution_path[end];
        solution_path[end] = temp;
        start++;
        end--;
    }
}

// Check if a specific wall is open relative to the cell
int is_wall_blocking(Maze *m, int x, int y, int direction) {
    // 0: North, 1: East, 2: South, 3: West
    switch(direction) {
        case 0: return m->cells[y][x].north;
        case 1: return m->cells[y][x].east;
        case 2: return m->cells[y][x].south;
        case 3: return m->cells[y][x].west;
    }
    return 1; // Default to blocked
}

// Left-Hand Rule Algorithm
int solve_left_hand_rule(Maze *m, int startX, int startY, int startDir, int goalX, int goalY) {
    int x = startX;
    int y = startY;
    int dir = startDir; // 0=N, 1=E, 2=S, 3=W
    
    solution_length = 0;
    
    // Add start point
    solution_path[solution_length++] = (PathNode){x, y};

    // Safety limit to prevent infinite loops
    int max_steps = 2000; 
    
    while ((x != goalX || y != goalY) && solution_length < max_steps) {
        
        // Relative directions based on current facing
        int leftDir = (dir + 3) % 4;
        int rightDir = (dir + 1) % 4;
        int backDir = (dir + 2) % 4;

        // Priority 1: Try turning Left
        if (!is_wall_blocking(m, x, y, leftDir)) {
            dir = leftDir;
        } 
        // Priority 2: Try going Straight
        else if (!is_wall_blocking(m, x, y, dir)) {
            // dir stays same
        }
        // Priority 3: Try turning Right
        else if (!is_wall_blocking(m, x, y, rightDir)) {
            dir = rightDir;
        }
        // Priority 4: Dead End, Turn Around
        else {
            dir = backDir;
        }

        // Move one step in the chosen direction
        switch(dir) {
            case 0: y--; break; // North
            case 1: x++; break; // East
            case 2: y++; break; // South
            case 3: x--; break; // West
        }

        // Record step
        solution_path[solution_length++] = (PathNode){x, y};
    }

    if (x == goalX && y == goalY) {
        // The animation system expects the array to be [Goal, ..., Start]
        // But we generated [Start, ..., Goal]. We must reverse it.
        reverse_path_array();
        return 1;
    }

    return 0; // Failed or too long
}

// Recursive function to find path from (x,y) to (goal_x, goal_y)
int find_path_from(Maze *m, int x, int y, int goal_x, int goal_y) 
{
    // 1. Bounds check
    if (x < 0 || y < 0 || x >= m->width || y >= m->height) return 0;
    
    // 2. Check if visited or solving (we re-use the 'visited' flag for the search)
    if (m->cells[y][x].visited) return 0;

    m->cells[y][x].visited = 1; // Mark as part of current search path

    // 3. Check if we reached the goal
    if (x == goal_x && y == goal_y) {
        solution_path[solution_length++] = (PathNode){x, y};
        return 1; // Found it!
    }

    // 4. Try all open neighbors
    // We try them in order. If one returns true, we add ourselves to the path and return true.

    // North (y-1)
    if (!m->cells[y][x].north) {
        if (find_path_from(m, x, y-1, goal_x, goal_y)) {
            solution_path[solution_length++] = (PathNode){x, y};
            return 1;
        }
    }
    // East (x+1)
    if (!m->cells[y][x].east) {
        if (find_path_from(m, x+1, y, goal_x, goal_y)) {
            solution_path[solution_length++] = (PathNode){x, y};
            return 1;
        }
    }
    // South (y+1)
    if (!m->cells[y][x].south) {
        if (find_path_from(m, x, y+1, goal_x, goal_y)) {
            solution_path[solution_length++] = (PathNode){x, y};
            return 1;
        }
    }
    // West (x-1)
    if (!m->cells[y][x].west) {
        if (find_path_from(m, x-1, y, goal_x, goal_y)) {
            solution_path[solution_length++] = (PathNode){x, y};
            return 1;
        }
    }

    return 0; // Dead end
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

void draw_sun(void) {
    // Convert degrees to radians
    float az_rad = sun_azimuth * M_PI / 180.0f;
    float el_rad = sun_elevation * M_PI / 180.0f;

    // Calculate Cartesian Coordinates (X, Y, Z) from Spherical Coordinates
    
    // R * cos(Elevation) gives the projection onto the XZ plane
    float horizontal_projection = sun_radius * cosf(el_rad);
    
    // X = horizontal_projection * cos(Azimuth)
    float x = horizontal_projection * cosf(az_rad);
    
    // Z = horizontal_projection * sin(Azimuth)
    float z = horizontal_projection * sinf(az_rad);
    
    // Y = R * sin(Elevation)
    float y = sun_radius * sinf(el_rad);

    // Update the global light position vector
    light_pos.x = x;
    light_pos.y = y;
    light_pos.z = z;
    light_pos.w = 1.0f; // 1.0 indicates a point light (not directional)

    // Draw the sun as a small cube at the calculated position
    sun_ctm = matrix_multi(
                    matrix_translation(light_pos.x, light_pos.y, light_pos.z),
                    matrix_scaling(0.2f, 0.2f, 0.2f)
                );
    //addCube(sunT, TEX_STONE);
    addCube(sun_ctm, TEX_STONE);
    inital_sun = 0;
    //printf("Sun position updated to (%.2f, %.2f, %.2f)\n", light_pos.x, light_pos.y, light_pos.z);
    //num_vertices += 36; // sun uses one cube (36 vertices)
}

void draw_entrence_square(void)
{
    // Uses the calculated global startX and startY
    float entrance_x = (float)startX;
    float entrance_z = (float)startY;
    
    entrance_ctm = matrix_multi(
                    matrix_translation(entrance_x, -0.95f, entrance_z),
                    matrix_scaling(1.0f, 0.05f, 1.0f)
                );
    addCube(entrance_ctm, TEX_STONE);
}

void draw_exit_square(void)
{
    // Uses the calculated global goalX and goalY
    float exit_x = (float)goalX;
    float exit_z = (float)goalY;
    
    exit_ctm = matrix_multi(
                    matrix_translation(exit_x, -0.95f, exit_z),
                    matrix_scaling(1.0f, 0.05f, 1.0f)
    );
    addCube(exit_ctm, TEX_STONE);
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
    global_maze_ptr = m; // Store maze globally for rendering
    num_vertices += 36; // sun uses one cube (36 vertices)
    inital_sun = 0;
    //free_maze(m);
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

// -------------------------- lighting functions -------------------------------------------------------------------------------------//


void set_ambient(float r, float g, float b, float a) {
    ambient_intensity = (vec4){r, g, b, a};
    glUniform4fv(amb_intensity_loc, 1, (GLfloat*)&ambient_intensity);
}

void set_diffuse(float r, float g, float b, float a) {
    diffuse_intensity = (vec4){r, g, b, a};
    glUniform4fv(diff_intensity_loc, 1, (GLfloat*)&diffuse_intensity);
}

void set_specular(float r, float g, float b, float a) {
    specular_color = (vec4){r, g, b, a};
    glUniform4fv(spec_color_loc, 1, (GLfloat*)&specular_color);
}


// -------------------------- end lighting functions -------------------------------------------------------------------------------------//

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
    // glBufferData(GL_ARRAY_BUFFER, sizeof(vec4) * num_vertices + sizeof(vec2) * num_vertices, NULL, GL_STATIC_DRAW); 
    glBufferData(GL_ARRAY_BUFFER, sizeof(vec4) * num_vertices + sizeof(vec4) * num_vertices + sizeof(vec2) * num_vertices, NULL, GL_STATIC_DRAW); // added

    // Upload only actual data (vertIndex == num_vertices)
    glBufferSubData(GL_ARRAY_BUFFER, 0, num_vertices * sizeof(vec4), vertices);
    glBufferSubData(GL_ARRAY_BUFFER, sizeof(vec4) * num_vertices, num_vertices * sizeof(vec4), normals); // ADDED NORMALS
    glBufferSubData(GL_ARRAY_BUFFER, sizeof(vec4) * num_vertices * 2, sizeof(vec2) * num_vertices, tex_coords);
    //glBufferSubData(GL_ARRAY_BUFFER, sizeof(vec4) * num_vertices, sizeof(vec2) * num_vertices, tex_coords); // old

    GLuint vPosition = glGetAttribLocation(program, "vPosition");
    glEnableVertexAttribArray(vPosition);
    glVertexAttribPointer(vPosition, 4, GL_FLOAT, GL_FALSE, 0, (GLvoid *) 0);

    // ADDED vNormal ATTRIBUTE
    GLuint vNormal = glGetAttribLocation(program, "vNormal");
    glEnableVertexAttribArray(vNormal);
    glVertexAttribPointer(vNormal, 4, GL_FLOAT, GL_FALSE, 0, (GLvoid *) (sizeof(vec4) * num_vertices));

    GLuint vTexCoord = glGetAttribLocation(program, "vTexCoord");
    glEnableVertexAttribArray(vTexCoord);
    // glVertexAttribPointer(vTexCoord, 2, GL_FLOAT, GL_FALSE, 0, (GLvoid *) (sizeof(vec4) * num_vertices)); // old
    glVertexAttribPointer(vTexCoord, 2, GL_FLOAT, GL_FALSE, 0, (GLvoid *) (sizeof(vec4) * num_vertices * 2));

    ctm_location = glGetUniformLocation(program, "ctm");
    mv_loc = glGetUniformLocation(program, "model_view_matrix");
    pr_loc = glGetUniformLocation(program, "projection_matrix");

    light_color_loc = glGetUniformLocation(program, "uLightColor");
    // Set the light source color with the new yellow value
    glUniform4fv(light_color_loc, 1, (GLfloat*)&light_source_color);

    // --- Uniform Locations (New Lighting Intensities) ---
    amb_intensity_loc = glGetUniformLocation(program, "uAmbientIntensity");
    diff_intensity_loc = glGetUniformLocation(program, "uDiffuseIntensity");
    spec_color_loc = glGetUniformLocation(program, "uSpecularColor");

    // --- Set Default Lighting Values (Set once during init) ---
    glUniform4fv(amb_intensity_loc, 1, (GLfloat*)&ambient_intensity);
    glUniform4fv(diff_intensity_loc, 1, (GLfloat*)&diffuse_intensity);
    glUniform4fv(spec_color_loc, 1, (GLfloat*)&specular_color);

    // --- Uniform Locations (New Lighting) ---
    light_pos_loc = glGetUniformLocation(program, "light_position");
    shininess_loc = glGetUniformLocation(program, "shininess");

    GLuint texture_location = glGetUniformLocation(program, "simpler2D_texture");
    //GLuint texture_location = glGetUniformLocation(program, "texture");        // old
    glUniform1i(texture_location, 0);


    // Get the location of the uniform named "myIntValue"
    sun_mode_loc = glGetUniformLocation(program, "sun_mode_toggle");

    // Get the locations for the new spotlight uniforms
    spot_cutoff_loc = glGetUniformLocation(program, "spot_cutoff");
    spot_exponent_loc = glGetUniformLocation(program, "spot_exponent");

    // Define flashlight parameters (e.g., 15-degree half-angle cone, moderate falloff)
    GLfloat cutoff_angle = 15.0; // 15 degrees half-angle (30 degree total cone)
    GLfloat cutoff_cos = cos(cutoff_angle * DEG_TO_RAD);
    GLfloat exponent_val = 20.0; // Exponent for sharp falloff

    // Set initial values for the new spotlight uniforms
    glUniform1f(spot_cutoff_loc, cutoff_cos);
    glUniform1f(spot_exponent_loc, exponent_val);

    glEnable(GL_DEPTH_TEST);
    glClearColor(0.0, 0.0, 0.0, 1.0);
    glDepthRange(0,1);
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
        up  = (vec4){0.0f, 0.0f, 1.0f, 0.0f};
                            //    ^ changes if looking at front or back of maze with -1 or 1
    }
    //printf("Eye: (%f, %f, %f)\n", eye.x, eye.y, eye.z);
    mat4 model_view = look_at(eye, at, up);
    mat4 projection = frustum(left, right, bottom, top, near, far);
    //mat4 projection = ortho(left, right, bottom, top, near, far); // debugging

    glUniformMatrix4fv(mv_loc, 1, GL_FALSE, (GLfloat*)&model_view);
    glUniformMatrix4fv(pr_loc, 1, GL_FALSE, (GLfloat*)&projection);
    glUniformMatrix4fv(ctm_location, 1, GL_FALSE, (GLfloat*)&my_ctm);
    
    if(inital_sun == 1)
    {
        draw_sun();
        //draw_entrence_square();
        //draw_exit_square();
    }

    // Pass the updated light position to the VShader uniform
    // light_pos is the calculated position from draw_sun
    glUniform4fv(light_pos_loc, 1, (GLfloat*)&light_pos);
    glUniform1i(sun_mode_loc, sun_mode_toggle); // pass sun mode toggle to shader

    // --- DRAW ---
    glPolygonMode(GL_FRONT, GL_FILL);
    glPolygonMode(GL_BACK, GL_LINE);

    glDrawArrays(GL_TRIANGLES, 0, num_vertices-(36)); // draw all but sun cube

    // --- DRAW SUN CUBE (DYNAMIC) ---
    // The sun cube must use its own transformation matrix (sun_ctm)
    glUniformMatrix4fv(ctm_location, 1, GL_FALSE, (GLfloat*)&sun_ctm);
    glDrawArrays(GL_TRIANGLES, maze_num_vertices, 36);

    glUniformMatrix4fv(ctm_location, 1, GL_FALSE, (GLfloat*)&entrance_ctm);
    glDrawArrays(GL_TRIANGLES, maze_num_vertices, 36);

    glUniformMatrix4fv(ctm_location, 1, GL_FALSE, (GLfloat*)&exit_ctm);
    glDrawArrays(GL_TRIANGLES, maze_num_vertices, 36);

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
        case 't': // start intro animation
            isAnimating = 1;
            currentState = FLYING_UP;  // start flying animation

            max_steps = 1;          // set max steps for flying up   // was 2000!!!!!!
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
        case 'w': // walk forward

            isAnimating = 1;
            currentState = WALK_FORWARD;  // start forward animation

            max_steps = 100;          // set max steps 
            current_step = 0;        // reset current step

            // 1. Capture Start Points
            starting_eye = eye;
            starting_at = at;

            // 2. Calculate Forward Vector (Where are we looking?)
            // Direction = At - Eye
            dx = at.x - eye.x;
            dz = at.z - eye.z;

            // 3. Normalize (Make length 1.0)
            // We ignore Y so we walk flat on the ground
            magnitude = sqrtf(dx*dx + dz*dz);
            if (magnitude == 0) magnitude = 1.0f; // Safety check

            step_size = 1.0f; // Move 1 cube unit

            // 4. Set Changing Vectors
            // Move Eye forward
            changing_eye.x = (dx / magnitude) * step_size;
            changing_eye.y = 0.0f; // Don't change height while walking
            changing_eye.z = (dz / magnitude) * step_size;

            // Move Look-At point by the SAME amount 
            // (This keeps the camera rotation locked, looking in the same direction)
            changing_at = changing_eye;
            //changing_at.z = target_at.z - starting_at.z;
            break;

        case 's': // walk backward
            // similar to walk forward but reverse direction
            isAnimating = 1;
            currentState = WALK_BACKWARD;  // start backwards animation

            max_steps = 100;          // set max steps 
            current_step = 0;        // reset current step

            // 1. Capture Start Points
            starting_eye = eye;
            starting_at = at;

            // 2. Calculate backward Vector (Where are we looking?)
            // Direction = At - Eye
            dx = at.x - eye.x;
            dz = at.z - eye.z;

            // 3. Normalize (Make length 1.0)
            // We ignore Y so we walk flat on the ground
            magnitude = sqrtf(dx*dx + dz*dz);
            if (magnitude == 0) magnitude = 1.0f; // Safety check

            step_size = 1.0f; // Move 1 cube unit

            // 4. Set Changing Vectors
            // Move Eye backward
            changing_eye.x = -(dx / magnitude) * step_size;
            changing_eye.y = 0.0f; // Don't change height while walking
            changing_eye.z = -(dz / magnitude) * step_size;

            // Move Look-At point by the SAME amount 
            // (This keeps the camera rotation locked, looking in the same direction)
            changing_at = changing_eye;
            break;
        case 'd': // strafe right
            // similar to walk forward but reverse direction
            isAnimating = 1;
            currentState = SIDE_STEP_RIGHT;  // start right animation

            max_steps = 100;          // set max steps 
            current_step = 0;        // reset current step

            // 1. Capture Start Points
            starting_eye = eye;
            starting_at = at;

            // 2. Calculate left Vector (Where are we looking?)
            // Direction = At - Eye
            dx = at.x - eye.x;
            dz = at.z - eye.z;

            // 3. Normalize (Make length 1.0)
            // We ignore Y so we walk flat on the ground
            magnitude = sqrtf(dx*dx + dz*dz);
            if (magnitude == 0) magnitude = 1.0f; // Safety check

            step_size = 1.0f; // Move 1 cube unit

            // 4. Set Changing Vectors
            // Move Eye left
            changing_eye.x = -(dz / magnitude) * step_size;
            changing_eye.y = 0.0f; // Don't change height while walking
            changing_eye.z = (dx / magnitude) * step_size;

            // Move Look-At point by the SAME amount 
            // (This keeps the camera rotation locked, looking in the same direction)
            changing_at = changing_eye;
            break;
        case 'a': // strafe left
            // similar to walk forward but reverse direction
            isAnimating = 1;
            currentState = SIDE_STEP_LEFT;  // start left animation

            max_steps = 100;          // set max steps 
            current_step = 0;        // reset current step

            // 1. Capture Start Points
            starting_eye = eye;
            starting_at = at;

            // 2. Calculate left Vector (Where are we looking?)
            // Direction = At - Eye
            dx = at.x - eye.x;
            dz = at.z - eye.z;

            // 3. Normalize (Make length 1.0)
            // We ignore Y so we walk flat on the ground
            magnitude = sqrtf(dx*dx + dz*dz);
            if (magnitude == 0) magnitude = 1.0f; // Safety check

            step_size = 1.0f; // Move 1 cube unit

            // 4. Set Changing Vectors
            // Move Eye left
            changing_eye.x = (dz / magnitude) * step_size;
            changing_eye.y = 0.0f; // Don't change height while walking
            changing_eye.z = -(dx / magnitude) * step_size;

            // Move Look-At point by the SAME amount 
            // (This keeps the camera rotation locked, looking in the same direction)
            changing_at = changing_eye;
            break;
        case 'z': // turn left
            isAnimating = 1;
            currentState = TURN_LEFT; 
            max_steps = 100; // Fast turn
            current_step = 0;

            starting_eye = eye;
            starting_at = at; 

            // We use changing_angle for rotation amount
            // PI/2 = 90 degrees (Left turn)
            changing_angle = M_PI / 2.0f;
            break;
        case 'x': // turn right
            isAnimating = 1;
            currentState = TURN_RIGHT; 
            max_steps = 100; // Fast turn
            current_step = 0;

            starting_eye = eye;
            starting_at = at; 

            // We use changing_angle for rotation amount
            // -PI/2 = -90 degrees (Right turn)
            changing_angle = M_PI / 2.0f;
            break;
        case 'y': // solve maze
            // implement later
            break;

        case 'l': // L for "Left-Hand Rule"
            if (!global_maze_ptr) break;

            // 1. Get Current Position
            int lh_startX = (int)roundf(eye.x);
            int lh_startY = (int)roundf(eye.z);

            // Constraint: Ignore if outside the maze
            if (lh_startY < 0 || lh_startX < 0 || lh_startX >= global_maze_ptr->width) {
                printf("Ignored: Outside maze boundaries.\n");
                break;
            }

            // 2. Determine Current Facing Direction (0=N, 1=E, 2=S, 3=W)
            // We use the vector (at - eye) to determine this.
            float faceX = at.x - eye.x;
            float faceZ = at.z - eye.z;
            int startDir = 0;
            
            if (fabs(faceX) > fabs(faceZ)) {
                startDir = (faceX > 0) ? 1 : 3; // East or West
            } else {
                startDir = (faceZ > 0) ? 2 : 0; // South or North
            }

            // 3. Solve using Left-Hand Rule
            int lh_found = solve_left_hand_rule(global_maze_ptr, lh_startX, lh_startY, startDir, global_maze_ptr->width-1, global_maze_ptr->height-1);

            if (lh_found) {
                printf("Left-Hand Path found! Steps: %d\n", solution_length);
                
                isAnimating = 1;
                currentState = SOLVE_WALK; 
                solve_index = solution_length - 2; 
                max_steps = 600; // 60 frames per step
                current_step = 0;

                // Snap eye to grid to align start
                eye.x = (float)lh_startX;
                eye.z = (float)lh_startY;

                // Setup First Step Animation
                starting_eye = eye;
                starting_at = at;

                PathNode nextNode = solution_path[solve_index];
                vec4 target_eye = (vec4){(float)nextNode.x, eye.y, (float)nextNode.y, 1.0f};

                changing_eye.x = target_eye.x - starting_eye.x;
                changing_eye.y = 0.0f;
                changing_eye.z = target_eye.z - starting_eye.z;

                // Set View Target
                float dirX = (float)nextNode.x - eye.x;
                float dirZ = (float)nextNode.y - eye.z;
                
                vec4 final_at;
                final_at.x = target_eye.x + dirX;
                final_at.y = eye.y; 
                final_at.z = target_eye.z + dirZ;
                final_at.w = 1.0f;

                changing_at.x = final_at.x - starting_at.x;
                changing_at.y = final_at.y - starting_at.y;
                changing_at.z = final_at.z - starting_at.z;
            } else {
                printf("Left-Hand Rule failed (Loop detected or goal unreachable)\n");
            }
            break;
        
        case 'p': // P for "Path" or "Play Solve"
            if (!global_maze_ptr) {
                printf("Error: global_maze_ptr is NULL. Please set it in testmaze()!\n");
                break;
            }

            // 1. Reset Maze Visited Flags (Clean slate for search)
            for(int j=0; j<global_maze_ptr->height; j++) {
                for(int i=0; i<global_maze_ptr->width; i++) {
                    global_maze_ptr->cells[j][i].visited = 0;
                }
            }

            // 2. Get Current Position (Rounded to nearest integer grid coordinate)
            int startX = (int)roundf(eye.x);
            int startY = (int)roundf(eye.z);

            // Handle being "outside" the maze at the entrance (z < 0)
            int is_outside = 0;
            if (startY < 0) {
                startY = 0; // Force pathfinding to start at the first cell (0,0)
                startX = 0; 
                is_outside = 1; // Mark flag so we know to animate entrance walk
            }
            
            // Safety clamp for X as well
            if (startX < 0) startX = 0;
            if (startX >= global_maze_ptr->width) startX = global_maze_ptr->width - 1;

            // 3. Calculate Path
            solution_length = 0;
            // Solve from Current -> Exit (width-1, height-1)
            int found = find_path_from(global_maze_ptr, startX, startY, global_maze_ptr->width-1, global_maze_ptr->height-1);
            
            if (found) {
                printf("Path found! Length: %d steps\n", solution_length);
                
                isAnimating = 1;
                currentState = SOLVE_WALK; // New State
                
                // The DFS adds nodes from Goal -> Start.
                // So index 0 is the Goal, index (length-1) is Start.
                // We want to move to (length-2) first.
                solve_index = solution_length - 2; 

                // Setup the first step immediately
                max_steps = 600; // Speed of each step (30 frames = 0.5 seconds per tile)
                current_step = 0;

                // Snap eye to exact grid center to prevent drift
                eye.x = (float)startX;
                eye.z = (float)startY;

                starting_eye = eye;
                
                // Determine target for first step
                PathNode nextNode = solution_path[solve_index];
                vec4 target_eye = (vec4){(float)nextNode.x, eye.y, (float)nextNode.y, 1.0f};

                changing_eye.x = target_eye.x - starting_eye.x;
                changing_eye.y = 0.0f;
                changing_eye.z = target_eye.z - starting_eye.z;

                // Face the direction of movement
                // We simply look 1 unit ahead in the direction of the step
                float dirX = (float)nextNode.x - eye.x;
                float dirZ = (float)nextNode.y - eye.z;
                
                starting_at.x = eye.x + dirX;
                starting_at.y = eye.y; // Look straight ahead
                starting_at.z = eye.z + dirZ;
                starting_at.w = 1.0f;
                
                at = starting_at; // Snap view immediately to face path
                
                // We keep 'at' relative to 'eye' fixed during the linear slide
                changing_at = changing_eye; 
            } else {
                printf("No path found from (%d, %d)\n", startX, startY);
            }
            break;
        case 'j': // increase sun elevation
            sun_elevation += 5.0f;
            //if (sun_elevation > 90.0f) sun_elevation = 90.0f;
            inital_sun = 1;
            break;
        case 'k': // decrease sun elevation
            sun_elevation -= 5.0f;
            //if (sun_elevation < 0.0f) sun_elevation = 0.0f;
            inital_sun = 1;
            break;
        case 'v': // turn off lighting
            sun_mode_toggle = 0;
            break;
        case 'b': // turn on lighting
            sun_mode_toggle = 1;
            break;
        case 'm': // toggle ambient only mode
            sun_mode_toggle = 2;
            break;
        case ',': // toggle diffuse only mode
            sun_mode_toggle = 3;
            break;
        case '.': // toggle specular only mode
            sun_mode_toggle = 4;
            break;
        case 'g': // toggle Flshlight mode
            sun_mode_toggle = 5;
            break;

    }
    printf("Sun mode toggle: %d\n", sun_mode_toggle);
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
                currentState = FLYING_DOWN; // go to next state
                current_step = 0;
                max_steps = 1; // Take 1000 frames to fly down     // was 1000!!!!!!

                starting_eye = eye; 
                starting_at = at;   // Currently looking at maze_center

                vec4 target_eye = (vec4){0.0f, 0.0f, -2.0f, 1.0f}; // Define where to go
                                                    //  ^ changes how far in

                // Calculate vector to get there
                changing_eye.x = target_eye.x - starting_eye.x;
                changing_eye.y = target_eye.y - starting_eye.y;
                changing_eye.z = target_eye.z - starting_eye.z;

                target_at = (vec4){0.0f, 0.5f, 10.0f, 1.0f}; // Look at maze entrance

                // Calculate vector to get there
                changing_at.x = target_at.x - starting_at.x;
                changing_at.y = target_at.y - starting_at.y;
                changing_at.z = target_at.z - starting_at.z;

            }
            else
            {
                alpha = (float)current_step / (float)max_steps; //

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
        }
    
        else if(currentState == FLYING_DOWN || currentState == WALK_FORWARD || currentState == WALK_BACKWARD || currentState == SIDE_STEP_LEFT || currentState == SIDE_STEP_RIGHT) 
        {
            if(current_step > max_steps) 
            {
                isAnimating = 0; 
                currentState = LOOK_DOWN; // Animation Sequence Complete

                /*
                // take the current look vector and snap it too
                float lookX = at.x - eye.x;
                float lookZ = at.z - eye.z;
                float len_2 = sqrtf(lookX*lookX + lookZ*lookZ);

                if(fabs(lookX) > fabs(lookZ)) {
                    at.x = eye.x + (lookX > 0 ? len_2 : -len_2);
                    at.z = eye.z;
                } else {
                    at.x = eye.x;
                    at.z = eye.z + (lookZ > 0 ? len_2 : -len_2);
                }
                    */
                
            }
            else
            {
                // Calculate Interpolation Alpha
                float alpha = (float)current_step / (float)max_steps;

                // 1. Interpolate Eye Position (Move from Sky to Ground)
                eye.x = starting_eye.x + (alpha * changing_eye.x);
                eye.y = starting_eye.y + (alpha * changing_eye.y);
                eye.z = starting_eye.z + (alpha * changing_eye.z);

                // 2. Interpolate Look-At Point (Shift focus from Center to Entrance)
                at.x = starting_at.x + (alpha * changing_at.x);
                at.y = starting_at.y + (alpha * changing_at.y);
                at.z = starting_at.z + (alpha * changing_at.z);
            }
        }
        else if(currentState == TURN_LEFT) 
        {
            if(current_step > max_steps)
            {
                isAnimating = 0;
                currentState = LOOK_DOWN; // Animation Complete

            }
            else
            {
                float alpha = (float)current_step / (float)max_steps;
                
                // Calculate the angle to rotate by based on progress
                float theta = alpha * changing_angle; 

                // 1. Get the original View Vector from the start of the animation
                dx = starting_at.x - starting_eye.x;
                dz = starting_at.z - starting_eye.z;

                // 2. Apply 2D Rotation Formula (Turn Left)
                // x' = x*cos(theta) + z*sin(theta)
                // z' = -x*sin(theta) + z*cos(theta)
                float dx_new = dx * cosf(theta) + dz * sinf(theta);
                float dz_new = -dx * sinf(theta) + dz * cosf(theta);

                // 3. Update 'at' by adding new vector to the stationary 'eye'
                at.x = eye.x + dx_new;
                at.z = eye.z + dz_new;
                // Keep height the same
                at.y = starting_at.y;
            }
        }
        else if(currentState == TURN_RIGHT) 
        {
            if(current_step > max_steps)
            {
                isAnimating = 0;
                currentState = LOOK_DOWN; // Animation Complete

            }
            else
            {
                float alpha = (float)current_step / (float)max_steps;
                
                // Calculate the angle to rotate by based on progress
                float theta = alpha * changing_angle; 

                // 1. Get the original View Vector from the start of the animation
                dx = starting_at.x - starting_eye.x;
                dz = starting_at.z - starting_eye.z;

                // 2. Apply 2D Rotation Formula (Turn Right)
                // x' = x*cos(theta) - z*sin(theta)
                // z' = x*sin(theta) + z*cos(theta)
                float dx_new = dx * cosf(theta) - dz * sinf(theta);
                float dz_new = dx * sinf(theta) + dz * cosf(theta);

                // 3. Update 'at' by adding new vector to the stationary 'eye'
                at.x = eye.x + dx_new;
                at.z = eye.z + dz_new;
                // Keep height the same
                at.y = starting_at.y;
            }
        }
        else if (currentState == SOLVE_WALK)
        {
            if (current_step > max_steps)
            {
                // Step Finished.
                // 1. Snap to grid to prevent drift
                eye.x = roundf(eye.x);
                eye.z = roundf(eye.z);

                // 2. Check if we have more steps
                solve_index--; 

                if (solve_index < 0) {
                    // Reached the goal!
                    isAnimating = 0;
                    currentState = NONE;
                }
                else {
                    // Setup Next Step
                    current_step = 0;
                    
                    // Start from current position/view (Continuity)
                    starting_eye = eye;
                    starting_at = at;

                    // Get next target
                    PathNode nextNode = solution_path[solve_index];
                    vec4 target_eye = (vec4){(float)nextNode.x, eye.y, (float)nextNode.y, 1.0f};

                    // Movement Vector
                    changing_eye.x = target_eye.x - starting_eye.x;
                    changing_eye.y = 0.0f;
                    changing_eye.z = target_eye.z - starting_eye.z;

                    // Look Target: Where we want to look at the END of the step
                    // (Position + Direction of travel)
                    // Note: changing_eye represents the direction of travel (length 1)
                    vec4 final_at;
                    final_at.x = target_eye.x + changing_eye.x;
                    final_at.y = eye.y;
                    final_at.z = target_eye.z + changing_eye.z;
                    final_at.w = 1.0f;

                    // Rotation Vector
                    changing_at.x = final_at.x - starting_at.x;
                    changing_at.y = final_at.y - starting_at.y;
                    changing_at.z = final_at.z - starting_at.z;
                }
            }
            else
            {
                // Linear Interpolation (Slide + Turn simultaneously)
                float alpha = (float)current_step / (float)max_steps;

                eye.x = starting_eye.x + (alpha * changing_eye.x);
                eye.y = starting_eye.y + (alpha * changing_eye.y);
                eye.z = starting_eye.z + (alpha * changing_eye.z);

                at.x = starting_at.x + (alpha * changing_at.x);
                at.y = starting_at.y + (alpha * changing_at.y);
                at.z = starting_at.z + (alpha * changing_at.z);
            }
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
    printf("Controls:\n");
    printf("  e: Enlarge Object\n");
    printf("  r: Reset Object Size\n");
    printf("  t: Start Intro Animation\n");
    printf("  w: Walk Forward a: Strafe Left s: Walk Backward d: Strafe Right\n");
    printf("  z: Turn left\n");
    printf("  x: Turn Right\n");
    printf("  p: Solve Maze (Shortest Path)\n");
    printf("  L: Solve Maze (Left Hand Rule)\n");
    printf("  j: increase sun elevation\n");
    printf("  k: decrease sun elevation\n");
    printf("  v: turn off lighting\n");
    printf("  b: turn on lighting\n");
    printf("  m: toggle ambient only mode\n");
    printf("  ,: toggle diffuse only mode\n");
    printf("  .: toggle specular only mode\n");
    printf("  g: Turn On/Off Flashlight\n");
     printf("  q: Quit\n");

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
    draw_sun(); // draw sun at initial position
    //draw_sun_cube(); // draw sun cube at initial position

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
