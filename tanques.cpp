#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <sysexits.h>
#include <sys/time.h>
#include <GL/glut.h>
#include <TriMesh.h>
#include <png.h>
#include "objLoader.h"

#ifdef AUDIO
#include <SDL/SDL.h>
#include <SDL/SDL_mixer.h>
#endif

#define DELAY_PBALA       500
#define MIN_A_CANNON      0.0
#define MAX_A_CANNON     85.0
#define MIN_A_TURRET   -120.0
#define MAX_A_TURRET    120.0
#define MIN_V_PBALA      0.01
#define MAX_V_PBALA      0.10
#define N_DELTAS           50
#define N_BALAS             2
#define N_PBALAS            3
#define MIN_BALA_LIGHT      3
#define H_TABLERO       250.0
#define W_TABLERO       250.0
#define D_TABLERO         2.0
#define H_WALL           10.0
#define W_WALL            7.5
#define D_WALL            1.0
#define V_WALL              1
#define L_WALL            3.0
#define H_TANQUE          1.5
#define W_TANQUE          5.0
#define D_TANQUE          1.5
#define H_SEGMENT         2.0
#define W_SEGMENT         5.0
#define W_SEGMENT_T       1.0
#define V_X            0.0020
#define V_Y            0.0020
#define V_BALA_MAX     0.0425
#define V_BALA_MIN     0.0150
#define V_NAVE           0.01
#define V_FACTOR         1.15
#define T_BALA           1000
#define T_NAVE           3000
#define DIGITS_SCORE        2
#define PTS_WALL        (-30)
#define COSA                3
#define LIVES               0
#define BALAS              10
#define N_BOOMS            50
#define N_BOOM_SETS        10
#define T_BOOM           3000
#define GRAVEDAD   (-0.00003)
#define N_DIVS             30
#define N_DIVS_HD         300
#define L_CANNON          2.5
#define D_CANNON          0.5
#define N_SLICES_RINGBASE  12
#define N_SLICES           36
#define N_LOOPS            10
#define W_BARRA          0.25
#define H_BARRA          0.50
#define TURRET_Z       -0.782
#define THREAD_X         2.83
#define CANNON_Y        3.826
#define CANNON_Z        3.252
#define H_LIGHT_TABLERO  25.0
#define H_OVERHEAD       50.0
#define N_LISTS            15
#define T_CAM_OLD       500.0
#define T_RINGS        5000.0
#define H_RINGBASE        0.3
#define H_RING            1.0
#define D_RING           10.0
#define S_RING            1.0
#define D_RING_GLOW       2.0
#define BLUR_TEXTURE_SIZE 512
#define N_BLURS             8
#define D_BLUR           0.05
#define BOOM_SCALE        0.2

int cuadrado, cuadrado_hd, cuadrado_simple, cubo, cubo_simple, checker, borde, light_cone, bala, segmento, barra, cielo, veleta, ringbase, ring;

enum mesh_enum {
        MESH_BRAIN = 0,
        MESH_SPIDER,
        MESH_TIE,
        MESH_TANK,
        MESH_TURRET,
        MESH_CANNON,
        MESH_T0,
        MESH_T1,
        MESH_T2,
        MESH_T3,
        MESH_T4,
        MESH_T5,
        MESH_T6,
        MESH_T7,
        MESH_T8,
        MESH_T9
};
#define N_MESH (MESH_T9+1)

enum cam_enum {
        CAM_OVERHEAD = 0,
        CAM_FPS,
        CAM_TPS,
        CAM_MANUAL,
        CAM_RINGS
};
#define N_CAMS 4

/*
GLfloat vx1 = 0;
GLfloat vx2 = 0;
GLfloat vy1 = 0;
GLfloat vy2 = 0;
GLfloat vz1 = 0;
GLfloat vz2 = 0;
*/

int ww = 800;
int wh = 600;
int pass;

enum passes {
        PASS_BLUR = 0,
        PASS_LAST
};
#define FIRST_PASS PASS_BLUR
#define N_PASSES 2

int level;
enum levels {
        LEVEL_DESERT = 0,
        LEVEL_SKY,
        LEVEL_SPACE
};
#define START_LEVEL LEVEL_DESERT
#define N_LEVELS 3

GLfloat aspectratio = 4.0/3.0;
GLfloat fov         =    60.0;
GLfloat znear       =    0.01;
GLfloat zfar        =  1000.0;
GLfloat cam_x       =     0.0;
GLfloat cam_y       =     0.0;
GLfloat cam_z       =    12.0;
GLfloat cam_rotx    =     0.0;
GLfloat cam_roty    =     0.0;

GLfloat cam_old_x;
GLfloat cam_old_y;
GLfloat cam_old_z;
GLfloat cam_old_rotx;
GLfloat cam_old_roty;
int cam_old_t;
int cam_old_adj;

int key_fwd          =  'w';
int key_back         =  's';
int key_left         =  'a';
int key_right        =  'd';
int key_use_l        =  'q';
int key_use_r        =  'e';
int key_lights_up    =  '.';
int key_lights_down  =  ',';
int key_cam_up       =  'p';
int key_cam_down     =  ';';
int key_cam_left     =  'j';
int key_cam_right    =  'l';
int key_cam_fwd      =  'i';
int key_cam_back     =  'k';
int key_cam_rotup    =  'y';
int key_cam_rotdown  =  'h';
int key_cam_rotleft  =  'u';
int key_cam_rotright =  'o';
int key_cam_switch   = '\t';
int key_shoot        =  ' ';
int key_enter        =   13;
int key_blur_toggle  =  'b';

int keystate_fwd          = 0;
int keystate_back         = 0;
int keystate_left         = 0;
int keystate_right        = 0;
int keystate_use_l        = 0;
int keystate_use_r        = 0;
int keystate_lights_up    = 0;
int keystate_lights_down  = 0;
int keystate_cam_up       = 0;
int keystate_cam_down     = 0;
int keystate_cam_left     = 0;
int keystate_cam_right    = 0;
int keystate_cam_fwd      = 0;
int keystate_cam_back     = 0;
int keystate_cam_rotup    = 0;
int keystate_cam_rotdown  = 0;
int keystate_cam_rotleft  = 0;
int keystate_cam_rotright = 0;
int keystate_shoot        = 0;
int keystate_enter        = 0;
int keystate_l            = 0;
int keystate_r            = 0;
int keystate_u            = 0;
int keystate_d            = 0;

int cosa = 0;
int cam = 0;
int posneg;

int deltas[N_DELTAS];
int deltas_cur = 0;
int deltas_sum = 0;
int delta;
int delay = 0;
int new_time;
int old_time;
float max_frame_delay;
float frame_delay;
int bala_t;
int frozen;
int rings_t;
int rings_s;
int rings_d;
int old_cam;
float windx, windy;

#ifdef AUDIO
int key_music        =  'm';
int keystate_music        = 0;
Mix_Chunk *phaser = NULL;
Mix_Chunk *motorL = NULL;
Mix_Chunk *motorC = NULL;
Mix_Chunk *moveL = NULL;
Mix_Chunk *moveC = NULL;
Mix_Chunk *reverseL = NULL;
Mix_Chunk *reverseC = NULL;
Mix_Chunk *fireL = NULL;
Mix_Chunk *fireC = NULL;
Mix_Chunk *turretL = NULL;
Mix_Chunk *turretC = NULL;
Mix_Chunk *exploL = NULL;
Mix_Chunk *exploC = NULL;
Mix_Chunk *buho = NULL;
Mix_Music *music = NULL;
int musicflag;
int motorChannel;
int moveChannel = -1;
int reverseChannel = -1;
//TODO: Esto es necesario?
int fireChannel = -1;
int turretChannel = -1;
int exploChannel = -1;
#endif

GLfloat black4f[4]            = {0, 0, 0, 1};
GLfloat white4f[4]            = {1, 1, 1, 1};
GLfloat light_pos[4]          = {0, 0, 0, 1};
GLfloat light_diffuse[4]      = {1, 1, 1, 1};
GLfloat bala_light_diffuse[4] = {1, 1, 0.8, 1};
GLfloat bala_light_dir[3]     = {0, 0, 0};
GLfloat bala_light_pos[4]     = {0, 0, 0, 1};

GLubyte color_ss_on[4]   = {255,   0,   0, 160};
GLubyte color_ss_off[4]  = { 50,   0,   0, 160};
GLubyte color_borde[4]   = { 90,  60,  30, 255};
GLubyte color_mesa[4]    = { 45, 130,  70, 255};
GLubyte color_inicial[4] = {124, 252,   0, 255};
GLubyte color_medio[4]   = {255, 235,   0, 255};
GLubyte color_lleno[4]   = {255,  0,   0,  255};
GLubyte color_empty[4]   = {139, 136, 120, 255};

struct mesh_data {
        int list;
        float size_x, size_y, size_z;
        float xm, ym, zm;
        float xM, yM, zM;
} mesh[N_MESH];

struct boom_data {
        float  x[N_BOOMS];
        float  y[N_BOOMS];
        float  z[N_BOOMS];
        float rx[N_BOOMS];
        float ry[N_BOOMS];
        float rz[N_BOOMS];
        float  a[N_BOOMS];
        float  s[N_BOOMS];
        float vx[N_BOOMS];
        float vy[N_BOOMS];
        float vz[N_BOOMS];
        float va[N_BOOMS];
        float  r[N_BOOMS];
        float  g[N_BOOMS];
        float  b[N_BOOMS];
        int on;
        int level;
} boom[N_BOOM_SETS];
int nboom;

/*
struct wall {
        float x, y, z;
        int life;
        int pts;
} w[N_WALLS];
*/

bool blur;
float px, py, pz, prz, pv, pvx, pvy, pvz, pvrz;
float thread_l, thread_r;
float v_x;
float v_y;
float v_cannon;
float a_cannon; // TODO: velocidad e inercia para el cañón?
float a_turret; // TODO: velocidad e inercia para la torreta?
float retract;
int pb[N_PBALAS];
int pbl[N_PBALAS];
float pbx[N_PBALAS], pby[N_PBALAS], pbz[N_PBALAS], pbvx[N_PBALAS], pbvy[N_PBALAS], pbvz[N_PBALAS];
float pbv[N_PBALAS];
int pbi, pbn;
int win;
int lose;
int c_bala, c_nave;
int pts;
int bi;
float bx[N_BALAS];
float by[N_BALAS];
float bz[N_BALAS];
int b[N_BALAS];
int bl[N_BALAS];
int i, j, k;
int lives;
int balas;
float light_intensity;
float headlights;

int ss[12][7] = {
        {1, 1, 1, 1, 1, 1, 0}, /* 0 */
        {0, 1, 1, 0, 0, 0, 0}, /* 1 */
        {1, 1, 0, 1, 1, 0, 1}, /* 2 */
        {1, 1, 1, 1, 0, 0, 1}, /* 3 */
        {0, 1, 1, 0, 0, 1, 1}, /* 4 */
        {1, 0, 1, 1, 0, 1, 1}, /* 5 */
        {1, 0, 1, 1, 1, 1, 1}, /* 6 */
        {1, 1, 1, 0, 0, 0, 0}, /* 7 */
        {1, 1, 1, 1, 1, 1, 1}, /* 8 */
        {1, 1, 1, 1, 0, 1, 1}, /* 9 */
        {0, 0, 0, 0, 0, 0, 1}, /* - */
        {0, 0, 0, 0, 0, 0, 0}  /*   */
};

int segs[12];

GLubyte *tstars_img, *tdesert_img, *tcielo_img, *tbumpmetal_img, *tbrushmetal_img, *troca_img, *tcamobump_img, *tcamoline_img, *ttriangles_img, *tplates_img, *tblue_img, *trust_img, *tcorroded_img, *tcarved_img;
GLuint   tstars,      tdesert,      tcielo,      tbumpmetal,      tbrushmetal,      troca,      tcamobump,      tcamoline    ,  ttriangles    ,  tplates    ,  tblue    ,  trust    ,  tcorroded    ,  tcarved;
GLuint   tblur;
int tw, th, ta;

int power(int b, unsigned int e) {
        int r = 1;
        for (; e > 0; e--) {
                r *= b;
        }
        return r;
}

int loadPNG(char *name, int *outWidth, int *outHeight, int *outHasAlpha, GLubyte **outData) {
        png_structp png_ptr;
        png_infop info_ptr;
        unsigned int sig_read = 0;
        FILE *fp;

        if (outWidth == NULL || outHeight == NULL || outHasAlpha == NULL) return -1;
        if ((fp = fopen(name, "rb")) == NULL) return -1;

        png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
        if (png_ptr == NULL) {
                fclose(fp);
                return -1;
        }

        info_ptr = png_create_info_struct(png_ptr);
        if (info_ptr == NULL) {
                fclose(fp);
                png_destroy_read_struct(&png_ptr, png_infopp_NULL, png_infopp_NULL);
                return -1;
        }

        if (setjmp(png_jmpbuf(png_ptr))) {
                png_destroy_read_struct(&png_ptr, &info_ptr, png_infopp_NULL);
                fclose(fp);
                return -1;
        }

        png_init_io(png_ptr, fp);
        png_set_sig_bytes(png_ptr, sig_read);

        png_read_png(png_ptr, info_ptr, PNG_TRANSFORM_STRIP_16 | PNG_TRANSFORM_PACKING | PNG_TRANSFORM_EXPAND, png_voidp_NULL);

        *outWidth  = info_ptr->width;
        *outHeight = info_ptr->height;
        switch (info_ptr->color_type) {
                case PNG_COLOR_TYPE_RGBA:
                        *outHasAlpha = 1;
                        break;
                case PNG_COLOR_TYPE_RGB:
                        *outHasAlpha = 0;
                        break;
                default:
                        png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
                        fclose(fp);
                        return false;
        }

        unsigned int row_bytes = png_get_rowbytes(png_ptr, info_ptr);
        *outData = (unsigned char*) malloc(row_bytes * (*outHeight));
        png_bytepp row_pointers = png_get_rows(png_ptr, info_ptr);
        for (int i = 0; i < *outHeight; i++) {
                memcpy(*outData + (row_bytes*((*outHeight) - 1 - i)), row_pointers[i], row_bytes);
        }

        png_destroy_read_struct(&png_ptr, &info_ptr, png_infopp_NULL);
        fclose(fp);

        return 1;
}

void initJuego() {
#ifdef AUDIO
        musicflag = 0;
        Mix_ExpireChannel(motorChannel, 1);
        if(cam>0){
                motorChannel = Mix_PlayChannel(-1, motorC, -1);
        }
        else{
                motorChannel = Mix_PlayChannel(-1, motorL, -1);
        }
#endif

        nboom      = 0;
        for (i = 0; i < N_BOOM_SETS; i++) {
                boom[i].on = 0;
        }
        frozen     = 0;
        level      = START_LEVEL;
        rings_t    = 0;
        bala_t     = 0;
        cam_old_t  = 0;
        headlights = 0.5;

        px       = 0;
        py       = 0;
        pz       = -mesh[MESH_T0].ym;
        pv       = 0;
        prz      = 0;
        pvx      = 0;
        pvy      = 0;
        pvz      = 0;
        pvrz     = 0;
        thread_l = 0;
        thread_r = 0;
        v_x      = V_X;
        v_y      = V_Y;
        pbi      = 0;
        pbn      = 0;
        for (i = 0; i < N_PBALAS; i++) {
                pb[i] = 0;
                pbv[i] = V_BALA_MAX;
        }
        win   = 0;
        lose  = 0;
        pts   = 0;
        lives = LIVES + 1;
        balas = BALAS;

        c_bala = 0;
        c_nave = 0;

        a_turret = 0;
        a_cannon = 20;
        v_cannon = V_BALA_MIN;
        retract = 1;

        bi = 0;
        for (i = 0; i < N_BALAS; i++) {
                b[i] = 0;
        }

//      windx = (0.5 - (((float)rand())/((float)RAND_MAX)));
//      windy = (0.5 - (((float)rand())/((float)RAND_MAX)));
        windx = 0;
        windy = 0;
        posneg = 0;
}

