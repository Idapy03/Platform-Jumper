extern void enable_interrupt();

void set_leds(int led_mask) {
  volatile int *leds = (volatile int*) 0x04000000;

  *leds = led_mask;
}

void delays (int count){
    for(volatile int i = 0; i < count; i++){
        asm volatile("nop");
    }
}

volatile const int led_delay = 100;
volatile int led_state = 0;
volatile int led_timer = 0;
volatile char *VGA = (volatile char *)0x08000000; // Base framebuffer address

//get info about the button under the reset button
int get_btn(void) {
  volatile int* toggle = (volatile int*) 0x040000d0;
  return *toggle;

}

void handle_interrupt(unsigned cause     ) {
    // Handle the interrupt logic here
    
    //type of interrupt
    int interrupt = cause & 0x3FFFFFFF;  // Mask with 30 bits set

    //led lamps that increment the binary value displayed on 
    //the first 4 LED
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

    if(interrupt == 16){  
        led_timer++;

        const int charging_delay = -100;

        if(led_timer >= led_delay){

        switch (led_state)
        {
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
        
        if(get_btn() && led_state >= 9){
            set_leds(0b0);
            led_timer = charging_delay;
            led_state = -1; 
        }

        led_state++;   
        if(led_timer < 0){
            led_timer = led_timer + 10;
        }
        else{
            //reset timer
            led_timer=0;
        }
        }
    }
}


void labinit(void)
{
  volatile unsigned short *periodL = (volatile unsigned short*) 0x04000028;
  volatile unsigned short *periodH = (volatile unsigned short*) 0x0400002c;
 
  *(periodL) = (29999999/1000) & 0xFFFF;
  *(periodH) = (29999999/1000) >> 16;

  
  volatile unsigned short* ctrl = (volatile unsigned short*) 0x04000024;
  *ctrl = 0b111;

    // Enable interrupt for the timer
    volatile unsigned short* interruptmask = (volatile unsigned short*) 0x04000018;
    *interruptmask = 0b10;
  
  enable_interrupt();
}


void draw_rectangle(volatile char *VGA, int x, int y, int size, char color) {
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

struct platform {
    int x_pos;
    int y_pos;
    int length; 
};

// void generete_platforms(arr[]){
//     int h = 10;
//     int w = 40;

//     for(int i = 0; i < 10; i++){
//     }
// }

int main( void )
{
    
    //create leds that light up with time
    labinit();

     int x = 160;
    int y = 180;
    int size = 20;
    int color = 0x00;

    for(int i = 0; i < 320 * 240; i++){
        VGA[i] = 87;
    }

    struct platform platforms[3];

    //Ground
    draw_platform(VGA, 0, 200, 320, 40, 0x78);
    platforms[0] = (struct platform){0, 200, 320};

    draw_platform(VGA, 40, 150, 70, 10, 0x00);
    platforms[1] = (struct platform){40,150,70};

    draw_platform(VGA, 200, 100, 70, 10, 0x00);
    platforms[2] = (struct platform){200, 100, 70};

    while (1) {
        int collision = 0;
        //Jump up
        for (int i = 0; i < 100; i++){
            draw_rectangle(VGA, x, y, size, 87);
                if (get_sw()) {
                long long switches = get_sw();

                int right_switch = (switches >> (9)) & 0x1;
                int left_switch = (switches) & 0x1;

                if (right_switch) {
                    draw_rectangle(VGA, x, y, size, 87);
                    if (x != 0) {
                        x -= 1;
                    }
                }
    
                if (left_switch) {
                    draw_rectangle(VGA, x, y, size, 87);
                    if (x + size != 320) {
                        x += 1;
                    }

                }

            }
            y--;
            draw_rectangle(VGA, x, y, size, color);
            delays(10000);
        }

        //Jump down
        for (int i = 0; i < 1000; i++){
            draw_rectangle(VGA, x, y, size, 87);
            if (get_sw()) {
                long long switches = get_sw();

                int right_switch = (switches >> (9)) & 0x1;
                int left_switch = (switches) & 0x1;

                if (right_switch) {
                    draw_rectangle(VGA, x, y, size, 87);
                    if (x != 0) {
                        x -= 1;
                    }
                }
    
                if (left_switch) {
                    draw_rectangle(VGA, x, y, size, 87);
                    if (x + size != 320) {
                        x += 1;
                    }
                }
            }
            y++;
            draw_rectangle(VGA, x, y, size, color);

            for (i = 0; i < 3; i++) {
                if (platforms[i].y_pos == y + size){
                    if (x+(size/2) > platforms[i].x_pos && x+(size/2) < platforms[i].x_pos + platforms[i].length) {
                        y = platforms[i].y_pos - size;
                        collision = 1;
                        break;
                    }
                }
                
            }
            if(collision) {
                break;
            }
            if(y + size == 240) {
            for (int i = 0; i < 320 * 240; i++) {
                VGA[i] = 0x0E; // Black color index
            }
            print("You died");
            return 0;
            }
            delays(10000);
        }


        draw_rectangle(VGA, x, y, size, color);
        delays(5000);
    }
    
    return 0;
}
