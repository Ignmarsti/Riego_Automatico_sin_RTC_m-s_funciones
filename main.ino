
#include <Keypad.h>
#include <SPI.h>
#include <Wire.h>//Librería necesaria para la comunicación I2C conel chip de la pantalla(Reloj)
#include <Adafruit_GFX.h>//Librería gráfica para la pantalla OLED
#include <Adafruit_SSD1306.h>//Librería necesaria para laspantallas OLED
#include "RTClib.h"
//#include "SDHT.h"

// Definimos constantes
#define ANCHO_PANTALLA 128 // ancho pantalla OLED. Valor típico 128
#define ALTO_PANTALLA 64 // alto pantalla OLED. Valor típico son 64 y 32
#define pinsensorT 2////Definimos el puerto donde conectaremos el sensor de temperatura
#define TipoSensor DHT11/////Definimos el tipo de sensor que es
#define ldr_pin 16

const byte filas =4;
const byte columnas=4;
char teclas [filas][columnas] ={
  {'1', '2', '3', 'A'},
  {'4', '5', '6', 'B'},
  {'7', '8', '9', 'C'},
  {'*', '0', '#', 'D'}
  };
byte pinesFilas [filas] = {11, 10, 9, 8};//Definimos los pines correspondientes a las filas del teclado matricial
byte pinesColumnas [columnas] = {7, 6, 5, 4};//Definimos los pines correspondientes a las filas del teclado matricial
char tecla_pulsada;


byte ano,mes,dia,hora,minuto,segundo;
char fecha[] = ("  :  :  ");
char calendario[] = ("  /  /20  ");

//float t;
unsigned int ldr=0;
const long A = 1000;     //Resistencia en oscuridad en KΩ
const int B = 15;        //Resistencia a la luz (10 Lux) en KΩ
const int Rc = 10;       //Resistencia calibracion en KΩ
int V;
int ilum;

char horariego1[6];
char horariego2[6];
char horariego3[6];

int horariego1_entero[3];
int horariego2_entero[3];
int horariego2_entero[3];

int i=0;
int tecla_numerica;
int tecla_anterior;
char opcion_seleccionada;
bool bandera_seleccion=0;
bool bandera_riego=0;

int horariego1_0;
int horariego1_1;
int horariego1_3;
int horariego1_4;
int horariego2_0;
int horariego2_1;
int horariego2_3;
int horariego2_4;
int horariego3_0;
int horariego3_1;
int horariego3_3;
int horariego3_4;


void menu_principal();
void opcion_a();
void opcion_b();
void opcion_c();
void error_numerico();
void recuperar_fecha();
void ver_fecha();
void opcion_sensado();
void escribir_texto();
void tecla_no_numerica();
 
// Objeto de la clase Adafruit_SSD1306 y del RTC
Adafruit_SSD1306 display(ANCHO_PANTALLA, ALTO_PANTALLA, &Wire, -1);//&Wire es un puntero de la clase estática Wire. -1 es el pin de Arduino o ESP8266 que se utiliza para resetear la pantalla en caso de que la pantalla tenga un pin RST (no es nuestro caso)
Keypad teclado = Keypad(makeKeymap(teclas), pinesFilas, pinesColumnas, filas, columnas); ///Crea el mapa del teclado 
RTC_DS1307 rtc;
DateTime hoy;
//SDHT dht;
 
void setup() {
  
  
  bandera_seleccion=0;//Bajamos la bandera de selección
  bandera_riego=0;
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);//Iniciamos la pantalla OLED que se encuentra en la dirección 0x3C
  Wire.begin();
  rtc.begin();
  pinMode(LED_BUILTIN, OUTPUT);
  

  rtc.adjust(DateTime(__DATE__, __TIME__));///ESTA LINEA SE TIENE QUE COMENTAR NADA MAS SE CARGUE UNA VEZ, Y VOLVER A CARGAR EL SKECTH PARA QUE DE ESTA MANERA SOLO SE PONGA EN HORA UNA VEZ A TRAVÉS DEL ORDENADOR
  //rtc.adjust(DateTime(2021, 02, 17, 19, 44, 00));


  //menu_principal();

 
}
 
