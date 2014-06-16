#include <stdio.h>
#include <math.h>
#define maze_length 7
#define maze_width 7
// libraries for AVR
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include "lcd.c"
#define THRESHOLD 60

// ############################################################## Beginning of the robot part ############################################################ 

unsigned char data_received [4];
unsigned char ADC_Conversion(unsigned char);
unsigned char ADC_Value;
unsigned char flag = 0;
unsigned char Left_white_line = 0;
unsigned char Center_white_line = 0;
unsigned char Right_white_line = 0;

unsigned long int ShaftCountLeft = 0; //to keep track of left position encoder 
unsigned long int ShaftCountRight = 0; //to keep track of right position encoder
unsigned int Degrees; //to accept angle in degrees for turning

//Function to configure ports to enable robot's motion
void motion_pin_config (void) 
{
 DDRA = DDRA | 0x0F;
 PORTA = PORTA & 0xF0;
 DDRL = DDRL | 0x18;   //Setting PL3 and PL4 pins as output for PWM generation
 PORTL = PORTL | 0x18; //PL3 and PL4 pins are for velocity control using PWM.
}

//Function to configure INT4 (PORTE 4) pin as input for the left position encoder
void left_encoder_pin_config (void)
{
 DDRE  = DDRE & 0xEF;  //Set the direction of the PORTE 4 pin as input
 PORTE = PORTE | 0x10; //Enable internal pull-up for PORTE 4 pin
}

//Function to configure INT5 (PORTE 5) pin as input for the right position encoder
void right_encoder_pin_config (void)
{
 DDRE  = DDRE & 0xDF;  //Set the direction of the PORTE 4 pin as input
 PORTE = PORTE | 0x20; //Enable internal pull-up for PORTE 4 pin
}

//Function to initialize ports

void left_position_encoder_interrupt_init (void) //Interrupt 4 enable
{
 cli(); //Clears the global interrupt
 EICRB = EICRB | 0x02; // INT4 is set to trigger with falling edge
 EIMSK = EIMSK | 0x10; // Enable Interrupt INT4 for left position encoder
 sei();   // Enables the global interrupt 
}

void right_position_encoder_interrupt_init (void) //Interrupt 5 enable
{
 cli(); //Clears the global interrupt
 EICRB = EICRB | 0x08; // INT5 is set to trigger with falling edge
 EIMSK = EIMSK | 0x20; // Enable Interrupt INT5 for right position encoder
 sei();   // Enables the global interrupt 
}

//ISR for right position encoder
ISR(INT5_vect)  
{
 ShaftCountRight++;  //increment right shaft position count
}


//ISR for left position encoder
ISR(INT4_vect)
{
 ShaftCountLeft++;  //increment left shaft position count
}


//Function used for setting motor's direction
void motion_set (unsigned char Direction)
{
 unsigned char PortARestore = 0;

 Direction &= 0x0F; 		// removing upper nibbel for the protection
 PortARestore = PORTA; 		// reading the PORTA original status
 PortARestore &= 0xF0; 		// making lower direction nibbel to 0
 PortARestore |= Direction; // adding lower nibbel for forward command and restoring the PORTA status
 PORTA = PortARestore; 		// executing the command
}

void forward (void) //both wheels forward
{
  motion_set(0x06);
}

void back (void) //both wheels backward
{
  motion_set(0x09);
}

void left (void) //Left wheel backward, Right wheel forward
{
  motion_set(0x05);
}

void right (void) //Left wheel forward, Right wheel backward
{
  motion_set(0x0A);
}

void soft_left (void) //Left wheel stationary, Right wheel forward
{
 motion_set(0x04);
}

void soft_right (void) //Left wheel forward, Right wheel is stationary
{
 motion_set(0x02);
}

void soft_left_2 (void) //Left wheel backward, right wheel stationary
{
 motion_set(0x01);
}

void soft_right_2 (void) //Left wheel stationary, Right wheel backward
{
 motion_set(0x08);
}

void stop (void)
{
  motion_set(0x00);
}


//Function used for turning robot by specified degrees
void angle_rotate(unsigned int Degrees)
{
 float ReqdShaftCount = 0;
 unsigned long int ReqdShaftCountInt = 0;

 ReqdShaftCount = (float) Degrees/ 4.090; // division by resolution to get shaft count
 ReqdShaftCountInt = (unsigned int) ReqdShaftCount;
 ShaftCountRight = 0; 
 ShaftCountLeft = 0; 

 while (1)
 {
  if((ShaftCountRight >= ReqdShaftCountInt) | (ShaftCountLeft >= ReqdShaftCountInt))
  break;
 }
 stop(); //Stop robot
}

//Function used for moving robot forward by specified distance