void buildLists() {
        int p;

        glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

        glGenTextures(1, &tblur);
        glBindTexture(GL_TEXTURE_2D, tblur);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        {
                int size = 4 * BLUR_TEXTURE_SIZE * BLUR_TEXTURE_SIZE * sizeof(GLuint);
                GLuint *tblur_data = (GLuint *)malloc(size);
                memset(tblur_data, 0, size);
                glTexImage2D(GL_TEXTURE_2D, 0, 4, BLUR_TEXTURE_SIZE, BLUR_TEXTURE_SIZE, 0, GL_RGBA, GL_UNSIGNED_BYTE, tblur_data);
                free(tblur_data);
        }

        glGenTextures(1, &tstars);
        glBindTexture(GL_TEXTURE_2D, tstars);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        loadPNG((char *)"png/stars.png", &tw, &th, &ta, &tstars_img);
        glTexImage2D(GL_TEXTURE_2D, 0, ta ? 4 : 3, tw, th, 0, ta ? GL_RGBA : GL_RGB, GL_UNSIGNED_BYTE, tstars_img);

        glGenTextures(1, &tdesert);
        glBindTexture(GL_TEXTURE_2D, tdesert);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        loadPNG((char *)"png/desert.png", &tw, &th, &ta, &tdesert_img);
        glTexImage2D(GL_TEXTURE_2D, 0, ta ? 4 : 3, tw, th, 0, ta ? GL_RGBA : GL_RGB, GL_UNSIGNED_BYTE, tdesert_img);

        glGenTextures(1, &tcielo);
        glBindTexture(GL_TEXTURE_2D, tcielo);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        loadPNG((char *)"png/cielo.png", &tw, &th, &ta, &tcielo_img);
        glTexImage2D(GL_TEXTURE_2D, 0, ta ? 4 : 3, tw, th, 0, ta ? GL_RGBA : GL_RGB, GL_UNSIGNED_BYTE, tcielo_img);

        glGenTextures(1, &troca);
        glBindTexture(GL_TEXTURE_2D, troca);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        loadPNG((char *)"png/roca.png", &tw, &th, &ta, &troca_img);
        glTexImage2D(GL_TEXTURE_2D, 0, ta ? 4 : 3, tw, th, 0, ta ? GL_RGBA : GL_RGB, GL_UNSIGNED_BYTE, troca_img);

        glGenTextures(1, &tbumpmetal);
        glBindTexture(GL_TEXTURE_2D, tbumpmetal);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        loadPNG((char *)"png/bumpmetal.png", &tw, &th, &ta, &tbumpmetal_img);
        glTexImage2D(GL_TEXTURE_2D, 0, ta ? 4 : 3, tw, th, 0, ta ? GL_RGBA : GL_RGB, GL_UNSIGNED_BYTE, tbumpmetal_img);

        glGenTextures(1, &tbrushmetal);
        glBindTexture(GL_TEXTURE_2D, tbrushmetal);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        loadPNG((char *)"png/brushmetal.png", &tw, &th, &ta, &tbrushmetal_img);
        glTexImage2D(GL_TEXTURE_2D, 0, ta ? 4 : 3, tw, th, 0, ta ? GL_RGBA : GL_RGB, GL_UNSIGNED_BYTE, tbrushmetal_img);

        glGenTextures(1, &tcamobump);
        glBindTexture(GL_TEXTURE_2D, tcamobump);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        loadPNG((char *)"png/camobump.png", &tw, &th, &ta, &tcamobump_img);
        glTexImage2D(GL_TEXTURE_2D, 0, ta ? 4 : 3, tw, th, 0, ta ? GL_RGBA : GL_RGB, GL_UNSIGNED_BYTE, tcamobump_img);

        glGenTextures(1, &tcamoline);
        glBindTexture(GL_TEXTURE_2D, tcamoline);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        loadPNG((char *)"png/camoline.png", &tw, &th, &ta, &tcamoline_img);
        glTexImage2D(GL_TEXTURE_2D, 0, ta ? 4 : 3, tw, th, 0, ta ? GL_RGBA : GL_RGB, GL_UNSIGNED_BYTE, tcamoline_img);

        glGenTextures(1, &tplates);
        glBindTexture(GL_TEXTURE_2D, tplates);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        loadPNG((char *)"png/plates.png", &tw, &th, &ta, &tplates_img);
        glTexImage2D(GL_TEXTURE_2D, 0, ta ? 4 : 3, tw, th, 0, ta ? GL_RGBA : GL_RGB, GL_UNSIGNED_BYTE, tplates_img);

        glGenTextures(1, &tcorroded);
        glBindTexture(GL_TEXTURE_2D, tcorroded);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        loadPNG((char *)"png/corroded.png", &tw, &th, &ta, &tcorroded_img);
        glTexImage2D(GL_TEXTURE_2D, 0, ta ? 4 : 3, tw, th, 0, ta ? GL_RGBA : GL_RGB, GL_UNSIGNED_BYTE, tcorroded_img);

        glGenTextures(1, &trust);
        glBindTexture(GL_TEXTURE_2D, trust);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        loadPNG((char *)"png/rust.png", &tw, &th, &ta, &trust_img);
        glTexImage2D(GL_TEXTURE_2D, 0, ta ? 4 : 3, tw, th, 0, ta ? GL_RGBA : GL_RGB, GL_UNSIGNED_BYTE, trust_img);

        glGenTextures(1, &tblue);
        glBindTexture(GL_TEXTURE_2D, tblue);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        loadPNG((char *)"png/blue.png", &tw, &th, &ta, &tblue_img);
        glTexImage2D(GL_TEXTURE_2D, 0, ta ? 4 : 3, tw, th, 0, ta ? GL_RGBA : GL_RGB, GL_UNSIGNED_BYTE, tblue_img);

        glGenTextures(1, &ttriangles);
        glBindTexture(GL_TEXTURE_2D, ttriangles);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        loadPNG((char *)"png/triangles.png", &tw, &th, &ta, &ttriangles_img);
        glTexImage2D(GL_TEXTURE_2D, 0, ta ? 4 : 3, tw, th, 0, ta ? GL_RGBA : GL_RGB, GL_UNSIGNED_BYTE, ttriangles_img);

        glGenTextures(1, &tcarved);
        glBindTexture(GL_TEXTURE_2D, tcarved);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        loadPNG((char *)"png/carved.png", &tw, &th, &ta, &tcarved_img);
        glTexImage2D(GL_TEXTURE_2D, 0, ta ? 4 : 3, tw, th, 0, ta ? GL_RGBA : GL_RGB, GL_UNSIGNED_BYTE, tcarved_img);

        if ((cuadrado = glGenLists(N_LISTS + 12 + N_MESH)) == 0) exit(EX_OSERR);
        cuadrado_hd     = cuadrado +  1;
        cuadrado_simple = cuadrado +  2;
        cubo            = cuadrado +  3;
        cubo_simple     = cuadrado +  4;
        checker         = cuadrado +  5;
        borde           = cuadrado +  6;
        bala            = cuadrado +  7;
        barra           = cuadrado +  8;
        cielo           = cuadrado +  9;
        veleta          = cuadrado + 10;
        segmento        = cuadrado + 11;
        ringbase        = cuadrado + 12;
        ring            = cuadrado + 13;
        light_cone      = cuadrado + 14;
        for (i = 0; i <= 11; i++) {
                segs[i] = cuadrado + N_LISTS + i;
        }
        for (i = 0; i < N_MESH; i++) {
                mesh[i].list = N_LISTS + 12 + i;
        }

        glNewList(cuadrado, GL_COMPILE);
                glBegin(GL_TRIANGLES);
                        glNormal3f(0, 0, 1);
                        for (i = 0; i < N_DIVS; i++) {
                                for (j = 0; j < N_DIVS; j++) {
                                        glTexCoord2f(       (((float)(i    ))/((float)N_DIVS)),        (((float)(j    ))/((float)N_DIVS))     );
                                          glVertex3f(-0.5 + (((float)(i    ))/((float)N_DIVS)), -0.5 + (((float)(j    ))/((float)N_DIVS)), 0.0);
                                        glTexCoord2f(       (((float)(i + 1))/((float)N_DIVS)),        (((float)(j    ))/((float)N_DIVS))     );
                                          glVertex3f(-0.5 + (((float)(i + 1))/((float)N_DIVS)), -0.5 + (((float)(j    ))/((float)N_DIVS)), 0.0);
                                        glTexCoord2f(       (((float)(i + 1))/((float)N_DIVS)),        (((float)(j + 1))/((float)N_DIVS))     );
                                          glVertex3f(-0.5 + (((float)(i + 1))/((float)N_DIVS)), -0.5 + (((float)(j + 1))/((float)N_DIVS)), 0.0);

                                        glTexCoord2f(       (((float)(i + 1))/((float)N_DIVS)),        (((float)(j + 1))/((float)N_DIVS))     );
                                          glVertex3f(-0.5 + (((float)(i + 1))/((float)N_DIVS)), -0.5 + (((float)(j + 1))/((float)N_DIVS)), 0.0);
                                        glTexCoord2f(       (((float)(i    ))/((float)N_DIVS)),        (((float)(j + 1))/((float)N_DIVS))     );
                                          glVertex3f(-0.5 + (((float)(i    ))/((float)N_DIVS)), -0.5 + (((float)(j + 1))/((float)N_DIVS)), 0.0);
                                        glTexCoord2f(       (((float)(i    ))/((float)N_DIVS)),        (((float)(j    ))/((float)N_DIVS))     );
                                          glVertex3f(-0.5 + (((float)(i    ))/((float)N_DIVS)), -0.5 + (((float)(j    ))/((float)N_DIVS)), 0.0);
                                }
                        }
                glEnd();
        glEndList();

        glNewList(cuadrado_hd, GL_COMPILE);
                glBegin(GL_TRIANGLES);
                        glNormal3f(0, 0, 1);
                        for (i = 0; i < N_DIVS_HD; i++) {
                                for (j = 0; j < N_DIVS_HD; j++) {
                                        glTexCoord2f(       (((float)(i    ))/((float)N_DIVS_HD)),        (((float)(j    ))/((float)N_DIVS_HD))     );
                                          glVertex3f(-0.5 + (((float)(i    ))/((float)N_DIVS_HD)), -0.5 + (((float)(j    ))/((float)N_DIVS_HD)), 0.0);
                                        glTexCoord2f(       (((float)(i + 1))/((float)N_DIVS_HD)),        (((float)(j    ))/((float)N_DIVS_HD))     );
                                          glVertex3f(-0.5 + (((float)(i + 1))/((float)N_DIVS_HD)), -0.5 + (((float)(j    ))/((float)N_DIVS_HD)), 0.0);
                                        glTexCoord2f(       (((float)(i + 1))/((float)N_DIVS_HD)),        (((float)(j + 1))/((float)N_DIVS_HD))     );
                                          glVertex3f(-0.5 + (((float)(i + 1))/((float)N_DIVS_HD)), -0.5 + (((float)(j + 1))/((float)N_DIVS_HD)), 0.0);

                                        glTexCoord2f(       (((float)(i + 1))/((float)N_DIVS_HD)),        (((float)(j + 1))/((float)N_DIVS_HD))     );
                                          glVertex3f(-0.5 + (((float)(i + 1))/((float)N_DIVS_HD)), -0.5 + (((float)(j + 1))/((float)N_DIVS_HD)), 0.0);
                                        glTexCoord2f(       (((float)(i    ))/((float)N_DIVS_HD)),        (((float)(j + 1))/((float)N_DIVS_HD))     );
                                          glVertex3f(-0.5 + (((float)(i    ))/((float)N_DIVS_HD)), -0.5 + (((float)(j + 1))/((float)N_DIVS_HD)), 0.0);
                                        glTexCoord2f(       (((float)(i    ))/((float)N_DIVS_HD)),        (((float)(j    ))/((float)N_DIVS_HD))     );
                                          glVertex3f(-0.5 + (((float)(i    ))/((float)N_DIVS_HD)), -0.5 + (((float)(j    ))/((float)N_DIVS_HD)), 0.0);
                                }
                        }
                glEnd();
        glEndList();

        glNewList(cuadrado_simple, GL_COMPILE);
                glBegin(GL_TRIANGLES);
                        glNormal3f(0, 0, 1);
                        glTexCoord2f(0, 0); glVertex3f(-0.5, -0.5, 0.0);
                        glTexCoord2f(1, 0); glVertex3f( 0.5, -0.5, 0.0);
                        glTexCoord2f(1, 1); glVertex3f( 0.5,  0.5, 0.0);

                        glTexCoord2f(1, 1); glVertex3f( 0.5,  0.5, 0.0);
                        glTexCoord2f(0, 1); glVertex3f(-0.5,  0.5, 0.0);
                        glTexCoord2f(0, 0); glVertex3f(-0.5, -0.5, 0.0);
                glEnd();
        glEndList();

        glNewList(cubo, GL_COMPILE);
                glPushMatrix();
                        glTranslatef(0, 0, 0.5);
                        glCallList(cuadrado);
                glPopMatrix();
                glPushMatrix();
                        glTranslatef(0, 0, -0.5);
                        glRotatef(180, 0, 1, 0);
                        glCallList(cuadrado);
                glPopMatrix();
                glPushMatrix();
                        glTranslatef(0, 0.5, 0);
                        glRotatef(-90, 1, 0, 0);
                        glCallList(cuadrado);
                glPopMatrix();
                glPushMatrix();
                        glTranslatef(0, -0.5, 0);
                        glRotatef(90, 1, 0, 0);
                        glCallList(cuadrado);
                glPopMatrix();
                glPushMatrix();
                        glTranslatef(0.5, 0, 0);
                        glRotatef(90, 0, 1, 0);
                        glCallList(cuadrado);
                glPopMatrix();
                glPushMatrix();
                        glTranslatef(-0.5, 0, 0);
                        glRotatef(-90, 0, 1, 0);
                        glCallList(cuadrado);
                glPopMatrix();
        glEndList();

        glNewList(cubo_simple, GL_COMPILE);
                glPushMatrix();
                        glTranslatef(0, 0, 0.5);
                        glCallList(cuadrado_simple);
                glPopMatrix();
                glPushMatrix();
                        glTranslatef(0, 0, -0.5);
                        glRotatef(180, 0, 1, 0);
                        glCallList(cuadrado_simple);
                glPopMatrix();
                glPushMatrix();
                        glTranslatef(0, 0.5, 0);
                        glRotatef(-90, 1, 0, 0);
                        glCallList(cuadrado_simple);
                glPopMatrix();
                glPushMatrix();
                        glTranslatef(0, -0.5, 0);
                        glRotatef(90, 1, 0, 0);
                        glCallList(cuadrado_simple);
                glPopMatrix();
                glPushMatrix();
                        glTranslatef(0.5, 0, 0);
                        glRotatef(90, 0, 1, 0);
                        glCallList(cuadrado_simple);
                glPopMatrix();
                glPushMatrix();
                        glTranslatef(-0.5, 0, 0);
                        glRotatef(-90, 0, 1, 0);
                        glCallList(cuadrado_simple);
                glPopMatrix();
        glEndList();

        glNewList(checker, GL_COMPILE);
                glBegin(GL_QUADS);
                        glNormal3f(0, 1, 0);
                        p = 0;
                        for (i = -100; i < 100; i += 1) {
                                for (j = -100; j < 100; j++) {
                                        p ^= 1;
                                        if (p) continue;
                                        glVertex3f(i    , 0, j    );
                                        glVertex3f(i + 1, 0, j    );
                                        glVertex3f(i + 1, 0, j + 1);
                                        glVertex3f(i    , 0, j + 1);
                                }
                                p ^= 1;
                        }
                glEnd();
        glEndList();

        glNewList(borde, GL_COMPILE);
                glPushMatrix();
                        glTranslatef(0, H_TABLERO / 2.0, 0);
                        glScalef(W_TABLERO + (2 * D_TABLERO), D_TABLERO, 1);
                        glTranslatef(0, 0.5, 0);
                        glColor4ubv(color_borde);
                        glCallList(cubo);
                glPopMatrix();

                glPushMatrix();
                        glTranslatef(0, -H_TABLERO / 2.0, 0);
                        glScalef(W_TABLERO + (2 * D_TABLERO), D_TABLERO, 1);
                        glTranslatef(0, -0.5, 0);
                        glColor4ubv(color_borde);
                        glCallList(cubo);
                glPopMatrix();

                glPushMatrix();
                        glTranslatef(W_TABLERO / 2.0, 0, 0);
                        glScalef(D_TABLERO, H_TABLERO, 1);
                        glTranslatef(0.5, 0, 0);
                        glColor4ubv(color_borde);
                        glCallList(cubo);
                glPopMatrix();

                glPushMatrix();
                        glTranslatef(-W_TABLERO / 2.0, 0, 0);
                        glScalef(D_TABLERO, H_TABLERO, 1);
                        glTranslatef(-0.5, 0, 0);
                        glColor4ubv(color_borde);
                        glCallList(cubo);
                glPopMatrix();
        glEndList();

        glNewList(bala, GL_COMPILE);
                glPushMatrix();
                        glScalef(D_CANNON, L_CANNON, D_CANNON);
                        glRotatef(-90, 1, 0, 0);
                        glTranslatef(0, 0, -0.55);
                        {
                                GLUquadric *quad = gluNewQuadric();
                                gluQuadricTexture(quad, GL_TRUE);
                                gluQuadricDrawStyle(quad, GLU_FILL);
                                gluQuadricNormals(quad, GLU_SMOOTH);
                                glPushMatrix();
                                        gluQuadricOrientation(quad, GLU_INSIDE);
                                        gluDisk(quad, 0, 0.6, N_SLICES, N_LOOPS);
                                        gluQuadricOrientation(quad, GLU_OUTSIDE);
                                        gluCylinder(quad, 0.6, 0.6, 1.0/3.0, N_SLICES, N_LOOPS);
                                        glTranslatef(0, 0, 1.0/3.0);
                                        gluCylinder(quad, 0.6, 0, 1.0/6.0, N_SLICES, N_LOOPS);
                                glPopMatrix();
                                gluDeleteQuadric(quad);
                        }
                glPopMatrix();
        glEndList();

        glNewList(barra, GL_COMPILE);
                glPushMatrix();
                        glBegin(GL_QUADS);
                                glVertex3f(-1, -1, 0);
                                glVertex3f( 1, -1, 0);
                                glVertex3f( 1,  1, 0);
                                glVertex3f(-1,  1, 0);
                        glEnd();
                glPopMatrix();
        glEndList();

        glNewList(cielo, GL_COMPILE);
                {
                        GLUquadric *quad = gluNewQuadric();
                        gluQuadricTexture(quad, GL_TRUE);
                        gluQuadricDrawStyle(quad, GLU_FILL);
                        gluQuadricNormals(quad, GLU_SMOOTH);
                        gluQuadricOrientation(quad, GLU_INSIDE);
                        gluSphere(quad, 400.0, 100, 100);
                        gluDeleteQuadric(quad);
                }
        glEndList();

        glNewList(veleta, GL_COMPILE);
                {
                        GLUquadric *quad = gluNewQuadric();
                        gluQuadricTexture(quad, GL_TRUE);
                        gluQuadricDrawStyle(quad, GLU_FILL);
                        gluQuadricNormals(quad, GLU_SMOOTH);
                        gluQuadricOrientation(quad, GLU_OUTSIDE);
                        for (i = 0; i < 10; i++) {
                                if (i % 2 == 0) glColor4ub(255, 70, 0, 255);
                                else glColor4ub(255, 255, 255, 255);
                                gluCylinder(quad, 1.0 - 0.1 * i, 0.9 - 0.1 * i, 0.1, N_SLICES, N_LOOPS);
                                glTranslatef(0, 0, 0.1);
                        }
                        gluDeleteQuadric(quad);
                }
        glEndList();

        glNewList(segmento, GL_COMPILE);
                glBegin(GL_TRIANGLES);
                        glVertex3f(-W_SEGMENT / 2.0              ,  H_SEGMENT / 2.0, 0);
                        glVertex3f(-W_SEGMENT / 2.0 - W_SEGMENT_T,                0, 0);
                        glVertex3f(-W_SEGMENT / 2.0              , -H_SEGMENT / 2.0, 0);

                        glVertex3f( W_SEGMENT / 2.0,  H_SEGMENT / 2.0, 0);
                        glVertex3f(-W_SEGMENT / 2.0,  H_SEGMENT / 2.0, 0);
                        glVertex3f(-W_SEGMENT / 2.0, -H_SEGMENT / 2.0, 0);

                        glVertex3f(-W_SEGMENT / 2.0, -H_SEGMENT / 2.0, 0);
                        glVertex3f( W_SEGMENT / 2.0, -H_SEGMENT / 2.0, 0);
                        glVertex3f( W_SEGMENT / 2.0,  H_SEGMENT / 2.0, 0);

                        glVertex3f( W_SEGMENT / 2.0              , -H_SEGMENT / 2.0, 0);
                        glVertex3f( W_SEGMENT / 2.0 + W_SEGMENT_T,  0              , 0);
                        glVertex3f( W_SEGMENT / 2.0              ,  H_SEGMENT / 2.0, 0);
                glEnd();

                glLineWidth(1);
                glBegin(GL_LINE_LOOP);
                        glColor4ub(0, 0, 0, 255);
                        glVertex3f(-W_SEGMENT / 2.0              ,  H_SEGMENT / 2.0, 0);
                        glVertex3f(-W_SEGMENT / 2.0 - W_SEGMENT_T,  0, 0);
                        glVertex3f(-W_SEGMENT / 2.0              , -H_SEGMENT / 2.0, 0);
                        glVertex3f( W_SEGMENT / 2.0              , -H_SEGMENT / 2.0, 0);
                        glVertex3f( W_SEGMENT / 2.0 + W_SEGMENT_T,  0, 0);
                        glVertex3f( W_SEGMENT / 2.0              ,  H_SEGMENT / 2.0, 0);
                glEnd();
        glEndList();

        glNewList(ringbase, GL_COMPILE);
                glPushMatrix();
                        glScalef(D_RING, D_RING, H_RINGBASE * 10.0);
                        glMatrixMode(GL_TEXTURE);
                        glScalef(10, 10, 1);
                        glMatrixMode(GL_MODELVIEW);
                        glColor4ub(127, 127, 127, 255);
                        glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
                        glBindTexture(GL_TEXTURE_2D, tplates);
                        glEnable(GL_TEXTURE_2D);
                        glTranslatef(0, 0, -0.05);
                        {
                                GLUquadric *quad = gluNewQuadric();
                                gluQuadricTexture(quad, GL_TRUE);
                                gluQuadricDrawStyle(quad, GLU_FILL);
                                gluQuadricNormals(quad, GLU_SMOOTH);
                                gluQuadricOrientation(quad, GLU_INSIDE);
                                gluDisk(quad, 0.8, 1.0, N_SLICES_RINGBASE, N_LOOPS);

                                glMatrixMode(GL_TEXTURE);
                                glScalef(4, 0.1, 1);
                                glMatrixMode(GL_MODELVIEW);

                                gluCylinder(quad, 0.8, 0.8, 0.1, N_SLICES_RINGBASE, 1);
                                gluQuadricOrientation(quad, GLU_OUTSIDE);
                                gluCylinder(quad, 1.0, 1.0, 0.1, N_SLICES_RINGBASE, 1);

                                glMatrixMode(GL_TEXTURE);
                                glLoadIdentity();
                                glScalef(10, 10, 1);
                                glMatrixMode(GL_MODELVIEW);

                                glTranslatef(0, 0, 0.1);
                                gluDisk(quad, 0.8, 1.0, N_SLICES_RINGBASE, N_LOOPS);
                                glTranslatef(0, 0, -0.05);
                                glBindTexture(GL_TEXTURE_2D, tcarved);
                                glMatrixMode(GL_TEXTURE);
                                glScalef(0.5, 0.5, 1);
                                gluDisk(quad, 0.0, 1.0, N_SLICES_RINGBASE, N_LOOPS);
                                glMatrixMode(GL_MODELVIEW);
                                gluDeleteQuadric(quad);
                        }
                        glMatrixMode(GL_TEXTURE);
                        glLoadIdentity();
                        glMatrixMode(GL_MODELVIEW);
                glPopMatrix();
        glEndList();

        glNewList(ring, GL_COMPILE);
                glPushMatrix();
                        glScalef(D_RING, D_RING, H_RING * 10.0);
                        glMatrixMode(GL_TEXTURE);
                        glScalef(10, 10, 1);
                        glMatrixMode(GL_MODELVIEW);
                        glColor4ub(127, 127, 127, 255);
                        glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
                        glBindTexture(GL_TEXTURE_2D, trust);
                        glEnable(GL_TEXTURE_2D);
                        {
                                GLUquadric *quad = gluNewQuadric();
                                gluQuadricTexture(quad, GL_TRUE);
                                gluQuadricDrawStyle(quad, GLU_FILL);
                                gluQuadricNormals(quad, GLU_SMOOTH);
                                gluQuadricOrientation(quad, GLU_INSIDE);
                                gluDisk(quad, 0.8, 1.0, N_SLICES, N_LOOPS);

                                glMatrixMode(GL_TEXTURE);
                                glScalef(4, 0.1, 1);
                                glMatrixMode(GL_MODELVIEW);

                                gluCylinder(quad, 0.8, 0.8, 0.1, N_SLICES, 1);
                                gluQuadricOrientation(quad, GLU_OUTSIDE);
                                gluCylinder(quad, 1.0, 1.0, 0.1, N_SLICES, 1);

                                glMatrixMode(GL_TEXTURE);
                                glLoadIdentity();
                                glScalef(10, 10, 1);
                                glMatrixMode(GL_MODELVIEW);

                                glTranslatef(0, 0, 0.1);
                                gluDisk(quad, 0.8, 1.0, N_SLICES, N_LOOPS);
                                gluDeleteQuadric(quad);
                        }
                        glMatrixMode(GL_TEXTURE);
                        glLoadIdentity();
                        glMatrixMode(GL_MODELVIEW);
                        glColor4ub(255, 255, 255, 255);
                        for (i = 0; i < 18; i++) {
                                glPushMatrix();
                                        glRotatef(20.0 * i, 0, 0, 1);
                                        glTranslatef(0, 0.9, -0.05);
                                        glScalef(0.10, 0.22, 0.12);
                                        glCallList(cubo_simple);
                                glPopMatrix();
                        }
                        glDisable(GL_TEXTURE_2D);
                glPopMatrix();
        glEndList();

        glNewList(light_cone, GL_COMPILE);
                glBegin(GL_TRIANGLES);
                        for (i = 0; i < N_SLICES; i++) {
                                glColor4f(0.3, 0.3, 0.3, 1.0);
                                glVertex3f(0, 0, 0);
                                glColor4f(0, 0, 0, 1);
                                glVertex3f(cos((((float) i     )*2*M_PI)/N_SLICES), sin((((float) i     )*2*M_PI)/N_SLICES), 1);
                                glVertex3f(cos((((float)(i + 1))*2*M_PI)/N_SLICES), sin((((float)(i + 1))*2*M_PI)/N_SLICES), 1);
                        }
                glEnd();
        glEndList();

        for (i = 0; i <= 11; i++) {
                glNewList(segs[i], GL_COMPILE);
                        /* a */
                        if (ss[i][0]) glColor4ubv(color_ss_on );
                        else          glColor4ubv(color_ss_off);
                        glPushMatrix();
                                glTranslatef(0, W_SEGMENT + 2.0 * W_SEGMENT_T, 0);
                                glCallList(segmento);
                        glPopMatrix();

                        /* b */
                        if (ss[i][1]) glColor4ubv(color_ss_on );
                        else          glColor4ubv(color_ss_off);
                        glPushMatrix();
                                glTranslatef(W_SEGMENT / 2.0 + W_SEGMENT_T, W_SEGMENT / 2.0 + W_SEGMENT_T, 0);
                                glRotatef(90, 0, 0, 1);
                                glCallList(segmento);
                        glPopMatrix();

                        /* c */
                        if (ss[i][2]) glColor4ubv(color_ss_on );
                        else          glColor4ubv(color_ss_off);
                        glPushMatrix();
                                glTranslatef(W_SEGMENT / 2.0 + W_SEGMENT_T, -(W_SEGMENT / 2.0 + W_SEGMENT_T), 0);
                                glRotatef(90, 0, 0, 1);
                                glCallList(segmento);
                        glPopMatrix();

                        /* d */
                        if (ss[i][3]) glColor4ubv(color_ss_on );
                        else          glColor4ubv(color_ss_off);
                        glPushMatrix();
                                glTranslatef(0, -(W_SEGMENT + 2.0 * W_SEGMENT_T), 0);
                                glCallList(segmento);
                        glPopMatrix();

                        /* e */
                        if (ss[i][4]) glColor4ubv(color_ss_on );
                        else          glColor4ubv(color_ss_off);
                        glPushMatrix();
                                glTranslatef(-(W_SEGMENT / 2.0 + W_SEGMENT_T), -(W_SEGMENT / 2.0 + W_SEGMENT_T), 0);
                                glRotatef(90, 0, 0, 1);
                                glCallList(segmento);
                        glPopMatrix();

                        /* f */
                        if (ss[i][5]) glColor4ubv(color_ss_on );
                        else          glColor4ubv(color_ss_off);
                        glPushMatrix();
                                glTranslatef(-(W_SEGMENT / 2.0 + W_SEGMENT_T), W_SEGMENT / 2.0 + W_SEGMENT_T, 0);
                                glRotatef(90, 0, 0, 1);
                                glCallList(segmento);
                        glPopMatrix();

                        /* g */
                        if (ss[i][6]) glColor4ubv(color_ss_on );
                        else          glColor4ubv(color_ss_off);
                        glPushMatrix();
                                glCallList(segmento);
                        glPopMatrix();
                glEndList();
        }

        for (i = 0; i < N_MESH; i++) {
                TriMesh *t = NULL;
                objLoader *o = NULL;
                int normalize;

//              printf("entrando a mesh %d\n", i); // DEBUG
                switch (i) {
                        case MESH_BRAIN : t = TriMesh::read("ply/brain.ply" );                    normalize = 1; break;
                        case MESH_SPIDER: t = TriMesh::read("ply/spider.ply");                    normalize = 1; break;
                        case MESH_TIE   : t = TriMesh::read("ply/tie.ply"   );                    normalize = 1; break;
                        case MESH_TANK  : o = new objLoader(); o->load((char *)"obj/tank.obj"  ); normalize = 0; break;
                        case MESH_TURRET: o = new objLoader(); o->load((char *)"obj/turret.obj"); normalize = 0; break;
                        case MESH_CANNON: o = new objLoader(); o->load((char *)"obj/cannon.obj"); normalize = 0; break;
                        case MESH_T0    : o = new objLoader(); o->load((char *)"obj/t0.obj"    ); normalize = 0; break;
                        case MESH_T1    : o = new objLoader(); o->load((char *)"obj/t1.obj"    ); normalize = 0; break;
                        case MESH_T2    : o = new objLoader(); o->load((char *)"obj/t2.obj"    ); normalize = 0; break;
                        case MESH_T3    : o = new objLoader(); o->load((char *)"obj/t3.obj"    ); normalize = 0; break;
                        case MESH_T4    : o = new objLoader(); o->load((char *)"obj/t4.obj"    ); normalize = 0; break;
                        case MESH_T5    : o = new objLoader(); o->load((char *)"obj/t5.obj"    ); normalize = 0; break;
                        case MESH_T6    : o = new objLoader(); o->load((char *)"obj/t6.obj"    ); normalize = 0; break;
                        case MESH_T7    : o = new objLoader(); o->load((char *)"obj/t7.obj"    ); normalize = 0; break;
                        case MESH_T8    : o = new objLoader(); o->load((char *)"obj/t8.obj"    ); normalize = 0; break;
                        case MESH_T9    : o = new objLoader(); o->load((char *)"obj/t9.obj"    ); normalize = 0; break;

                        default: break;
                }
//              printf("entré a mesh %d\n", i); // DEBUG
                if (t == NULL && o == NULL) {
                        fprintf(stderr, "Error cargando mesh %d.\n", i);
                } else {
                        float maxsize;
                        int j, k;

                        /* Calcular bounding box */
                        if (t != NULL) {
                                t->need_bbox();
                                t->need_faces();
                                t->need_normals();
                                mesh[i].xm = t->bbox.min[0];
                                mesh[i].ym = t->bbox.min[1];
                                mesh[i].zm = t->bbox.min[2];
                                mesh[i].xM = t->bbox.max[0];
                                mesh[i].yM = t->bbox.max[1];
                                mesh[i].zM = t->bbox.max[2];
                        }
                        if (o != NULL) {
//                              printf("cargando mesh %d con objLoad\n", i); // DEBUG
                                if (o->faceCount < 1 || o->faceList[0]->vertex_count < 1) {
                                        fprintf(stderr, "Error cargando mesh %d.\n", i);
                                        delete o;
                                        continue;
                                }
//                              printf("el mesh %d tiene %d caras, %d vertices, %d normales y %d coordenadas de textura\n", i, o->faceCount, o->vertexCount, o->normalCount, o->textureCount); // DEBUG
                                mesh[i].xm = mesh[i].xM = o->vertexList[o->faceList[0]->vertex_index[0]]->e[0];
                                mesh[i].ym = mesh[i].yM = o->vertexList[o->faceList[0]->vertex_index[0]]->e[1];
                                mesh[i].zm = mesh[i].zM = o->vertexList[o->faceList[0]->vertex_index[0]]->e[2];
                                for (j = 0; j < o->faceCount; j++) {
                                        for (k = 0; k < o->faceList[j]->vertex_count; k++) {
                                                if (mesh[i].xm > o->vertexList[o->faceList[j]->vertex_index[k]]->e[0]) mesh[i].xm = o->vertexList[o->faceList[j]->vertex_index[k]]->e[0];
                                                if (mesh[i].xM < o->vertexList[o->faceList[j]->vertex_index[k]]->e[0]) mesh[i].xM = o->vertexList[o->faceList[j]->vertex_index[k]]->e[0];
                                                if (mesh[i].ym > o->vertexList[o->faceList[j]->vertex_index[k]]->e[1]) mesh[i].ym = o->vertexList[o->faceList[j]->vertex_index[k]]->e[1];
                                                if (mesh[i].yM < o->vertexList[o->faceList[j]->vertex_index[k]]->e[1]) mesh[i].yM = o->vertexList[o->faceList[j]->vertex_index[k]]->e[1];
                                                if (mesh[i].zm > o->vertexList[o->faceList[j]->vertex_index[k]]->e[2]) mesh[i].zm = o->vertexList[o->faceList[j]->vertex_index[k]]->e[2];
                                                if (mesh[i].zM < o->vertexList[o->faceList[j]->vertex_index[k]]->e[2]) mesh[i].zM = o->vertexList[o->faceList[j]->vertex_index[k]]->e[2];
                                        }
                                }
//                              printf("calculé el bounding box del mesh %d con objLoad\n", i); // DEBUG
                        }
                        mesh[i].size_x = mesh[i].xM - mesh[i].xm;
                        mesh[i].size_y = mesh[i].yM - mesh[i].ym;
                        mesh[i].size_z = mesh[i].zM - mesh[i].zm;

                        if (normalize) {
                                maxsize = mesh[i].size_x;
                                if (maxsize < mesh[i].size_y) {
                                        maxsize = mesh[i].size_y;
                                }
                                if (maxsize < mesh[i].size_z) {
                                        maxsize = mesh[i].size_z;
                                }
                                mesh[i].size_x /= maxsize;
                                mesh[i].size_y /= maxsize;
                                mesh[i].size_z /= maxsize;
                        }

//                      printf("entrando a list del mesh %d\n", i); // DEBUG
                        glNewList(mesh[i].list, GL_COMPILE);
                                glPushMatrix();
                                        if (normalize && maxsize > 0) glScalef(1.0/maxsize, 1.0/maxsize, 1.0/maxsize);
                                        switch (i) {
                                                case MESH_SPIDER: glRotatef(60, 0, 0, 1); break;
                                                case MESH_TIE   : glRotatef(90, 0, 0, 1); break;

                                                default: break;
                                        }
                                        if (normalize) {
                                                glTranslatef(-(mesh[i].xm + mesh[i].xM)/2.0,
                                                             -(mesh[i].ym + mesh[i].yM)/2.0,
                                                             -(mesh[i].zm + mesh[i].zM)/2.0);
                                        }
                                        glBegin(GL_TRIANGLES);
                                                if (t != NULL) {
                                                        for (j = 0; j < (signed int)t->faces.size(); j++) {
                                                                glNormal3f(t->normals[(int)t->faces[j][0]][0],
                                                                           t->normals[(int)t->faces[j][0]][1],
                                                                           t->normals[(int)t->faces[j][0]][2]);
                                                                glVertex3f(t->vertices[(int)t->faces[j][0]][0],
                                                                           t->vertices[(int)t->faces[j][0]][1],
                                                                           t->vertices[(int)t->faces[j][0]][2]);
                                                                glNormal3f(t->normals[(int)t->faces[j][1]][0],
                                                                           t->normals[(int)t->faces[j][1]][1],
                                                                           t->normals[(int)t->faces[j][1]][2]);
                                                                glVertex3f(t->vertices[(int)t->faces[j][1]][0],
                                                                           t->vertices[(int)t->faces[j][1]][1],
                                                                           t->vertices[(int)t->faces[j][1]][2]);
                                                                glNormal3f(t->normals[(int)t->faces[j][2]][0],
                                                                           t->normals[(int)t->faces[j][2]][1],
                                                                           t->normals[(int)t->faces[j][2]][2]);
                                                                glVertex3f(t->vertices[(int)t->faces[j][2]][0],
                                                                           t->vertices[(int)t->faces[j][2]][1],
                                                                           t->vertices[(int)t->faces[j][2]][2]);
                                                        }
                                                } else if (o != NULL) {
                                                        for (j = 0; j < o->faceCount; j++) {
//                                                              printf("pintando cara %d del mesh %d\n", j, i); // DEBUG
                                                                if (o->faceList[j]->vertex_count != 3) {
                                                                        fprintf(stderr, "WARNING: skipping face %d in mesh %d since it has %d sides.\n", j, i, o->faceList[j]->vertex_count);
                                                                        continue;
                                                                }
//                                                              printf("tiene %d vertices\n", o->faceList[j]->vertex_count); // DEBUG
                                                                for (k = 0; k < o->faceList[j]->vertex_count; k++) {
//                                                                      printf("vértice %d\n", k); // DEBUG
//                                                                      printf("hay %d normales, y esta es la %d\n", o->normalCount, o->faceList[j]->normal_index[k]); // DEBUG
                                                                        if (o->faceList[j]->normal_index[k] >= 0) {
//                                                                              printf("su valor es (%f, %f, %f)\n", o->normalList[o->faceList[j]->normal_index[k]]->e[0], o->normalList[o->faceList[j]->normal_index[k]]->e[1], o->normalList[o->faceList[j]->normal_index[k]]->e[2]); // DEBUG
                                                                                glNormal3f(o->normalList[o->faceList[j]->normal_index[k]]->e[0],
                                                                                           o->normalList[o->faceList[j]->normal_index[k]]->e[1],
                                                                                           o->normalList[o->faceList[j]->normal_index[k]]->e[2]);
                                                                        }

//                                                                      printf("hay %d coordenadas de textura, y esta es la %d\n", o->textureCount, o->faceList[j]->texture_index[k]); // DEBUG
                                                                        if (o->faceList[j]->texture_index[k] >= 0) {
//                                                                              printf("su valor es (%f, %f, %f)\n", o->textureList[o->faceList[j]->texture_index[k]]->e[0], o->textureList[o->faceList[j]->texture_index[k]]->e[1], o->textureList[o->faceList[j]->texture_index[k]]->e[2]); // DEBUG
                                                                                glTexCoord2f(o->textureList[o->faceList[j]->texture_index[k]]->e[0],
                                                                                             o->textureList[o->faceList[j]->texture_index[k]]->e[1]);
                                                                        }

//                                                                      printf("hay %d vértices, y este es el %d\n", o->vertexCount, o->faceList[j]->vertex_index[k]); // DEBUG
//                                                                      printf("su valor es (%f, %f, %f)\n", o->vertexList[o->faceList[j]->vertex_index[k]]->e[0], o->vertexList[o->faceList[j]->vertex_index[k]]->e[1], o->vertexList[o->faceList[j]->vertex_index[k]]->e[2]); // DEBUG
                                                                        glVertex3f(o->vertexList[o->faceList[j]->vertex_index[k]]->e[0],
                                                                                   o->vertexList[o->faceList[j]->vertex_index[k]]->e[1],
                                                                                   o->vertexList[o->faceList[j]->vertex_index[k]]->e[2]);
                                                                }
                                                        }
                                                }
                                        glEnd();
                                glPopMatrix();
                        glEndList();
                        if (o != NULL) delete o;
                }
        }
}