void loop() {

    if(menu){
        menu_principal();
    }
    
//    dht.read(TipoSensor, pinsensorT);
    
    V = analogRead(ldr_pin); 

    hora_de_regar();
    recuperar_fecha();

    //t = dht.readTemperature();////En grados celsius por defecto
    hoy = rtc.now();///Recuperamos la fecha actual
    tecla_pulsada = teclado.getKey();//Comprobamos que tecla se ha pulsado
    opcion_seleccionada=tecla_pulsada;//La guardamos en una nueva variable
    //Tenemos varias opciones
    if(opcion_seleccionada == 'A'){
      //opcion_a();
      //bandera_seleccion=1;
      programar_hora(horariego1_entero);///Llamamos a la función de programar las horas
      opcion_seleccionada = '\0';////Borramos la variable guardada en opcion_seleccionada
      error=false;
    }
    if(opcion_seleccionada == 'B'){
      //opcion_b();
      //bandera_seleccion=1;
      programar_hora(horariego2_entero);
      opcion_seleccionada = '\0';
      error = false;
    }
    if(opcion_seleccionada == 'C'){
      //opcion_c();
      //bandera_seleccion=1;
      programar_hora(horariego3_entero);
      opcion_seleccionada = '\0';
      error = false;
    }
    if(opcion_seleccionada== 'D'){
      //bandera_seleccion=1;
      opcion_seleccionada = '\0';
      ver_fecha();
    }
    if(opcion_seleccionada== '#'){
      //bandera_seleccion=1;
      opcion_seleccionada = '\0';
      opcion_sensado();
    }



}