void linear_distance_mm(unsigned int DistanceInMM)
{
 float ReqdShaftCount = 0;
 unsigned long int ReqdShaftCountInt = 0;

 ReqdShaftCount = DistanceInMM / 5.338; // division by resolution to get shaft count
 ReqdShaftCountInt = (unsigned long int) ReqdShaftCount;
  
 ShaftCountRight = 0;
 while(1)
 {
  if(ShaftCountRight > ReqdShaftCountInt)
  {
  	break;
  }
 } 
 stop(); //Stop robot
}

void forward_mm(unsigned int DistanceInMM)
{
 forward();
 linear_distance_mm(DistanceInMM);
}

void back_mm(unsigned int DistanceInMM)
{
 back();
 linear_distance_mm(DistanceInMM);
}

void left_degrees(unsigned int Degrees) 
{
// 88 pulses for 360 degrees rotation 4.090 degrees per count
 left(); //Turn left
 angle_rotate(Degrees);
}



void right_degrees(unsigned int Degrees)
{
// 88 pulses for 360 degrees rotation 4.090 degrees per count
 right(); //Turn right
 angle_rotate(Degrees);
}


void soft_left_degrees(unsigned int Degrees)
{
 // 176 pulses for 360 degrees rotation 2.045 degrees per count
 soft_left(); //Turn soft left
 Degrees=Degrees*2;
 angle_rotate(Degrees);
}

void soft_right_degrees(unsigned int Degrees)
{
 // 176 pulses for 360 degrees rotation 2.045 degrees per count
 soft_right();  //Turn soft right
 Degrees=Degrees*2;
 angle_rotate(Degrees);
}

void soft_left_2_degrees(unsigned int Degrees)
{
 // 176 pulses for 360 degrees rotation 2.045 degrees per count
 soft_left_2(); //Turn reverse soft left
 Degrees=Degrees*2;
 angle_rotate(Degrees);
}

void soft_right_2_degrees(unsigned int Degrees)
{
 // 176 pulses for 360 degrees rotation 2.045 degrees per count
 soft_right_2();  //Turn reverse soft right
 Degrees=Degrees*2;
 angle_rotate(Degrees);
}


//ADC pin configuration
void adc_pin_config (void)
{
 DDRF = 0x00; 
 PORTF = 0x00;
 DDRK = 0x00;
 PORTK = 0x00;
}

void spi_pin_config (void)
{
 DDRB = DDRB | 0x07;
 PORTB = PORTB | 0x07;
}

void lcd_port_config (void)
{
 DDRC = DDRC | 0xF7; //all the LCD pin's direction set as output
 PORTC = PORTC & 0x80; // all the LCD pins are set to logic 0 except PORTC 7
}

//Initialization of ports
void port_init(void)
{
 adc_pin_config();
 spi_pin_config();
 lcd_port_config();
 motion_pin_config(); //robot motion pins config
 left_encoder_pin_config(); //left encoder pin config
 right_encoder_pin_config(); //right encoder pin config	
}
// Timer Frequency:225.000Hz
void timer5_init()
{
	TCCR5B = 0x00;	//Stop
	TCNT5H = 0xFF;	//Counter higher 8-bit value to which OCR5xH value is compared with
	TCNT5L = 0x01;	//Counter lower 8-bit value to which OCR5xH value is compared with
	OCR5AH = 0x00;	//Output compare register high value for Left Motor
	OCR5AL = 0xFF;	//Output compare register low value for Left Motor
	OCR5BH = 0x00;	//Output compare register high value for Right Motor
	OCR5BL = 0xFF;	//Output compare register low value for Right Motor
	OCR5CH = 0x00;	//Output compare register high value for Motor C1
	OCR5CL = 0xFF;	//Output compare register low value for Motor C1
	TCCR5A = 0xA9;	/*{COM5A1=1, COM5A0=0; COM5B1=1, COM5B0=0; COM5C1=1 COM5C0=0}
 					  For Overriding normal port functionality to OCRnA outputs.
				  	  {WGM51=0, WGM50=1} Along With WGM52 in TCCR5B for Selecting FAST PWM 8-bit Mode*/

	TCCR5B = 0x0B;	//WGM12=1; CS12=0, CS11=1, CS10=1 (Prescaler=64)
}

