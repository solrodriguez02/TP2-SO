// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include <stdint.h>
#include <clock.h>

#define SECONDS 0x00
#define MINUTES 0x02
#define HOURS 0x04
#define DAYS 0x07
#define WEEKDAY 0x06
#define MONTH 0x08
#define YEAR 0x09

#define TIME_ZONE -3

/**
 * @brief Vector de cadenas de caracteres que contiene los días de la semana empezando por domingo.
 */
const char * daysOfTheWeek[7] = {"Sunday","Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};

/**
 * @brief Vector de cadenas de caracteres que contiene los meses del año.
 */
const char * months[12] = {"January", "February", "March", "April", "May", "June", "July", "August", "September", "October", "November", "December"};

/**
 * @brief Vector sobre el cual se modifica la fecha actual del sistema en la función rtc_handler.
 */
char date[MAX_DATE_LENGTH];

/**
 * @brief Handler para el módulo de Real Time Clock del sistema. Escribe en pantalla la fecha actual.
 */
void rtc_handler() {
    fulldate(date);
    drawDate(date);
}

/**
 * @brief Decodifica el valor del tiempo obtenido del módulo de RTC.
 * 
 * @param time Valor leído del mapa de entrada y salida que se desea decodificar.
 * @return Valor decodificado.
 */
unsigned int decode(unsigned char time){
    return (time >> 4) * 10 + (time & 0x0F);
}

/**
 * @brief Retorna los segundos actuales del sistema.
 * 
 * @return Segundos actuales del sistema.
 */
unsigned int seconds(){
    return decode(getTime(SECONDS));
}

/**
 * @brief Retorna los minutos actuales del sistema.
 * 
 * @return Minutos actuales del sistema.
 */
unsigned int minutes(){
    return decode(getTime(MINUTES));
}

/**
 * @brief Retorna los minutos actuales dependiendo de la TIME ZONE deseada.
 * 
 * @return Minutos actuales dependiendo de la TIME ZONE.
 */
unsigned int hours(){
    unsigned int hours=decode(getTime(HOURS));
    /* Analizo el caso si TIME_ZONE ES POSITIVA Y NEGATIVA*/
    if (TIME_ZONE>=0) {
        /* verifico que no me paso de 24 horas*/
        if (hours+TIME_ZONE>=24){
            return hours+TIME_ZONE-24;
        } else {
            return hours+TIME_ZONE;
        }
    } else {
        /* verifico que la resta no sea negativa*/
        if ((signed char)hours+TIME_ZONE<0){
            return hours+TIME_ZONE+24;
        } else {
            return hours+TIME_ZONE;
        }
    }
}

/**
 * @brief Retorna el valor numerico del día actual dependiendo de la TIME ZONE deseada.
 * 
 * @return Día actual dependiendo de la TIME ZONE.
 */
unsigned int day(){
    return decode(getTime(DAYS));
}

/**
 * @brief Retona el valor numérico del día actual de la semana, empezando en domingo con el valor 1.
 * 
 * @return Día actual de la semana.
 */
unsigned int weekday(){
    return decode(getTime(WEEKDAY));
}

/**
 * @brief Retorna el valor numérico del mes actual dependiendo de la TIME ZONE deseada.
 * 
 * @return Mes actual dependiendo de la TIME ZONE deseada.
 */
unsigned int month(){
    return decode(getTime(MONTH));
}

/**
 * @brief Retorna el valor numérico de las últimas dos cifras del año actual dependiendo de la TIME ZONE deseada.
 * 
 * @return Año actual dependiendo de la TIME ZONE deseada.z
 */
unsigned int year(){
    return decode(getTime(YEAR));
}

/**
 * @brief Convierte sobre el puntero proporcionado como parámetro la hora, minutos y segundos actuales
 * dependiendo de la TIME ZONE deseada.
 * 
 * @param dest Cadena de caracteres sobre la cual se escribe la hora, minutos y segundos actuales. 
 * Tiene longitud fija.
 */
void timeToStr(char * dest){
    uint64_t h=hours(),m=minutes(),s=seconds();
    dest[0]= (h/10)%10 + '0';
    dest[1]= h%10 + '0';
    dest[2]= dest[5]=':';
    dest[3]= (m/10)%10 + '0';
    dest[4]= m%10 + '0';
    dest[6]= (s/10)%10 + '0';
    dest[7]= s%10 + '0';
    dest[8] = '\0';
}

/**
 * @brief Convierte sobre el puntero proporcionado como parámetro el día, mes y año actuales dependiendo
 * de la TIME ZONE deseada.
 * 
 * @param dest Cadena de caracteres sobre la cual se escribe la hora, minutos y segundos actuales.
 * Tiene longitud fija.
 */
void dateToStr(char * dest){
    uint64_t d=day(),m=month(),y=year();
    dest[0]= (d/10)%10 + '0';
    dest[1]= d%10 + '0';
    dest[2]='/';
    dest[3]= (m/10)%10 + '0';
    dest[4]= m%10 + '0';
    dest[5]='/';
    dest[6]= (y/10)%10 + '0';
    dest[7]= y%10 + '0';
    dest[8] = '\0';
}

/**
 * @brief Convierte sobre el puntero proporcionado como parámetro el día de la semana, el mes, el día y el año.
 * 
 * @param dateStr Cadena de caracteres sobre la cual se escribe el día de la semana, el mes, el día y el año.
 */
void fulldate(char * dateStr){
    uint64_t d=day(), wd=weekday(), m=month(), y=year();
    const char * weekdayStr = daysOfTheWeek[wd-1];
    const char * monthStr = months[m-1];
    uint32_t dateStrIndex = 0;
    uint32_t eachIndex=0;
    // weekday
    for(eachIndex=0;weekdayStr[eachIndex];eachIndex++,dateStrIndex++) {
        dateStr[dateStrIndex]=weekdayStr[eachIndex];
    }
    dateStr[dateStrIndex++] = ' ';   // add the blank
    // month
    for (eachIndex=0;monthStr[eachIndex];eachIndex++,dateStrIndex++) {
        dateStr[dateStrIndex] = monthStr[eachIndex];
    }
    dateStr[dateStrIndex++] = ' ';
    // monthday
    dateStr[dateStrIndex++] = (d/10)%10 + '0';
    dateStr[dateStrIndex++] = d%10 + '0';
    // ", "
    dateStr[dateStrIndex++] = ',';
    dateStr[dateStrIndex++] = ' ';
    // year
    dateStr[dateStrIndex++] = '2';
    dateStr[dateStrIndex++] = '0';
    dateStr[dateStrIndex++] = (y/10)%10 + '0';
    dateStr[dateStrIndex++] = y%10 + '0';
    dateStr[dateStrIndex] = '\0';
}