//Aquí tenemos las distintas funciones en las que podemos programar la hora de riego
/*void opcion_a(){
    if(bandera_seleccion==1){


    //Limpiamos el display, situamos el cursor en la posición adecuada e imprimimos por pantalla el mensaje
    display.clearDisplay();
    escribir_texto(0,0, "Opcion A seleccionada", 1);


    //Creamos un bucle en el que cada vez que recibimos una tecla, la guardamos en el vector corresponiente
    for(i=0;i<6;i++){
      tecla_pulsada=teclado.waitForKey();////Esta función es importante, ya que hasta que no pulsemos una tecla, el micro se queda parado en está instrucción. La unica manera de avanzar es pulsar una tecla o salir de bucle for
      if(tecla_pulsada=='A'||tecla_pulsada=='B'||tecla_pulsada=='C'||tecla_pulsada=='D'||tecla_pulsada=='*'||tecla_pulsada=='#'){
        display.clearDisplay();
        
        tecla_no_numerica();

        delay(2000);
        bandera_seleccion=0;
        menu_principal();
        break;
      }
      else{
        horariego1[i]=tecla_pulsada;
        tecla_numerica=horariego1[i]-48;
        if(i==0&&tecla_numerica>2){          
          error_numerico();
          menu_principal();
          break;
        }
        if(i==0&&tecla_numerica==2){
          tecla_anterior=tecla_numerica;
        }
        if(i==1&&tecla_anterior==2&&tecla_numerica>4){
          error_numerico();
          menu_principal();
          break;
        }
        if(i==2&&tecla_numerica>5){
          error_numerico();
          menu_principal();
          break;
        }
        if(i==4&&tecla_numerica>5){
          error_numerico();
          menu_principal();
          break;
        }
        
        if(i==5){//Si ya hemos terminado de meter la hora completa, esta ocupará 6 espacios, por lo que una vez estén rellenos imprimiremos por pantalla lo que tenemos en el vector
          display.clearDisplay();
          
          escribir_texto(3,0, " Se ha programado la hora de riego a las ", 1);
          
          display.write(horariego1[0]);
          display.write(horariego1[1]);
          display.print(':');
          display.write(horariego1[2]);
          display.write(horariego1[3]);
          display.print(':');
          display.write(horariego1[4]);
          display.write(horariego1[5]); 
          display.display();
          delay(5000);
          display.clearDisplay();
          menu_principal();
          break;
         }
    }
    }
    }
}

//Aquí tenemos las distintas funciones en las que podemos programar la hora de riego
void opcion_b(){
    if(bandera_seleccion==1){
    bandera_seleccion=0;//Lo primero que hacemos es bajar la bandera de seleccion

    //Limpiamos el display, situamos el cursor en la posición adecuada e imprimimos por pantalla el mensaje
    display.clearDisplay();
    
    escribir_texto(0, 0, "Opcion B seleccionada", 1);

    display.display();

    //Creamos un bucle en el que cada vez que recibimos una tecla, la guardamos en el vector corresponiente
    for(i=0;i<6;i++){
      tecla_pulsada=teclado.waitForKey();////Esta función es importante, ya que hasta que no pulsemos una tecla, el micro se queda parado en está instrucción. La unica manera de avanzar es pulsar una tecla o salir de bucle for
      if(tecla_pulsada=='A'||tecla_pulsada=='B'||tecla_pulsada=='C'||tecla_pulsada=='D'||tecla_pulsada=='*'||tecla_pulsada=='#'){
        display.clearDisplay();
        
        tecla_no_numerica();
        
        delay(2000);
        menu_principal();
        break;
      }
      else{
        horariego1[i]=tecla_pulsada;
        tecla_numerica=horariego1[i]-48;
        if(i==0&&tecla_numerica>2){          
          error_numerico();
          menu_principal();
          break;
        }
        if(i==0&&tecla_numerica==2){
          tecla_anterior=tecla_numerica;
        }
        if(i==1&&tecla_anterior==2&&tecla_numerica>4){
          error_numerico();
          menu_principal();
          break;
        }
        if(i==2&&tecla_numerica>5){
          error_numerico();
          menu_principal();
          break;
        }
        if(i==4&&tecla_numerica>5){
          error_numerico();
          menu_principal();
          break;
        }
      horariego2[i]=tecla_pulsada;
      if(i==5){//Si ya hemos terminado de meter la hora completa, esta ocupará 6 espacios, por lo que una vez estén rellenos imprimiremos por pantalla lo que tenemos en el vector
        display.clearDisplay();
          
        escribir_texto(0, 0, "Se ha programado la hora de riego a las ", 1);
        display.write(horariego2[0]);
        display.write(horariego2[1]);
        display.print(':');
        display.write(horariego2[2]);
        display.write(horariego2[3]);
        display.print(':');
        display.write(horariego2[4]);
        display.write(horariego2[5]); 
        display.display();
        delay(5000);
        display.clearDisplay();
        menu_principal();
        break;
      }
    }
    

    }
}
}

//Aquí tenemos las distintas funciones en las que podemos programar la hora de riego
void opcion_c(){
    if(bandera_seleccion==1){
    bandera_seleccion=0;//Lo primero que hacemos es bajar la bandera de seleccion

    //Limpiamos el display, situamos el cursor en la posición adecuada e imprimimos por pantalla el mensaje
    display.clearDisplay();
    
    escribir_texto(0, 0, "Opcion C seleccionada", 1);

    //Creamos un bucle en el que cada vez que recibimos una tecla, la guardamos en el vector corresponiente
    for(i=0;i<6;i++){
      tecla_pulsada=teclado.waitForKey();////Esta función es importante, ya que hasta que no pulsemos una tecla, el micro se queda parado en está instrucción. La unica manera de avanzar es pulsar una tecla o salir de bucle for
      if(tecla_pulsada=='A'||tecla_pulsada=='B'||tecla_pulsada=='C'||tecla_pulsada=='D'||tecla_pulsada=='*'||tecla_pulsada=='#'){
        display.clearDisplay();
        
        tecla_no_numerica();
        
        delay(2000);
        menu_principal();
        break;
      }
      else{
        horariego3[i]=tecla_pulsada;
        tecla_numerica=horariego3[i]-48;
        if(i==0&&tecla_numerica>2){          
          error_numerico();
          menu_principal();
          break;
        }
        if(i==0&&tecla_numerica==2){
          tecla_anterior=tecla_numerica;
        }
        if(i==1&&tecla_anterior==2&&tecla_numerica>4){
          error_numerico();
          menu_principal();
          break;
        }
        if(i==2&&tecla_numerica>5){
          error_numerico();
          menu_principal();
          break;
        }
        if(i==4&&tecla_numerica>5){
          error_numerico();
          menu_principal();
          break;
        }
      if(i==5){//Si ya hemos terminado de meter la hora completa, esta ocupará 6 espacios, por lo que una vez estén rellenos imprimiremos por pantalla lo que tenemos en el vector
        display.clearDisplay(); 
        
        escribir_texto(0, 0, "Se ha programado la hora de riego a las ", 1);
        
        display.write(horariego3[0]);
        display.write(horariego3[1]);
        display.print(':');
        display.write(horariego3[2]);
        display.write(horariego3[3]);
        display.print(':');
        display.write(horariego3[4]);
        display.write(horariego3[5]);
        display.display();
        delay(5000);
        display.clearDisplay();
        menu_principal();
        break;
      }
      
    }
    

    }
}
}*/

void menu_principal(){

  i=0;
 
    // Limpiar buffer
  display.clearDisplay();
 
  // Color del texto
  display.setTextColor(SSD1306_WHITE);

  //Escribimos el texto en la posición indicada y con el tamaño indicado
  escribir_texto(40, 16, "Menu", 2);
  escribir_texto(10, 32, "Principal", 2);
  
  delay(2000);
  
  // Limpiamos la pantalla, cambiamos el tamaño del texto y resituamos el cursor para escribir el nuevo mensaje. Luego lo mandamos
  display.clearDisplay();
  escribir_texto(0, 0, "Pulse A, B y C para  cambiar las horas de riego 1, 2 y 3 respectivamente", 1);
}