void adc_init()
{
	ADCSRA = 0x00;
	ADCSRB = 0x00;		//MUX5 = 0
	ADMUX = 0x20;		//Vref=5V external --- ADLAR=1 --- MUX4:0 = 0000
	ACSR = 0x80;
	ADCSRA = 0x86;		//ADEN=1 --- ADIE=1 --- ADPS2:0 = 1 1 0
}
unsigned char ADC_Conversion(unsigned char Ch) 
{
	unsigned char a;
	if(Ch>7)
	{
		ADCSRB = 0x08;
	}
	Ch = Ch & 0x07;  			
	ADMUX= 0x20| Ch;	   		
	ADCSRA = ADCSRA | 0x40;		//Set start conversion bit
	while((ADCSRA&0x10)==0);	//Wait for conversion to complete
	a=ADCH;
	ADCSRA = ADCSRA|0x10; //clear ADIF (ADC Interrupt Flag) by writing 1 to it
	ADCSRB = 0x00;
	return a;
}
void velocity (unsigned char left_motor, unsigned char right_motor)
{
	OCR5AL = (unsigned char)left_motor;
	OCR5BL = (unsigned char)right_motor;
}

//Function To Initialize SPI bus
// clock rate: 921600hz
void spi_init(void)
{
 SPCR = 0x53; //setup SPI
 SPSR = 0x00; //setup SPI
 SPDR = 0x00;
}

//Function to send byte to the slave microcontroller and get ADC channel data from the slave microcontroller
unsigned char spi_master_tx_and_rx (unsigned char data)
{
 unsigned char rx_data = 0;

 PORTB = PORTB & 0xFE; // make SS pin low
 SPDR = data;
 while(!(SPSR & (1<<SPIF))); //wait for data transmission to complete

 _delay_ms(1); //time for ADC conversion in the slave microcontroller
 
 SPDR = 0x50; // send dummy byte to read back data from the slave microcontroller
 while(!(SPSR & (1<<SPIF))); //wait for data reception to complete
 rx_data = SPDR;
 PORTB = PORTB | 0x01; // make SS high
 return rx_data;
}

//Function To Print Sesor Values At Desired Row And Coloumn Location on LCD
void print_sensor(char row, char coloumn,unsigned char channel)
{

	ADC_Value = ADC_Conversion(channel);
	lcd_print(row, coloumn, ADC_Value, 3);
}

//call this routine to initialize all peripherals
void init_devices(void)
{
 cli(); //disable all interrupts
 port_init();
 spi_init();
	adc_init();
	timer5_init();
 sei(); //re-enable interrupts
}

// checking values from the sensors
void check_sensors(){
	Left_white_line = ADC_Conversion(1);
	Right_white_line = spi_master_tx_and_rx(1);
	Center_white_line = spi_master_tx_and_rx(0);
}

// ############################################################## Beginning of the A-star part ###########################################################
// Declaration of variables
int is_at_end;
int parent_of[maze_length][maze_width],G[maze_length][maze_width],H[maze_length][maze_width],F[maze_length][maze_width];
int on_optimal_path[maze_length][maze_width];
int path[maze_length][maze_width][maze_length][maze_width];
int parent_of[maze_length][maze_width];
int is_open[maze_length][maze_width];
int direction_indicator = 0;

// Initialization of variables
void init(){
	int i,j,k,l;
	init_devices();
	
	for(i=0;i<maze_length;i++){
	
		for(j=0;j<maze_width;j++){
		
			for(k=0;k<maze_length;k++){
			
				for(l=0;l<maze_width;l++){
					path[i][j][k][l] = 0;
				}
			}
		}
	}
	
	for(i=0;i<maze_length;i++){
	
		for(j=0;j<maze_width;j++){
			parent_of[i][j] = -1;
			on_optimal_path[i][j] = 0;
			is_open[i][j] = 0;
			G[i][j] = maze_length*maze_length*maze_width*maze_width;
		}
		
	}
	
	parent_of[0][0] = 0;
}

// Moves the bot from specified start to specified destination
void moveto(int arg1,int arg2,int arg3,int arg4){ // Should edit moveto function
	
	if(arg2 == arg4){
	
		if(arg3>arg1){
			forward();
			_delay_ms(1000);
			check_sensors();
			while(!(Left_white_line < THRESHOLD || Right_white_line < THRESHOLD) && Center_white_line < THRESHOLD){
				forward();
				check_sensors();
			}
		}
		
		else{
			back();
			_delay_ms(1000);
			check_sensors();
			while(!(Left_white_line < THRESHOLD || Right_white_line < THRESHOLD) && Center_white_line < THRESHOLD){
				back();
				check_sensors();
			}
		}
	}
	
	else if(arg1 == arg3){
	
		if(arg4>arg2){
			left_degrees(90);//turn left;
			forward();
			_delay_ms(1000);
			check_sensors();
			while(!(Left_white_line < THRESHOLD || Right_white_line < THRESHOLD) && Center_white_line < THRESHOLD){
				forward();
				check_sensors();
			}
			right_degrees(90);
		}
		
		else{
			right_degrees(90);//turn right;
			forward();
			_delay_ms(1000);
			check_sensors();
			while(!(Left_white_line < THRESHOLD || Right_white_line < THRESHOLD) && Center_white_line < THRESHOLD){
				forward();
				check_sensors();
			}
			left_degrees(90);
		}
	}
	
}

