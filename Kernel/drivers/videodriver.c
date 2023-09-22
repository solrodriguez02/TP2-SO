#include <stdint.h>
#include <videodriver.h>
#include <fonts.h>
#include <lib.h>

#define ENTRY_SIZE 8
#define TOPBAR_HEIGHT 0x19
#define START TOPBAR_HEIGHT + 0xA
#define DEFAULT_FONTSIZE PX_24
#define SCREEN_HEIGHT 768
#define SCREEN_WIDTH 1024
#define FRAMEBUFFER_SIZE 3*SCREEN_WIDTH*SCREEN_HEIGHT

uint64_t horizontal_offset = 0x00;
uint64_t previous_horizontal[SCREEN_HEIGHT/PX_32];
uint64_t scroll = 0;
uint8_t *videoPtr;
int doubleBufferEnabled = 0;
uint8_t framebuffer_off_screen[FRAMEBUFFER_SIZE];		// Para double buffer
uint64_t vertical_offset = SCREEN_HEIGHT-DEFAULT_FONTSIZE;  //START /* Starting Y level after the Top Bar*/

struct vbe_mode_info_structure
{
	uint16_t attributes;  // deprecated, only bit 7 should be of interest to you, and it indicates the mode supports a linear frame buffer.
	uint8_t window_a;	  // deprecated
	uint8_t window_b;	  // deprecated
	uint16_t granularity; // deprecated; used while calculating bank numbers
	uint16_t window_size;
	uint16_t segment_a;
	uint16_t segment_b;
	uint32_t win_func_ptr; // deprecated; used to switch banks from protected mode without returning to real mode
	uint16_t pitch;		   // number of bytes per horizontal line
	uint16_t width;		   // width in pixels
	uint16_t height;	   // height in pixels
	uint8_t w_char;		   // unused...
	uint8_t y_char;		   // ...
	uint8_t planes;
	uint8_t bpp;   // bits per pixel in this mode
	uint8_t banks; // deprecated; total number of banks in this mode
	uint8_t memory_model;
	uint8_t bank_size; // deprecated; size of a bank, almost always 64 KB but may be 16 KB...
	uint8_t image_pages;
	uint8_t reserved0;

	uint8_t red_mask;
	uint8_t red_position;
	uint8_t green_mask;
	uint8_t green_position;
	uint8_t blue_mask;
	uint8_t blue_position;
	uint8_t reserved_mask;
	uint8_t reserved_position;
	uint8_t direct_color_attributes;

	uint32_t framebuffer; // physical address of the linear frame buffer; write here to draw to the screen
	uint32_t off_screen_mem_off;
	uint16_t off_screen_mem_size; // size of memory in the framebuffer but not being displayed on the screen
	uint8_t reserved1[206];
} __attribute__((packed));

typedef struct vbe_mode_info_structure *VBEInfoPtr;

VBEInfoPtr VBE_mode_info = (VBEInfoPtr)0x0000000000005C00;

#define LINE_SIZE 3*PX_32*VBE_mode_info->width
#define TOPBAR_SIZE 3*(TOPBAR_HEIGHT+0xA)*VBE_mode_info->width

// ----------------------------------- FONT ----------------------------------- //

/*
	Array que contiene los punteros a cada vector de bitmap-array.
*/
const unsigned char **bitpx_array[3] = {bit8px_array, bit24px_array, bit32px_array};

/*
	Index para obtener el char correspondiente en su font correspondiente.
	Valor por defecto: 24px.
*/
uint8_t fontSizeIndexer = 1;

/**
*	@brief Retorna el bitmap-array para el char correspondiente con su font correspodinete.
	@param c El caracter al cual se quiere obtener su bitmap correspondiente.
	@return Un array de caracteres que corresponden al bitmap.
*/
unsigned char *getBitmapArrayEntry(unsigned char c){return bitpx_array[fontSizeIndexer][c];}

/**
 * @brief Retorna el tamaño de la fuente utilizada
 * 
 * @return Un valor entero correspondiente al tamaño de la fuente utilizada.
 */
uint32_t getFontSize() { return DEFAULT_FONTSIZE;}

// ----------------------------------- SCREEN CONTROL ----------------------------------- //

/**
 * @brief Reemplaza el valor del puntero de pantalla actual, por si se desea utilizar el frame buffer
 * por defecto, o un framebuffer off-screen.
 * 
 * @param directly Valor booleano de si se desea utilizar el puntero de pantalla de la shell directamente.
 */
