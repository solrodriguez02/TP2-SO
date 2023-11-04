#include <stdint.h>
#include <lib.h>

extern char buffer;


/**
 * @brief Copia un caracter las veces deseadas sobre un puntero dado
 * 
 * @param destination Puntero donde se desea copiar el caracter
 * @param c Caracter que se desea copiar sobre el puntero
 * @param length Veces que se desea copiar el caracter sobre el puntero
 * @return El puntero reemplazado
 */
void * memset(void * destination, int32_t c, uint64_t length) {
	uint8_t chr = (uint8_t)c;
	char * dst = (char*)destination;

	while(length--)
		dst[length] = chr;

	return destination;
}

/**
 * @brief Copia la cantidad de caracteres deseados desde un puntero fuente sobre un puntero destino.
 * 
 * @param destination 
 * @param source 
 * @param length 
 * @return void* 
 */
void * memcpy(void * destination, const void * source, uint64_t length) {
	/*
	* memcpy does not support overlapping buffers, so always do it
	* forwards. (Don't change this without adjusting memmove.)
	*
	* For speedy copying, optimize the common case where both pointers
	* and the length are word-aligned, and copy word-at-a-time instead
	* of byte-at-a-time. Otherwise, copy by bytes.
	*
	* The alignment logic below should be portable. We rely on
	* the compiler to be reasonably intelligent about optimizing
	* the divides and modulos out. Fortunately, it is.
	*/
	uint64_t i;

	if ((uint64_t)destination % sizeof(uint32_t) == 0 &&
		(uint64_t)source % sizeof(uint32_t) == 0 &&
		length % sizeof(uint32_t) == 0)
	{
		uint32_t *d = (uint32_t *) destination;
		const uint32_t *s = (const uint32_t *)source;

		for (i = 0; i < length / sizeof(uint32_t); i++)
			d[i] = s[i];
	}
	else
	{
		uint8_t * d = (uint8_t*)destination;
		const uint8_t * s = (const uint8_t*)source;

		for (i = 0; i < length; i++)
			d[i] = s[i];
	}

	return destination;
}

/**
 * @brief Convierte un número de hasta 8 bytes en su representación ASCII en la base deseada
 * sobre una cadena de caracteres proporcionado. Acepta valores negativos.
 * 
 * @param num Número que se desea representar. Bases soportadas: decimal y hexadecimal.
 * @param base Base a convertir el número proporcionado
 * @param buffer Cadena de caracteres ya realocada para sobrescribir sobre ella la representación del número
 * en caracteres ASCII.
 */
void numToStr(int64_t num, int base, char * buffer) {
    /* calculo cuantos dígitos tiene */
    /* BASE != 0 (hay que verificar?)*/
    int i=0;
    int isPositive = num >= 0;
    if (!isPositive) {
        buffer++; /* reservo un lugar */
        num*=-1;
    } else if (num==0){ /* caso especial de si el número es 0*/
        buffer[0]='0';
        buffer[1]='\0';
        return;
    }
        
    uint64_t aux = num;
    while(aux!=0) {
        i++;
        aux/=base;
    }
    
    buffer[i--] = '\0';

    int mod=0;
    while(i>=0){
        mod = num%base;
        if (mod<10) {
            buffer[i] = mod + '0';
        } else {
            /* hex */
            buffer[i] = mod - 10 + 'A';
        }
        num/=base;
        i--;
    }
    if (!isPositive) {
        buffer--;
        buffer[0]='-';
    }
        
}

/**
 * @brief Compara la igualdad entre dos cadenas de caracteres. Retorna 1 si ambos son iguales, y 0 en otro caso.
 * 
 * @param s1 Primera cadena de caracteres a comparar.
 * @param s2 Segunda cadena de caracteres a comparar.
 * @return Valor booleano de si ambas cadenas son iguales en representación ASCII.
 */
int strCmp(const char* s1, const char* s2){
    int i=0;
    while(s1[i]!='\0' && s2[i]!='\0') {
        if (s1[i]!=s2[i]) {
            return 0;
        }
        i++;
    }
    if(s1[i]!='\0' || s2[i]!='\0') {
        return 0;
    }
    return 1;
}
