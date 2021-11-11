#include "stdio.h"
#include "stdlib.h"
#include "sdl.h"
#include "SDL2_gfxPrimitives.h"
#include "time.h"

#include "formulas.h"
#include "wall.h"
#include "robot.h"
#include "math.h"

int max_speed_maintain = 0;
int done = 0;
int robot_status = 0;

int main(int argc, char *argv[]) {
    SDL_Window *window;
    SDL_Renderer *renderer;

    if(SDL_Init(SDL_INIT_VIDEO) < 0){
        return 1;
    }

    window = SDL_CreateWindow("Robot Maze", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, OVERALL_WINDOW_WIDTH, OVERALL_WINDOW_HEIGHT, SDL_WINDOW_OPENGL);
    window = SDL_CreateWindow("Robot Maze", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, OVERALL_WINDOW_WIDTH, OVERALL_WINDOW_HEIGHT, SDL_WINDOW_OPENGL);
    renderer = SDL_CreateRenderer(window, -1, 0);

    struct Robot robot;
    struct Wall_collection *head = NULL;
    int front_left, front_right, left_forward, left_behind, front_range = 0;
    clock_t start_time, end_time;
    int msec;

    // SETUP MAZE
    // You can create your own maze here. line of code is adding a wall.
    // You describe position of top left corner of wall (x, y), then width and height going down/to right
    // Relative positions are used (OVERALL_WINDOW_WIDTH and OVERALL_WINDOW_HEIGHT)
    // But you can use absolute positions. 10 is used as the width, but you can change this.
     insertAndSetFirstWall(&head, 2,  220, 400, 10, 80);
    insertAndSetFirstWall(&head, 2,  20, 400, 200, 10);
    insertAndSetFirstWall(&head, 2,  20, 50, 10, 350);
    insertAndSetFirstWall(&head, 2,  20, 50, 280, 10);
    insertAndSetFirstWall(&head, 2,  300, 50, 10, 100);
    insertAndSetFirstWall(&head, 2,  300, 150, 110, 10);
    insertAndSetFirstWall(&head, 2,  400, 50, 10, 100);
    insertAndSetFirstWall(&head, 2,  400, 50, 220, 10);
    insertAndSetFirstWall(&head, 2,  620, 50, 10, 290);
    insertAndSetFirstWall(&head, 2,  620, 340, 20, 10);

    insertAndSetFirstWall(&head, 1,  320, 300, 10, 180);
    insertAndSetFirstWall(&head, 2,  120, 300, 200, 10);
    insertAndSetFirstWall(&head, 2,  120, 150, 10, 150);
    insertAndSetFirstWall(&head, 2,  120, 150, 80, 10);
    insertAndSetFirstWall(&head, 2,  200, 150, 10, 100);
    insertAndSetFirstWall(&head, 2,  200, 250, 310, 10);
    insertAndSetFirstWall(&head, 2,  500, 150, 10, 100);
    insertAndSetFirstWall(&head, 2,  500, 150, 10, 100);
    insertAndSetFirstWall(&head, 2,  500, 150, 20, 10);
    insertAndSetFirstWall(&head, 2,  520, 150, 10, 290);
    insertAndSetFirstWall(&head, 2,  520, 440, 120, 10);

    setup_robot(&robot);
    updateAllWalls(head, renderer);

    SDL_Event event;
    while(!done){
        SDL_SetRenderDrawColor(renderer, 200, 200, 200, 255);
        SDL_RenderClear(renderer);

        //Move robot based on user input commands/auto commands

        if (robot.auto_mode == 1) {
            int speed = returnCurrentSpeed(&robot);
            if (speed >= 7)
                max_speed_maintain += 1;
            else{
                max_speed_maintain = 0;
            }
            int robot_status = robotAutoMotorMove(&robot, front_left, front_right, left_forward, left_behind, front_range, robot_status, max_speed_maintain);
            printf("\n");
        }
        robotMotorMove(&robot);

        //Check if robot reaches endpoint. and check sensor values
     if (checkRobotReachedEnd(&robot, 640, 340, 10, 100)){
            end_time = clock();
            msec = (end_time-start_time) * 1000 / CLOCKS_PER_SEC;
            robotSuccess(&robot, msec);
        }
        else if(checkRobotHitWalls(&robot, head))
            robotCrash(&robot);
        //Otherwise compute sensor information
        else {
            front_left = checkSensorFrontLeft(&robot, head);
               /* if (front_left > 0)
                printf("Front Left Score: %d\n", front_left); */

            front_right = checkSensorFrontRight(&robot, head);
              /* if (front_right > 0)
                printf("Front Right Score: %d\n", front_right); */

            left_forward = checkSensorLeftFront(&robot, head);
              /* if (left_forward>0)
                printf("Left Forward. Score = %d\n", left_forward); */

            left_behind = checkSensorLeftBehind(&robot, head);
              /* if (left_behind>0)
                printf("Left Behind. Score = %d\n", left_behind); */

            front_range = checkSensorFrontLongRange(&robot, head);
             /* if (front_range > 0)
                printf("Front Range: %d left\n", front_range); */
        }
        robotUpdate(renderer, &robot);
        updateAllWalls(head, renderer);

        // Check for user input
        SDL_RenderPresent(renderer);
        while(SDL_PollEvent(&event)){
            if(event.type == SDL_QUIT){
                done = 1;
            }
            const Uint8 *state = SDL_GetKeyboardState(NULL);
            if(state[SDL_SCANCODE_UP] && robot.direction != DOWN){
                robot.direction = UP;
            }
            if(state[SDL_SCANCODE_DOWN] && robot.direction != UP){
                robot.direction = DOWN;
            }
            if(state[SDL_SCANCODE_LEFT] && robot.direction != RIGHT){
                robot.direction = LEFT;
            }
            if(state[SDL_SCANCODE_RIGHT] && robot.direction != LEFT){
                robot.direction = RIGHT;
            }
            if(state[SDL_SCANCODE_SPACE]){
                setup_robot(&robot);
            }
            if(state[SDL_SCANCODE_RETURN]){
                // robot auto start
                robot.auto_mode = 1;
                start_time = clock();
            }
        }

        SDL_Delay(30); // originally 120
    }
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    printf("DEAD\n");
}
