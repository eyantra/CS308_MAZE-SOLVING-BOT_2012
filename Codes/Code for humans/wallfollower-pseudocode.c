#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

#include "init_devices.c"
#include "lcd.c"
#include "ophead.c"

#define VELOCITY_MAX 33 //maximum velocity
#define VELOCITY_MIN 25 //minimum velocity
#define VELOCITY_LOW 0 //to set velocity to lowest
#define THRESHOLD 120 //whiteline threshold

unsigned char ADC_Conversion(unsigned char);
unsigned char ADC_Value;
unsigned char Left_white_line = 0;
unsigned char Center_white_line = 0;
unsigned char Right_white_line = 0;

int L_velocity = VELOCITY_MAX, R_velocity = VELOCITY_MAX;


void get_vector() {
Left_white_line = ADC_Conversion(3);
Center_white_line = ADC_Conversion(4);
Right_white_line = ADC_Conversion(5);
print_sensor(2,1,3); //Prints value of White Line Sensor Left
print_sensor(2,5,4); //Prints value of White Line Sensor Center
print_sensor(2,9,5); //Prints Value of White Line Sensor Right
}


void turn_bot(char ch) {
get_vector();
velocity(VELOCITY_MIN,VELOCITY_MIN);
/*
if (ch == 'l') {
velocity(VELOCITY_MAX,VELOCITY_MIN);
_delay_ms(2000);
}
else if (ch == 'r') {
velocity(VELOCITY_MIN,VELOCITY_MAX);
_delay_ms(2000);
}
*/
stop();
_delay_ms(1000);
if (ch == 'l') 
left_degrees(90); 
else 
right_degrees(90); 
/*
while (1) {
get_vector();

lcd_cursor(1,1);
lcd_string("LorR           ");
L_velocity = VELOCITY_MIN; 
R_velocity = VELOCITY_MIN;
if (Center_white_line > THRESHOLD) {
if (Left_white_line > THRESHOLD)
L_velocity = VELOCITY_MIN;
else if (Right_white_line > THRESHOLD)
R_velocity = VELOCITY_MIN;
}
else {
if (Left_white_line > THRESHOLD)
L_velocity = 20;
else if (Right_white_line > THRESHOLD)
R_velocity = 20;
}
velocity(L_velocity, R_velocity);
if (Center_white_line > THRESHOLD  && Left_white_line < THRESHOLD && Right_white_line < THRESHOLD) {
break;
}
}
*/
} 

int if_intersection () {
get_vector();
if (Center_white_line > THRESHOLD && Left_white_line > THRESHOLD && Right_white_line > THRESHOLD) {
forward();
//velocity(VELOCITY_MIN, VELOCITY_MIN);
lcd_cursor(1,1);
lcd_string("Intersection");
stop();
return 1;

}
return 0;
}

void black_line() {
L_velocity = VELOCITY_MAX; 
R_velocity = VELOCITY_MAX;
get_vector();
lcd_cursor(1,1);
lcd_string("Straight");
forward();
if (Center_white_line > THRESHOLD) {
if (Left_white_line > THRESHOLD)
L_velocity = VELOCITY_MIN;
else if (Right_white_line > THRESHOLD)
R_velocity = VELOCITY_MIN;
}
else {
if (Left_white_line > THRESHOLD)
L_velocity = 20;
else if (Right_white_line > THRESHOLD)
R_velocity = 20;
}
velocity(L_velocity, R_velocity);
if (Center_white_line < THRESHOLD && Left_white_line < THRESHOLD && Right_white_line < THRESHOLD) {
forward();
stop();
}
}


int main () {
char* path;
path = (char*)malloc(100*sizeof(char));
char* final;
final = (char*)malloc(100*sizeof(char));
int v,p=0;
strcat(final,"");
lcd_set_4bit();
init_devices();
get_vector();
while(1) {
if(strcmp(final,"")==0){
/*
if(goal reached){
final=reduced(path);
v=strlen(final);
}

*/
if(if_intersection() == 1) {	//T junction and cross junction 
turn_bot('l');
//strcat(path,"L")
}
/*
else if(cond for left turn only junction){
turn_bot('l');
strcat(path,"L")
}
else if(cond for right turn only junction){
turn_bot('r');
strcat(path,"R")
}
else if(cond for left turn and straight junction){
turn_bot('l');
strcat(path,"L")
}
else if(cond for right turn and straight junction){
forward();
_delay_ms(1000);
strcat(path,"S")
}
else if(cond for deadend){
right_degrees(180);
strcat(path,"B")
}
*/
else {
black_line();
}
}
else {
/*
if(any junction appears){
if(final[p]=='L') turn_left();
else if(final[p]=='R') turn_right();
else if(final[p]=='S') {forward();_delay_ms(1000);}
else right_degrees(180);
p++;
}
else black_line();
*/
}
}
}