void display() {
        GLubyte tank_alpha;

        switch (pass) {
                case PASS_BLUR:
                        tank_alpha = 255;
                        glViewport(0, 0, BLUR_TEXTURE_SIZE, BLUR_TEXTURE_SIZE);
                        glClearColor(0.0, 0.0, 0.0, 0.0);
                        break;
                case PASS_LAST:
                        if (rings_t) {
                                if (rings_s) {
                                        if (rings_t > (2.0*T_RINGS)/3.0) tank_alpha = 255;
                                        else if (rings_t > T_RINGS/3.0) tank_alpha = 255 * (1.0 - ((((((float)T_RINGS) - ((float)rings_t))*3.0)/((float)T_RINGS)) - 1.0));
                                        else tank_alpha = 0;
                                } else {
                                        if (rings_t > (2.0*T_RINGS)/3.0) tank_alpha = 0;
                                        else if (rings_t > T_RINGS/3.0) tank_alpha = 255 * ((((((float)T_RINGS) - ((float)rings_t))*3.0)/((float)T_RINGS)) - 1.0);
                                        else tank_alpha = 255;
                                }
                        } else if (cam == CAM_FPS) tank_alpha = 100;
                        else tank_alpha = 255;
                        glViewport(0, 0, ww, wh);
                        glClearColor(0.0, 0.0, 0.0, 1.0);
                        break;
        }
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        gluPerspective(fov, aspectratio, znear, zfar);

        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();

/*
        glColor4ub(255, 255, 255, 255);
        glCallList(checker);
        glColor4ub(0, 0, 0, 255);
        glTranslatef(1, 0, 0);
        glCallList(checker);
*/
/*
        glPushMatrix();
                //glLoadIdentity();
                glRasterPos3f(0, 0, 10);
                glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24,'h');
        glPopMatrix();
*/

        switch (pass) {
                case PASS_BLUR:
                        glEnable(GL_LIGHTING);
                        for (i = 0; i < 8; i++) {
                                glDisable(GL_LIGHT0 + i);
                        }
                        break;
                case PASS_LAST:
                        glEnable(GL_LIGHTING);
                        for (i = MIN_BALA_LIGHT; i < 8; i++) {
                                glDisable(GL_LIGHT0 + i);
                        }
                        break;
        }

        if (pass == PASS_LAST) {
                glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
                switch (level) {
                        case LEVEL_DESERT:
                                glBindTexture(GL_TEXTURE_2D, tdesert);
                                break;
                        case LEVEL_SKY:
                                glBindTexture(GL_TEXTURE_2D, tcielo);
                                break;
                        case LEVEL_SPACE:
                                glBindTexture(GL_TEXTURE_2D, tstars);
                                break;
                }
                glEnable(GL_TEXTURE_2D);
                glPushMatrix();
                        glRotatef(cam_rotx, 1, 0, 0);
                        glRotatef(cam_roty, 0, 1, 0);
                        glRotatef((level == LEVEL_SKY ? 1 : -1)*90, 1, 0, 0);
                        glCallList(cielo);
                glPopMatrix();
                glDisable(GL_TEXTURE_2D);
        }

        glPushMatrix();
                glRotatef(cam_rotx, 1.0, 0.0, 0.0);
                glRotatef(cam_roty, 0.0, 1.0, 0.0);
                glTranslatef(-cam_x, -cam_y, -cam_z);
                glRotatef(-90, 0, 1, 0);
                glRotatef(-90, 1, 0, 0);
                glScalef(0.2, 0.2, 0.2);

                if (pass == PASS_LAST) {
                        if (!rings_t) {
                                light_pos[0] = 0;
                                light_pos[1] = 0;
                                light_pos[2] = H_LIGHT_TABLERO;
                        } else {
                                light_pos[0] = 0;
                                light_pos[1] = 0;
                                if ((2.0*T_RINGS)/3.0 < rings_t) {
                                        if (rings_s) {
                                                light_pos[2] = H_LIGHT_TABLERO * ((rings_t * 3.0)/T_RINGS - 2.0);
                                        } else {
                                                light_pos[2] = H_LIGHT_TABLERO * ((rings_t * 3.0)/T_RINGS - 2.0) + 5 * (H_RING + S_RING) * (3.0 - (rings_t * 3.0)/T_RINGS);
                                        }
                                } else if (rings_t > T_RINGS/3.0) {
                                        if (rings_s) {
                                                light_pos[2] = 5 * (H_RING + S_RING) * (2.0 - (rings_t * 3.0)/T_RINGS);
                                        } else {
                                                light_pos[2] = 5 * (H_RING + S_RING) * ((rings_t*3.0)/T_RINGS - 1.0);
                                        }
                                } else {
                                        if (rings_s) {
                                                light_pos[2] = H_LIGHT_TABLERO * (1.0 - (rings_t * 3.0)/T_RINGS) + 5 * (H_RING + S_RING) * ((rings_t * 3.0)/T_RINGS);
                                        } else {
                                                light_pos[2] = H_LIGHT_TABLERO * (1.0 - (rings_t * 3.0)/T_RINGS);
                                        }
                                }
                        }
                        glLightfv(GL_LIGHT0, GL_POSITION, light_pos);
                        glEnable(GL_LIGHT0);
                }

                if (headlights > 0) {
                        if (rings_t) {
                                if ((2.0*T_RINGS)/3.0 < rings_t) {
                                        if (rings_s) {
                                                light_intensity = 1;
                                        } else {
                                                light_intensity = 0;
                                        }
                                } else if (rings_t > T_RINGS/3.0) {
                                        if (rings_s) {
                                                light_intensity = (rings_t*3.0)/T_RINGS - 1.0;
                                        } else {
                                                light_intensity = 2.0 - (rings_t * 3.0)/T_RINGS;
                                        }
                                } else {
                                        if (rings_s) {
                                                light_intensity = 0;
                                        } else {
                                                light_intensity = 1;
                                        }
                                }
                        } else {
                                light_intensity = 1;
                        }
                        glPushMatrix();
                                glTranslatef(px, py, pz);
                                glRotatef(prz, 0, 0, 1);
                                glTranslatef(-2.83, 7.8, 2.2);
                                glLightfv(GL_LIGHT1, GL_POSITION, black4f);
                                light_pos[0] = -0.25;
                                light_pos[1] = 1;
                                light_pos[2] = 0;
                                glLightfv(GL_LIGHT1, GL_SPOT_DIRECTION, light_pos);
                        glPopMatrix();

                        glPushMatrix();
                                glTranslatef(px, py, pz);
                                glRotatef(prz, 0, 0, 1);
                                glTranslatef( 2.83, 7.8, 2.2);
                                glLightfv(GL_LIGHT2, GL_POSITION, black4f);
                                light_pos[0] = 0.25;
                                light_pos[1] = 1;
                                light_pos[2] = 0;
                                glLightfv(GL_LIGHT2, GL_SPOT_DIRECTION, light_pos);
                        glPopMatrix();
                }

                if (pass == PASS_LAST) {
                        if (headlights > 0) {
                                float param = 1.0 - headlights*light_intensity;
                                param = param*param*exp(5*(param - 1));
                                glLightf(GL_LIGHT1, GL_CONSTANT_ATTENUATION , 0.3*param + 0.01   );
                                glLightf(GL_LIGHT1, GL_LINEAR_ATTENUATION   , 0.3*param + 0.01   );
                                glLightf(GL_LIGHT1, GL_QUADRATIC_ATTENUATION, 0.3*param + 0.00001);
                                glEnable(GL_LIGHT1);
                                glLightf(GL_LIGHT2, GL_CONSTANT_ATTENUATION , 0.3*param + 0.01   );
                                glLightf(GL_LIGHT2, GL_LINEAR_ATTENUATION   , 0.3*param + 0.01   );
                                glLightf(GL_LIGHT2, GL_QUADRATIC_ATTENUATION, 0.3*param + 0.00001);
                                glEnable(GL_LIGHT2);
                        }

                        /* Luces de balas del jugador */
                        bala_light_dir[1] = -1;
                        for (i = 0; i < N_PBALAS; i++) if (pb[i]) {
                                if (MIN_BALA_LIGHT + i < 8) {
                                        bala_light_pos[0] = pbx[i];
                                        bala_light_pos[1] = pby[i];
                                        bala_light_pos[2] = pbz[i];
                                        glLightfv(GL_LIGHT0 + MIN_BALA_LIGHT + i, GL_POSITION, bala_light_pos);
                                        glEnable(GL_LIGHT0 + MIN_BALA_LIGHT + i);
                                }
                        }

                        /* Luces de balas de enemigos */
                        bala_light_dir[1] = 1;
                        for (i = 0; i < N_BALAS; i++) if (b[i]) {
                                if (MIN_BALA_LIGHT + N_PBALAS + i < 8) {
                                        bala_light_pos[0] = bx[i];
                                        bala_light_pos[1] = by[i];
                                        bala_light_pos[2] = bz[i];
                                        glLightfv(GL_LIGHT0 + N_PBALAS + MIN_BALA_LIGHT + i, GL_POSITION, bala_light_pos);
                                        glEnable(GL_LIGHT0 + N_PBALAS + MIN_BALA_LIGHT + i);
                                }
                        }
                }

                /* Balas del jugador */
                for (i = 0; i < N_PBALAS; i++) if (pb[i] && pbl[i] == level) {
                        glPushMatrix();
                                glTranslatef(pbx[i], pby[i], pbz[i]);
                                // TODO: usar solo dirección del cañón para mostrar la inclinación de la bala
                                glRotatef((-180.0*atan2(pbvx[i] + windx, pbvy[i] + windy))/M_PI, 0, 0, 1);
                                glRotatef(( 180.0*atan2(pbvz[i], sqrt((pbvx[i] + windx)*(pbvx[i] + windx) + (pbvy[i] + windy)*(pbvy[i] + windy))))/M_PI, 1, 0, 0);
                                glScalef(0.6, 1.0, 0.6);
                                switch (pass) {
                                        case PASS_BLUR:
                                                glDisable(GL_LIGHTING);
                                                glColor4ub(255, 255, 255, 255);
                                                break;
                                        case PASS_LAST:
                                                glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
                                                glBindTexture(GL_TEXTURE_2D, tbrushmetal);
                                                glEnable(GL_TEXTURE_2D);
                                                glColor4ub(70, 70, 70, 255);
                                                break;
                                }
                                glCallList(bala);
                                switch (pass) {
                                        case PASS_BLUR:
                                                glEnable(GL_LIGHTING);
                                                break;
                                        case PASS_LAST:
                                                glDisable(GL_TEXTURE_2D);
                                                break;
                                }
                        glPopMatrix();
                }

                /* Balas de enemigos */
                for (i = 0; i < N_BALAS; i++) if (b[i] && bl[i] == level) {
                        glPushMatrix();
                                glTranslatef(bx[i], by[i], bz[i]);
                                glRotatef(180.0, 0, 0, 1);
                                switch (pass) {
                                        case PASS_BLUR:
                                                glDisable(GL_LIGHTING);
                                                glColor4ub(255, 255, 255, 100);
                                                break;
                                        case PASS_LAST:
                                                glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
                                                glBindTexture(GL_TEXTURE_2D, tbrushmetal);
                                                glEnable(GL_TEXTURE_2D);
                                                glColor4ub(70, 70, 70, 255);
                                                break;
                                }
                                glCallList(bala);
                                switch (pass) {
                                        case PASS_BLUR:
                                                glEnable(GL_LIGHTING);
                                                break;
                                        case PASS_LAST:
                                                glDisable(GL_TEXTURE_2D);
                                                break;
                                }
                        glPopMatrix();
                }

                glPushMatrix();
                        glScalef(W_TABLERO, H_TABLERO, 1.0);
                        glTranslatef(0, 0, -0.5);
                        switch (pass) {
                                case PASS_BLUR:
                                        glCallList(cuadrado_simple);
                                        break;
                                case PASS_LAST:
                                        glColor4f(1, 1, 1, 1);
                                        glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
                                        switch (level) {
                                                case LEVEL_DESERT:
                                                        glBindTexture(GL_TEXTURE_2D, tcarved    );
                                                        break;
                                                case LEVEL_SKY:
                                                        glBindTexture(GL_TEXTURE_2D, tbumpmetal);
                                                        break;
                                                case LEVEL_SPACE:
                                                        glBindTexture(GL_TEXTURE_2D, tplates    );
                                                        break;
                                        }
                                        glEnable(GL_TEXTURE_2D);
                                        glMatrixMode(GL_TEXTURE);
                                        glLoadIdentity();
                                        if (level == LEVEL_SKY) glScalef(20, 20, 20);
                                        else                    glScalef(10, 10, 10);
                                        glMatrixMode(GL_MODELVIEW);
                                        glCallList(cuadrado_hd);
                                        glMatrixMode(GL_TEXTURE);
                                        glLoadIdentity();
                                        glMatrixMode(GL_MODELVIEW);
                                        glDisable(GL_TEXTURE_2D);
                                        break;
                        }
                glPopMatrix();

                glCallList(borde);

                glPushMatrix();
                        glTranslatef(0, 0, 0.01);
                        glCallList(ringbase);
                glPopMatrix();

                if (pass == PASS_LAST) {
                        /* Veleta */
                        glDisable(GL_CULL_FACE);
                        glPushMatrix();
                                glTranslatef(0, 0, 20);
                                glColor4ub(255, 255, 0, 255);
                                glLineWidth(5);
                                glScalef(-1, -1, 1);
                                {
                                        float w = sqrt(windx*windx + windy*windy);
                                        glScalef(w, w, w);
                                        glRotatef((180*atanf(windy/(windx == 0 ? 0.000001 : windx)))/M_PI + 180 + 2.5 * cos((new_time*w)/250.0), 0, 0, 1);
                                }
                                glRotatef(90, 0, 1, 0);
                                glScalef(1.5, 1.5, 15);
                                glCallList(veleta);
                        glPopMatrix();
                        glEnable(GL_CULL_FACE);
                }

                if (rings_t) {
                        float t = T_RINGS - rings_t;
                        for (i = 0; i < 5; i++) {
                                glPushMatrix();
                                        if (rings_t > (i*T_RINGS)/15.0 + (2.0 * T_RINGS)/3.0) {
                                                float ti = t;
                                                if (t == 0) ti = 1;
                                                glTranslatef(0, 0, ((H_RING + S_RING) * i + S_RING) - ((ti - (T_RINGS/3.0 - (i*T_RINGS)/15.0))*(ti - (T_RINGS/3.0 - (i*T_RINGS)/15.0)))/(ti*ti));
                                                glCallList(ring);
                                        } else if (rings_t < ((5 - i)*T_RINGS)/15.0) {
                                                float ti = rings_t * ((2.0*T_RINGS)/3.0 - ((5 - i)*T_RINGS)/15.0)/(((5 - i)*T_RINGS)/15.0);
                                                if (ti == 0) ti = 1;
                                                glTranslatef(0, 0, ((H_RING + S_RING) * i + S_RING) - ((ti - ((2.0*T_RINGS)/3.0 - ((5 - i)*T_RINGS)/15.0))*(ti - ((2.0*T_RINGS)/3.0 - ((5 - i)*T_RINGS)/15.0)))/(ti*ti));
                                                glCallList(ring);
                                        } else {
                                                glTranslatef(0, 0, (H_RING + S_RING) * i + S_RING);
                                                glCallList(ring);
                                        }
                                glPopMatrix();
                        }
                        if (pass == PASS_BLUR && T_RINGS/3.0 < t && t < (2.0*T_RINGS)/3.0) {
                                glDisable(GL_LIGHTING);
                                glBegin(GL_TRIANGLES);
                                        for (i = 0; i <= 6; i++) {
                                                for (j = 0; j < N_SLICES; j++) {
                                                        glColor4f(1, 1, 1, -(36/(T_RINGS*T_RINGS))*(t - T_RINGS/3.0)*(t - (2.0*T_RINGS)/3.0));
                                                        glVertex3f(0, 0, 5 * (H_RING + S_RING) * ((((rings_s ? t : rings_t)*3.0)/T_RINGS) - 1.0));
                                                        glColor4f(1, 1, 1, 0);
                                                        glVertex3f(D_RING_GLOW * D_RING * cos((((float) j     )*2*M_PI)/((float)N_SLICES)), D_RING_GLOW * D_RING * sin((((float) j     )*2*M_PI)/((float)N_SLICES)), (H_RING + S_RING) * i + S_RING);
                                                        glVertex3f(D_RING_GLOW * D_RING * cos((((float)(j + 1))*2*M_PI)/((float)N_SLICES)), D_RING_GLOW * D_RING * sin((((float)(j + 1))*2*M_PI)/((float)N_SLICES)), (H_RING + S_RING) * i + S_RING);

                                                        glColor4f(1, 1, 1, -(36/(T_RINGS*T_RINGS))*(t - T_RINGS/3.0)*(t - (2.0*T_RINGS)/3.0));
                                                        glVertex3f(0, 0, 5 * (H_RING + S_RING) * (((rings_s ? t : rings_t) - T_RINGS/3.0)/((2.0*T_RINGS)/3.0)));
                                                        glColor4f(1, 1, 1, 0);
                                                        glVertex3f(D_RING_GLOW * D_RING * cos((((float) j     )*2*M_PI)/((float)N_SLICES)), D_RING_GLOW * D_RING * sin((((float) j     )*2*M_PI)/((float)N_SLICES)), (H_RING + S_RING) * (i + 1));
                                                        glVertex3f(D_RING_GLOW * D_RING * cos((((float)(j + 1))*2*M_PI)/((float)N_SLICES)), D_RING_GLOW * D_RING * sin((((float)(j + 1))*2*M_PI)/((float)N_SLICES)), (H_RING + S_RING) * (i + 1));
                                                }
                                        }
                                glEnd();
                                glEnable(GL_LIGHTING);
                        }
                }

                /* Malos */
/*
                for (j = 0; j < ALIEN_ROWS; j++) {
                        for (i = 0; i < ALIEN_COLS; i++) if (a[j][i].life) {
                                glPushMatrix();
                                        glTranslatef(a[j][i].x, a[j][i].y, a[j][i].z);
                                        glScalef(W_ALIEN, H_ALIEN, D_ALIEN);
                                        glTranslatef(0, 0, 0.5);
                                        if (a[j][i].life == 2) {
                                                glColor4ubv(color_strong);
                                        } else {
                                                glColor4ubv(color_alien);
                                        }
                                        glCallList(mesh[a[j][i].mesh].list);
                                glPopMatrix();
                        }
                }
 */

                if (cam != CAM_FPS && tank_alpha > 0) {
                        glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
                        glPushMatrix();
                                glColor4ub(255, 255, 255, tank_alpha);
                                glTranslatef(px, py, pz);
                                glRotatef(prz + 180, 0, 0, 1);
                                glRotatef(90, 1, 0, 0);
                                glBindTexture(GL_TEXTURE_2D, tcamobump);
                                glEnable(GL_TEXTURE_2D);
                                glCallList(mesh[MESH_TANK].list);
                                glDisable(GL_TEXTURE_2D);
                                glPushMatrix();
                                        glTranslatef(0, 0, TURRET_Z);
                                        glDisable(GL_CULL_FACE);
                                        glBindTexture(GL_TEXTURE_2D, tcamobump);
                                        glEnable(GL_TEXTURE_2D);
                                        glRotatef(a_turret, 0, 1, 0);
                                        glCallList(mesh[MESH_TURRET].list);
                                        glTranslatef(0, CANNON_Y, CANNON_Z - TURRET_Z);
                                        glRotatef(-a_cannon, 1, 0, 0);
                                        glCallList(mesh[MESH_CANNON].list);
                                        glDisable(GL_TEXTURE_2D);
                                        glEnable(GL_CULL_FACE);
                                glPopMatrix();
                                glPushMatrix();
                                        glTranslatef(THREAD_X, 0, 0);
                                        glBindTexture(GL_TEXTURE_2D, tcorroded);
                                        glEnable(GL_TEXTURE_2D);
                                        i = ((int)(thread_l/0.08)) % 10;
                                        glCallList(mesh[MESH_T0 + (i >= 0 ? i : i + 10)].list);
                                        glTranslatef(-2.0 * THREAD_X, 0, 0);
                                        i = ((int)(thread_r/0.08)) % 10;
                                        glCallList(mesh[MESH_T0 + (i >= 0 ? i : i + 10)].list);
                                        glDisable(GL_TEXTURE_2D);
                                glPopMatrix();
                        glPopMatrix();
                }

/*
                glPushMatrix();
                        float cosax, cosay, cosaz, cosavx, cosavy, cosavz;
                        float ncosax, ncosay, ncosaz, ncosavx, ncosavy, ncosavz;

                        cosax  = 0;
                        cosay  = mesh[MESH_CANNON].zM + 1;
                        cosaz  = 0;
                        ncosax = cosax;
                        ncosay = cosay * cos((a_cannon*M_PI)/180.0) - cosaz * sin((a_cannon*M_PI)/180.0);
                        ncosaz = cosay * sin((a_cannon*M_PI)/180.0) + cosaz * cos((a_cannon*M_PI)/180.0);
                        cosax  = ncosax;
                        cosay  = ncosay;
                        cosaz  = ncosaz;
                        cosax += 0;
                        cosay += CANNON_Z - TURRET_Z;
                        cosaz += CANNON_Y;
                        ncosax = cosax * cos((a_turret*M_PI)/180.0) - cosay * sin((a_turret*M_PI)/180.0);
                        ncosay = cosax * sin((a_turret*M_PI)/180.0) + cosay * cos((a_turret*M_PI)/180.0);
                        ncosaz = cosaz;
                        cosax  = ncosax;
                        cosay  = ncosay;
                        cosaz  = ncosaz;
                        cosax += 0;
                        cosay += TURRET_Z;
                        cosaz += 0;
                        ncosax = cosax * cos((prz*M_PI)/180.0) - cosay * sin((prz*M_PI)/180.0);
                        ncosay = cosax * sin((prz*M_PI)/180.0) + cosay * cos((prz*M_PI)/180.0);
                        ncosaz = cosaz;
                        cosax  = ncosax;
                        cosay  = ncosay;
                        cosaz  = ncosaz;
                        cosax += px;
                        cosay += py;
                        cosaz += pz;
                        cosavx = cosax;
                        cosavy = cosay;
                        cosavz = cosaz;

                        cosax  = 0;
                        cosay  = mesh[MESH_CANNON].zM;
                        cosaz  = 0;
                        ncosax = cosax;
                        ncosay = cosay * cos((a_cannon*M_PI)/180.0) - cosaz * sin((a_cannon*M_PI)/180.0);
                        ncosaz = cosay * sin((a_cannon*M_PI)/180.0) + cosaz * cos((a_cannon*M_PI)/180.0);
                        cosax  = ncosax;
                        cosay  = ncosay;
                        cosaz  = ncosaz;
                        cosax += 0;
                        cosay += CANNON_Z - TURRET_Z;
                        cosaz += CANNON_Y;
                        ncosax = cosax * cos((a_turret*M_PI)/180.0) - cosay * sin((a_turret*M_PI)/180.0);
                        ncosay = cosax * sin((a_turret*M_PI)/180.0) + cosay * cos((a_turret*M_PI)/180.0);
                        ncosaz = cosaz;
                        cosax  = ncosax;
                        cosay  = ncosay;
                        cosaz  = ncosaz;
                        cosax += 0;
                        cosay += TURRET_Z;
                        cosaz += 0;
                        ncosax = cosax * cos((prz*M_PI)/180.0) - cosay * sin((prz*M_PI)/180.0);
                        ncosay = cosax * sin((prz*M_PI)/180.0) + cosay * cos((prz*M_PI)/180.0);
                        ncosaz = cosaz;
                        cosax  = ncosax;
                        cosay  = ncosay;
                        cosaz  = ncosaz;
                        cosax += px;
                        cosay += py;
                        cosaz += pz;
                        cosavx -= cosax;
                        cosavy -= cosay;
                        cosavz -= cosaz;

                        glLineWidth(3);
                        glColor4ub(255, 0, 0, 80);
                        glDisable(GL_LIGHTING);
                        glBegin(GL_LINES);
                                glVertex3f(cosax, cosay, cosaz);
                                glVertex3f(cosax+600*cosavx, cosay+600*cosavy, cosaz+600*cosavz);
                        glEnd();
                        glEnable(GL_LIGHTING);
                        glTranslatef(cosax, cosay, cosaz);
                        glRotatef((-180.0*atan2(cosavx, cosavy))/M_PI, 0, 0, 1);
                        glRotatef(( 180.0*atan(cosavz/sqrt(cosavx*cosavx + cosavy*cosavy)))/M_PI, 1, 0, 0);
                        glScalef(0.3, 1.0, 0.3);
                        glColor4ub(255, 255, 255, 255);
                        glCallList(bala);
                glPopMatrix();
*/

                glDisable(GL_CULL_FACE);
                glDisable(GL_LIGHTING);
                if (pass == PASS_LAST) for (i = 0; i < N_BOOM_SETS; i++) {
                        if (boom[i].on > 0 && boom[i].level == level) {
                                for (k = 0; k < N_BOOMS; k++) {
                                        glPushMatrix();
                                                glTranslatef(boom[i].x[k], boom[i].y[k], boom[i].z[k]);
                                                glScalef(3 * boom[i].s[k], 3 * boom[i].s[k], 3 * boom[i].s[k]);
                                                glRotatef(boom[i].a[k], boom[i].rx[k], boom[i].ry[k], boom[i].rz[k]);
                                                glBegin(GL_TRIANGLES);
                                                        glColor4ub(boom[i].r[k], boom[i].g[k], boom[i].b[k], 255 * ((float)boom[i].on) / ((float)T_BOOM));
                                                        glVertex3f(-0.5, -0.5, 0);
                                                        glVertex3f( 0.5, -0.5, 0);
                                                        glVertex3f( 0  ,  0.5, 0);
                                                glEnd();
                                        glPopMatrix();
                                }
                        }
                }
                if (pass == PASS_BLUR && headlights > 0) {
                        glPushMatrix();
                                glTranslatef(px, py, pz);
                                glRotatef(prz, 0, 0, 1);
                                glTranslatef(-2.83, 7.8, 2.2);
                                glTranslatef(0, -0.5, 0);
                                glRotatef((180.0 * atan2f(0.25, 1))/M_PI, 0, 0, 1);
                                glScalef(headlights*light_intensity + 0.001, headlights*light_intensity + 0.001, headlights*light_intensity + 0.001);
                                glScalef(30, 100, 50);
                                glRotatef(-120, 1, 0, 0);
                                glCallList(light_cone);
                        glPopMatrix();

                        glPushMatrix();
                                glTranslatef(px, py, pz);
                                glRotatef(prz, 0, 0, 1);
                                glTranslatef( 2.83, 7.8, 2.2);
                                glTranslatef(0, -0.5, 0);
                                glRotatef((180.0 * atan2f(-0.25, 1))/M_PI, 0, 0, 1);
                                glScalef(headlights*light_intensity + 0.001, headlights*light_intensity + 0.001, headlights*light_intensity + 0.001);
                                glScalef(30, 100, 50);
                                glRotatef(-120, 1, 0, 0);
                                glCallList(light_cone);
                        glPopMatrix();
                }

                if (pass == PASS_BLUR) for (i = 0; i < N_BOOM_SETS; i++) {
                        if (boom[i].on > 0 && boom[i].level == level) {
                                for (k = 0; k < N_BOOMS; k++) {
                                        glPushMatrix();
                                                glTranslatef(boom[i].x[k], boom[i].y[k], boom[i].z[k]);
                                                glScalef(3 * boom[i].s[k], 3 * boom[i].s[k], 3 * boom[i].s[k]);
                                                glRotatef(boom[i].a[k], boom[i].rx[k], boom[i].ry[k], boom[i].rz[k]);
                                                glBegin(GL_TRIANGLES);
                                                        glColor4ub(boom[i].r[k] * ((float)boom[i].on) / ((float)T_BOOM), boom[i].g[k] * ((float)boom[i].on) / ((float)T_BOOM), boom[i].b[k] * ((float)boom[i].on) / ((float)T_BOOM), 255);
//                                                      glColor4ub(255, 255, 255, ((boom[i].r[k] + boom[i].g[k] + boom[i].b[k])/3.0) * ((float)boom[i].on) / ((float)T_BOOM));
                                                        glVertex3f(-0.5, -0.5, 0);
                                                        glVertex3f( 0.5, -0.5, 0);
                                                        glVertex3f( 0  ,  0.5, 0);
                                                glEnd();
                                        glPopMatrix();
                                }
                        }
                }
                glEnable(GL_LIGHTING);
                glEnable(GL_CULL_FACE);

                if (cam == CAM_FPS && tank_alpha > 0) {
                        glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
                        glPushMatrix();
                                glColor4ub(255, 255, 255, tank_alpha);
                                glTranslatef(px, py, pz);
                                glRotatef(prz + 180, 0, 0, 1);
                                glRotatef(90, 1, 0, 0);
                                glBindTexture(GL_TEXTURE_2D, tcamobump);
                                glEnable(GL_TEXTURE_2D);
                                glCallList(mesh[MESH_TANK].list);
                                glDisable(GL_TEXTURE_2D);
                                glPushMatrix();
                                        glTranslatef(0, 0, TURRET_Z);
                                        glDisable(GL_CULL_FACE);
                                        glBindTexture(GL_TEXTURE_2D, tcamobump);
                                        glEnable(GL_TEXTURE_2D);
                                        glRotatef(a_turret, 0, 1, 0);
                                        glCallList(mesh[MESH_TURRET].list);
                                        glTranslatef(0, CANNON_Y, CANNON_Z - TURRET_Z);
                                        glRotatef(-a_cannon, 1, 0, 0);
                                        glCallList(mesh[MESH_CANNON].list);
                                        glDisable(GL_TEXTURE_2D);
                                        glEnable(GL_CULL_FACE);
                                glPopMatrix();
                                glPushMatrix();
                                        glTranslatef(THREAD_X, 0, 0);
                                        glBindTexture(GL_TEXTURE_2D, tbumpmetal);
                                        glEnable(GL_TEXTURE_2D);
                                        i = ((int)(thread_l/0.08)) % 10;
                                        glCallList(mesh[MESH_T0 + (i >= 0 ? i : i + 10)].list);
                                        glTranslatef(-2.0 * THREAD_X, 0, 0);
                                        i = ((int)(thread_r/0.08)) % 10;
                                        glCallList(mesh[MESH_T0 + (i >= 0 ? i : i + 10)].list);
                                        glDisable(GL_TEXTURE_2D);
                                glPopMatrix();
                        glPopMatrix();
                }
        glPopMatrix();

        if (pass == PASS_LAST) {
                /* HUD */
                glDisable(GL_LIGHTING);

                glMatrixMode(GL_PROJECTION);
                glLoadIdentity();
                glOrtho(0, 1, 0, 1, -1, 1);

                glDisable(GL_DEPTH_TEST);
                glMatrixMode(GL_MODELVIEW);
                glLoadIdentity();
                glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
                glBindTexture(GL_TEXTURE_2D, tblur);
                glEnable(GL_TEXTURE_2D);
                glBlendFunc(GL_SRC_ALPHA, GL_ONE);
                glPushMatrix();
                        glTranslatef(0.5 - D_BLUR/2, 0.5 - D_BLUR/2, -1);
                        for (i = 0; i < N_BLURS; i++) {
                                glTranslatef(D_BLUR/N_BLURS, 0, 0);
                                for (j = 0; j < N_BLURS; j++) {
                                        glTranslatef(0, D_BLUR/N_BLURS, 0);
                                        glPushMatrix();
                                                glScalef(1.0 + D_BLUR, 1.0 + D_BLUR, 1);
                                                glColor4f(1, 1, 1, (12.0/(N_BLURS*N_BLURS))*(sqrtf(0.5) - sqrtf((((float)i)/N_BLURS - 0.5)*(((float)i)/N_BLURS - 0.5) + (((float)j)/N_BLURS - 0.5)*(((float)j)/N_BLURS - 0.5))));
                                                glCallList(cuadrado_simple);
                                        glPopMatrix();
                                }
                                glTranslatef(0, -D_BLUR, 0);
                        }
                glPopMatrix();
                glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
                glDisable(GL_TEXTURE_2D);
                glEnable(GL_DEPTH_TEST);
                glClear(GL_DEPTH_BUFFER_BIT);

                glMatrixMode(GL_PROJECTION);
                glLoadIdentity();
                if (ww<=wh) {
                        glOrtho(-10.0, 10.0, -10.0 / aspectratio, 10.0 / aspectratio, 1.0, -1.0);
                } else {
                        glOrtho(-10.0 * aspectratio, 10.0 * aspectratio, -10.0, 10.0, 1.0, -1.0);
                }

                glMatrixMode(GL_MODELVIEW);
                glLoadIdentity();

                /* Score */
                for (i = 0; i < DIGITS_SCORE; i++) {
                        glPushMatrix();
                                glTranslatef(0, 9.2, 0);
                                glScalef(0.1, 0.1, 0.1);
                                glTranslatef((((DIGITS_SCORE + 1) / 2.0) - i - 0.5) * (W_SEGMENT + 4.0 * W_SEGMENT_T), 0, 0);
                                glCallList(segs[(abs(v_cannon == 0 ? 0 : (99.9 * (v_cannon - V_BALA_MIN))/(V_BALA_MAX - V_BALA_MIN)) / power(10, i)) % 10]);
        //                      glCallList(segs[(abs(pts) / power(10, i)) % 10]);
                        glPopMatrix();
                }

                /* Signo */
/*
                glPushMatrix();
                        glTranslatef(0, 9.2, 0);
                        glScalef(0.1, 0.1, 0.1);
                        glTranslatef(-(DIGITS_SCORE / 2.0) * (W_SEGMENT + 4.0 * W_SEGMENT_T), 0, 0);
                        if (pts < 0) glCallList(segs[10]);
                        else         glCallList(segs[11]);
                glPopMatrix();
 */

                glPushMatrix();
                        glTranslatef(0, 7.2, 0);
                        glPushMatrix();
                                glScalef(W_BARRA, H_BARRA, 0.1);
                                glTranslatef(-(6*W_BARRA+18), 0, 0);
                                float percent = 0;
                                if (v_cannon) percent = (v_cannon - V_BALA_MIN)/(V_BALA_MAX - V_BALA_MIN);
                                for (i = 0; i <12; i++) {
                                        glTranslatef(W_BARRA + 3, 0, 0);
                                        if (i < percent * 12) {
                                                if (i <  4         ) glColor4ubv(color_inicial);
                                                if (4 <= i && i < 8) glColor4ubv(color_medio  );
                                                if (8 <= i         ) glColor4ubv(color_lleno  );
                                        } else glColor4ubv(color_empty);
                                        glCallList(barra);
                                }
                        glPopMatrix();
                glPopMatrix();

                /* Vidas */
/*
                for (i = 0; i < lives - 1; i++) {
                        glPushMatrix();
                                glTranslatef(-12.25 + 2.1 * i, 8.5, 0);
                                glScalef(0.5, 0.5, 0.5);
                                glCallList(tanque);
                        glPopMatrix();
                }
 */

                glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
                glBindTexture(GL_TEXTURE_2D, tbrushmetal);
                glEnable(GL_TEXTURE_2D);
                for (i = 0; i < balas; i++) {
                        glPushMatrix();
                                glTranslatef(-12.25 + 0.8 * i, 8.5, 0);
                                glScalef(1, 2, 1);
                                glColor4ub(255, 255, 255, 255);
                                glCallList(bala);
                        glPopMatrix();
                }
                glDisable(GL_TEXTURE_2D);
        }

        switch (pass) {
                case PASS_BLUR:
                        glBindTexture(GL_TEXTURE_2D, tblur);
                        glCopyTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 0, 0, BLUR_TEXTURE_SIZE, BLUR_TEXTURE_SIZE, 0);
                        pass = PASS_LAST;
                        break;
                case PASS_LAST:
                        pass = PASS_BLUR;
                        break;
        }
}

