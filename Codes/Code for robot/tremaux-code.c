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
//############################################################## Start of AVR code ############################################################

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

//############################################################## Start of Tremaux code ##########################################################
// Declaration of variables
int is_at_end;
int path[maze_length][maze_width][maze_length][maze_width];
int parent_of[maze_length][maze_width];
int on_optimal_path[maze_length][maze_width];

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
		}
		
	}
}

// Moves to given destination from given source point
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

// Checks whether the node has paths to its children or not
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

// Checks whether the children have outward paths or not
int no_child_with_outpath(int arg1,int arg2){
	int child_x,child_y;
	int flag = 0;
	
	child_x = arg1 + 1;
	child_y = arg2;
	
	if(child_x < maze_length && parent_of[arg1][arg2]!=10*child_x + child_y){
		// turn left
		// move forward
		if(path[arg1][arg2][child_x][child_y]!=2) flag++;
	}
	
	child_x = arg1 - 1;
	child_y = arg2;
	
	if(child_x > 0 && parent_of[arg1][arg2]!=10*child_x + child_y){
		// turn right
		// move forward
		if(path[arg1][arg2][child_x][child_y]!=2) flag++;
	}
	
	child_x = arg1;
	child_y = arg2 + 1;
	
	if(child_y < maze_width && parent_of[arg1][arg2]!=10*child_x + child_y){
		// move forward
		if(path[arg1][arg2][child_x][child_y]!=2) flag++;
	}
	
	child_x = arg1;
	child_y = arg2 - 1;
	
	if(child_y > 0 && parent_of[arg1][arg2]!=10*child_x + child_y){
		// turn back
		// move forward
		if(path[arg1][arg2][child_x][child_y]!=2) flag++;
	}
	
	if(flag>0) return 1;
	else return 0;
}

// Goto the parent of the given node
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

// Go to one of the children at random
int goto_child_of(int arg1,int arg2){// Return an integer in this function to depict which child is the bot going to
	int current_x=arg1,current_y=arg2;
	int child_x,child_y,no_of_paths;
	int flag = 0,flagl = 0,flagr = 0,flags = 0,flagb = 0;
	int random_path_number;
	
	child_x = arg1 + 1;
	child_y = arg2;
	
	if(child_x < maze_length && parent_of[arg1][arg2]!=10*child_x + child_y && path[arg1][arg2][child_x][child_y]!=2 && path[arg1][arg2][child_x][child_y]>0){
		// turn left
		// move forward
		flag++;
		flagl++;
	}
	
	child_x = arg1 - 1;
	child_y = arg2;
	
	if(child_x > 0 && parent_of[arg1][arg2]!=10*child_x + child_y && path[arg1][arg2][child_x][child_y]!=2 && path[arg1][arg2][child_x][child_y]>0){
		// turn right
		// move forward
		flag++;
		flagr++;
	}
	
	child_x = arg1;
	child_y = arg2 + 1;
	
	if(child_y < maze_width && parent_of[arg1][arg2]!=10*child_x + child_y && path[arg1][arg2][child_x][child_y]!=2 && path[arg1][arg2][child_x][child_y]>0){
		// move forward
		flag++;
		flags++;
	}
	
	child_x = arg1;
	child_y = arg2 - 1;
	
	if(child_y > 0 && parent_of[arg1][arg2]!=10*child_x + child_y && path[arg1][arg2][child_x][child_y]!=2 && path[arg1][arg2][child_x][child_y]>0){
		// turn back
		// move forward
		flag++;
		flagb++;
	}
	
	no_of_paths = flag;
	random_path_number = rand(flag);
	
	if(random_path_number = 0){
		if(flagl > 0){
			moveto(arg1,arg2,arg1+1,arg2);// turn left
			// move forward
			return 0;
		}
		
		else if(flags > 0){
			moveto(arg1,arg2,arg1-1,arg2);// go forward
			return 1;
		}
		
		else if(flagr > 0){
			moveto(arg1,arg2,arg1,arg2+1);// turn right
			return 2;
		}
		
		else if(flagb > 0){
			moveto(arg1,arg2,arg1,arg2-1);// go backward
			return 3;
		}
	}
	
	if(random_path_number = 1){
		if(flags > 0){
			moveto(arg1,arg2,arg1-1,arg2);// turn left
			// move forward
			return 1;
		}
		
		else if(flagr > 0){
			moveto(arg1,arg2,arg1,arg2+1);// go forward
			return 2;
		}
		
		else if(flagb > 0){
			moveto(arg1,arg2,arg1,arg2-1);// turn right
			return 3;
		}
		
		else if(flagl > 0){
			moveto(arg1,arg2,arg1+1,arg2);// go backward
			return 0;
		}
	}
	
	if(random_path_number = 2){
		if(flagr > 0){
			moveto(arg1,arg2,arg1,arg2+1);// turn left
			// move forward
			return 2;
		}
		
		else if(flagb > 0){
			moveto(arg1,arg2,arg1,arg2-1);// go forward
			return 3;
		}
		
		else if(flagl > 0){
			moveto(arg1,arg2,arg1+1,arg2);// turn right
			return 0;
		}
		
		else if(flags > 0){
			moveto(arg1,arg2,arg1-1,arg2);// go backward
			return 1;
		}
	}
}

