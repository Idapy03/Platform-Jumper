# Not-IceCreamJump

## Downloading specifications
### Basic software for the board
1. In order to play the game you first have to download dtekv-tools (if you are on Windows/Linux) and connect the board to your computer from Canvas, which you can do after you have followed the software(installation) guide under software resources. 
2. Afterwards, install the time4iriscv.zip file under the first laboratory exercise on Canvas and unpack (unzip) the contents inside dtekv-tools (if on Windows/Linux) that was previously downloaded. This is all needed to create a main.bin file, which will be the main file run on the dtekv-board.
3. After these steps download the labmain.zip file (that was handed in together with this file).
### Run the program on VGA
4. To play the game on a screen you have to connect the de10-lite board with a VGA cable (either with a VGA to VGA with a monitor that supports it or VGA to HDMI, make sure you have the right direction VGA -> HDMI, cable on a school monitor).
5. If everything is correctly connected and downloaded, then create the main.bin file by using the command 'make' in the time4riscv folder. Use the './dtekv-run time4riscv/main.bin' command to start the game.


## Objective and Requirements 
The project aims to create a game inspired by other games in the ‘platform jumping’ genre, ig Doodle Jump and Ice Cream Jump. The player controls a constantly jumping character and is trying to jump onto platforms to reach greater heights. The lateral movement of the character is controlled through the switches. Setting the leftmost switch to 1, while the rightmost switch is turned to 0, should change the direction of the character's movement to the left and if the right switch is instead pushed to 1, while the leftmost switch is set to 0, the character should move to the right. If the character falls off all platforms, the game will be over and the highest reached height of that round is displayed in the terminal or on the hexadecimal display. 
- The program must translate the movement of switches from the de10-lite board to the movement of the character.
- The program ‘scrolls down’ a set height as the player hits a platform.
- The ‘power up’ bar will fill up during the game represented by the lighting up of leds. When the bar is full, the player can press a button on the DE10-lite board to get a boost which propels them upward for a set distance.
- The screen is “cylindrical” which means that when the character jumps out the left part of the screen it will show up in the right part of the screen and vice versa.
- The score should be visible to the player. Every time the player hits a platform it should increase the score.
The following features are optional:
- Let the ‘power up’ bar fill up every time the player collects an item in the game. 
- The score will be shown on the 7-segment hex display.
- The highscores are stored in a document.
- Create a cloudy background that scrolls with the platforms.
- Create breakable platforms which are destroyed after the player jumps on them.

Since we aim to do the advanced project we will also conduct a performance analysis towards the end.

## Solution 
The program was developed in VSCode in the programming language C. The program was developed in parts: character, platform and background creation; controls of the character; scrolling of platforms; generating new platforms endlessly; adding a ‘cylindrical’ screen for the character only; creating the powerup function; displaying the points and fixing bugs. 

The game used the VGA screen buffer for generating the background, the platforms and the character. The background was implemented by letting every pixel on the VGA screen hold the same value, which was 87 (blue). The character’s appearance on the screen was similarly implemented but was limited to a square with a certain size starting on a certain y- and x-coordinate and was chosen to have the color black. These important details of the character were stored as local variables in the main function. For the 360 rotation if conditions were added, which made it so the character would appear at the opposite side of the screen if it hit one of the sides. The platforms were implemented using a struct with x-position, y-position and length as attributes. The platforms were then stored in an array with the size 3 which means that 3 platforms can exist at the same time. Then a draw_platform function was created similarly to the one which draws the character. New platform generation was implemented in steps. First, an array with x positions was created, this was used to determine the x-coordinate of the new platforms. When a platform reached the bottom of the screen it would be removed from the platform's array and replaced by a new platform using the x-coordinate from the array of x-coordinates. If the game had gone through all x-coordinates in the array then the platform's position would resume from the first element. This was to ensure that the game had an endless generation of platforms. If the position of the platform was entirely randomized the player might not be able to reach that platform.

The program was developed in GitHub with both members working on their issues, branches and creating pull requests. The last step of the programming also involved fixing bugs, like glitching platforms and improving the clarity of the code as well as adding comments.





## Verification 
The program was verified through the use of the DTEK-V emulator (see appendix) where the program was thoroughly tested for bugs and performance issues. The program has also been tested on a VGA screen using the DTEK-V board to verify that the program will work well on an actual monitor as well. The boosters, the score calculation and platform reachability were also verified in the game.


## Contributions
The program was developed in parts where in the beginning Ida worked on the display and generating background as well as drawing the characters and platforms. Linn at this time worked mostly with the powerup functions, the points and displaying them on the leds and the hex display. Next, Ida developed the movement of the character, such as jumping mechanisms as well as the side-to-side movement with the switches. Linn worked on the platform generation as well as the 360 movement of the character. Toward the end of the project, both of us worked on finding and fixing bugs as well as optimizing the performance of the program.


## Reflections
The project as a whole provided some good practice with applying previous knowledge, such as polling switches, timers and working with the hex display etc., to a real project. When it comes to workflow, we had some issues in the beginning since we initially wanted to use tilting mechanics for the movement of the character, which, in the end, we decided against. However, we were able to find another solution which worked just as well. Otherwise, the workflow worked well since we could easily cooperate using GitHub and its many functions. The biggest improvement to be made for another project would be to prototype the idea a bit more since we had so many problems with that particular step.

