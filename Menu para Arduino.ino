#include <LiquidCrystal.h>

// Nome amigavel para o pino dos botoes
#define BOTOES_PIN       A0 

// Defines para descobrir as chaves
#define LIMITE_RIGHT    20    // padrao = 0
#define LIMITE_UP       180   // padrao = 131
#define LIMITE_DOWN     370   // padrao = 308
#define LIMITE_LEFT     550   // padrao = 482
#define LIMITE_SELECT   800   // padrao = 724

byte coracao[8]={ B00000, B01010, B11111, B11111, B11111, B01110, B00100, B00000 }; //CODIGO PARA O CARACTER CORAÇÃO
byte seta[8]={ B00000, B00100, B00010, B11111, B11111, B00010, B00100, B00000 };    //CODIGO PARA O CARACTER SETA
int linha[3] = {0,1,2};                                                             //VETOR DAS LINHAS USADO NO MENU PRINCIPAL
int num_menu = 1;                                                                   //DEFINE QUAL O MENU APRESENTADO NO DISPLAY
int botao_atual;                                                                    //USADO PARA DEFINIR QUAL O BOTÃO PRESSIONADO ATUALMENTE
int botao_pressionado;                                                              //USADO PARA DEFINIR O BOTÃO ANTERIOR
int index;                                                                          //VARIAVEL PARA CONTROLE DO MOVIMENTO DE LISTA DO MENU
int linha_bk[3];                                                                    //BACKUP DA LINHA DO MENU
bool menu_principal = HIGH;                                                         //VARIAVEL PARA ATIVA/DESATIVAR MENU PRINCIPAL (INICIA ATIVADA)
float analogA2 = A2;                                                                //VARIAVEL DO PINO A2
int digital3 = 3;                                                                   //VARIAVEL DO PINO DIGITAL 3
float pin_ldr = A1;                                                                 //VARIAVEL DO PINO A1
float valor_sense;                                                                  //VALOR USADO PARA SENSIBILIDADE DO CIRCUITO A LUZ
float tensao_a2;                                                                    //ARMAZENA A TENSAO NO PINO A2
float tensao_a1;                                                                    //ARMAZENA A TENSAO NO PINO A1
bool estado_led = LOW;                                                              //DIZ SE O LED ESTA LIGADO OU NÃO (INICIA DESLIGADO)
String led;                                                                         //VARIAVEL 'ESTADO_LED' TRADUZIDA PARA ON/OFF

LiquidCrystal lcd(8, 9, 4, 5, 6, 7);

void setup() {

  //DEFINE ENTRADAS E SAIDAS
  pinMode (digital3, OUTPUT);
  pinMode (analogA2, INPUT);
  pinMode (pin_ldr, INPUT);
  pinMode(BOTOES_PIN, INPUT);

  //INICIA DISPLAY
  lcd.begin(16, 4);
  delay(30);                        // Saindo de reset, esperar o display ficar pronto (> 10 ms)
  // clear screen for the next loop:
  lcd.clear();
  delay(3);                        // Depois de limpar o LCD esperar ficar pronto (> 1.6 ms)

  Serial.begin(9600);             //INICIA A PORTA SERIAL

  
  botao_pressionado = analogRead (BOTOES_PIN);
  valor_sense = 2.00;          //Definindo um valor de tensão para ativar o LED

  //CRIANDO CARACTERES ESPECIAIS
  lcd.createChar(1, coracao);
  lcd.createChar(2, seta);

  //ESCREVENDO UMA SETA
  lcd.setCursor(0,0);
  lcd.write(2);
}