void setScreenBuffer(int directly){
    if (directly) {
		videoPtr = VBE_mode_info->framebuffer;
	} else {
		videoPtr = framebuffer_off_screen;
	}
}

/**
 * @brief Carga sobre el framebuffer por defecto, el framebuffer off-screen.
 */
void loadScreen(){ memcpy(VBE_mode_info->framebuffer,framebuffer_off_screen,FRAMEBUFFER_SIZE);}

/**
 * @brief Activa y desactiva el segundo framebuffer
 * 
 * @param enable Valor booleano de si activa el segundo frame-buffer.
 */
void enableDoubleBuffer(int enable) {
	if (enable){
		setScreenBuffer(0);
		doubleBufferEnabled = 1;
	} else {
		drawRectangle(0,0,SCREEN_HEIGHT,SCREEN_WIDTH,0);
		loadScreen();
		setScreenBuffer(1);
		doubleBufferEnabled = 0;
		drawTopLine();
	}
}

/**
 * @brief Desplaza hacia arriba cada línea si se completa la misma o si se pasa a la siguiente línea
 */
void scrollUp(){
	scroll++;
	memcpy(VBE_mode_info->framebuffer+TOPBAR_SIZE,VBE_mode_info->framebuffer+TOPBAR_SIZE+LINE_SIZE,9*VBE_mode_info->width*VBE_mode_info->height -LINE_SIZE-TOPBAR_SIZE);
}

/**
 * @brief Retorna el número de línea actual en la pantalla.
 * 
 * @return El valor de la línea vertical. 
 */
unsigned int getLine(){ return vertical_offset-START-scroll;}

/**
 * @brief Mueve el puntero de la posición horizontal y vertical hacia el comienzo de una nueva línea.
 */
void drawNextLine() {	
	previous_horizontal[getLine()] = horizontal_offset;
	// Llego a fin de pagina 
	if ( vertical_offset  > VBE_mode_info->height -PX_32 ){
		scrollUp();
		//no cambia vertical_off, me quedo en la misma linea
	} else
		vertical_offset += PX_32;

	horizontal_offset = 0x00;
}

/**
 * @brief Mueve el puntero de la posición horizontal en la pantalla, y lo reinicia en el caso de haber llegado
 * al límite de la pantalla, y desplaza una unidad la posición vertical.
 */
void moveWritePointer() {
	horizontal_offset += getFontSize() * 0.75;
	if (horizontal_offset + getFontSize() * 0.75 > VBE_mode_info->width) {
		drawNextLine();
	}
}

// ----------------------------------- DRAWABLE ----------------------------------- //
/**
 * @brief Coloca un pixel en la pantalla en la posición correspondiente con un color de la forma RGB.
 * 
 * @param r Corresponde al byte para el código de color rojo.
 * @param g Corresponde al byte para el código de color verde.
 * @param b Corresponde al byte para el código de color azul.
 * @param x Corresponde a la posición horizontal en la pantalla.
 * @param y Corresponde a la posición vertical en la pantalla.
 */
void putPixel(uint8_t r, uint8_t g, uint8_t b, uint32_t x, uint32_t y) {
	// B G R (orden)
	int offset = y * VBE_mode_info->pitch + x * (VBE_mode_info->bpp / 8);
	videoPtr[offset] = b;
	videoPtr[offset + 1] = g;
	videoPtr[offset + 2] = r;
}

/**
 * @brief Dibuja sobre la pantalla el caracter correspondiente, utilizando un color para el caracter, y
 * otro color para el fondo del caracter en la posición deseada.
 * 
 * @param c Caracter al cual se quiere dibujar.
 * @param FGColor Color del caracter deseado en el formato 0xRRGGBB, siendo RR el byte para el código de 
 * color rojo, GG el código de color verde, y BB el código de color azul.
 * @param BGColor Color de fondo del caracter deseado en el formato 0xRRGGBB, siendo RR el byte para el código de 
 * color rojo, GG el código de color verde, y BB el código de color azul.
 * @param horizontalOffset Posición horizontal donde se desea colocar el caracter.
 * @param verticalOffset Posición vertical donde se desea colocar el caracter.
 */