void realDisplay() {
        if (blur) {
                pass = FIRST_PASS;
                do display();
                while (pass != FIRST_PASS);
        } else {
                pass = PASS_LAST;
                display();
        }
        glutSwapBuffers();
}

void reshape(int w, int h){
        if (h==0) h=1;
        ww = w;
        wh = h;
        aspectratio = ((float)w)/((float)h);
        glViewport(0, 0, w, h);
}

void skeydown(int key, int mx, int my) {
        if      (key == GLUT_KEY_LEFT)  keystate_l = 1;
        else if (key == GLUT_KEY_RIGHT) keystate_r = 1;
        else if (key == GLUT_KEY_UP)    keystate_u = 1;
        else if (key == GLUT_KEY_DOWN)  keystate_d = 1;
}

void skeyup(int key, int mx, int my) {
        if      (key == GLUT_KEY_LEFT)  keystate_l = 0;
        else if (key == GLUT_KEY_RIGHT) keystate_r = 0;
        else if (key == GLUT_KEY_UP)    keystate_u = 0;
        else if (key == GLUT_KEY_DOWN)  keystate_d = 0;
}

void keyup(unsigned char key, int mx, int my) {
        if      (key == key_fwd)          keystate_fwd          = 0;
        else if (key == key_back)         keystate_back         = 0;
        else if (key == key_left)         keystate_left         = 0;
        else if (key == key_right)        keystate_right        = 0;
        else if (key == key_use_l)        keystate_use_l        = 0;
        else if (key == key_use_r)        keystate_use_r        = 0;
        else if (key == key_lights_up)    keystate_lights_up    = 0;
        else if (key == key_lights_down)  keystate_lights_down  = 0;
        else if (key == key_cam_up)       keystate_cam_up       = 0;
        else if (key == key_cam_down)     keystate_cam_down     = 0;
        else if (key == key_cam_left)     keystate_cam_left     = 0;
        else if (key == key_cam_right)    keystate_cam_right    = 0;
        else if (key == key_cam_fwd)      keystate_cam_fwd      = 0;
        else if (key == key_cam_back)     keystate_cam_back     = 0;
        else if (key == key_cam_rotup)    keystate_cam_rotup    = 0;
        else if (key == key_cam_rotdown)  keystate_cam_rotdown  = 0;
        else if (key == key_cam_rotleft)  keystate_cam_rotleft  = 0;
        else if (key == key_cam_rotright) keystate_cam_rotright = 0;
        else if (key == key_enter)        keystate_enter        = 0;
        else if (key == key_shoot)      { keystate_shoot        = 0; posneg ^= 1; }
#ifdef AUDIO
        else if (key == key_music)        keystate_music        = 0;
#endif
}

