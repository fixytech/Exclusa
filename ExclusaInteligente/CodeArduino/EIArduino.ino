/*
    Name:       EIArduino.ino
    Created:	13/7/2021 13:03:07
    Author:     Ezequiel Liptai
*/


// SALIDAS
int bloqueaPuerta = 6;
int enmascaraPersianaInt = 7;
int enmascaraPersianaExt = 8;
int salidaAlarma = 9;

// ENTRADAS
int magPercianaInt = 2;
int magPercianaExt = 3;
int magPuerta = 4;
int pir = 5;

int ultimoIngreso = 1; // 2 - puerta | 1 - perciana

int estadoPir;
int estadoMagPercianaInt;
int estadoMagPercianaExt;
int estadoMagPuerta;

boolean inicio = true;
boolean mov = false;
int direccion = 0;


int espera = 4000;
unsigned long contador = 0;



String estadoGeneral = "INGRESOS HABILITADOS";


// The setup() function runs once each time the micro-controller starts
void setup()
{

    // SALIDAS
    pinMode(bloqueaPuerta, OUTPUT);
    pinMode(enmascaraPersianaInt, OUTPUT);
    pinMode(enmascaraPersianaExt, OUTPUT);
    pinMode(salidaAlarma, OUTPUT);

    // ENTRADAS
    pinMode(estadoPir, INPUT);
    pinMode(estadoMagPercianaInt, INPUT);
    pinMode(estadoMagPercianaExt, INPUT);
    pinMode(estadoMagPuerta, INPUT);

    Serial.begin(9600);

    

}


// Add the main program code into the continuous loop() function
void loop()
{

    funcInicio(); // se ejecuta si la variable de inicio es TRUE.
    




}


void funcInicio() {

    if (inicio) {

        verificaEstados(); // Verifico sensores DS de persiana y puerta

        // ASIGNO SALIDAS
        asignaSalidas(0,1,0,0);

        consolaPrint("$INICIO&");

        // SI LA PERSIANA EXT ESTA ABIERTA.
        if (!estadoMagPercianaExt) {

            consolaPrint("$PERSIANA EXTERIOR - ABIERTA&");
           

            inicio = false; // Salgo del inicio
            direccion = 1;

            funPersianaExt(); // Ejecuto proceso de persiana externa
            

        }

        // SI SE ABRE LA PUERTA
        if (!estadoMagPuerta) {

            consolaPrint("$PUERTA - ABIERTA&");

            inicio = false; // Salgo del inicio
            direccion = 1;

            funcPuerta(); // Ejecuto proceso de persiana externa
        }

        // SI DETECTA MOVIMIENTO
        if (estadoPir) {
            consolaPrint("$ALARMA!&");
            inicio = false;
            funcPir();
        }
    }
}