// Moves the bot from specified start to specified destination
void moveto_far(int arg1,int arg2,int arg3,int arg4){
	int parent_x_1,parent_x_2,parent_y_1,parent_y_2;
	
	if(!((arg1==arg3&&fabs(arg2-arg4)==1)||(arg2==arg4&&fabs(arg1-arg3)==1))){
		parent_x_1 = parent_of[arg1][arg2]/10;
		parent_y_1 = parent_of[arg1][arg2]%10;
		parent_x_2 = parent_of[arg3][arg4]/10;
		parent_y_2 = parent_of[arg3][arg4]%10;
		moveto(arg1,arg2,parent_x_1,parent_y_1);
		moveto_far(parent_x_1,parent_y_1,parent_x_2,parent_y_2);
		moveto(parent_x_2,parent_y_2,arg3,arg4);
	}
	
	else moveto(arg1,arg2,arg3,arg4);
}

// Returns minimum of given variables
int min(int arg1,int arg2,int arg3,int arg4){
	int minimum;
	minimum = arg1;
	
	if(minimum<arg2) minimum = arg2;
	if(minimum<arg3) minimum = arg3;
	if(minimum<arg4) minimum = arg4;
	
	return minimum;
}

// Checks whether the node has a child with outward path or not
int no_child_with_outpath(int arg1,int arg2){
	int F1,F2,F3,F4;
	int flag = 0;
	
	F1 = maze_length*maze_length*maze_width*maze_width;
	F2 = maze_length*maze_length*maze_width*maze_width;
	F3 = maze_length*maze_length*maze_width*maze_width;
	F4 = maze_length*maze_length*maze_width*maze_width;
	
	if(arg1+1<=maze_length-1 && path[arg1][arg2][arg1+1][arg2]==1 && parent_of[arg1][arg2]!=10*(arg1+1) + arg2 && G[arg1][arg2]+1<G[arg1+1][arg2]){
        F1 = F[arg1 + 1][arg2];
    }
	
    if(arg1-1>=0 && path[arg1][arg2][arg1-1][arg2]==1 && parent_of[arg1][arg2]!=10*(arg1-1) + arg2 && G[arg1][arg2]+1<G[arg1-1][arg2]){
        F2 = F[arg1 - 1][arg2];
    }
	
    if(arg2+1<=maze_width-1 && path[arg1][arg2][arg1][arg2+1]==1 && parent_of[arg1][arg2]!=10*(arg1) + arg2+1 && G[arg1][arg2]+1<G[arg1][arg2+1]){
        F3 = F[arg1][arg2 + 1];
    }
	
    if(arg2-1>=0 && path[arg1][arg2][arg1][arg2-1]==1 && parent_of[arg1][arg2]!=10*(arg1) + arg2-1 && G[arg1][arg2]+1<G[arg1][arg2-1]){
        F4 = F[arg1][arg2 - 1];
    }
	
	if(F1 == maze_length*maze_length*maze_width*maze_width && F2 == maze_length*maze_length*maze_width*maze_width && 
		F3 == maze_length*maze_length*maze_width*maze_width && F4 == maze_length*maze_length*maze_width*maze_width) flag++;// Checking F values
		
	return flag;
}