void keydown(unsigned char key, int mx, int my) {
        if      (key == key_fwd)          keystate_fwd          = 1;
        else if (key == key_back)         keystate_back         = 1;
        else if (key == key_left)         keystate_left         = 1;
        else if (key == key_right)        keystate_right        = 1;
        else if (key == key_use_l)        keystate_use_l        = 1;
        else if (key == key_use_r)        keystate_use_r        = 1;
        else if (key == key_lights_up)    keystate_lights_up    = 1;
        else if (key == key_lights_down)  keystate_lights_down  = 1;
        else if (key == key_cam_up)       keystate_cam_up       = 1;
        else if (key == key_cam_down)     keystate_cam_down     = 1;
        else if (key == key_cam_left)     keystate_cam_left     = 1;
        else if (key == key_cam_right)    keystate_cam_right    = 1;
        else if (key == key_cam_fwd)      keystate_cam_fwd      = 1;
        else if (key == key_cam_back)     keystate_cam_back     = 1;
        else if (key == key_cam_rotup)    keystate_cam_rotup    = 1;
        else if (key == key_cam_rotdown)  keystate_cam_rotdown  = 1;
        else if (key == key_cam_rotleft)  keystate_cam_rotleft  = 1;
        else if (key == key_cam_rotright) keystate_cam_rotright = 1;
        else if (key == key_enter)        keystate_enter        = 1;
        else if (key == key_shoot)        keystate_shoot        = 1;
#ifdef AUDIO
        else if (key == key_music)        keystate_music        = 1;
#endif
        else if (key == key_blur_toggle) blur ^= 1;
        else if (key == key_cam_switch && !rings_t) {
                if ((cam += 1) >= N_CAMS) cam = 0;
#ifdef AUDIO
                if (cam > 0) {
                        Mix_ExpireChannel(motorChannel, 1);
                        motorChannel = Mix_PlayChannel(-1, motorC, -1);
                        if (moveChannel != -1){
                                Mix_ExpireChannel(moveChannel, 1);
                                moveChannel = Mix_PlayChannel(-1, moveC, -1);
                        }
                        if (reverseChannel != -1){
                                Mix_ExpireChannel(reverseChannel, 1);
                                moveChannel = Mix_PlayChannel(-1, reverseC, -1);
                        }
                        if (turretChannel != -1){
                                Mix_ExpireChannel(turretChannel, 1);
                                turretChannel = Mix_PlayChannel(-1, turretC, -1);
                        }
                }
                if(cam==0){
                        Mix_ExpireChannel(motorChannel, 1);
                        motorChannel = Mix_PlayChannel(-1, motorL, -1);
                        if (moveChannel != -1) {
                                Mix_ExpireChannel(moveChannel, 1);
                                moveChannel = Mix_PlayChannel(-1, moveL, -1);
                        }
                        if (reverseChannel != -1) {
                                Mix_ExpireChannel(reverseChannel, 1);
                                reverseChannel = Mix_PlayChannel(-1, reverseL, -1);
                        }
                        if (turretChannel != -1) {
                                Mix_ExpireChannel(turretChannel, 1);
                                turretChannel = Mix_PlayChannel(-1, turretL, -1);
                        }
                }
#endif
                cam_old_x    = cam_x;
                cam_old_y    = cam_y;
                cam_old_z    = cam_z;
                cam_old_rotx = cam_rotx;
                cam_old_roty = cam_roty;
                cam_old_t    = T_CAM_OLD;
                cam_old_adj  = 1;
        }
        else if (key == 27) exit(EX_OK);
}

