//0u-105ADC-0.5us
//1u-ADC-1.2us
//2u-ADC-1.5us
//3u-ADC-2.0us 
//675ADC - threshold
//12VDC-2.29ms
//t(us)=ADC*0.002713+0.17

#define tspark vmax*0.002713+0.17

#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Fonts/FreeSerif9pt7b.h>

const int ADCthr=675; //threshold spark time >2ms
const int SCREEN_WIDTH=128; // OLED display width, in pixels
const int SCREEN_HEIGHT=64; // OLED display height, in pixels
const int first_string=12;  //first string on LCD
const int second_string=28;  //second string on LCD
const int third_string=44;  //third string on LCD
const int fourth_string=62;  //fourth string on LCD

const int adc_pin=14; //A0// ADC0 - T-measuring
const int relay_pin=2; //relay for discharge input cap
const int buzzer_pin=9; //relay for discharge input cap
const int input_cap_pin=15; //A1 //ADC1 - input cap dicharge control
const int vthr=50; //v threshold //50-def
const int vrearmthr=15; //v extra rearm threshold //15-def


int i=0; //output iterator
int iextra=0; //extra threshold iterator
int v=0; //v read from ADC
int vmax=0; //measured v max

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1); // Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)

void rearm(){
             
  
  while(analogRead(input_cap_pin)>vrearmthr){    
    digitalWrite(relay_pin,1); 
    display.fillRect(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, BLACK);
    display.setCursor(0,first_string); 
    Serial.println("REARM...");
    display.print("   REARM..."); 
    display.display(); 
    delay(300);
    display.fillRect(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, BLACK);
    display.display(); 
    delay(100);
  }
  delay(1000);
  display.fillRect(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, BLACK);
  digitalWrite(relay_pin,0);
  display.setCursor(0,first_string);
  display.println("   READY !");
  display.println("TOUCH THE");
  display.println("     BPIS !");        
  display.display();  
  delay(100);
}


void setup() {
  // put your setup code here, to run once:

ADCSRA |= (1 << ADPS2);                     //Биту ADPS2 присваиваем единицу - коэффициент деления 16 //ADC speed up accuracy down
ADCSRA &= ~ ((1 << ADPS1) | (1 << ADPS0));  //Битам ADPS1 и ADPS0 присваиваем нули  //ADC speed up accuracy down

if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { 
  Serial.println("SSD1306 allocation failed");
  for(;;);
}
display.setFont(&FreeSerif9pt7b);
display.setTextSize(1);             
display.setTextColor(WHITE); 

display.fillRect(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, BLACK);
display.setCursor(0,first_string);
display.println("     BPIS");
display.println("SPARK DELAY");
display.println("   TESTER");        
display.display();  
delay(1500);


  
pinMode(adc_pin, INPUT);  
pinMode(input_cap_pin, INPUT); 
pinMode(relay_pin, OUTPUT);  
analogReference(DEFAULT);
v=analogRead(adc_pin); //init
analogRead(input_cap_pin); //init
Serial.begin(9600);
digitalWrite(relay_pin,0);
delay(100);
rearm();
delay(50);
}

void loop() {
  // put your main code here, to run repeatedly:


v=analogRead(adc_pin);

if(v>vthr){
  if(v>vmax){
    vmax=v;
  }
}

if( (vmax>vthr) and (v<vthr) ){
  Serial.println(vmax);
  display.fillRect(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, BLACK);
  display.setCursor(0,first_string);                 
  if(vmax<=ADCthr){
    display.print("T=");
    display.print(tspark);   
    display.print(" us");
  } else display.print("   T>2 us !!!");   
  
  display.display();
  tone(buzzer_pin,4000,200);
  delay(5000);   
  vmax=0;
  rearm();   
}

iextra++;
if(iextra>30000){  
  if((vmax<vthr)and(v<vthr)){
    if(analogRead(input_cap_pin)>vrearmthr)rearm(); //extra rearm
  }
  iextra=0;
}

}
