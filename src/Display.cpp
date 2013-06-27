//=============================================================================
// Sistema de display, fun��es relacionadas a parte gr�fica do motor.
// Marcos Medeiros
//=============================================================================
#include <allegro5/allegro.h>
#include <allegro5/allegro_ttf.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_primitives.h>
#include <iostream>
#include "Display.hpp"


namespace Display
{

//=============================================================================
// Fontes padr�es
//=============================================================================
static const int sys_font_size = 16;
static const int normal_font_size = 20;
static const int info_font_size = 14;
static const char *sys_font_face = "data/fonts/ProggyTiny.ttf";
static const char *normal_font_face = "data/fonts/ProFontWindows.ttf";
static const char *info_font_face = "data/fonts/DejaVuSans.ttf";
ALLEGRO_FONT *sys_font = NULL;
ALLEGRO_FONT *normal_font = NULL;
ALLEGRO_FONT *info_font = NULL;

//=============================================================================
// incializa��o...
//=============================================================================
static bool isInitialized = false;
ALLEGRO_DISPLAY *screen = NULL;
static int screenW = 0;
static int screenH = 0;
volatile int frameCounter = 0;
static bool debugMode = false;
static bool boxMode = false;

static bool limitFrames = true;

void toogleFrameRate()
{
    limitFrames = !limitFrames;
}
void resetFrameCounter()
{
    frameCounter = 0;
}
int getFrameCounter()
{
    return frameCounter;
}

//=============================================================================
// Inicializa nossas fontes
//=============================================================================
bool fontInit()
{
    al_init_font_addon();
    al_init_ttf_addon();

    sys_font = al_load_font(sys_font_face, sys_font_size, 0);
    if (!sys_font)
    {
        std::cerr << "N�o foi poss�vel carregar a fonte padr�o!" << std::endl;
        return false;
    }
    normal_font = al_load_font(normal_font_face, normal_font_size, 0);
    if (!normal_font)
    {
        std::cerr << "N�o foi poss�vel carregar a fonte padr�o!" << std::endl;
        return false;
    }
    info_font = al_load_font(info_font_face, info_font_size, 0);
    if (!info_font)
    {
        std::cerr << "N�o foi poss�vel carregar a fonte padr�o!" << std::endl;
        return false;
    }
    return true;
}

void drawText(int font, ALLEGRO_COLOR color, int x, int y, const char *str)
{
    ALLEGRO_FONT *fontd;
    switch (font)
    {
    case SYS_FONT:
        fontd = sys_font;
        break;
    case NORMAL_FONT:
        fontd = normal_font;
        break;
    case INFO_FONT:
        fontd = info_font;
        break;
    default:
        return;
    }
    al_draw_text(fontd, color, x, y, 0, str);
}


int textLineHeight(int font)
{
    ALLEGRO_FONT *fontd;
    switch (font)
    {
    case SYS_FONT:
        fontd = sys_font;
        break;
    case NORMAL_FONT:
        fontd = normal_font;
        break;
    case INFO_FONT:
        fontd = info_font;
        break;
    default:
        return 0;
    }
    return al_get_font_line_height(fontd);
}

int textWidth(int font, const char *str)
{
    ALLEGRO_FONT *fontd;
    switch (font)
    {
    case SYS_FONT:
        fontd = sys_font;
        break;
    case NORMAL_FONT:
        fontd = normal_font;
        break;
    case INFO_FONT:
        fontd = info_font;
        break;
    default:
        return 0;
    }
    return al_get_text_width(fontd, str);
}

void drawRectFill(int x, int y, int w, int h, ALLEGRO_COLOR color)
{
    al_draw_filled_rectangle(x, y, x+w, y+h, color);
}

void drawRect(int x, int y, int w, int h, ALLEGRO_COLOR color)
{
    al_draw_rectangle(x, y, x+w, y+h, color, 1);
}

//=============================================================================
// Inicia o video, criando a janela.
//=============================================================================
bool setVideoMode(int width, int height, int bpp)
{
    // verifica se j� n�o foi iniciado
    if (!isInitialized)
    {
        al_init();
        //al_set_new_display_flags(ALLEGRO_FULLSCREEN);
        al_set_new_display_option(ALLEGRO_VSYNC, 0, ALLEGRO_REQUIRE);
        al_init_primitives_addon();
        al_install_keyboard();
        al_install_mouse();
        // inicia as fontes
        fontInit();
        screen = al_create_display(width, height);
        if (!screen)
            return false;

        al_init_image_addon();
        screenW = width;
        screenH = height;
        //al_set_new_bitmap_flags(al_get_new_bitmap_flags() | ALLEGRO_MIN_LINEAR |ALLEGRO_MAG_LINEAR);
        al_clear_to_color(al_map_rgb(0, 0, 0));

        isInitialized = true;
    }
    return true;
}

//=============================================================================
// Mostra o quadro renderizado na tela
//=============================================================================
void showBackBuffer()
{
    al_flip_display();
}

//=============================================================================
// Termina nosso sistema de display
//=============================================================================
void destroy()
{
    if (isInitialized)
    {
        al_destroy_display(screen);
        isInitialized = false;
    }
}

//=============================================================================
// Retorna o comprimento da tela em pixels
//=============================================================================
int getWidth()
{
    if (isInitialized)
        return screenW;
    return 0;
}

//=============================================================================
// Retorna a altura da tela em pixels
//=============================================================================
int getHeight()
{
    if (isInitialized)
        return screenH;
    return 0;
}

//=============================================================================
// Apenas um wrapper para chamar a fun��o para criarmos um novo BITMAP...
// N�o � muito �til.
//=============================================================================
ALLEGRO_BITMAP *createBitmap(int w, int h)
{
    return al_create_bitmap(w, h);
}

void setBlendMode(int mode)
{
    switch (mode)
    {
        case Normal:
            al_set_blender(ALLEGRO_ADD, ALLEGRO_ONE, ALLEGRO_INVERSE_ALPHA);
            break;
        case Additive:
            al_set_blender(ALLEGRO_ADD, ALLEGRO_ONE, ALLEGRO_ONE);
            break;
    }
}

bool showDebug()
{
    return debugMode;
}

void toogleDebug()
{
    debugMode = !debugMode;
}

bool showBoxes()
{
    return boxMode;
}

void toogleBoxes()
{
    boxMode = !boxMode;
}


//=============================================================================
// Implementa��es do limitador de quadros por segundo.
//=============================================================================
static double frameRate = 60.0;
double endTime = 0.0;
double beginTime = 0.0;
double deltaTime = 0.0;
double delayTime = 0.0;
double lastTime = 0.0;

//=============================================================================
// Define a quantidade m�xima de quadros por segundo.
//=============================================================================
void setFrameRate(double rate)
{
    frameRate = rate;
}

//=============================================================================
// Inicia um novo quadro.
// Essa fun��o tem que ser chamada no in�cio de cada quadro sempre.
//=============================================================================
void begin()
{
    beginTime = al_get_time();
}

//=============================================================================
// Finaliza o quadro atual
// Essa fun��o tem que ser chamada no fim de cada quadro sempre, � aqui que
// o algoritmo calcula o tempo necess�rio de delay baseado no tempo que foi
// levado para renderizar o quadro atual com a quantidade m�xima de quadros
// por segundo;
//=============================================================================
void end()
{
    endTime = al_get_time();

    // Calculamos o tempo em ticks do frame
    deltaTime = endTime - beginTime;

    // Calcula o delay para esse frame
    double tps = 1.0 / frameRate;
    delayTime = tps - deltaTime - lastTime;
    // verifica se o delay � maior que zero, fazemos isso porque
    // o frame pode ter sido mais lento que o anterior, logo o
    // delay desse frame ter� que ser subtraido no pr�ximo frame.
    if (delayTime > 0)
    {
        if (limitFrames)
            al_rest(delayTime);
        lastTime = 0.0;
    }
    else
    {
        lastTime = delayTime;
    }
    frameCounter++;
}


}
