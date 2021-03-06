#include "gps.h"                                                  //Incluye la cabecera del archivo GPS
char GPSGSM::getBattInf(char *str_perc, char *str_vol)           // Configuración de sistemas de coordenadas del Shield
{
     char ret_val=0;
     char *p_char;
     char *p_char1;

     gsm.SimpleWriteln("AT+CBC");
     gsm.WaitResp(5000, 100, "OK");
     if(gsm.IsStringReceived("+CBC"))
          ret_val=1;

     //BCL
     p_char = strchr((char *)(gsm.comm_buf),',');
     p_char1 = p_char+1;                                              // Estamos en el primer char de BCS 
     p_char = strchr((char *)(p_char1), ',');
     if (p_char != NULL) {
          *p_char = 0;
     }
     strcpy(str_perc, (char *)(p_char1));

     //Voltage
     p_char++;
     p_char1 = strchr((char *)(p_char), '\r');
     if (p_char1 != NULL) {
          *p_char1 = 0;
     }
     strcpy(str_vol, (char *)(p_char));
     return ret_val;
}

char GPSGSM::getBattTVol(char *str_vol)
{
     char *p_char;
     char *p_char1;
     char ret_val=0;

     gsm.SimpleWriteln("AT+CBTE?");
     gsm.WaitResp(5000, 100, "OK");
     if(gsm.IsStringReceived("+CBTE"))
          ret_val=1;

     //BCL
     p_char = strchr((char *)(gsm.comm_buf),':');
     p_char1 = p_char+2;                                              // Estamos en el primer char de BCS 
     p_char = strchr((char *)(p_char1), '\r');
     if (p_char != NULL) {
          *p_char = 0;
     }
     strcpy(str_vol, (char *)(p_char1));
     return ret_val;
}

char GPSGSM::attachGPS()
{
     if(AT_RESP_ERR_DIF_RESP == gsm.SendATCmdWaitResp("AT+CGPSPWR=1", 500, 100, "OK", 5))
          return 0;
     if(AT_RESP_ERR_DIF_RESP == gsm.SendATCmdWaitResp("AT+CGPSRST=1", 500, 100, "OK", 5))
          return 0;
     return 1;
}

char GPSGSM::deattachGPS()
{
     if(AT_RESP_ERR_DIF_RESP == gsm.SendATCmdWaitResp("AT+CGPSPWR=0", 500, 100, "OK", 5))
          return 0;
     return 1;
}

char GPSGSM::getStat()
{
     char ret_val=-1;
     gsm.SimpleWriteln("AT+CGPSSTATUS?");
     gsm.WaitResp(5000, 100, "OK");
     if(gsm.IsStringReceived("Unknown")||gsm.IsStringReceived("unknown"))
          ret_val=0;
     else if(gsm.IsStringReceived("Not"))
          ret_val=1;
     else if(gsm.IsStringReceived("2D")||gsm.IsStringReceived("2d"))
          ret_val=2;
     else if(gsm.IsStringReceived("3D")||gsm.IsStringReceived("3d"))
          ret_val=3;
     return ret_val;
}

char GPSGSM::getPar(char *str_long, char *str_lat, char *str_alt, char *str_time, char *str_speed)
{
     char ret_val=0;
     char *p_char;
     char *p_char1;
     gsm.SimpleWriteln("AT+CGPSINF=0");
     gsm.WaitResp(5000, 100, "OK");
     if(gsm.IsStringReceived("OK"))
          ret_val=1;

     //Longitud
     p_char = strchr((char *)(gsm.comm_buf),',');
     p_char1 = p_char+1;  //we are on the first char of longitude
     p_char = strchr((char *)(p_char1), ',');
     if (p_char != NULL) {
          *p_char = 0;
     }
     strcpy(str_long, (char *)(p_char1));

     //Latitud
     p_char++;
     p_char1 = strchr((char *)(p_char), ',');
     if (p_char1 != NULL) {
          *p_char1 = 0;
     }
     strcpy(str_lat, (char *)(p_char));

     //Altitud
     p_char1++;
     p_char = strchr((char *)(p_char1), ',');
     if (p_char != NULL) {
          *p_char = 0;
     }
     strcpy(str_alt, (char *)(p_char1));

     // Configuración del UTC (Tiempo Universal Coordinado
     p_char++;
     p_char1 = strchr((char *)(p_char), ',');
     if (p_char1 != NULL) {
          *p_char1 = 0;
     }
     strcpy(str_time, (char *)(p_char));

     //-Configuración del Time To First Fix lo que conocidamente se dice "Tiempo para la primera posición".
     //TTFF es la cantidad de tiempo requerido para que el receptor GPS adquiera las señales de los satélites 
     //y datos de navegación necesarios para calcular una posición particular.
     p_char1++;
     p_char = strchr((char *)(p_char1), ',');
     if (p_char != NULL) {
          *p_char = 0;
     }

     // num
     p_char++;
     p_char1 = strchr((char *)(p_char), ',');
     if (p_char1 != NULL) {
          *p_char1 = 0;
     }

     // speed
     p_char1++;
     p_char = strchr((char *)(p_char1), ',');
     if (p_char != NULL) {
          *p_char = 0;
     }
     strcpy(str_speed, (char *)(p_char1));

     return ret_val;
}

void parseTime(char *field, int *time)
{
     //-TIEMPO-
     char tmp[4];
     tmp[2]=0;                                                        // Inicia la temporización y la terminación nula
     tmp[0] = field[8];
     tmp[1] = field[9];
     time[0] = atoi(tmp);                                             //HORAS
     tmp[0] = field[10];
     tmp[1] = field[11];
     time[1] = atoi(tmp);                                             //MINUTOS
     tmp[0] = field[12];
     tmp[1] = field[13];
     time[2] = atoi(tmp);                                             //SEGUNDOS
     //-FECHA-
     tmp[0] = field[0];
     tmp[1] = field[1];
     tmp[2] = field[2];
     tmp[3] = field[3];
     tmp[4]=0;                                                        //INICIA TEMPORIZACION CON TERMINACIÓN NULA (SISTEMA DE RELOJ)
     time[3] = atoi(tmp);                                             //DETERMINACIÓN DEL AÑO
     tmp[0] = field[4];
     tmp[1] = field[5];
     tmp[2]=0;                                                        //INICIA TEMPORIZACION CON TERMINACIÓN NULA (SISTEMA DE RELOJ)    
     time[4] = atoi(tmp);                                             //DETERMINACIÓN DEL MES
     tmp[0] = field[6];
     tmp[1] = field[7];
     tmp[2]=0;                                                        //INICIA TEMPORIZACION CON TERMINACIÓN NULA (SISTEMA DE RELOJ)
     time[5] = atoi(tmp);                                             //DÍA
}

// Lee la latitud en formato decimal de una cadena GGA
double convertLat(char* latString)
{
     double latitude = atof(latString);                               // Convierte a una variable de doble (precisión)
     int deg = (int) latitude / 100;                                  // Extrae el numero de grados 
     double min = latitude - (100 * deg);                             // Clcula el número de minutos
     latitude = deg + (double) min/60.0;                              // Convierte a formato decimal
     return latitude;
}

// Lee la longitud en formato decimal de una cadena GGA          
double convertLong(char* longString)
{
     double longitude = atof(longString);                              // Convierte a una variable de doble (precisión)
     int deg = (int) longitude / 100;                                  // Extrae el numero de grados 
     double min = longitude - (100 * deg);                             // Clcula el número de minutos
     longitude = deg + (double) min/60.00;                             // Convierte a formato decimal
     return longitude;
}