// Checking whether the node has outward paths or not
int no_out_path_from(int arg1,int arg2){
	int child_x,child_y;
	int flag = 0;
	int turned_back = 0;
	
	child_x = arg1 + 1;
	child_y = arg2;
	
	if(child_x < maze_length && parent_of[arg1][arg2] != 10*child_x + child_y){
		left_degrees(90);//turn left;
		forward();
		_delay_ms(1000);
		check_sensors();
		
		while(!(Left_white_line < THRESHOLD || Right_white_line < THRESHOLD) && Center_white_line < THRESHOLD){
			forward();
			check_sensors();
		}
		
		if(Center_white_line > THRESHOLD){
			turned_back++;
			
			while(!(Left_white_line < THRESHOLD || Right_white_line < THRESHOLD)){
				back();
				check_sensors();
			}
			
			right_degrees(90);
			
		}
		
		else{
			back();
			_delay_ms(1000);
			
			while(!(Left_white_line < THRESHOLD || Right_white_line < THRESHOLD)){
				back();
				check_sensors();
			}
			
			right_degrees(90);
			
		}

		if(!turned_back) flag++;
	}
	
	child_x = arg1 - 1;
	child_y = arg2;
	
	if(child_x > 0 && parent_of[arg1][arg2] != 10*child_x + child_y){
		right_degrees(90);//turn right;
		forward();
		_delay_ms(1000);
		check_sensors();
		
		while(!(Left_white_line < THRESHOLD || Right_white_line < THRESHOLD) && Center_white_line < THRESHOLD){
			forward();
			check_sensors();
		}
		
		if(Center_white_line > THRESHOLD){
			turned_back++;
			
			while(!(Left_white_line < THRESHOLD || Right_white_line < THRESHOLD)){
				back();
				check_sensors();
			}
			
			left_degrees(90);
			
		}
		
		else{
			back();
			_delay_ms(1000);
			
			while(!(Left_white_line < THRESHOLD || Right_white_line < THRESHOLD)){
				back();
				check_sensors();
			}
			
			left_degrees(90);
			
		}
		
		if(!turned_back) flag++;
	}
	
	child_x = arg1;
	child_y = arg2 + 1;
	
	if(child_y < maze_width && parent_of[arg1][arg2] != 10*child_x + child_y){
		forward();
		_delay_ms(1000);
		check_sensors();
		
		while(!(Left_white_line < THRESHOLD || Right_white_line < THRESHOLD) && Center_white_line < THRESHOLD){
			forward();
			check_sensors();
		}
		
		if(Center_white_line > THRESHOLD){
			turned_back++;
			
			while(!(Left_white_line < THRESHOLD || Right_white_line < THRESHOLD)){
				back();
				check_sensors();
			}
			
		}
		
		else{
			back();
			_delay_ms(1000);
			
			while(!(Left_white_line < THRESHOLD || Right_white_line < THRESHOLD)){
				back();
				check_sensors();
			}
			
		}// move forward
		
		if(!turned_back) flag++;
	}
	
	child_x = arg1;
	child_y = arg2 - 1;
	
	if(child_y > 0 && parent_of[arg1][arg2] != 10*child_x + child_y){
		back();
		_delay_ms(1000);
		check_sensors();
		
		while(!(Left_white_line < THRESHOLD || Right_white_line < THRESHOLD) && Center_white_line < THRESHOLD){
			back();
			check_sensors();
		}
		
		if(Center_white_line > THRESHOLD){
			turned_back++;
			
			while(!(Left_white_line < THRESHOLD || Right_white_line < THRESHOLD)){
				forward();
				check_sensors();
			}
			
		}
		
		else{
			forward();
			_delay_ms(1000);
			
			while(!(Left_white_line < THRESHOLD || Right_white_line < THRESHOLD)){
				forward();
				check_sensors();
			}
			
		}// go back
		
		if(!turned_back) flag++;
	}
	
	if(flag == 0) return 1;
	else return 0;
}