void error_numerico(){
  display.clearDisplay();
  escribir_texto(0, 0, "La hora introducida no es valida", 1);
  delay(2000);
}

void recuperar_fecha(){
  
  hoy = rtc.now();///Recuperamos la fecha actual
  
  ano= hoy.year() % 100;// Eliminamos el siglo y queda el año con dos dígitos
  
  mes=hoy.month();

  dia=hoy.day();
 
  hora=hoy.hour();
  
  minuto=hoy.minute();
  
  segundo=hoy.second();
  
  
  fecha[7] = segundo % 10 + 48;
  fecha[6] = segundo / 10 + 48;
  fecha[4] = minuto % 10 + 48;
  fecha[3] = minuto / 10 + 48;
  fecha[1] = hora % 10 + 48;
  fecha[0] = hora / 10 + 48;

  
  calendario[9] = ano % 10 + 48;
  calendario[8] = ano / 10 + 48;
  calendario[4] = mes % 10 + 48;
  calendario[3] = mes / 10 + 48;
  calendario[1] = dia % 10 + 48;
  calendario[0] = dia / 10 + 48;
}

void ver_fecha(){
    
  display.clearDisplay();
  escribir_texto(0, 0, fecha, 1);
  escribir_texto(60, 0, calendario, 1);
  escribir_texto(0,30, "Hora:",1);
  display.print(horariego1_0);
  display.print(horariego1_1);
  display.print(':');
  display.print(horariego1_3);
  display.print((minuto % 10)+1);
  display.display();
  delay(2000);
  menu_principal();
  
}

void hora_de_regar(){
  ///A pesar de que cuando introducimos manualmente la hora de riego se piden la hora, los minutos y los segundos, en esta parte del código nos fijaremos solo en la hora y los minutos
  ///De esta manera si por casualidad estamos tocando el keypad justo a la hora de riego y nos encontramos con un delay, no perderemos el riego, si no que regaremos una vez pase el delay
  ///Y levantaremos una bandera para que solo se riegue una sola vez.
  
  horariego1_0 = horariego1[0] - '0';
  horariego1_1 = horariego1[1] - '0';
  horariego1_3 = horariego1[2] - '0';
  horariego1_4 = horariego1[3] - '0';
  horariego2_0 = horariego2[0] - '0';
  horariego2_1 = horariego2[1] - '0';
  horariego2_3 = horariego2[2] - '0';
  horariego2_4 = horariego2[3] - '0';
  horariego3_0 = horariego3[0] - '0';
  horariego3_1 = horariego3[1] - '0';
  horariego3_3 = horariego3[2] - '0';
  horariego3_4 = horariego3[3] - '0';
  

  if((horariego1_0==hora/10 && horariego1_1==hora%10 && horariego1_3==minuto/10 && horariego1_4==minuto%10)&&bandera_riego==0){
    /////ENCENDEMOS LA BOMBA DURANTE 10 segundos////////
    bandera_riego=1;
    digitalWrite(LED_BUILTIN, HIGH);   // turn the LED on (HIGH is the voltage level)
    delay(5000);                       // wait for a second
    digitalWrite(LED_BUILTIN, LOW);    // turn the LED off by making the voltage LOW
    delay(5000);
  }

//////////-----------------------------AQUI TENDRA QUE IR ALGUN METODO QUE BAJE LA BANDERA CUANDO HAYA PASADO UN MINUTO DESDE LA HORA DE RIEGO PARA NO ESTAR REGANDO EL MINUTO ENTERO-----------////////////
/////////------------------------------DEBERIA INTENTAR USAR LA FUNCION MILLIS PARA ESTO Y PARA EL RESTO DE TAREAS-------------------------////////////////

  if((horariego2_0==hora/10 && horariego2_1==hora%10 && horariego2_3==minuto/10 && horariego2_4==minuto%10)&&bandera_riego==0){
    /////ENCENDEMOS LA BOMBA DURANTE 10 segundos////////
  }
  if((horariego3_0==hora/10 && horariego3_1==hora%10 && horariego3_3==minuto/10 && horariego3_4==minuto%10)&&bandera_riego==0){
    /////ENCENDEMOS LA BOMBA DURANTE 10 segundos////////
  }
  
  
}
void opcion_sensado(){
  if(bandera_seleccion==1){

      /*float humedad=dht.humidity;
      int hum=humedad*10;
      float temperatura=dht.celsius;
      int temp=temperatura*10;
      
      
      
      escribir_texto(0, 0, "Temperatura:", 1);
      display.print(temp/100);
      //display.print(",");
      //display.print(temp%100);
      display.display();
      
      /*escribir_texto(0, 15, "Humedad:", 1);
      display.print(hum/100);
      display.print(",");
      display.println(hum%100);
      display.display();*/

      display.clearDisplay();

      ilum = ((long)V*A*10)/((long)B*Rc*(1024-V));
      
      escribir_texto(0, 30, "La cantidad de luz es: ", 1);
      display.print(ilum);////Cuando la luz es muy brillante, entonces la resistencia de la fotocélula es muy baja en comparación con la resistencia de valor fijo
                          ////Cuando la fotocélula está en una luz apagada, la resistencia es mayor que la resistencia fija de 1 kΩ
      display.display();
      
      delay(2000);
      bandera_seleccion=0;
      menu_principal();
  }
}


