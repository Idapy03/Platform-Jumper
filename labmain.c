/*
- This program was made as a project for the course IS1500.
- The program is a platform jumping mini-game for the DE10-Lite board.
- The switches on the game controls the character and the player is able 
to boost once all 10 leds are lit. Using the button the character will boost 
when falling down on a platform.
- Authors: Linn Benali and Ida Pylvänäinen
- Date: 2024-12-07
*/


//volatile const int led_delay = 100; 
volatile int led_state = 0;
volatile char *VGA = (volatile char *)0x08000000; 
//x coordinates of all genereated platforms
volatile int platform_coordinates[] = {150, 120, 30, 200, 50, 10, 110, 100, 210, 50, 0, 200, 24};
//number of platform x-coordinates
volatile int platform_amount = 13;
//which platform coordinate x-coordinate we are currently using
volatile int current_platform_coordnr = 0;
//total points in the game
volatile int points = 0;
//number of platforms that can be seen on the screen
volatile int amount_platforms = 3;

void handle_interrupt(unsigned cause) {}

//print the instruction to the user
void instruction(){
    print("-----------------------------------------------\n");
    print("Welcome to this platform jumping minigame!\n\n");
    
    print("CONTROLS: The controls on the DE10-lite board controls the character:\n");
    print("The leftmost switch turns the character to the left and the rightmost switch turns it to the right.\n");
    print("Don't forget the 360 movement when reaching the left edge or the right edge of the screen.\n");
    
    print("GOAL: Get as many points as possible.\n");
    print("The creator's highscore is 104, can you get a better score???\n");
    
    print("BOOSTER: Once all the 10 LEDs are lit you can boost up to the top by\n");
    print("pressing the button while falling onto a platform.\n");
    
    print("REMARK: This is only a simple game created for educational purposes :)\n\n");
    
    print("Good luck!\n");
}

//when reaching the bottom of the screen this message should be printed
void print_death_message() {
    print("\n");
    print("========================================================\n");
    print("||  Y     Y                                            ||\n");
    print("||   YY YY   OOOOO   U     U       DDDDD   III  EEEEE ||\n");
    print("||     Y    O     O  U     U       D    D   I   E     ||\n");
    print("||     Y    O     O  U     U       D     D  I   EEEE  ||\n");
    print("||     Y    O     O  U     U       D    D   I   E     ||\n");
    print("||     Y     OOOOO    UUUUU        DDDDD   III  EEEEE ||\n");
    print("========================================================\n");
    print("\n");
}

//set the leds on the board, decide the which leds should be lit using the led_mask
void set_leds(int led_mask) {
  volatile int *leds = (volatile int*) 0x04000000;

  *leds = led_mask;
}

//Choose which value to set the leds to
void set_led_state(int led_state) {

    //leds 0-9 on the board
    int led0 = 0b1;
    int led1 = 0b11;
    int led2 = 0b111;
    int led3 = 0b1111;
    int led4 = 0b11111;
    int led5 = 0b111111;
    int led6 = 0b1111111;
    int led7 = 0b11111111;
    int led8 = 0b111111111;
    int led9 = 0b1111111111;

    //turn on the leds after certain delay (based on timer)
    //they will turn on in order    
    switch (led_state){
        case 0: set_leds(led0); break;
        case 1: set_leds(led1); break;
        case 2: set_leds(led2); break;
        case 3: set_leds(led3); break;
        case 4: set_leds(led4); break;
        case 5: set_leds(led5); break;
        case 6: set_leds(led6); break;
        case 7: set_leds(led7); break;
        case 8: set_leds(led8); break;
        case 9: set_leds(led9); break;
        default: break;
    }

}


//delay in the game
void delays (int count){
    for(volatile int i = 0; i < count; i++){
        asm volatile("nop");
    }
}

//get info about the button under the reset button
int get_btn(void) {
  volatile int* toggle = (volatile int*) 0x040000d0;
  return *toggle;

}