// Whether a given child is reachable or not
void check_reachability_of_children_of(int arg1,int arg2){
	int child_x,child_y;
	int turned_back = 0,node_reached = 0;
	
	child_x = arg1 + 1;
	child_y = arg2;
	
	if(child_x < maze_length && parent_of[arg1][arg2] != 10*child_x + child_y){
		left_degrees(90);//turn left;
		forward();
		_delay_ms(1000);
		check_sensors();
		
		while(!(Left_white_line < THRESHOLD || Right_white_line < THRESHOLD) && Center_white_line < THRESHOLD){
			forward();
			check_sensors();
		}
		
		if(Center_white_line > THRESHOLD){
			turned_back++;
			
			while(!(Left_white_line < THRESHOLD || Right_white_line < THRESHOLD)){
				back();
				check_sensors();
			}
			
			right_degrees(90);
			
		}
		
		else{
			node_reached++;
			back();
			_delay_ms(1000);
			
			while(!(Left_white_line < THRESHOLD || Right_white_line < THRESHOLD)){
				back();
				check_sensors();
			}
			
			right_degrees(90);
			
		}// turn left
		// move forward
		
		if(node_reached){
			path[arg1][arg2][child_x][child_y] = 1;
			node_reached--;
		}
		if(turned_back) path[arg1][arg2][child_x][child_y] = 0;
	}
	
	child_x = arg1 - 1;
	child_y = arg2;
	
	if(child_x > 0 && parent_of[arg1][arg2] != 10*child_x + child_y){
		right_degrees(90);//turn right;
		forward();
		_delay_ms(1000);
		check_sensors();
		
		while(!(Left_white_line < THRESHOLD || Right_white_line < THRESHOLD) && Center_white_line < THRESHOLD){
			forward();
			check_sensors();
		}
		
		if(Center_white_line > THRESHOLD){
			turned_back++;
			
			while(!(Left_white_line < THRESHOLD || Right_white_line < THRESHOLD)){
				back();
				check_sensors();
			}
			
			left_degrees(90);
			
		}
		
		else{
			node_reached++;
			back();
			_delay_ms(1000);
			
			while(!(Left_white_line < THRESHOLD || Right_white_line < THRESHOLD)){
				back();
				check_sensors();
			}
			
			left_degrees(90);
			
		}// turn right
		// move forward
		if(node_reached){
			path[arg1][arg2][child_x][child_y] = 1;
			node_reached--;
		}
		if(turned_back) path[arg1][arg2][child_x][child_y] = 0;
	}
	
	child_x = arg1;
	child_y = arg2 + 1;
	
	if(child_y < maze_width && parent_of[arg1][arg2] != 10*child_x + child_y){
		forward();
		_delay_ms(1000);
		check_sensors();
		
		while(!(Left_white_line < THRESHOLD || Right_white_line < THRESHOLD) && Center_white_line < THRESHOLD){
			forward();
			check_sensors();
		}
		
		if(Center_white_line > THRESHOLD){
			turned_back++;
			
			while(!(Left_white_line < THRESHOLD || Right_white_line < THRESHOLD)){
				back();
				check_sensors();
			}
			
		}
		
		else{
			node_reached++;
			back();
			_delay_ms(1000);
			
			while(!(Left_white_line < THRESHOLD || Right_white_line < THRESHOLD)){
				back();
				check_sensors();
			}
			
		}// move forward// move forward
		if(node_reached){
			path[arg1][arg2][child_x][child_y] = 1;
			node_reached--;
		}
		
		if(turned_back) path[arg1][arg2][child_x][child_y] = 0;
	}
	
	child_x = arg1;
	child_y = arg2 - 1;
	
	if(child_y > 0 && parent_of[arg1][arg2] != 10*child_x + child_y){
		back();
		_delay_ms(1000);
		check_sensors();
		
		while(!(Left_white_line < THRESHOLD || Right_white_line < THRESHOLD) && Center_white_line < THRESHOLD){
			back();
			check_sensors();
		}
		
		if(Center_white_line > THRESHOLD){
			turned_back++;
			
			while(!(Left_white_line < THRESHOLD || Right_white_line < THRESHOLD)){
				forward();
				check_sensors();
			}
			
		}
		
		else{
			node_reached++;
			forward();
			_delay_ms(1000);
			
			while(!(Left_white_line < THRESHOLD || Right_white_line < THRESHOLD)){
				forward();
				check_sensors();
			}
			
		}// go back// turn back
		// move forward
		if(node_reached){
			path[arg1][arg2][child_x][child_y] = 1;
			node_reached--;
		}
		
		if(turned_back) path[arg1][arg2][child_x][child_y] = 0;
	}
}

// Getting coordinates for next node on the optimal path
int child_value(int arg1,int arg2){
	int F1,F2,F3,F4;
	int flag = 0;
	int retval;
	
	F1 = maze_length*maze_length*maze_width*maze_width;
	F2 = maze_length*maze_length*maze_width*maze_width;
	F3 = maze_length*maze_length*maze_width*maze_width;
	F4 = maze_length*maze_length*maze_width*maze_width;
	
	if(arg1+1<=maze_length-1 && path[arg1][arg2][arg1+1][arg2]==1 && parent_of[arg1][arg2]!=10*(arg1+1) + arg2 && on_optimal_path[arg1+1][arg2]==1){
        F1 = F[arg1 + 1][arg2];
		retval = 10*(arg1+1) + arg2;
    }
	
    if(arg1-1>=0 && path[arg1][arg2][arg1-1][arg2]==1 && parent_of[arg1][arg2]!=10*(arg1-1) + arg2 && on_optimal_path[arg1-1][arg2]==1){
        F2 = F[arg1 - 1][arg2];
		retval = 10*(arg1-1) + arg2;
    }
	
    if(arg2+1<=maze_width-1 && path[arg1][arg2][arg1][arg2+1]==1 && parent_of[arg1][arg2]!=10*(arg1) + arg2+1 && on_optimal_path[arg1][arg2+1]==1){
        F3 = F[arg1][arg2 + 1];
		retval = 10*arg1 + arg2 + 1;
    }
	
    if(arg2-1>=0 && path[arg1][arg2][arg1][arg2-1]==1 && parent_of[arg1][arg2]!=10*(arg1) + arg2-1 && on_optimal_path[arg1][arg2-1]==1){
        F4 = F[arg1][arg2 - 1];
		retval = 10*arg1 + arg2 - 1;
    }
	
	// if(min(F1,F2,F3,F4)==F1) retval = 10*(arg1+1) + arg2;
	// else if(min(F1,F2,F3,F4)==F2) retval = 10*(arg1-1) + arg2;
	// else if(min(F1,F2,F3,F4)==F3) retval = 10*arg1 + arg2 + 1;
	// else if(min(F1,F2,F3,F4)==F4) retval = 10*arg1 + arg2 - 1;
	
	return retval;
}