void escribir_texto(byte x_pos, byte y_pos, char *text, byte text_size){
  display.setCursor(x_pos, y_pos);
  display.setTextSize(text_size);
  display.print(text);
  display.display();
}

void tecla_no_numerica(){
  escribir_texto(0, 0, "ERROR", 1);
  escribir_texto(0, 10, "La tecla pulsada no es numerica", 1);

}

void programar_horarios(int horario[3]){//Recibiremos 3 numeros enteros
    int horario_anterior[3] = {horario[0], horario[1], horario[2]};
    display.clearDisplay();
    display.clearDisplay();
    display.setTextSize(1);
    display.setCursor(0, 0);
    display.print("Opcion ");
    display.print(opcion_seleccionada);
    display.println(" seleccionada");
    display.display();
    programar_hora(horario);
    if(error){
        horario[0] = horario_anterior[0];
        horario[1] = horario_anterior[1];
        horario[2] = horario_anterior[2];
    }
    String Hora_final = "";
    
    ///////------------------ME FALTA AQUÍ RECONSTRUIR LA HORA FINAL---------/////

    display.clearDisplay();
    escribir_texto(15, 0, "Se ha programado", 1);
    escribir_texto(9, 9, "la hora de riego: ", 1);
    escribir_texto(40, 18, Hora_final, 1);
    delay(2000);
    display.clearDisplay();
    display.display();
    menu = true;
}

void programar_hora(int horario_temporal[3]){
    String Hora = reunir_informacion(24, 0, 9, ":");///Recolectamos la información acerca de la hora
    if(Hora="invalido"){
        Invalido();
        error = true;
        menu = true;
    }
    String Minutos = reunir_informacion(60, 21, 9, ":");//Recolectamos información acerca de los minutos
    if(Minutos = "invalido"){
        Invalido();
        error = true;
        menu = true;
    }
    String Segundos = reunir_informacion(60, 42, 9, ":");//Recolectamos inforamción acerca de los segundos
    if(Segundos = "invalido"){
        Invalido();
        error = true;
        menu = true;
    }
    horario_temporal[0] = Hora.toInt();
    horario_temporal[1] = Minutos.toInt();
    horario_temporal[2] = Segundos.toInt();

}

String reunir_informacion(int limite, int coordenada_x, int coordenada_y, String separador){
    String buf;
    char T_pulsada = '\0';
    for (int j=0; j<2; j++){
        T_pulsada = teclado.waitForKey();
        if(isDigit(T_pulsada) && T_pulsada != '*' && T_pulsada != '#'){///Comprobamos que se haya introducido carácteres válidos
            escribir_texto(coordenada_x, coordenada_y, String (T_pulsada), 1);
            buf +=T_pulsada;
            coordenada_x += 7;///Para que no se vaya pisando lo que escribimos por pantalla.
        }
        else{
            j=2;//No recolectamos más datos
            return "invalido";///Indicamos que no es válido lo introducido
        }
    }
    int numero_completo = buf.toInt();///Convertimos el numero almacenado como string en int
    if(numero>=limite){///Comprobamos que el numero introducido no sea mayor que el límite
        return "invalido";
    }
    else{
        escribir_texto(coordenada_x, coordenada_y, separador, 1);///Escribimos el separador :
        return buf;
    }
}

void Invalido(){
    display.clearDisplay();
    escribir_texto(40, 0, "INVALIDO", 1);
    delay(1000);
}