//hexadecimal display
void set_displays(int display_number, int value) {
  //let segm7 point to the right diode display
  volatile int *segm7 = (volatile int*) (0x04000050 + (0x00000010*display_number));
  int output = 0;
  //display the right value
  switch (value) {
    case 0:
      output = 0b11000000;
      break;
    case 1:
      output = 0b11111001;
      break;
    case 2:
      output = 0b10100100;
      break;
    case 3:
      output = 0b10110000;
      break;
    case 4:
      output = 0b10011001;
      break;
    case 5:
      output = 0b10010010;
      break;
    case 6:
      output = 0b10000010;
      break;
    case 7:
      output = 0b11111000;
      break;
    case 8:
      output = 0b10000000;
      break;
    case 9:
      output = 0b10010000;
      break;
  }
  *segm7 = output;

}

//Converts the points so the points can be correctly displayed on the hex displays.
void set_points(int points) {

    int temp_points = points;  // Copy points to avoid altering it
    for (int p = 0; p < 6; p++) {
        int digit = temp_points % 10;  // Get the last decimal digit
        set_displays(p, digit);
        temp_points /= 10;  // Remove the last digit
    }
}


//initialize the timers, which will be used for the power up bar
void labinit(void)
{
  volatile unsigned short *periodL = (volatile unsigned short*) 0x04000028;
  volatile unsigned short *periodH = (volatile unsigned short*) 0x0400002c;
 
  *(periodL) = (29999999/10000) & 0xFFFF;
  *(periodH) = (29999999/10000) >> 16;

  //start timer
  volatile unsigned short* ctrl = (volatile unsigned short*) 0x04000024;
  *ctrl = 0b111;
}


//draws the character and create 360 movement for it.
void draw_character(volatile char *VGA, int x, int y , int size, char color){
    for (int row = y; row < y + size; row++) {
        for (int col = x; col < x + size; col++) {
            // Check boundaries to avoid drawing outside the screen
            if (row >= 0 && row < 240 && col >= 0 && col < 320) {
                VGA[row * 320 + col] = color; // Calculate the framebuffer index
            }
        }
    }

}

//get info about switches
int get_sw(void) {
  volatile int* toggle = (volatile int*) 0x04000010;
  return *toggle;
}

//draws the platform 
void draw_platform(volatile char *VGA, int x, int y, int size_x, int size_y, char color) {
    for (int row = y; row < y + size_y; row++) {
        for (int col = x; col < x + size_x; col++) {
            // Check boundaries to avoid drawing outside the screen
            if (row >= 0 && row < 240 && col >= 0 && col < 320) {
                VGA[row * 320 + col] = color; // Calculate the framebuffer index
            }
        }
    }
}

//platform information: thier x-position, y-position and the total length of platform horizontally
struct platform {
    int x_pos;
    int y_pos;
    int length; 
};

//Draws every platform in the platforms array
void draw_all_platforms(struct platform platforms[], int length){
    for(int i = 0; i < length; i++){
        int x = platforms[i].x_pos;
        int y = platforms[i].y_pos;
        int size_x = platforms[i].length;
        //draw the platform using the draw_platform function
        draw_platform(VGA, x, y, size_x, 10,0);
    }
}

//Generates a new platform
void generate_new_platform(struct platform platforms[], int length){
    //remove first elemt and replace with the elements after it in the platform array
    //move all elements in array by one 
    for(int i = 0; i < length - 1; i++ ){
        struct platform temp = platforms[i+1];
        platforms[i] = temp;
    }
    //if we reach the end of the x-coordinate array we will start again form the first
    if (current_platform_coordnr > platform_amount - 1) {
        current_platform_coordnr = 0;
    }
    //take a new x-coordinate from the array
    platforms[length-1].x_pos = platform_coordinates[current_platform_coordnr];
    platforms[length-1].y_pos = 100;
    platforms[length-1].length = 70;

    //make sure we take the next upcoming x-coordinate in the array next time
    current_platform_coordnr ++;
}