void drawCharAt(unsigned char c, int FGColor, int BGColor, uint64_t horizontalOffset, uint64_t verticalOffset) {
	// Enmascaramos el BGColor y el FGColor, ya que se reciben en formato RGB, y los queremos en BGR
	// Además, como necesitamos 3 bytes (24 bits), usamos una estructura de 32 bits
	uint8_t fgBGR[3] = {0x00, 0x00, 0x00};
	uint8_t bgBGR[3] = {0x00, 0x00, 0x00};

	uint32_t shiftingFGColor = FGColor;
	uint32_t shiftingBGColor = BGColor;

	uint8_t bitmask = 0b11111111;

	for (int i = 0; i < 3; i++) {
		fgBGR[i] = shiftingFGColor & bitmask;
		bgBGR[i] = shiftingBGColor & bitmask;
		shiftingBGColor = shiftingBGColor >> 8;
		shiftingFGColor = shiftingFGColor >> 8;
	}

	/* Ahora conseguimos el array del entry. */
	unsigned char *entry = getBitmapArrayEntry(c);
	uint32_t height = getFontSize();
	uint32_t width = height * 0.75;
	uint32_t rows = width / 8;

	uint8_t holdingbit = 0b00000000;
	uint8_t holdingbitrow = 0b00000000;

	uint32_t entryIndex = 0;

	uint32_t yOffset = 0;

	for (int rowNumber = 0; rowNumber <= rows; rowNumber++)
	{ // number of rows
		for (int i = 0; i < width; i++)
		{ // number of columns per row
			holdingbitrow = entry[entryIndex++];
			for (int j = yOffset; j < ENTRY_SIZE + yOffset; j++)
			{ // column per byte_entry
				holdingbit = holdingbitrow & 0b1;
				if (holdingbit)
				{
					putPixel(fgBGR[2], fgBGR[1], fgBGR[0], i + horizontalOffset, j + verticalOffset);
				}
				else
				{
					putPixel(bgBGR[2], bgBGR[1], bgBGR[0], i + horizontalOffset, j + verticalOffset);
				}
				holdingbitrow = holdingbitrow >> 1;
			}
		}
		yOffset += ENTRY_SIZE;
	}
}

/**
 * @brief Dibuja sobre la pantalla el caracter correspondiente, utilizando un color para el caracter, y
 * otro color para el fondo del caracter.
 * 
 * @param c Caracter al cual se quiere dibujar.
 * @param FGColor Color del caracter deseado en el formato 0xRRGGBB, siendo RR el byte para el código de 
 * color rojo, GG el código de color verde, y BB el código de color azul.
 * @param BGColor Color de fondo del caracter deseado en el formato 0xRRGGBB, siendo RR el byte para el código de 
 * color rojo, GG el código de color verde, y BB el código de color azul.
 */
void drawChar(unsigned char c, int FGColor, int BGColor) {
	drawCharAt(c, FGColor, BGColor, horizontal_offset, vertical_offset);
	moveWritePointer();
}

/**
 * @brief Dibuja una caracter con representación vacía, y pinta en la posición anterior del cursor con 
 * color negro. No desplaza el cursor hacia adelante.
 */
void drawBlank() {
	horizontal_offset -= getFontSize() * 0.75;
	drawCharAt(' ', 0x000000, 0x000000, horizontal_offset, vertical_offset);
}

/**
 * @brief Borra el caracter en la posición anterior del cursor, ya sea si el último caracter está en la misma
 * línea, o en la línea anterior
 */
void deleteChar() {
    uint8_t canDelete = horizontal_offset - getFontSize() * 0.75 >= 0;
    if (canDelete || vertical_offset - PX_32 >= TOPBAR_HEIGHT + 0xA) {
		if (!canDelete) {
			vertical_offset -= PX_32;
			horizontal_offset = previous_horizontal[getLine()];	
		}
		drawBlank();
	}
}

/**
 * @brief Dibuja una cadena de caracteres sobre la pantalla, con su color correspondiente.
 * 
 * @param str Cadena de caracteres que se desea dibujar sobre la pantalla.
 * @param FGColor Color del caracter deseado en el formato 0xRRGGBB, siendo RR el byte para el código de 
 * color rojo, GG el código de color verde, y BB el código de color azul.
 * @param BGColor Color de fondo del caracter deseado en el formato 0xRRGGBB, siendo RR el byte para el código de 
 * color rojo, GG el código de color verde, y BB el código de color azul. 
 */
void drawString(char *str, int FGColor, int BGColor) {
	for (int i = 0; str[i]; i++) {
		drawChar(str[i], FGColor, BGColor);
	}
}

									/* TOP BAR */
/**
 * @brief Dibuja una cadena de caracteres en la barra superior en una posición horizontal correspondiente.
 * 
 * @param str Cadena de caracteres que se desea dibujar sobre la pantalla.
 * @param topOffset Posición horizontal donde se desea dibujar la cadena de caracteres.
 * @param FGColor Color del caracter deseado en el formato 0xRRGGBB, siendo RR el byte para el código de 
 * color rojo, GG el código de color verde, y BB el código de color azul.
 * @param BGColor Color de fondo del caracter deseado en el formato 0xRRGGBB, siendo RR el byte para el código de 
 * color rojo, GG el código de color verde, y BB el código de color azul. 
 */
