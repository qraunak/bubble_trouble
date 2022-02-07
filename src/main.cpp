#include <simplecpp>

#include "shooter.h"
#include "bubble.h"

/* Simulation Vars */
const double STEP_TIME = 0.02;

/* Game Vars */
//const int PLAY_Y_HEIGHT = 450; //remove here
const int LEFT_MARGIN = 70;
const int TOP_MARGIN = 20;
const int BOTTOM_MARGIN = (PLAY_Y_HEIGHT+TOP_MARGIN);

int abs(int a)
{
    return a>=0 ? a : -a;
}

void print_congratulation()
{
    string congratulation("Congratulations !!");
    Text charPress(3.5*LEFT_MARGIN, 0.5*BOTTOM_MARGIN, congratulation);
    charPress.setMessage(congratulation);
    wait(2);
}

bool collision_bubble_shooter(vector<Bubble> &bubbles , Shooter &shooter)
{
    int x_shooter = shooter.get_head_center_x();
    int y_shooter = shooter.get_head_center_y();
    int x_bubbles,y_bubbles;
    for(unsigned int j=0; j<bubbles.size(); j++)
    {
        bubbles[j].get_coordinate_bubble(x_bubbles,y_bubbles);
        bool y_near = y_bubbles >= y_shooter;
        bool x_near = abs(x_shooter - x_bubbles) < 2*BUBBLE_DEFAULT_RADIUS;
        if(x_near && y_near)
        {
            string game_over("Game Over");
            Text charPress(3.5*LEFT_MARGIN, 0.5*BOTTOM_MARGIN, game_over);
            charPress.setMessage(game_over);
            wait(5);
            return true;
        }
    }
    return false;
}

void collision_bullet_bubble(vector<Bubble> &bubbles ,vector<Bullet> &bullets)
{
    int x_bullet ,y_bullet;
    int x_bubbles,y_bubbles;
    for(unsigned int i=0; i<bullets.size(); i++)
    {
        bullets[i].get_coordinate_bullet(x_bullet,y_bullet);
        for(unsigned int j=0; j<bubbles.size(); j++)
        {
            bubbles[j].get_coordinate_bubble(x_bubbles,y_bubbles);
            bool x_near = abs(x_bubbles-x_bullet) <= BUBBLE_DEFAULT_RADIUS;
            bool y_near = abs(y_bubbles-y_bullet) <= BUBBLE_DEFAULT_RADIUS;
            if ( x_near && y_near)
            {
                bullets.erase(bullets.begin()+i);
                bubbles.erase(bubbles.begin()+j);
            }
        }
    }
}

void move_bullets(vector<Bullet> &bullets){
    // move all bullets
    for(unsigned int i=0; i<bullets.size(); i++){
        if(!bullets[i].nextStep(STEP_TIME)){
            bullets.erase(bullets.begin()+i);
        }
    }
}

void move_bubbles(vector<Bubble> &bubbles){
    // move all bubbles
    for (unsigned int i=0; i < bubbles.size(); i++)
    {
        bubbles[i].nextStep(STEP_TIME);
    }
}

vector<Bubble> create_bubbles()
{
    // create initial bubbles in the game
    vector<Bubble> bubbles;
    bubbles.push_back(Bubble(WINDOW_X/2.0, BUBBLE_START_Y, BUBBLE_DEFAULT_RADIUS, -BUBBLE_DEFAULT_VX, 50, COLOR(255,105,180)));
    bubbles.push_back(Bubble(WINDOW_X/4.0, BUBBLE_START_Y, BUBBLE_DEFAULT_RADIUS, BUBBLE_DEFAULT_VX, 50, COLOR(255,105,180)));
    return bubbles;
}


int main()
{
    initCanvas("Bubble Trouble", WINDOW_X, WINDOW_Y);

    Line b1(0, PLAY_Y_HEIGHT, WINDOW_X, PLAY_Y_HEIGHT);
    b1.setColor(COLOR(0, 0, 255));

    string msg_cmd("Cmd: _");
    Text charPressed(LEFT_MARGIN, BOTTOM_MARGIN, msg_cmd);

    // Intialize the shooter
    Shooter shooter(SHOOTER_START_X, SHOOTER_START_Y, SHOOTER_VX);

    // Initialize the bubbles
    vector<Bubble> bubbles = create_bubbles();

    // Initialize the bullets (empty)
    vector<Bullet> bullets;

    XEvent event;

    // Main game loop
    while (true)
    {
        bool pendingEvent = checkEvent(event);
        if (pendingEvent)
        {
            // Get the key pressed
            char c = charFromEvent(event);
            msg_cmd[msg_cmd.length() - 1] = c;
            charPressed.setMessage(msg_cmd);

            // Update the shooter
            if(c == 'a')
                shooter.move(STEP_TIME, true);
            else if(c == 'd')
                shooter.move(STEP_TIME, false);
            else if(c == 'w')
                bullets.push_back(shooter.shoot());
            else if(c == 'q')
                return 0;
        }

        // Update the bubbles
        move_bubbles(bubbles);

        // Update the bullets
        move_bullets(bullets);

        // Check for collisiona and delete if collided
        collision_bullet_bubble(bubbles , bullets);

        //Game over message
        if (bubbles.size() == 0)
        {
            bullets.clear();
            print_congratulation();
            break;
        }
        bool is_over = collision_bubble_shooter(bubbles ,shooter);
        if (is_over)
            break;

        wait(STEP_TIME);
    }
}