// FUNCION PIR
void funcPir() {
    if (ultimoIngreso == 1) {
        funPersinanaInt();
    }
    if (ultimoIngreso == 2) {
        direccion = 2;
        funcPuerta();
    }
    inicio = true;
    loop();
}
// FUNCION DE PUERTA
void funcPuerta() {

    verificaEstados(); // Verifico sensores DS de persiana y puerta

       // ASIGNO SALIDAS
    asignaSalidas(0, 1, 1, 0);

    if (direccion == 1) {
        ultimoIngreso = 2;
        // ESPERO CIERRE DE PUERTA
        while (!estadoMagPuerta)
        {
            verificaEstados(); // Verifico sensores DS de persiana y puerta
            consolaPrint("$ESPERANDO CIERRE PUERTA&");
        }

        // ESTAN ADENTRO

        // ESPERO LA SALIDA

        while (estadoMagPuerta)
        {
            verificaEstados(); // Verifico sensores DS de persiana y puerta
            consolaPrint("$ESPERANDO APERTURA DE PUERTA&");
        }

        while (!estadoMagPuerta)
        {
            verificaEstados(); // Verifico sensores DS de persiana y puerta
            consolaPrint("$ESPERANDO CIERRE DE PUERTA&");
        }

        direccion = 0;
        inicio = true;
        

    }

    if (direccion == 2) {
        // ESPERO LA SALIDA

        while (estadoMagPuerta)
        {
            verificaEstados(); // Verifico sensores DS de persiana y puerta
            consolaPrint("$ESPERANDO APERTURA DE PUERTA&");
        }

        while (!estadoMagPuerta)
        {
            verificaEstados(); // Verifico sensores DS de persiana y puerta
            consolaPrint("$ESPERANDO CIERRE DE PUERTA&");
        }

        direccion = 0;
        inicio = true;
 
    }
 


}
//IMPRIME MENSAJES EN CONSOLA
void consolaPrint(String mensaje) {
    Serial.println(mensaje);
    delay(100);
}
// FUNCION PERSIANA EXTERIOR
void funPersianaExt() {

    delay(100);

    consolaPrint("$FUNCION PERSIANA EXTERIOR&");

    // ASIGNO SALIDAS
    asignaSalidas(1, 1, 0, 0);

    // PROCESO DE INGRESO
    if (direccion == 1) {

        verificaEstados();

        //ESPERA CIERRE PERSIANA EXTERIOR

        while (estadoMagPercianaExt == LOW)
        {
            verificaEstados();
            consolaPrint("$ESPERANDO CIERRE DE PERSIANA EXTERIOR&");

            // Verifico movimiento
            if (ultimoIngreso > 0 && estadoPir) {
                
                // aviso que hubo movimiento
                consolaPrint("$ATENCION: DISPARO ALARMA MOVIMIENTO&");
                disparoAlarma();
                mov = true;
            }
        }

        // si hubo movimiento mientras se bajaba la persiana
        if (mov) {

            // Asigno nuevamente la variable en false.
            mov = false;

            // Consulo ultimo ingreso, si fue por persiana...

            if (ultimoIngreso == 1) {

                // Solicito que se habra la de exterior y salga el que entro.
                direccion = 2;

                funPersianaExt();
                
            }
            if (ultimoIngreso == 2) {
                direccion = 2;
                funcPuerta();
            }
        }

        funPersinanaInt();
    }

    // PROCESO DE EGRESO
     if (direccion == 2) {

         verificaEstados();

        // ESPERA APERTURA EXTERIOR
        while (estadoMagPercianaExt)
        {
            verificaEstados();

            consolaPrint("$ESPERANDO APERTURA DE PERSIANA EXTERIOR&");

            if (estadoPir) {
                
              
                consolaPrint("$ATENCION: DISPARO ALARMA MOVIMIENTO&");
                disparoAlarma();
                funPersinanaInt();
                return;
            }
        }


        // ESPERA CIERRE EXTERIOR
        while (!estadoMagPercianaExt)
        {
            verificaEstados();
            consolaPrint("$ESPERANDO CIERRE DE PERSIANA EXTERIOR&");

            if (estadoPir) {
                mov = true;

                consolaPrint("$ATENCION: ALARMA MOVIMIENTO&");
               
                disparoAlarma();
                return;
            }
        }

        // SI HUBO MIVIMIENTO...


        consolaPrint("$CICLO COMPLETO. VOLVIENDO AL INICIO...&");
        direccion = 0;
        inicio = true;
        loop();

    }

}
// FUNCION PERSIANA INTERIOR
void funPersinanaInt() {

    consolaPrint("$FUNCION PERSIANA INTERIOR&");

    // ASIGNO SALIDAS
    asignaSalidas(1, 1, 1, 0);

    verificaEstados();

    // VIRIFICO QUE LA PERSIANA EXTERIOR ESTE CERRADA, SI ESTA ABIERTA DISPARO ALARMA
    // Y ESPERO EL CIERRE
    if (!estadoMagPercianaExt ) {

        consolaPrint("$ATENCION: PERSIANA EXTERIOR ABIERTA&");
        
        disparoAlarma();
        
        // ESPERO CIERRE PERSIANA EXTERIOR
        while (!estadoMagPercianaExt)
        {
            verificaEstados();

            consolaPrint("$ATENCION: PERSIANA EXTERIOR ABIERTA&");
        }
    }

    // VIRIFICO QUE LA PUERTA ESTE CERRADA, SI ESTA ABIERTA DISPARO ALARMA
    // Y ESPERO EL CIERRE
    if (!estadoMagPuerta) {

        consolaPrint("$ATENCION: PUERTA ABIERTA&");

        disparoAlarma();

        // ESPERO CIERRE DE PUERTA
        while (!estadoMagPuerta)
        {
            verificaEstados();
            consolaPrint("$ATENCION: PUERTA ABIERTA&");
        }
    }

    verificaEstados();

    // ESPERO APERTURA PERCIANA INTERIOR
    // ASIGNO SALIDAS
    asignaSalidas(1, 0, 1, 0);

    while (estadoMagPercianaInt)
    {

        verificaEstados();
        asignaSalidas(1, 0, 1, 0);

        consolaPrint("$ESPERANDO APERTURA DE PERSIANA INTERIOR (257)&");

        if (estadoPir) {
            direccion = 2;
            disparoAlarma();
            if(ultimoIngreso == 2){
              funcPuerta();
            }
            
        }
    }

    // ESPERO CIERRE PERSIANA INTERIOR
    while (!estadoMagPercianaInt)
    {
        verificaEstados();

        consolaPrint("$ESPERANDO CIERRE DE PERSIANA INTERIOR (263)&");
    }

    ultimoIngreso = 1;
    direccion = 2;

    funPersianaExt();
}
// DISPARO DE ALARMA
void disparoAlarma() {
    Serial.println("ALARMA!");
    digitalWrite(salidaAlarma, HIGH);
    delay(1000);
    digitalWrite(salidaAlarma, LOW);
    return;
}
// VERIFICA LOS ESTADOS DE LAS ENTRADAS
void verificaEstados() {

    estadoMagPuerta = digitalRead(magPuerta);
    estadoMagPercianaInt = digitalRead(magPercianaInt);
    estadoMagPercianaExt = digitalRead(magPercianaExt);
    estadoPir = digitalRead(pir);

    consolaPrint("P" + String(estadoMagPuerta) + 
        ",I" + String(estadoMagPercianaInt) +
    ",E" + String(estadoMagPercianaExt) +
    ",M" + String(estadoPir) +
    ",U" + String(ultimoIngreso) + ",");

}
// ACTIVA - DESACTIVA SALIDAS
void asignaSalidas(int sPta, int spI, int spE, int sA) {


    // Invierto valores por que el modulo de RELE trabaja a la inversa
    if (sPta == 1) {
        sPta = 0;
    }
    else {
        sPta = 1;
    }

    if (spI == 1) {
        spI = 0;
    }
    else {
        spI = 1;
    }

    if (spE == 1) {
        spE = 0;
    }
    else {
        spE = 1;
    }

    if (sA == 1) {
        sA = 0;
    }
    else {
        sA = 1;
    }
    
    // ENMARSCARAMIENTOS Y BLOQUEOS
    digitalWrite(bloqueaPuerta, sPta); // 
    digitalWrite(enmascaraPersianaInt, spI); // Activo Rele
    digitalWrite(enmascaraPersianaExt, spE); // 
    digitalWrite(salidaAlarma, sA);
}