//Fills in the background in light blue color
void background(){
    //background coloring 
    for(int i = 0; i < 320 * 240; i++){
        VGA[i] = 87;
    }
}

//this handles the controls of the character
void handle_switch_control(int *x, int y, int size) {
    //get the switch input from the player
    int switches = get_sw();

    //the value when the rightmost switch is pushed
    int right_switch = (switches >> 9) & 0x1;
    //the value when the leftmost switch is pushed
    int left_switch = switches & 0x1;

    //if the rightmost switch is pressed change the x coordinate of the character
    //and if 
    if (right_switch) {
        *x -= 1;
        //change the character's x coordinate when reaching the edges
        if (*x < 0) {
            *x = 320 - size;
        }
    }

    if (left_switch) {
        *x += 1;
        //change the character's x coordinate when reaching the edges
        if (*x + size > 320) {
            *x = 0;
        }
    }
    //draw the character
    draw_character(VGA, x, y, size, 87);
}

//main game program 
int main( void )
{
    //print the instruction to the user
    instruction();

    //Sets the score to 0 on the hexadecimal display
    for (int i = 0; i < 6; i++) {
      set_displays(i, 0);
    }  

    //create leds that light up with time
    labinit();

    //timeout
    volatile unsigned short* timer_TO = (unsigned short*) 0x4000020;
    
    //charachter stats created as local variables
    int x = 160;
    int y = 200;
    int size = 20;
    int color = 0x00;

    //the juming height of the character can be modified here
    int jump_height = 70;

    //Make the background blue
    background();

    //have a certain amount of platforms in the game
    struct platform platforms[amount_platforms];

    //Draw the initial platforms
    platforms[2] = (struct platform){40,100,70};
    platforms[1] = (struct platform){150, 150, 70};
    platforms[0] = (struct platform){120, 200, 70};

    while (1) {

        //Draws all platforms
        draw_all_platforms(platforms, amount_platforms);

        //power up bar that depends on polling from a timer
        if(*timer_TO & 0x1){

            //Sets the leds to the current state
            set_led_state(led_state);
            //increase nr of leds for the next time
            led_state++;

            //if the button is pressed the leds should reset (character should jump faster?) 
            if(get_btn() && led_state >= 9){
                background();
                points += 0b1100;
                while(y > 70 ){
                    y--;
                    draw_character(VGA, x, y, size, 87);
                    draw_all_platforms(platforms, amount_platforms);
                    delays(100);
                } 
                
                set_leds(0b0);
                led_state = -1; 
            } 
        }

        //Jump up
        for (int i = 0; i < jump_height; i++){
            draw_all_platforms(platforms, amount_platforms);
            draw_character(VGA, x, y, size, 87);
            handle_switch_control(&x, y, size);
            y--;
            draw_character(VGA, x, y, size, color);

        
            //specifially when colliding with the platform the platforms should be genereted
            for(int k = 0; k < amount_platforms; k++){
                if(platforms[k].y_pos + 10 == y + size){
                    if(x+(size/2) > platforms[k].x_pos && x+(size/2) < platforms[k].x_pos + platforms[k].length){
                        //draw_all_platforms(platforms, 3);
                        int x = platforms[k].x_pos;
                        int y = platforms[k].y_pos;
                        int size_x = platforms[k].length;
                        draw_platform(VGA, x, y, size_x, 10,0x0);
                    }
                }
            }

            draw_character(VGA, x, y, size, color);
            delays(100000);
        }


        //if this is 1 then there has been a collision
        int collision = 0;

        //Jump down
        for (int i = 0; i < jump_height; i++){
            draw_all_platforms(platforms, amount_platforms);
            draw_character(VGA, x, y, size, 87);
            handle_switch_control(&x, y, size);
            y++;
            draw_character(VGA, x, y, size, color);
            //draw the character at the new position 
            draw_character(VGA, x, y, size, color);
            
            //platform that the charachetr is colliding with
            int platform_nr = 0;

            for (i = 0; i < 3; i++) {
                //checking if y coordinates of platforms align with the characters y-coordinates
                if (platforms[i].y_pos == y + size){
                    if (x+(size/2) > platforms[i].x_pos && x+(size/2) < platforms[i].x_pos + platforms[i].length) {
                        //characters adjustment to the collision
                        y = platforms[i].y_pos - size;
                        collision = 1;
                        platform_nr = i;
                        break;
                    }
                }
                
            }

            //If there was a collision move all platforms down by 40 pixels.
            if(collision) {
                for(int j = 0; j < amount_platforms; j++){
                    draw_platform(VGA, platforms[j].x_pos, platforms[j].y_pos, 70, 10, 87 );
                    platforms[j].y_pos += 40;
                }

                //clear screen ad redraw the platforms
                background();
                draw_all_platforms(platforms, amount_platforms);
                
                // generate new platform if a platform reaches the bottom of the screen
                for (int j = 0; j < amount_platforms; j++) {
                    if (platforms[j].y_pos >= 240) { // Bottom of the screen
                    generate_new_platform(platforms, amount_platforms); // Replace platform
                    }
                }

                //Increase and display the points
                points += 0b1;
                set_points(points);

                break;
            }
            //If the character hits the bottom of teh screen the player dies
            if(y + size == 240) {
                for (int i = 0; i < 320 * 240; i++) {
                VGA[i] = 320; 
                }
                print("\nYou acheived: ");
                print_dec(points);
                print_death_message();
                return 0;
                }
            delays(100000);
        }


        draw_character(VGA, x, y, size, color);
        delays(100000);
    }
    print(points);
    return 0;
}