void juego(int v) {
        new_time = glutGet(GLUT_ELAPSED_TIME);
        delta = new_time - old_time;
        old_time = new_time;
        deltas_sum += delta - deltas[deltas_cur];
        deltas[deltas_cur] = delta;
        deltas_cur = (deltas_cur + 1) % N_DELTAS;

#ifdef AUDIO
        if (keystate_music && !musicflag) {
                musicflag = 1;
                //SDL_Delay(200);
                //printf("Entreeee");
                //keystate_music = 0;
                if (music == NULL) {
                        music = Mix_LoadMUS("music/soviet.wav");
                        Mix_PlayMusic(music, -1);
                        //Mix_HookMusicFinished(musicDone);
                } else {
                        Mix_HaltMusic();
                        Mix_FreeMusic(music);
                        music = NULL;
                }
        }

        if (!keystate_music) {
                musicflag = 0;
        }
#endif

      if (((keystate_use_l && level > 0) || (keystate_use_r && level < N_LEVELS - 1)) && !rings_t && sqrt(px*px + py*py) < 5.0) {
                if (keystate_use_l) rings_d = -1;
                else                rings_d =  1;
                frozen = 1;
                rings_t = T_RINGS;
                rings_s = 1;
                px = 0;
                py = 0;
                old_cam = cam;
                cam = CAM_RINGS;
                cam_old_x    = cam_x;
                cam_old_y    = cam_y;
                cam_old_z    = cam_z;
                cam_old_rotx = cam_rotx;
                cam_old_roty = cam_roty;
                cam_old_t    = T_CAM_OLD;
                cam_old_adj  = 1;
        }

        if (rings_t > 0) {
                rings_t -= delta;
                if (rings_t <= 0) {
                        if (rings_s) {
                                rings_t = T_RINGS;
                                rings_s = 0;
                                level += rings_d;
                        } else {
                                rings_t = 0;
                                frozen =  0;
                                cam = old_cam;
                        }
                }
        }

        if (!frozen) {
                if (keystate_lights_up)   if ((headlights += delta / 500.0) > 1           ) headlights = 1;
                if (keystate_lights_down) if ((headlights -= delta / 500.0) < 0           ) headlights = 0;
                if (keystate_u)           if ((a_cannon   += delta /  20.0) > MAX_A_CANNON) a_cannon   = MAX_A_CANNON;
                if (keystate_d)           if ((a_cannon   -= delta /  20.0) < MIN_A_CANNON) a_cannon   = MIN_A_CANNON;
                if (keystate_l)           if ((a_turret   += delta /  20.0) > MAX_A_TURRET) a_turret   = MAX_A_TURRET;
                if (keystate_r)           if ((a_turret   -= delta /  20.0) < MIN_A_TURRET) a_turret   = MIN_A_TURRET;
                if (keystate_fwd)   pv   += delta / 5000.0;
                if (keystate_back)  pv   -= delta / 5000.0;
                if (keystate_left)  pvrz += delta / 3000.0;
                if (keystate_right) pvrz -= delta / 3000.0;
                pv   += -0.005 * pv   * delta;
                if (!keystate_fwd && !keystate_back && fabs(pv) < 0.001) pv = 0;
                pvrz += -0.005 * pvrz * delta;
                prz += (pv < 0 ? -1 : 1) * pvrz * delta;
                prz -= (((int)prz)/360)*360;
                pvx = pv * sin((-prz*M_PI)/180.0);
                pvy = pv * cos((-prz*M_PI)/180.0);
/*
                pvx  += -0.005 * pvx  * delta;
                pvy  += -0.005 * pvy  * delta;
 */
                pvz  += -0.005 * pvz  * delta; // Roce vertical; cambiar?
                // TODO: gravedad, colisiones del tanque
                px  += pvx  * delta;
                py  += pvy  * delta;
                pz  += pvz  * delta;
                thread_l += (pv > 0 ? 1 : -1) * sqrt(pvx*pvx + pvy*pvy) * delta;
                thread_r += (pv > 0 ? 1 : -1) * sqrt(pvx*pvx + pvy*pvy) * delta;
                // TODO: bounding box isn't aligned anymore; consider prz
                if (px < -(W_TABLERO - mesh[MESH_TANK].size_x)/2) {
                        px  = -(W_TABLERO - mesh[MESH_TANK].size_x)/2;
                        pvx = 0;
                }
                if (px >  (W_TABLERO - mesh[MESH_TANK].size_x)/2) {
                        px  =  (W_TABLERO - mesh[MESH_TANK].size_x)/2;
                        pvx = 0;
                }
                if (py < -(H_TABLERO - mesh[MESH_TANK].size_z)/2) {
                        py  = -(H_TABLERO - mesh[MESH_TANK].size_z)/2;
                        pvy = 0;
                }
                if (py >  (H_TABLERO - mesh[MESH_TANK].size_z)/2) {
                        py  =  (H_TABLERO - mesh[MESH_TANK].size_z)/2;
                        pvy = 0;
                }
        }

#ifdef AUDIO
        if (keystate_u) {
                if ( turretChannel == -1) {
                        if (cam != CAM_OVERHEAD)  turretChannel = Mix_PlayChannel(-1,  turretC, -1);
                        else                      turretChannel = Mix_PlayChannel(-1,  turretL, -1);
                }
        }
        if (keystate_d) {
                if ( turretChannel == -1) {
                        if (cam != CAM_OVERHEAD)  turretChannel = Mix_PlayChannel(-1,  turretC, -1);
                        else                      turretChannel = Mix_PlayChannel(-1,  turretL, -1);
                }
        }
        if (keystate_l) {
                if ( turretChannel == -1) {
                        if (cam != CAM_OVERHEAD)  turretChannel = Mix_PlayChannel(-1,  turretC, -1);
                        else                      turretChannel = Mix_PlayChannel(-1,  turretL, -1);
                }
        }
        if (keystate_r) {
                if ( turretChannel == -1) {
                        if (cam != CAM_OVERHEAD)  turretChannel = Mix_PlayChannel(-1,  turretC, -1);
                        else                      turretChannel = Mix_PlayChannel(-1,  turretL, -1);
                }
        }
        if (!(keystate_r || keystate_l || keystate_d || keystate_u) &&  turretChannel != -1) {
                Mix_ExpireChannel( turretChannel, 1);
                turretChannel = -1;
        }
        if (keystate_fwd) {
                if (   moveChannel == -1) {
                        if (cam != CAM_OVERHEAD)    moveChannel = Mix_PlayChannel(-1,    moveC, -1);
                        else                        moveChannel = Mix_PlayChannel(-1,    moveL, -1);
                }
        }
        if (keystate_back) {
                if (reverseChannel == -1) {
                        if (cam != CAM_OVERHEAD) reverseChannel = Mix_PlayChannel(-1, reverseC, -1);
                        else                     reverseChannel = Mix_PlayChannel(-1, reverseL, -1);
                }
                if (   moveChannel == -1) {
                        if (cam != CAM_OVERHEAD)    moveChannel = Mix_PlayChannel(-1,    moveC, -1);
                        else                        moveChannel = Mix_PlayChannel(-1,    moveL, -1);
                }
        }
        if (keystate_left) {
                if (   moveChannel == -1) {
                        if (cam != CAM_OVERHEAD)    moveChannel = Mix_PlayChannel(-1,    moveC, -1);
                        else                        moveChannel = Mix_PlayChannel(-1,    moveL, -1);
                }
        }
        if (keystate_right) {
                if (   moveChannel == -1) {
                        if (cam != CAM_OVERHEAD)    moveChannel = Mix_PlayChannel(-1,    moveC, -1);
                        else                        moveChannel = Mix_PlayChannel(-1,    moveL, -1);
                }
        }
        if (!(keystate_fwd || keystate_back || keystate_left || keystate_right) &&    moveChannel != -1) {
                Mix_ExpireChannel(   moveChannel, 1);
                moveChannel = -1;
        }
        if (!(keystate_back) && reverseChannel != -1) {
                Mix_ExpireChannel(reverseChannel, 1);
                reverseChannel = -1;
        }
#endif

        switch (cam) {
                case CAM_OVERHEAD:
                        cam_x = 0;
                        cam_y = H_OVERHEAD;
                        cam_z = 0;
                        cam_rotx = 90;
                        cam_roty = 0;
                        break;

                case CAM_FPS:
                        {
                                // TODO: optimize
                                float ncam_x, ncam_y, ncam_z;
                                float cam_vx, cam_vy, cam_vz;
                                float ncam_vx, ncam_vy, ncam_vz;

                                cam_x  = 0;
                                cam_y  = mesh[MESH_CANNON].zM;
                                cam_z  = 0;
                                ncam_x = cam_x;
                                ncam_y = cam_y * cos((a_cannon*M_PI)/180.0) - cam_z * sin((a_cannon*M_PI)/180.0);
                                ncam_z = cam_y * sin((a_cannon*M_PI)/180.0) + cam_z * cos((a_cannon*M_PI)/180.0);
                                cam_x  = ncam_x;
                                cam_y  = ncam_y;
                                cam_z  = ncam_z;
                                cam_x += 0;
                                cam_y += CANNON_Z - TURRET_Z;
                                cam_z += CANNON_Y;
                                ncam_x = cam_x * cos((a_turret*M_PI)/180.0) - cam_y * sin((a_turret*M_PI)/180.0);
                                ncam_y = cam_x * sin((a_turret*M_PI)/180.0) + cam_y * cos((a_turret*M_PI)/180.0);
                                ncam_z = cam_z;
                                cam_x  = ncam_x;
                                cam_y  = ncam_y;
                                cam_z  = ncam_z;
                                cam_x += 0;
                                cam_y += TURRET_Z;
                                cam_z += 0;
                                ncam_x = cam_x * cos((prz*M_PI)/180.0) - cam_y * sin((prz*M_PI)/180.0);
                                ncam_y = cam_x * sin((prz*M_PI)/180.0) + cam_y * cos((prz*M_PI)/180.0);
                                ncam_z = cam_z;
                                cam_x  = ncam_x;
                                cam_y  = ncam_y;
                                cam_z  = ncam_z;
                                cam_x += px;
                                cam_y += py;
                                cam_z += pz;
                                cam_vx = cam_x;
                                cam_vy = cam_y;
                                cam_vz = cam_z;

                                cam_x  = 0;
                                cam_y  = mesh[MESH_CANNON].zM * (0.95 + 0.015 * retract);
                                cam_z  = 0;
                                ncam_x = cam_x;
                                ncam_y = cam_y * cos((a_cannon*M_PI)/180.0) - cam_z * sin((a_cannon*M_PI)/180.0);
                                ncam_z = cam_y * sin((a_cannon*M_PI)/180.0) + cam_z * cos((a_cannon*M_PI)/180.0);
                                cam_x  = ncam_x;
                                cam_y  = ncam_y;
                                cam_z  = ncam_z;
                                cam_x += 0;
                                cam_y += CANNON_Z - TURRET_Z;
                                cam_z += CANNON_Y;
                                ncam_x = cam_x * cos((a_turret*M_PI)/180.0) - cam_y * sin((a_turret*M_PI)/180.0);
                                ncam_y = cam_x * sin((a_turret*M_PI)/180.0) + cam_y * cos((a_turret*M_PI)/180.0);
                                ncam_z = cam_z;
                                cam_x  = ncam_x;
                                cam_y  = ncam_y;
                                cam_z  = ncam_z;
                                cam_x += 0;
                                cam_y += TURRET_Z;
                                cam_z += 0;
                                ncam_x = cam_x * cos((prz*M_PI)/180.0) - cam_y * sin((prz*M_PI)/180.0);
                                ncam_y = cam_x * sin((prz*M_PI)/180.0) + cam_y * cos((prz*M_PI)/180.0);
                                ncam_z = cam_z;
                                ncam_vx = cam_vx - (ncam_x + px);
                                ncam_vy = cam_vy - (ncam_y + py);
                                ncam_vz = cam_vz - (ncam_z + pz);
                                cam_vx = ncam_vy;
                                cam_vy = ncam_vz;
                                cam_vz = ncam_vx;
                                cam_x  = 0.2 * (ncam_y + py);
                                cam_y  = 0.2 * (ncam_z + pz);
                                cam_z  = 0.2 * (ncam_x + px);

                                cam_roty = 90.0 + (180.0*atan2(cam_vz, cam_vx))/M_PI;
                                cam_rotx = (-180.0*atan(cam_vy/sqrt(cam_vx*cam_vx + cam_vz*cam_vz)))/M_PI;
                        }
                        break;

                case CAM_TPS:
                        cam_x = 0.2 * (py + 18 * cos(((180.0 - prz)*M_PI)/180.0));
                        cam_y = 0.2 * (pz + mesh[MESH_TURRET].zM * 3.5);
                        cam_z = 0.2 * (px + 18 * sin(((180.0 - prz)*M_PI)/180.0));

                        cam_roty = 90.0 - prz;
                        cam_rotx = 30;
                        break;

                case CAM_MANUAL:
                        if (keystate_cam_up)       cam_y    += delta / 100.0;
                        if (keystate_cam_down)     cam_y    -= delta / 100.0;
                        if (keystate_cam_rotup)    cam_rotx -= delta / 25.0;
                        if (keystate_cam_rotdown)  cam_rotx += delta / 25.0;
                        if (keystate_cam_rotleft)  cam_roty -= delta / 25.0;
                        if (keystate_cam_rotright) cam_roty += delta / 25.0;
                        if (keystate_cam_fwd) {
                                cam_x += (delta * sin((cam_roty * M_PI)/180.0)) / 100.0;
                                cam_y -= (delta * sin((cam_rotx * M_PI)/180.0)) / 100.0;
                                cam_z -= (delta * cos((cam_roty * M_PI)/180.0)) / 100.0;
                        }
                        if (keystate_cam_back) {
                                cam_x -= (delta * sin((cam_roty * M_PI)/180.0)) / 100.0;
                                cam_y += (delta * sin((cam_rotx * M_PI)/180.0)) / 100.0;
                                cam_z += (delta * cos((cam_roty * M_PI)/180.0)) / 100.0;
                        }
                        if (keystate_cam_left) {
                                cam_x -= (delta * cos((cam_roty * M_PI)/180.0)) / 100.0;
                                cam_z -= (delta * sin((cam_roty * M_PI)/180.0)) / 100.0;
                        }
                        if (keystate_cam_right) {
                                cam_x += (delta * cos((cam_roty * M_PI)/180.0)) / 100.0;
                                cam_z += (delta * sin((cam_roty * M_PI)/180.0)) / 100.0;
                        }
                        break;

                case CAM_RINGS:
                        cam_x = 0.2 * (py + 25 * cos(((90.0 - (180.0 * ((T_RINGS - rings_t)/T_RINGS + (!rings_s))) - prz)*M_PI)/180.0));
                        cam_y = 0.2 * (pz + mesh[MESH_TURRET].zM * 1.5);
                        cam_z = 0.2 * (px + 25 * sin(((90.0 - (180.0 * ((T_RINGS - rings_t)/T_RINGS + (!rings_s))) - prz)*M_PI)/180.0));

                        cam_roty = -(180.0 * ((T_RINGS - rings_t)/T_RINGS + (!rings_s))) - prz;
                        cam_rotx = 0;
                        break;

                default:
                        break;
        }
        if (cam_old_t) {
                cam_old_t -= delta;
                if (cam_old_t < 0) {
                        cam_old_t = 0;
                } else {
                        if (cam_old_adj) {
                                cam_old_rotx += (((int)(cam_rotx - cam_old_rotx))/360)*360;
                                if ((cam_rotx - cam_old_rotx) > 180) cam_old_rotx += 360;
                                cam_old_roty += (((int)(cam_roty - cam_old_roty))/360)*360;
                                if ((cam_roty - cam_old_roty) > 180) cam_old_roty += 360;
                                cam_old_adj = 0;
                        }
                        float t = ((float)(T_CAM_OLD - cam_old_t))/T_CAM_OLD;
                        t = t*t*(t - 2)*(t - 2);
                        cam_x    = cam_x    * t + cam_old_x    * (1 - t);
                        cam_y    = cam_y    * t + cam_old_y    * (1 - t);
                        cam_z    = cam_z    * t + cam_old_z    * (1 - t);
                        cam_rotx = cam_rotx * t + cam_old_rotx * (1 - t);
                        cam_roty = cam_roty * t + cam_old_roty * (1 - t);
                }
        }

        if (pbn < N_PBALAS) {
                if (keystate_shoot) {
#ifdef AUDIO
//                      Mix_PlayChannel(-1, phaser, 0);
#endif
                        if (v_cannon == 0) v_cannon = V_BALA_MIN;
//                      else if ((v_cannon += 0.000025 * delta) > V_BALA_MAX) v_cannon = V_BALA_MAX;
                        else {
                                if (posneg) {
                                        if ((v_cannon -= 0.000025 * delta) < V_BALA_MIN) v_cannon = V_BALA_MIN;
                                } else {
                                        if ((v_cannon += 0.000025 * delta) > V_BALA_MAX) v_cannon = V_BALA_MAX;
                                }
                        }
                } else if (!frozen && v_cannon > 0 && keystate_enter && balas > 0 && retract == 1) {
#ifdef AUDIO
                        if (cam > 0) {
                                Mix_PlayChannel(-1, fireC, 0);
                        } else if (cam == 0) {
                                Mix_PlayChannel(-1, fireL, 0);
                        }
#endif
                        float npbx, npby, npbz;
                        pbn++;
                        if ((++pbi) == N_PBALAS) pbi = 0;
                        pb[pbi]   = 1;
                        pbl[pbi]  = level;

                        // TODO: optimize
                        // TODO: velocidad inicial por rotación de tanque y turret
                        pbx[pbi]  = 0;
                        pby[pbi]  = mesh[MESH_CANNON].zM + v_cannon;
                        pbz[pbi]  = 0;
                        npbx = pbx[pbi];
                        npby = pby[pbi] * cos((a_cannon*M_PI)/180.0) - pbz[pbi] * sin((a_cannon*M_PI)/180.0);
                        npbz = pby[pbi] * sin((a_cannon*M_PI)/180.0) + pbz[pbi] * cos((a_cannon*M_PI)/180.0);
                        pbx[pbi]  = npbx;
                        pby[pbi]  = npby;
                        pbz[pbi]  = npbz;
                        pbx[pbi] += 0;
                        pby[pbi] += CANNON_Z - TURRET_Z;
                        pbz[pbi] += CANNON_Y;
                        npbx = pbx[pbi] * cos((a_turret*M_PI)/180.0) - pby[pbi] * sin((a_turret*M_PI)/180.0);
                        npby = pbx[pbi] * sin((a_turret*M_PI)/180.0) + pby[pbi] * cos((a_turret*M_PI)/180.0);
                        npbz = pbz[pbi];
                        pbx[pbi]  = npbx;
                        pby[pbi]  = npby;
                        pbz[pbi]  = npbz;
                        pbx[pbi] += 0;
                        pby[pbi] += TURRET_Z;
                        pbz[pbi] += 0;
                        npbx = pbx[pbi] * cos((prz*M_PI)/180.0) - pby[pbi] * sin((prz*M_PI)/180.0);
                        npby = pbx[pbi] * sin((prz*M_PI)/180.0) + pby[pbi] * cos((prz*M_PI)/180.0);
                        npbz = pbz[pbi];
                        pbx[pbi]  = npbx;
                        pby[pbi]  = npby;
                        pbz[pbi]  = npbz;
                        pbx[pbi] += px;
                        pby[pbi] += py;
                        pbz[pbi] += pz;
                        pbvx[pbi] = pbx[pbi];
                        pbvy[pbi] = pby[pbi];
                        pbvz[pbi] = pbz[pbi];

                        pbx[pbi]  = 0;
                        pby[pbi]  = mesh[MESH_CANNON].zM;
                        pbz[pbi]  = 0;
                        npbx = pbx[pbi];
                        npby = pby[pbi] * cos((a_cannon*M_PI)/180.0) - pbz[pbi] * sin((a_cannon*M_PI)/180.0);
                        npbz = pby[pbi] * sin((a_cannon*M_PI)/180.0) + pbz[pbi] * cos((a_cannon*M_PI)/180.0);
                        pbx[pbi]  = npbx;
                        pby[pbi]  = npby;
                        pbz[pbi]  = npbz;
                        pbx[pbi] += 0;
                        pby[pbi] += CANNON_Z - TURRET_Z;
                        pbz[pbi] += CANNON_Y;
                        npbx = pbx[pbi] * cos((a_turret*M_PI)/180.0) - pby[pbi] * sin((a_turret*M_PI)/180.0);
                        npby = pbx[pbi] * sin((a_turret*M_PI)/180.0) + pby[pbi] * cos((a_turret*M_PI)/180.0);
                        npbz = pbz[pbi];
                        pbx[pbi]  = npbx;
                        pby[pbi]  = npby;
                        pbz[pbi]  = npbz;
                        pbx[pbi] += 0;
                        pby[pbi] += TURRET_Z;
                        pbz[pbi] += 0;
                        npbx = pbx[pbi] * cos((prz*M_PI)/180.0) - pby[pbi] * sin((prz*M_PI)/180.0);
                        npby = pbx[pbi] * sin((prz*M_PI)/180.0) + pby[pbi] * cos((prz*M_PI)/180.0);
                        npbz = pbz[pbi];
                        pbx[pbi]  = npbx;
                        pby[pbi]  = npby;
                        pbz[pbi]  = npbz;
                        pbx[pbi] += px;
                        pby[pbi] += py;
                        pbz[pbi] += pz;
                        pbvx[pbi] += pvx - pbx[pbi];
                        pbvy[pbi] += pvy - pby[pbi];
                        pbvz[pbi] += pvz - pbz[pbi];

                        retract   = 0;
                        posneg    = 0;
                        balas--;

                        boom[nboom].on = (T_BOOM*(V_BALA_MAX - V_BALA_MIN))/v_cannon;
                        for (k = 0; k < N_BOOMS; k++) {
                                boom[nboom].x[k]  = pbx[pbi];
                                boom[nboom].y[k]  = pby[pbi];
                                boom[nboom].z[k]  = pbz[pbi];
                                boom[nboom].a[k]  =                                                                                                ((float)rand())/((float)RAND_MAX)  * 180.0;
                                boom[nboom].rx[k] =                                                                                          0.5 - ((float)rand())/((float)RAND_MAX);
                                boom[nboom].ry[k] =                                                                                          0.5 - ((float)rand())/((float)RAND_MAX);
                                boom[nboom].rz[k] =                                                                                          0.5 - ((float)rand())/((float)RAND_MAX);
                                boom[nboom].vx[k] = (pbx[pbi] <= -W_TABLERO/2.0 || W_TABLERO/2.0 <= pbx[pbi] ? -1 : 1) * pbvx[pbi] + (0.5 - ((float)rand())/((float)RAND_MAX)) * 0.01;
                                boom[nboom].vy[k] = (pby[pbi] <= -H_TABLERO/2.0 || H_TABLERO/2.0 <= pby[pbi] ? -1 : 1) * pbvy[pbi] + (0.5 - ((float)rand())/((float)RAND_MAX)) * 0.01;
                                boom[nboom].vz[k] = (pbz[pbi] <= 0                                           ? -1 : 1) * pbvz[pbi] + (0.5 - ((float)rand())/((float)RAND_MAX)) * 0.01;
                                boom[nboom].va[k] = 0.5 - ((float)rand())/((float)RAND_MAX);
                                boom[nboom].s[k]  = ((float)rand())/((float)RAND_MAX)*BOOM_SCALE;
                                boom[nboom].r[k]  = 120 + 120.0 * ((float)rand())/((float)RAND_MAX);
                                boom[nboom].g[k]  = 120 + 120.0 * ((float)rand())/((float)RAND_MAX);
                                boom[nboom].b[k]  =        60.0 * ((float)rand())/((float)RAND_MAX);
                        }
                        boom[nboom].level = level;
                        if ((++nboom) == N_BOOM_SETS) nboom = 0;

                }
        }
        if (!frozen && (retract += delta * 0.0025) > 1) retract = 1;

        for (i = 0; i < N_BOOM_SETS; i++) {
                if (boom[i].on) {
                        if ((boom[i].on -= delta) < 0) boom[i].on = 0;
                        for (k = 0; k < N_BOOMS; k++) {
                                boom[i].x[k]  += boom[i].vx[k] * delta;
                                boom[i].y[k]  += boom[i].vy[k] * delta;
                                boom[i].vz[k] += (GRAVEDAD/3.0) * delta;
                                boom[i].z[k]  += boom[i].vz[k] * delta;
                                boom[i].a[k]  += boom[i].va[k] * delta;
                        }
                }
        }

        for (int q = 0; q < N_PBALAS; q++) if (pb[q]) {
                pbvz[q] += delta * GRAVEDAD;
                pbx[q]  += delta * pbvx[q] + windx / 15.0;
                pby[q]  += delta * pbvy[q] + windy / 15.0;
                pbz[q]  += delta * pbvz[q];
/*
                if (
                   o.life > 0                 &&
                   pby[q] >= o.y - H_OVNI/2.0 &&
                   pby[q] <= o.y + H_OVNI/2.0 &&
                   pbx[q] >= o.x - W_OVNI/2.0 &&
                   pbx[q] <= o.x + W_OVNI/2.0 &&
                   pbz[q] >= o.z - D_OVNI/2.0 &&
                   pbz[q] <= o.z + D_OVNI/2.0
                  ) {
                        pb[q] = 0;
                        pbn--;
                        o.life--;
                        if (o.life == 0) pts += PTS_OVNI;
                        ovni_time = glutGet(GLUT_ELAPSED_TIME);
                }
*/
/*
                for (i = 0; i < ALIEN_COLS; i++) {
                        if (col[i] == 0) continue;
                        if (xcol[i] - W_ALIEN/2.0 <= pbx[q] && pbx[q] <= xcol[i] + W_ALIEN/2.0) {
                                for (j = 0; j < ALIEN_ROWS; j++) {
                                        if (
                                                a[j][i].life > 0 &&
                                                a[j][i].y - (H_ALIEN/2.0) <= pby[q] && pby[q] <= a[j][i].y + (H_ALIEN/2.0) &&
                                                a[j][i].z - (D_ALIEN/2.0) <= pbz[q] && pbz[q] <= a[j][i].z + (D_ALIEN/2.0)
                                            ) {
                                                pb[q] = 0;
                                                pbn--;
                                                a[j][i].life--;
                                                if (a[j][i].life == 0) {
                                                        pts += a[j][i].pts;
                                                }
                                                col[i]--;
                                                if (i == lastcol  && col[i] == 0) while (--lastcol  >= 0          && col[lastcol ] == 0);
                                                if (i == firstcol && col[i] == 0) while (++firstcol <  ALIEN_COLS && col[firstcol] == 0);
                                                if (lastcol < 0 || firstcol > ALIEN_COLS) win = 1;
                                                break;
                                        }
                                }
                                if (!pb[q]) break;
                        }
                }

                for (j = 0; j < N_WALLS; j++) {
                        if (
                            w[j].life > 0                                                  &&
                            pby[q] <= w[j].y + H_WALL/2.0 && w[j].y - H_WALL/2.0 <= pby[q] &&
                            pbx[q] <= w[j].x + W_WALL/2.0 && w[j].x - W_WALL/2.0 <= pbx[q] &&
                            pbz[q] <= w[j].z + D_WALL/2.0 && w[j].z - D_WALL/2.0 <= pbz[q]
                           ) {
                                if ((--w[j].life) == 0) pts += PTS_WALL; //w[k].pts; // TODO: wtf, heap corrupted?
                                pb[q] = 0;
                                pbn--;
                                break;
                        }
                }
*/
                if (
                        pbx[q] <= -W_TABLERO/2.0 || W_TABLERO/2.0 <= pbx[q] ||
                        pby[q] <= -H_TABLERO/2.0 || H_TABLERO/2.0 <= pby[q] ||
                        pbz[q] <= 0
                    ) {
#ifdef AUDIO
                        if (cam > 0) {
                                exploChannel = Mix_PlayChannel(-1, exploC, 0);
                        } else if (cam == 0) {
                                exploChannel = Mix_PlayChannel(-1, exploL, 0);
                        }
#endif
                        pb[q] = 0;
                        pbn--;
                        boom[nboom].on = T_BOOM;
                        for (k = 0; k < N_BOOMS; k++) {
                                boom[nboom].x[k]  = pbx[q];
                                boom[nboom].y[k]  = pby[q];
                                boom[nboom].z[k]  = pbz[q];
                                boom[nboom].a[k]  =                                                                                   ((float)rand())/((float)RAND_MAX)  * 180.0;
                                boom[nboom].rx[k] =                                                                             0.5 - ((float)rand())/((float)RAND_MAX);
                                boom[nboom].ry[k] =                                                                             0.5 - ((float)rand())/((float)RAND_MAX);
                                boom[nboom].rz[k] =                                                                             0.5 - ((float)rand())/((float)RAND_MAX);
                                boom[nboom].vx[k] = (pbx[q] <= -W_TABLERO/2.0 || W_TABLERO/2.0 <= pbx[q] ? -1 : 1) * pbvx[q] * 0.25 + (0.5 - ((float)rand())/((float)RAND_MAX)) * 0.01;
                                boom[nboom].vy[k] = (pby[q] <= -H_TABLERO/2.0 || H_TABLERO/2.0 <= pby[q] ? -1 : 1) * pbvy[q] * 0.25 + (0.5 - ((float)rand())/((float)RAND_MAX)) * 0.01;
                                boom[nboom].vz[k] = (pbz[q] <= 0                                         ? -1 : 1) * pbvz[q] * 0.25 + (0.5 - ((float)rand())/((float)RAND_MAX)) * 0.01;
                                boom[nboom].va[k] = 0.5 - ((float)rand())/((float)RAND_MAX);
                                boom[nboom].s[k]  = ((float)rand())/((float)RAND_MAX)*BOOM_SCALE;
                                boom[nboom].r[k]  = 120 + 120.0 * ((float)rand())/((float)RAND_MAX);
                                boom[nboom].g[k]  = 120 + 120.0 * ((float)rand())/((float)RAND_MAX);
                                boom[nboom].b[k]  =        60.0 * ((float)rand())/((float)RAND_MAX);
                        }
                        boom[nboom].level = level;
                        if ((++nboom) == N_BOOM_SETS) nboom = 0;
                }
        }

        if (win) initJuego();
        else {
/*
                if ((c_bala += delta) >= T_BALA) {
                        c_bala = 0;
                        for (;;) {
                                j = ((double)(rand())/((double)RAND_MAX + 1))*((ALIEN_COLS*ALIEN_ROWS)-1);
                                if (a[j % ALIEN_ROWS][j / ALIEN_ROWS].life == 0) continue;
                                bx[bi] = a[j % ALIEN_ROWS][j / ALIEN_ROWS].x;
                                by[bi] = a[j % ALIEN_ROWS][j / ALIEN_ROWS].y;
                                bz[bi] = a[j % ALIEN_ROWS][j / ALIEN_ROWS].z + D_ALIEN/4.0;
                                break;
                        }
                        b[bi] = 1;
                        bi = (bi + 1) % N_BALAS;
                }
*/
                // TODO: colisión con tanque real, no con tanque de speisimbeyde
                for (i = 0; i < N_BALAS; i++) {
                        if (b[i] && bl[i] == level) {
                                by[i] -= delta * V_BALA_MAX;
                                if (by[i] <= -(H_TABLERO/2.0)) b[i] = 0;
                                else if (by[i] < 0.3*H_TANQUE - H_TABLERO/2.0 &&
                                         px - W_TANQUE/2.0 <= bx[i] && bx[i] <= px + W_TANQUE/2.0) {
                                        if ((--lives) == 0) lose = 1;
                                        b[i] = 0;
                                        break;
                                }
                        }
                }

                if (lose) initJuego();
        }

        if (balas == 0 && pbn == 0) initJuego();

        frame_delay = max_frame_delay - (deltas_sum / N_DELTAS);

        if ((cosa = (cosa + 1) % COSA) == 0) glutPostRedisplay();
        glutTimerFunc((frame_delay > 0) ? frame_delay / COSA : 0, juego, 0);
}