// Add all the children to the open list
void expand_children(int arg1,int arg2){
	int F1,F2,F3,F4;
	
	F1 = maze_length*maze_length*maze_width*maze_width;
	F2 = maze_length*maze_length*maze_width*maze_width;
	F3 = maze_length*maze_length*maze_width*maze_width;
	F4 = maze_length*maze_length*maze_width*maze_width;
	
	check_reachability_of_children_of(arg1,arg2);
	
	if(arg1+1<=maze_length-1 && path[arg1][arg2][arg1+1][arg2]==1 && parent_of[arg1][arg2]!=10*(arg1+1) + arg2 && G[arg1][arg2]+1<G[arg1+1][arg2]){
        G[arg1+1][arg2] = G[arg1][arg2]+1;
        F[arg1+1][arg2] = G[arg1+1][arg2] + H[arg1+1][arg2];
		F1 = F[arg1+1][arg2];
		if(H[arg1+1][arg2]==0) is_at_end++;
        parent_of[arg1+1][arg2] = 10*arg1 + arg2;
		is_open[arg1+1][arg2] = 1;
    }
	
    if(arg1-1>=0 && path[arg1][arg2][arg1-1][arg2]==1 && parent_of[arg1][arg2]!=10*(arg1-1) + arg2 && G[arg1][arg2]+1<G[arg1-1][arg2]){
        G[arg1-1][arg2] = G[arg1][arg2]+1;
        F[arg1-1][arg2] = G[arg1-1][arg2] + H[arg1-1][arg2];
		F2 = F[arg1-1][arg2];
		if(H[arg1 - 1][arg2]==0) is_at_end++;
        parent_of[arg1-1][arg2] = 10*arg1 + arg2;
		is_open[arg1-1][arg2] = 1;
    }
	
    if(arg2+1<=maze_width-1 && path[arg1][arg2][arg1][arg2+1]==1 && parent_of[arg1][arg2]!=10*(arg1) + arg2+1 && G[arg1][arg2]+1<G[arg1][arg2+1]){
        G[arg1][arg2+1] = G[arg1][arg2]+1;
        F[arg1][arg2+1] = G[arg1][arg2+1] + H[arg1][arg2+1];
		F3 = F[arg1][arg2+1];
		if(H[arg1][arg2+1]==0) is_at_end++; 
        parent_of[arg1][arg2+1] = 10*arg1 + arg2;
		is_open[arg1][arg2+1] = 1;
    }
	
    if(arg2-1>=0 && path[arg1][arg2][arg1][arg2-1]==1 && parent_of[arg1][arg2]!=10*(arg1) + arg2-1 && G[arg1][arg2]+1<G[arg1][arg2-1]){
        G[arg1][arg2-1] = G[arg1][arg2]+1;
        F[arg1][arg2-1] = G[arg1][arg2-1] + H[arg1][arg2-1];
		F4 = F[arg1][arg2-1];
		if(H[arg1][arg2-1]==0) is_at_end++;
        parent_of[arg1][arg2-1] = 10*arg1 + arg2;
		is_open[arg1][arg2-1] = 1;
    }
	
	// if(F1 == min(F1,F2,F3,F4)){
		// moveto(arg1,arg2,arg1+1,arg2);
		// on_optimal_path[arg1+1][arg2] = 1;
	// }//simply the bot must move left by one cell
	
	// else if(min(F1,F2,F3,F4) == F2){
		// moveto(arg1,arg2,arg1-1,arg2);
		// on_optimal_path[arg1-1][arg2] = 1;
	// }//simply the bot must move right by one cell
	
	// else if(F3 == min(F1,F2,F3,F4)){
		// moveto(arg1,arg2,arg1,arg2+1);
		// on_optimal_path[arg1][arg2+1] = 1;
	// }//simply the bot must move forward by one cell
	
	// else if(F4 == min(F1,F2,F3,F4)){
		// moveto(arg1,arg2,arg1,arg2-1);
		// on_optimal_path[arg1][arg2-1] = 1;
	// }//simply the bot must move backward by one cell
}

// Give the coordinates of the successor
int successor_value(int arg1,int arg2){
	int min = 0;
	int destination_x,destination_y;
	int retval,i,j;
	
	for(i=0;i<maze_length;i++){
	
		for(j=0;j<maze_width;j++){
		
			if(is_open[i][j]==1 && min<F[i][j]){
				destination_x = i;
				destination_y = j;
			}
		}
	}
	
	retval = 10*destination_x + destination_y;
	
	return retval;
}

