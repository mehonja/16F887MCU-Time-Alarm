#include <16F887.h>
#fuses HS,NOWDT,NOPROTECT,NOLVP,PUT
#use delay(clock=8000000)
#use standard_io(B)
#include <path\to\flex_lcd.c>

int lowerNibble = -1; int higherNibble = -1;                                            //vars for reading the keypad
int1 initCrTime = 0; int1 setCrTime = 0; int crTimeNumbers = 0;                         //initCrTime (is the current time initialised); setCrTime (is the current time being set); crTimeNumbers(how many numbers were input)
int1 initAlTime = 0; int1 setAlTime = 0; int alTimeNumbers = 0;                         //same as above only for alarm times
char crTime[4];                                                                       	//current time in char arrays
char alTime[4];                                                                        	//alarm time in char arrays
int intCounter = 0;                                                                     //timer1 interrupt counter
int1 incrementTime = 0;                                                                	//if set the time increments by one

char keypadMatrix[4][4] = {                                                           	//keypad matrix
                                 {'1', '2', '3', 'A'},
                               {'4', '5', '6', 'B'},
                                {'7', '8', '9', 'C'},
                                {'*', '0', '#', 'D'}
                           };

void checkLowerNibble(void)                                                            	//checks what pin of the lower nibble has been pressed
{
   if (input_state(PIN_D0)) lowerNibble = 0;                                		 	//input_state(args) reads the PIN value but doesn't edit the TRIS for set PIN/PORT
   if (input_state(PIN_D1)) lowerNibble = 1;
   if (input_state(PIN_D2)) lowerNibble = 2;
   if (input_state(PIN_D3)) lowerNibble = 3;
}

int1 checkD4(void)                                                              	 	//checks the D4 PIN
{
   output_low(PIN_D4);                                                           		//D4 reset
   
   switch (lowerNibble)                                                          		//switch reading the lowerNibble var
   {
      case 0:                                                                    		//if the lowerNibble == D0 then only D0 will be checked in the switch
      {
         if (!input_state(PIN_D0))                                               		//if D0 is turned of after reseting D4
         {
            higherNibble = 0;                                                    		//we have the higher nibble
            return 1;                                                            
         }
         break;
      }
      case 1:                                                                    		//and repeats everything for different setups
      {
         if (!input_state(PIN_D1))
         {
            higherNibble = 0;
            return 1;
         }
         break;
      }
      case 2:
      {
         if (!input_state(PIN_D2))
         {
            higherNibble = 0;
            return 1;
         }
         break;
      }
      case 3:
      {
         if (!input_state(PIN_D3))
         {
            higherNibble = 0;
            return 1;
         }
         break;
      }
   }
   
   return 0;
}

int1 checkD5(void)                                                            //for D5 same as above
{
   output_low(PIN_D5);
   
   switch (lowerNibble)
   {
      case 0:
      {
         if (!input_state(PIN_D0))
         {
            higherNibble = 1;
            return 1;
         }
         break;
      }
      case 1:
      {
         if (!input_state(PIN_D1))
         {
            higherNibble = 1;
            return 1;
         }
         break;
      }
      case 2:
      {
         if (!input_state(PIN_D2))
         {
            higherNibble = 1;
            return 1;
         }
         break;
      }
      case 3:
      {
         if (!input_state(PIN_D3))
         {
            higherNibble = 1;
            return 1;
         }
         break;
      }
   }
   return 0;
}

int1 checkD6(void)                                                            //same for D6
{
   output_low(PIN_D6);
   
   switch (lowerNibble)
   {
      case 0:
      {
         if (!input_state(PIN_D0))
         {
            higherNibble = 2;
            return 1;
         }
         break;
      }
      case 1:
      {
         if (!input_state(PIN_D1))
         {
            higherNibble = 2;
            return 1;
         }
         break;
      }
      case 2:
      {
         if (!input_state(PIN_D2))
         {
            higherNibble = 2;
            return 1;
         }
         break;
      }
      case 3:
      {
         if (!input_state(PIN_D3))
         {
            higherNibble = 2;
            return 1;
         }
         break;
      }
   }
   
   return 0;
}