void drawTopString(char *str, uint64_t topOffset, int FGColor, int BGColor) {
	for (int i = 0; str[i]; i++) {
		drawCharAt(str[i], FGColor, BGColor, topOffset, 0x3);
		topOffset += DEFAULT_FONTSIZE * 0.75; /* 24px is the default*/
	}
}

/**
 * @brief Dibuja la fecha actual del sistema con el formato correspondiente sí y sólo sí 
 * el double-buffer no está activado.
 * 
 * @param date Fecha actual del sistema
 */
void drawDate(char * date){
	if (!doubleBufferEnabled)
		drawTopString(date,0,GREEN,0);	
}

/**
 * @brief Dibuja la línea que separa la barra superior, del resto de la shell.
 */
void drawTopLine() {
	int8_t color= GREEN >>4;
	drawLine(color,color,color, TOPBAR_HEIGHT+5);
}

// ----------------------------------- FIGURES ----------------------------------- //
/**
 * @brief Dibuja una línea horizontal hasta el final derecho de la pantalla en la posición vertical
 * deseada, y con el color correspondiente.
 * 
 * @param r Corresponde al byte para el código de color rojo.
 * @param g Corresponde al byte para el código de color verde.
 * @param b Corresponde al byte para el código de color azul.
 * @param y Corresponde a la posición vertical en la pantalla.
 */
void drawLine(char r, char g, char b, int y) {
	for (int i = 0; i < VBE_mode_info->width; i++) {
		putPixel(r, g, b, i, y);
	}
}

/**
 * @brief Dibuja un rectángulo con la base, altura y color deseados, y en la posición indicada.
 * 
 * @param x_left Posición horizontal izquierda donde comienza a dibujar el rectángulo.
 * @param y_top Posición vertical inferior donde se comienza a dibujar el rectángulo.
 * @param h Altura del rectángulo
 * @param w Ancho del rectángulo
 * @param color Color del rectángulo
 */
void drawRectangle(int x_left, int y_top, int h, int w, uint8_t color){
	for(int i=0; i<w; i++) {
        for(int j=0; j<h; j++) {
            putPixel(color,color,color, x_left+i, y_top+j);
        }
    }
}

/**
 * @brief Dibuja un círculo centrado en un par de coordenadas (x,y) con radio y color deseados.
 * 
 * @param x_centre Coordenada horizontal del centro del círculo
 * @param y_centre Coordenada vertical del centro del círculo
 * @param r Radio del círculo
 * @param color Color del círculo
 */
void drawCircle (int x_centre, int y_centre, int r, uint8_t color) {
    int x = r, y = 0;
    // Printing the initial point on the axes after translation
	putPixel(color, color, color, x + x_centre, y + y_centre);   
    // When radius is zero only a single point will be printed
    if (r > 0) {
		putPixel(color, color, color, x + x_centre, -y + y_centre);
		putPixel(color, color, color, -x + x_centre, y + y_centre);
		putPixel(color, color, color, y + x_centre, x + y_centre);
		putPixel(color, color, color, -y + x_centre, -x + y_centre);
    }
    // Initialising the value of P
    int P = 1 - r;
    while (x > y) {
        y++;
        // Mid-point is inside or on the perimeter
        if (P <= 0)
            P = P + 2*y + 1;  
        // Mid-point is outside the perimeter
        else {
            x--;
            P = P + 2*y - 2*x + 1;
        } 
        // All the perimeter points have already been printed
        if (x < y)
            break;
        // Printing the generated point and its reflection in the other octants after translation

		putPixel(color, color, color, x + x_centre, y + y_centre);
		putPixel(color, color, color, -x + x_centre, y + y_centre);
		putPixel(color, color, color, x + x_centre, -y + y_centre);
		putPixel(color, color, color, -x + x_centre, -y + y_centre);

        // If the generated point is on the line x = y then the perimeter points have already been printed
        if (x != y) {
			putPixel(color, color, color, y + x_centre, x + y_centre);
			putPixel(color, color, color, -y + x_centre, x + y_centre);
			putPixel(color, color, color, y + x_centre, -x + y_centre);
			putPixel(color, color, color, -y + x_centre, -x + y_centre);
        }
    }
}

// ------------------------------------------------------------------------------- //