void initGL() {
        glEnable(GL_CULL_FACE);
        glFrontFace(GL_CCW);
        glEnable(GL_NORMALIZE);
        glEnable(GL_DEPTH_TEST);
//      glShadeModel(GL_SMOOTH);
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glDepthFunc(GL_LEQUAL);

        glEnable(GL_LINE_SMOOTH);
        glHint(GL_LINE_SMOOTH_HINT,GL_NICEST);
        glEnable(GL_POINT_SMOOTH);
        glHint(GL_POINT_SMOOTH_HINT,GL_NICEST);
        glEnable(GL_POLYGON_SMOOTH);
        glHint(GL_POLYGON_SMOOTH_HINT,GL_NICEST);
        glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);

        glLightModelfv(GL_LIGHT_MODEL_AMBIENT, black4f);
        glLightModeli(GL_LIGHT_MODEL_COLOR_CONTROL, 1);
        glLightModeli(GL_LIGHT_MODEL_LOCAL_VIEWER , 0);
        glLightModeli(GL_LIGHT_MODEL_TWO_SIDE     , 1);

        glLightfv(GL_LIGHT0, GL_AMBIENT , black4f);
        glLightfv(GL_LIGHT0, GL_DIFFUSE , white4f);
        glLightfv(GL_LIGHT0, GL_SPECULAR, black4f);
        glLightf(GL_LIGHT0, GL_SPOT_CUTOFF          , 180);
        glLightf(GL_LIGHT0, GL_SPOT_EXPONENT        ,   0);
        glLightf(GL_LIGHT0, GL_CONSTANT_ATTENUATION ,   1);
        glLightf(GL_LIGHT0, GL_LINEAR_ATTENUATION   ,   0);
        glLightf(GL_LIGHT0, GL_QUADRATIC_ATTENUATION,   0);
        glEnable(GL_LIGHT0);

        glLightfv(GL_LIGHT1, GL_AMBIENT , black4f      );
        glLightfv(GL_LIGHT1, GL_SPECULAR, black4f      );
        glLightfv(GL_LIGHT1, GL_DIFFUSE , light_diffuse);
        glLightf(GL_LIGHT1, GL_SPOT_CUTOFF          , 90);
        glLightf(GL_LIGHT1, GL_SPOT_EXPONENT        , 20);

        glLightfv(GL_LIGHT2, GL_AMBIENT , black4f      );
        glLightfv(GL_LIGHT2, GL_SPECULAR, black4f      );
        glLightfv(GL_LIGHT2, GL_DIFFUSE , light_diffuse);
        glLightf(GL_LIGHT2, GL_SPOT_CUTOFF          , 90);
        glLightf(GL_LIGHT2, GL_SPOT_EXPONENT        , 20);

        for (i = MIN_BALA_LIGHT; i < 8; i++) {
                glLightfv(GL_LIGHT0 + i, GL_AMBIENT , black4f           );
                glLightfv(GL_LIGHT0 + i, GL_DIFFUSE , bala_light_diffuse);
                glLightfv(GL_LIGHT0 + i, GL_SPECULAR, bala_light_diffuse);
//              glLightf(GL_LIGHT0 + i, GL_SPOT_CUTOFF          , 30);
//              glLightf(GL_LIGHT0 + i, GL_SPOT_EXPONENT        ,  2);
                glLightf(GL_LIGHT0 + i, GL_CONSTANT_ATTENUATION ,  0);
                glLightf(GL_LIGHT0 + i, GL_LINEAR_ATTENUATION   ,  1);
                glLightf(GL_LIGHT0 + i, GL_QUADRATIC_ATTENUATION,  2);
        }

        glEnable(GL_COLOR_MATERIAL);
        glMateriali(GL_FRONT_AND_BACK, GL_SHININESS, 1);
        glColorMaterial(GL_FRONT_AND_BACK, GL_EMISSION);
        glColor4ub(0, 0, 0, 255);
        glColorMaterial(GL_FRONT_AND_BACK, GL_SPECULAR);
        glColor4ub(255, 255, 255, 255);
        glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);