int1 verifyEntry(int number, int1 crAl)                                                                 //checks if the input time is legal, the time can't be 82:87
{
   switch (number)                                                                                      //the switch takes the current intput number
   {
      case 0:                                                                                           //frist number [1]2:00
      {
         if (crAl == 0)                                                                                 //crAl == 0 means that the current time is being input
         {
            if (crTime[0] == '0' || crTime[0] == '1' || crTime[0] == '2') return 1;                   	//if i like the input -> return 1
            else crTimeNumbers--; return 0;                                                             //if i don't like it decrement the number counter and return 0
         }
         else if (crAl == 1)                                                                            //same as before only for alarm times
         {
            if (alTime[0] == '0' || alTime[0] == '1' || alTime[0] == '2') return 1;
            else alTimeNumbers--; return 0;
         }
      }
      case 1:                                                                                         	//for number 1[2]:00
      {
         if (crAl == 0)                                                                               	//again alVr == 0 for time
         {
            if (crTime[0] == '0' || crTime[0] == '1')                                           		//the first must be checked [1]2:00 if the first number is 2 the second can only be 0,1,2,3
            {
               if (crTime[1] != '*' && crTime[1] != '#' && crTime[1] != 'A' && crTime[1] != 'B' && crTime[1] != 'C' && crTime[1] != 'D') return 1;   //if i like the input return 1
               else crTimeNumbers--; return 0;                                                          //if not return 0
            }
            else if (crTime[0] == '2')	
            {
               if (crTime[1] == '0' || crTime[1] == '1' || crTime[1] == '2' || crTime[1] == '3') return 1;	
               else crTimeNumbers--; return 0;	
            }
         }
         else if (crAl == 1)	
         {
            if (alTime[0] == '0' || alTime[0] == '1')
            {
               if (alTime[1] != '*' && alTime[1] != '#' && alTime[1] != 'A' && alTime[1] != 'B' && alTime[1] != 'C' && alTime[1] != 'D') return 1;
               else alTimeNumbers--; return 0;
            }
            else if (alTime[0] == '2')
            {
               if (alTime[1] == '0' || alTime[1] == '1' || alTime[1] == '2' || alTime[1] == '3') return 1;
               else alTimeNumbers--; return 0;
            }
         }
      }
      case 2:	
      {
         if (crAl == 0)	
         {
            if (crTime[2] == '0' || crTime[2] == '1' || crTime[2] == '2' || crTime[2] == '3' || crTime[2] == '4' || crTime[2] == '5') return 1;
            else crTimeNumbers--; return 0;
         }
         else if (crAl == 1)	
         {
            if (alTime[2] == '0' || alTime[2] == '1' || alTime[2] == '2' || alTime[2] == '3' || alTime[2] == '4' || alTime[2] == '5') return 1;
            else alTimeNumbers--; return 0;
         }
      }
      case 3:
      {
         if (crAl == 0)
         {
            if (crTime[1] != '*' && crTime[1] != '#' && crTime[1] != 'A' && crTime[1] != 'B' && crTime[1] != 'C' && crTime[1] != 'D') return 1;
            else crTimeNumbers--; return 0;
         }
         else if (crAl == 1)
         {
            if (alTime[1] != '*' && alTime[1] != '#' && alTime[1] != 'A' && alTime[1] != 'B' && alTime[1] != 'C' && alTime[1] != 'D') return 1;
            else alTimeNumbers--; return 0;
         }
      }
   }
}