// Go to the correct node of the open list
void goto_correct_successor(int arg1,int arg2){
	int min = 0;
	int destination_x,destination_y,i,j;
	
	for(i=0;i<maze_length;i++){
	
		for(j=0;j<maze_width;j++){
		
			if(is_open[i][j]==1 && min<F[i][j]){
				destination_x = i;
				destination_y = j;
			}
		}
	}
	
	moveto_far(arg1,arg2,destination_x,destination_y);
}

// Go to the parent of the specified node
void goto_parent_of(int arg1,int arg2){
	int current_x,current_y;
	int parent_x,parent_y,parent;
	
	current_x = arg1;
	current_y = arg2;
	parent = parent_of[current_x][current_y];
	parent_x = parent/10;
	parent_y = parent%10;
	on_optimal_path[current_x][current_y] = 0;
	
	moveto(current_x,current_y,parent_x,parent_y);
}

// Go to the next node in the optimal path
void goto_next_node(int arg1,int arg2){
	int child_x,child_y,mod = 0;
	
	child_x = arg1 + 1;
	child_y = arg2;
	
	if(child_x < maze_length && path[arg1][arg2][child_x][child_y]==1 && on_optimal_path[child_x][child_y]==1 && mod==0 && parent[arg1][arg2]!=10*child_x + child_y){
		moveto(arg1,arg2,child_x,child_y);
		if(H[child_x][child_y]==0) is_at_end++;
		mod++;
	}
		
	
	child_x = arg1 - 1;
	child_y = arg2;
	
	if(child_x >= 0 && path[arg1][arg2][child_x][child_y]==1 && on_optimal_path[child_x][child_y]==1 && mod==0 && parent[arg1][arg2]!=10*child_x + child_y){
		moveto(arg1,arg2,child_x,child_y);
		if(H[child_x][child_y]==0) is_at_end++;
		mod++;
	}
	
	child_x = arg1;
	child_y = arg2 + 1;
	
	if(child_y < maze_length && path[arg1][arg2][child_x][child_y]==1 && on_optimal_path[child_x][child_y]==1 && mod==0 && parent[arg1][arg2]!=10*child_x + child_y){
		moveto(arg1,arg2,child_x,child_y);
		if(H[child_x][child_y]==0) is_at_end++;
		mod++;
	}
	
	child_x = arg1;
	child_y = arg2 - 1;
	
	if(child_y >= 0 && path[arg1][arg2][child_x][child_y]==1 && on_optimal_path[child_x][child_y]==1 && mod ==0 && parent[arg1][arg2]!=10*child_x + child_y){
		moveto(arg1,arg2,child_x,child_y);
		if(H[child_x][child_y]==0) is_at_end++;
		mod++;
	}
}

// Exploring the maze using A-star
void a_star_exploring(int arg1,int arg2){
	int current_x=arg1,current_y=arg2;
	int successor_x,successor_y,parent_x,parent_y;
	is_at_end = 0;
	init();

	while(!is_at_end){
	
			if(no_out_path_from(current_x,current_y) || no_child_with_outpath(current_x,current_y)){
				goto_parent_of(current_x,current_y);
				F[current_x][current_y] = maze_length*maze_length*maze_width*maze_width;
				parent_x = parent_of[current_x][current_y]/10;
				parent_y = parent_of[current_x][current_y]%10;
				a_star_exploring(parent_x,parent_y);
			}
			
			else{
				expand_children(current_x,current_y);
				goto_correct_successor(current_x,current_y);
				successor_x = successor_value(current_x,current_y)/10;
				successor_y = successor_value(current_x,current_y)%10;
				is_open[successor_x][successor_y] = 0;
				on_optimal_path[successor_x][successor_y] = 1;
				a_star_exploring(successor_x,successor_y);
			}
	}
	
	if(is_at_end) printf("Done Exploring! Lets solve it!");
	
}

// Solving the maze using A-star
void a_star_solving(int arg1,int arg2){
	int current_x = arg1,current_y = arg2;
	int child_x,child_y;
	is_at_end = 0;
	
	while(!is_at_end){
		goto_next_node(arg1,arg2);
		child_x = child_value(current_x,current_y)/10;
		child_y = child_value(current_x,current_y)%10;
		a_star_solving(child_x,child_y);
	}
	
	if(is_at_end) printf("Solved it! At end!");
}

// Main program for exploring and solving the maze
int main(){
	int i = 0,j = 0;
	
	a_star_exploring(i,j);
	_delay_ms(20000); // Introducing delay for setting the position of the bot for second run
	a_star_solving(i,j);
	
	return 0;
}