// Get the coordinates of the child chosen by the random algorithm
int successor_value(int arg1,int arg2,int arg3){
	int rand = arg3;
	int retval;
	
	if(rand == 0) retval = 10*(arg1+1) + arg2;
	else if(rand == 1) retval = 10*(arg1-1) + arg2;
	else if(rand == 2) retval = 10*arg1 + arg2 + 1;
	else if(rand == 3) retval = 10*arg1 + arg2 + 1;
	
	return retval;
}

// Get the coordinates of the successor in the solution of the maze
int correct_successor_value(int arg1,int arg2){
	int child_x, child_y;
	
	child_x = arg1 + 1;
	child_y = arg2;
	
	if(child_x < maze_length && path[arg1][arg2][child_x][child_y]==1) return 10*child_x + child_y;
	
	child_x = arg1 - 1;
	child_y = arg2;
	
	if(child_x < maze_length && path[arg1][arg2][child_x][child_y]==1) return 10*child_x + child_y;
	
	child_x = arg1;
	child_y = arg2 + 1;
	
	if(child_x < maze_length && path[arg1][arg2][child_x][child_y]==1) return 10*child_x + child_y;
	
	child_x = arg1;
	child_y = arg2 - 1;
	
	if(child_x < maze_length && path[arg1][arg2][child_x][child_y]==1) return 10*child_x + child_y;
}

// Goes to the successor of the present node in the solution of the maze
void goto_successor_of(int arg1,int arg2){
	int child_x, child_y;
	
	child_x = arg1 + 1;
	child_y = arg2;
	
	if(child_x < maze_length && path[arg1][arg2][child_x][child_y]==1) moveto(arg1,arg2,child_x,child_y);
	
	child_x = arg1 - 1;
	child_y = arg2;
	
	if(child_x < maze_length && path[arg1][arg2][child_x][child_y]==1) moveto(arg1,arg2,child_x,child_y);
	
	child_x = arg1;
	child_y = arg2 + 1;
	
	if(child_x < maze_length && path[arg1][arg2][child_x][child_y]==1) moveto(arg1,arg2,child_x,child_y);
	
	child_x = arg1;
	child_y = arg2 - 1;
	
	if(child_x < maze_length && path[arg1][arg2][child_x][child_y]==1) moveto(arg1,arg2,child_x,child_y);
}

// Exploring the maze
void tremaux_exploring(int arg1,int arg2){
	int current_x=arg1,current_y=arg2;
	int successor_x,successor_y,parent_x,parent_y;
	int child_id;
	is_at_end = 0;
	init();
	
	if(!is_at_end){
		
		if(no_out_path_from(current_x,current_y) || no_child_with_outpath(current_x,current_y)){
			goto_parent_of(current_x,current_y);
			parent_x = parent_of[current_x][current_y]/10;
			parent_y = parent_of[current_x][current_y]%10;
			path[parent_x][parent_y][current_x][current_y]++;
			tremaux_exploring(parent_x,parent_y);
		}
		
		else{
			child_id = goto_child_of(current_x,current_y);
			successor_x = successor_value(current_x,current_y,child_id)/10;
			successor_y = successor_value(current_x,current_y,child_id)%10;
			path[current_x][current_y][successor_x][successor_y]++;
			tremaux_exploring(successor_x,successor_y);
		}
		
	}
}

// Back-tracing maze
void tremaux_solving(int arg1,int arg2){
	int current_x=arg1,current_y=arg2;
	int successor_x,successor_y;
	
	if(!(arg1==0 && arg2==0)){
		goto_successor_of(arg1,arg2);
		successor_x = correct_successor_value(current_x,current_y)/10;
		successor_y = correct_successor_value(current_x,current_y)%10;
		tremaux_solving(successor_x,successor_y);
	}
}

// Main function
int main(){
	int i=0,j=0;
	tremaux_exploring(i,j);
	//_delay_ms(5000)
	tremaux_solving(maze_length-1,maze_width-1);
	return 0;
}