void incrementTimeF(void)                                                       //incrementing while taking in consideration how time is supposed to go
{
   switch (crTime[3])                                                        	//incrementing from 12:0[0] to 12:0[1]
   {
      case '0': crTime[3] = '1'; break;
      case '1': crTime[3] = '2'; break;
      case '2': crTime[3] = '3'; break;
      case '3': crTime[3] = '4'; break;
      case '4': crTime[3] = '5'; break;
      case '5': crTime[3] = '6'; break;
      case '6': crTime[3] = '7'; break;
      case '7': crTime[3] = '8'; break;
      case '8': crTime[3] = '9'; break;
      case '9':                                                                  //if the number is 9 setting 12:0[9] -> 12:0[0]
      {
         crTime[3] = '0';
         switch (crTime[2])                                                   	//now 12:[0]0 -> 12:[1]0
         {
            case '0': crTime[2] = '1'; break;
            case '1': crTime[2] = '2'; break;
            case '2': crTime[2] = '3'; break;
            case '3': crTime[2] = '4'; break;
            case '4': crTime[2] = '5'; break;
            case '5':                                                            //if the the minutes are 12:59 incrementing to 12:00 and setting hours afterwards
            {
               crTime[2] = '0';
               if (crTime[0] == '0' || crTime[0] == '1')
               {
                  switch (crTime[1])
                  {
                     case '0': crTime[1] = '1'; break;
                     case '1': crTime[1] = '2'; break;
                     case '2': crTime[1] = '3'; break;
                     case '3': crTime[1] = '4'; break;
                     case '4': crTime[1] = '5'; break;
                     case '5': crTime[1] = '6'; break;
                     case '6': crTime[1] = '7'; break;
                     case '7': crTime[1] = '8'; break;
                     case '8': crTime[1] = '9'; break;
                     case '9':
                     {
                        crTime[1] = '0';
                        switch (crTime[0])
                        {
                           case '0': crTime[0] = '1'; break;
                           case '1': crTime[0] = '2'; break;
                           case '2': crTime[0] = '0'; break;
                        }
                     }
                  }
               }
               else if (crTime[0] == '2')
               {
                  switch (crTime[1])
                  {
                     case '0': crTime[1] = '1'; break;                  
                     case '1': crTime[1] = '2'; break;
                     case '2': crTime[1] = '3'; break;
                     case '3': crTime[1] = '0'; crTime[0] = '0'; break;
                  }
               }
            }
         }
      }
   }
}

void alarm(void)                                                                 //alarm on the lcd
{
   while(1)
   {
      lcd_putc("\fALARM!\nALARM!");
      delay_ms(100);
   }
}

#INT_TIMER1                                                                     
void timer1isr(void)
{
   set_timer1(3036);                                                             //timer on 3036d for 250ms interrupts
   clear_interrupt(INT_TIMER1);                                                  
   
   intCounter++;                                         
   if (intCounter == 240)                                                      	//4 interrupts for 1s, 60 for a minut; 250ms * 4 = 1s * 60 = 60s
   {
      intCounter = 0;                                                          	//reseting the counter
      incrementTime = 1;                                                      	//incrementTime flag because the flex_lcd.c calls delays
   }
}