/*
        GLfloat fog_color[4] = {0.8, 0.8, 0.8, 1.0};
        glEnable(GL_FOG);
        glFogi(GL_FOG_MODE, GL_EXP2);
        glFogi(GL_FOG_COORD_SRC, GL_FRAGMENT_DEPTH);
        glFogf(GL_FOG_START, 1.0);
        glFogf(GL_FOG_END, 30.0);
        glFogf(GL_FOG_DENSITY, 0.05);
        glFogfv(GL_FOG_COLOR, fog_color);
*/

        buildLists();
}

int main(int argc, char **argv) {
#ifdef AUDIO
        {
                Uint16 audio_format = AUDIO_S16;
                int audio_rate = 22050;
                int audio_channels = 2;
                int audio_buffers = 4096;
                SDL_Init(SDL_INIT_AUDIO);
                if (Mix_OpenAudio(audio_rate, audio_format, audio_channels, audio_buffers)) {
                        printf("Unable to open audio!\n");
                        exit(1);
                }
        }
        phaser   = Mix_LoadWAV("music/phaser.wav"  );
        motorL   = Mix_LoadWAV("music/motorL.wav"  );
        motorC   = Mix_LoadWAV("music/motorC.wav"  );
        //buho   = Mix_LoadWAV("music/zero.wav"    );
        moveL    = Mix_LoadWAV("music/moveL.wav"   );
        moveC    = Mix_LoadWAV("music/moveC.wav"   );
        reverseL = Mix_LoadWAV("music/reverseL.wav");
        reverseC = Mix_LoadWAV("music/reverseC.wav");
        fireL    = Mix_LoadWAV("music/fireL.wav"   );
        fireC    = Mix_LoadWAV("music/fireC.wav"   );
        turretL  = Mix_LoadWAV("music/turretL.wav" );
        turretC  = Mix_LoadWAV("music/turretC.wav" );
        exploL   = Mix_LoadWAV("music/exploL.wav"  );
        exploC   = Mix_LoadWAV("music/exploC.wav"  );
#endif
        glutInit(&argc, argv);
        glutInitWindowSize(ww, wh);
        glutInitDisplayMode(GLUT_RGBA      |
                            GLUT_DOUBLE    |
                            GLUT_ALPHA     |
                            GLUT_DEPTH     |
                            GLUT_LUMINANCE |
                            GLUT_MULTISAMPLE);
        glutCreateWindow("Tanques");
        glutIgnoreKeyRepeat(1);
        blur = false;

        {
                struct timeval t;

                if (gettimeofday(&t, NULL) == -1) {
                        perror("gettimeofday");
                        exit(EX_OSERR);
                }
                srand(t.tv_usec);
        }

        initGL();
        initJuego();

        glutDisplayFunc(realDisplay);
        glutReshapeFunc(reshape);
        glutKeyboardFunc(keydown);
        glutKeyboardUpFunc(keyup);
        glutSpecialFunc(skeydown);
        glutSpecialUpFunc(skeyup);
        max_frame_delay = 1000.0/60.0;
        for (i = 0; i < N_DELTAS; i++) deltas[i] = 0;
        glutTimerFunc(max_frame_delay, juego, 0);
        old_time = glutGet(GLUT_ELAPSED_TIME);
        glutMainLoop();

        return EX_OK;
}