// int main(void) {
//     // Clock cycles that elapsed.
//     unsigned int game_time;
//     unsigned int game_instr;
//     unsigned int game_i_stall;
//     unsigned int game_d_stall;
//     unsigned int game_data_hazard_stall;
//     unsigned int game_alu_stall;

//     // Clear mcycle CSR
//     asm volatile("csrw mcycle, x0");
//     // Clear minstret CSR
//     asm volatile("csrw minstret, x0");
    
//     // Call the game function
//     game();

//     // Read the mcycle value into game_time
//     asm volatile("csrr %0, mcycle" : "=r" (game_time));
//     // Read the minstret value into game_instr
//     asm volatile("csrr %0, minstret" : "=r" (game_instr));
//     //read the i cache stall counter into game_i_stall
//     asm volatile("csrr %0, mhpmcounter6" : "=r"(game_i_stall));
//     //read the d cache stall counter into game_d_stall
//     asm volatile("csrr %0, mhpmcounter7" : "=r"(game_d_stall));
//     //read from the data hazard stall counter
//     asm volatile("csrr %0, mhpmcounter8" : "=r"(game_data_hazard_stall));
//     //read from the alu stall counter
//     asm volatile("csrr %0, mhpmcounter9" : "=r"(game_alu_stall));
    


//     // Print results
//     print("\nTime/clock cycles for game() was: ");
//     print_dec(game_time);
//     print("\nInstructions for game() were: ");
//     print_dec(game_instr);
//     print("\n");
//     print("\nI-cache stall counter for game() were: ");
//     print_dec(game_i_stall);
//     print("\nD-cache stall counter for game() were: ");
//     print_dec(game_d_stall);
//     print("\nData hazard stall counter for game() were: ");
//     print_dec(game_data_hazard_stall);
//     print("\nALU stall counter for game() were: ");
//     print_dec(game_alu_stall);
    

//     return 0;
// }
//0,5997 for 7 points
//o,5997 for 3 points
//I-cache stall counter for game() were: 9165
//D-cache stall counter for game() were: 227806078