void loop() {

  //Conversão da leitura dos pinos analógicos para tensão
  tensao_a1 = (analogRead(pin_ldr) * 5.0) / 1023;
  tensao_a2 = (5 - (analogRead(analogA2) * 5.0) / 1023);

  //Teste para ativação do LED
  if(tensao_a1 <= valor_sense)      //SE TENSAO NO LDR FOR MENOR QUE A SENSIBILIDADE DEFINIDA ATIVA O LED
    { 
      digitalWrite(digital3, HIGH);
      estado_led = HIGH;
    }
    else                            //SENÃO DESLIGA
     {
      digitalWrite(digital3, LOW);
      estado_led = LOW;
     }
     
  //Nova conversão da leitura dos pinos analógicos para tensão, com intuito de atualizar valores para teste de segurança
  tensao_a1 = (analogRead(pin_ldr) * 5.0) / 1023;
  tensao_a2 = (5 - (analogRead(analogA2) * 5.0) / 1023);

  //TESTES DE SEGURANÇA DO CIRCUITO
  //USADO 'WHILE' PARA NÃO PERMITIR A CONTINUIDADE DO LOOP
  while(tensao_a2 <= 0.5 && estado_led ==  HIGH)  
    {
      lcd.setCursor(1, 0);
      lcd.print("LED QUEIMADO!!!");
      lcd.setCursor(0, 1);
      lcd.print("                ");
      tensao_a2 = (5 - (analogRead(analogA2) * 5.0) / 1023);
    }

   while(tensao_a2 >= 4.5 && estado_led == HIGH)
     {
       lcd.setCursor(1, 0);
       lcd.print("LED EM CURTO!!!");
       tensao_a2 = (5 - (analogRead(analogA2) * 5.0) / 1023);
     }

  //TRADUZ A VARIAVEL BOOLEANA PARA ESCRITA 'ON/OFF'
  if(estado_led == LOW)
   {
     led = "OFF";  
   }
  else
   {
     led = "ON "; 
   }

  //Lendo pino A0/botões
  botao_atual = analogRead (BOTOES_PIN);
   
  //ESCREVE O MENU PRINCIPAL NO DISPLAY
  if(menu_principal == HIGH)
  {
    lcd.setCursor(1,linha[0]);
    lcd.print("INFO CIRCUITO     ");
    delay(6);
    lcd.setCursor(1,linha[1]);
    lcd.print("SENSE LDR             ");
    delay(6);
    lcd.setCursor(1,linha[2]);
    lcd.print("CREDITOS          ");
    delay(6);
  }

  control_menu();           //CHAMA A FUNÇÃO RESPONSÁVEL PELA FUNÇÃO DOS BOTÕES

  //ESCREVE OS SUBMENUS QUANDO PEDIDO
  if(menu_principal == LOW)  //SE O MENU PRINCIPAL ESTIVER DESLIGADO, EXECUTAR
  {
    switch(num_menu)
      {
                    //SUBMENU DE INFORMAÇÕES DE TENSÃO NO LDR E LED(PONTO X) E DIZER ESTADO DO LED
                    case 1:
                            lcd.setCursor(1, 0);
                            lcd.print("LED   LDR>    V");
                            lcd.setCursor(10, 0);
                            lcd.write(2);
                            lcd.setCursor(11, 0);
                            lcd.print(tensao_a1);
                            lcd.setCursor(1, 1);
                            lcd.print("      LED>    V");
                            lcd.setCursor(10, 1);
                            lcd.write(2);
                            lcd.setCursor(1, 1);
                            lcd.print(led);
                            lcd.setCursor(11, 1);
                            lcd.print(tensao_a2);
                            break;

                    //SUBMENU RESPONSAVEL POR ALTERAR A SENSIBILIDADE DE LUZ QUE O CIRCUITO IRÁ TRABALHAR
                    case 2:
                            lcd.setCursor(1, 0);
                            lcd.print("SENSE:    V   +");
                            lcd.setCursor(7, 0);  
                            lcd.print(valor_sense);
                            lcd.setCursor(1, 1);
                            lcd.print("              -");
                            break;

                    //SUBMENU PARA APRESENTAÇÃO DO PROGRAMADOR        
                    case 3:
                            lcd.setCursor(1, 0);
                            lcd.print("SEU NOME");
                            lcd.setCursor(1, 1);
                            lcd.print("ANO/DATA/OUTRA INFORMAÇÃO");
                            lcd.setCursor(8, 1);
                            lcd.write(1);
                            break;
      }
  }
  delay(30); //DELAY PARA EXECUÇÃO DO PROXIMO LOOP
}