void main (void)
{   
   int setTime = 0;
   set_tris_d(0x0F);
   
   output_high(PIN_D4);
   output_high(PIN_D5);
   output_high(PIN_D6);
   output_high(PIN_D7);
   
   lcd_init();
   lcd_putc("\fA to set\ncr. time");
   
   while(1)
   {
      checkLowerNibble();                                                       //reading the logical one from the lower nibble
      
      delay_ms(30);                                                             //wanna be debounce
      switch (lowerNibble)                                                      //reading is the input PIN still pressed
      {
         case 0: if (!input_state(PIN_D0)) lowerNibble = -1; break;             //if it's not reseting to -1
         case 1: if (!input_state(PIN_D1)) lowerNibble = -1; break;
         case 2: if (!input_state(PIN_D2)) lowerNibble = -1; break;
         case 3: if (!input_state(PIN_D3)) lowerNibble = -1; break;
      }
      
      if (lowerNibble != -1 && (!initCrTime || !initAlTime))                    //if the key went trough the debounce checking for the lower nibble
      {
         if (!checkD4())                                                    	//checking D4
         {
            if (!checkD5())                                                  	//if it's not D4 checking D5
            {
               if (!checkD6())                                               	//if it's not D6?
               {
                  higherNibble = 3;                                             //then it's D7
               }
            }
         }
         
         output_high(PIN_D4);                                                   //
         output_high(PIN_D5);                                                   // the higher nible to high
         output_high(PIN_D6);                                                   //
         
         switch (lowerNibble)                               
         {
            case 0: while(input_state(PIN_D0)); break;                          //
            case 1: while(input_state(PIN_D1)); break;                          // waiting for
            case 2: while(input_state(PIN_D2)); break;                          // releasing the key
            case 3: while(input_state(PIN_D3)); break;                          //
         }
         
         if (!initAlTime && setAlTime)                                          				//if setAlarm is set, set the pressed char into the var and go trough verfication processed
         {
            alTime[alTimeNumbers] = keypadMatrix[higherNibble][lowerNibble];                 	//writing into the variable
            
            switch (alTimeNumbers)                                                  			//switch is looking for how many numbers are input
            {
               case 0:
               {
                  if (verifyEntry(0, 1)) printf(lcd_putc, " %c", alTime[alTimeNumbers]);   		//verification and setting the input onto the lcd
                  break;
               }
               case 1:
               {
                  if (verifyEntry(1, 1)) printf(lcd_putc, "%c:", alTime[alTimeNumbers]);   		//verification and setting the input onto the lcd
                  break;
               }
               case 2:
               {
                  if (verifyEntry(2, 1)) lcd_putc(alTime[alTimeNumbers]);                  		//verification and setting the input onto the lcd
                  break;
               }
               case 3:
               {
                  if (verifyEntry(3, 1)) lcd_putc(alTime[alTimeNumbers]);                  		//verification and setting the input onto the lcd
                  
                  initAlTime = 1;                                                           	//alarm set
                  setAlTime = 0;                                                           		//reseting setAlTime
               }
            }
            alTimeNumbers++;                                                                  	//increment, if the verfication has not been passed the counter is decremented and incremented again
         }
         
         
         if (!initCrTime && setCrTime)                                                 			//for the current time the procedure is the same as the alarm time, only case 3 is different
         {
            crTime[crTimeNumbers] = keypadMatrix[higherNibble][lowerNibble];
            
            switch (crTimeNumbers)
            {
               case 0:
               {
                  if (verifyEntry(0, 0)) printf(lcd_putc, " %c", crTime[crTimeNumbers]);            
                  break;
               }
               case 1: 
               {
                  if(verifyEntry(1, 0)) printf(lcd_putc, "%c:", crTime[crTimeNumbers]);
                  break;
               }
               case 2:
               {
                  if (verifyEntry(2, 0)) lcd_putc(crTime[crTimeNumbers]);
                  break;
               }
               case 3:
               {
                  if (verifyEntry(3, 0))
                  {
                     lcd_putc(crTime[crTimeNumbers]);                                      	//1s delay to see the input
                     delay_ms(1000);
                  
                     initCrTime = 1;                                                     	//initCrTime set; meaning the time is set
                     setCrTime = 0;
                     
                     setAlTime = 1;                                                       	//activating alarm time input
                     
                     lcd_putc("\fEnter alarm\ntime:");
                  }
                  break;
               }
            }  
            crTimeNumbers++;
          }
         
         
         
         if(!higherNibble && lowerNibble == 3 && !initCrTime && !setCrTime)
         {
            lcd_putc("\fEnter current\ntime:");
            setCrTime = 1;
         }
                
         lowerNibble = -1;                                                             //reseting vars to -1 and wating for the next one
         higherNibble = -1;
      }
      
      if (initCrTime && initAlTime)                                                 	//when both are set displaying the time
      {
         if (!setTime)
         {
            lcd_putc("\fCr. tm.: ");
            printf(lcd_putc, "%c%c:%c%c", crTime[0], crTime[1], crTime[2], crTime[3]);
            lcd_putc("\nAl. tm.: ");
            printf(lcd_putc, "%c%c:%c%c", alTime[0], alTime[1], alTime[2], alTime[3]);
            
            setup_timer_1(T1_INTERNAL|T1_DIV_BY_8|T1_CLK_OUT);
            set_timer1(3036);
         
            enable_interrupts(INT_TIMER1);
            enable_interrupts(GLOBAL);
            
            setTime = 1;
            
            if (crTime[0] == alTime[0] && crTime[1] == alTime[1] && crTime[2] == alTime[2] && crTime[3] == alTime[3]) alarm();
         }
         if (incrementTime)                                                   			//if the incrementTime was set increment time and display again
         {
            lcd_putc("\fCr. tm.: ");
            incrementTimeF();
            printf(lcd_putc, "%c%c:%c%c", crTime[0], crTime[1], crTime[2], crTime[3]);
            lcd_putc("\nAl. tm.: ");
            printf(lcd_putc, "%c%c:%c%c", alTime[0], alTime[1], alTime[2], alTime[3]);
            
            if (crTime[0] == alTime[0] && crTime[1] == alTime[1] && crTime[2] == alTime[2] && crTime[3] == alTime[3]) alarm();
            
            incrementTime = 0;
         }
      }
   }
}