//FUNÇÃO RESPONSAVEL PELO CONTROLE DOS BOTÕES
void control_menu(){

  if( botao_atual == botao_pressionado)   //SE NÃO HOUVE MUDANÇA DE ESTADO DO PINO
  {                                   //A0, ENTÃO RETORNA
    return;
  }  
  else                                //SENÃO, CONTINUA
    botao_pressionado = botao_atual;

    delay(50);
    
  // Houve mudanca de botao, entao mostra;
  if(botao_atual <= LIMITE_RIGHT){
    Serial.print("Descoberto Botao RIGHT:  ");      //BOTÃO RIGHT NÃO TEM FUNÇÃO PRATICA NO CODIGO
    Serial.println(botao_atual);
    return;
  }
  
  if(botao_atual <= LIMITE_UP){
    if(menu_principal == HIGH)
      {                                            //BOTÃO UP SOBE A LISTA DO MENU OU 
        control_list_up();                         //AUMENTA EM 0.5V A SENSIBILIDADE 
        num_menu--;                                //DE LUZ DO CIRCUITO
          if (num_menu <= 0)
           {
             num_menu = 3;
           }
      }

     if(menu_principal == LOW && num_menu == 2)
     {                                                 
        valor_sense = valor_sense + 0.5;
        if(valor_sense > 5.0)
         {
          valor_sense = 2.0;
         }
     }
    
    return;
  }
  
  if(botao_atual <= LIMITE_DOWN){
    if(menu_principal == HIGH)
      {                                     //BOTÃO DOWN DESCE A LISTA DO MENU OU
        control_list_down();                //DIMINUI EM 0.5V A SENSIBILIDADE DE LUZ
        num_menu++;                         //DO CIRCUITO
        if (num_menu > 3)
          {
            num_menu = 1;
          }
      }

    if(menu_principal == LOW && num_menu == 2)
     {
        valor_sense = valor_sense - 0.5;
        if(valor_sense < 0.0)
         {
          valor_sense = 2.0;
         }
     }
    return;
  }
  
  if(botao_atual <= LIMITE_LEFT){               //BOTÃO LEFT RETORNA DO SUBMENU PARA O MENU
    menu_principal = HIGH;       //ATIVA O MENU PRINCIPAL                    
    return;
  }
  
  if(botao_atual <= LIMITE_SELECT){            //BOTÃO SELECT SELECIONA O SUBMENU
    menu_principal = LOW;          //DESATIVA O MENU PRINCIPAL
    return;
  }
}

//FUNÇÃO RESPONSAVEL PELO MOVIMENTO DE SUBIDA DO MENU
void control_list_up(){
                                                              //REORGANIZA A LISTA DO MENU CONFORME PRESSIONADO O BOTÃO 'UP'
  for(int i=0; i<3; i++) 
  {
     index = i+1;
     linha_bk[i] = linha[i];
     
     if(index > 2)
     {
      linha[i] = linha_bk[i-2];
     }
     
     else 
     {
      linha[i] = linha[i+1];
     }
  }
}

//FUNÇÃO RESPONSAVEL PELO MOVIMENTO DE DESCIDA DO MENU
void control_list_down(){
                                                              //REORGANIZA A LISTA DO MENU CONFORME PRESSIONADO O BOTÃO 'DOWN'
  for(int i=2; i>-1; i--) 
  {
     index = i-1;
     linha_bk[i] = linha[i];
     
     if(index < 0)
     {
      linha[i] = linha_bk[i+2];
     }
     
     else 
     {
      linha[i] = linha[i-1];
     }
  }
}
