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

//find absolute value
int abs(int a)
{
    return a>=0 ? a : -a;
}

//print game over and congratulations banner
void print_banner(int type_over)
{
    string print_data = "";
    if (type_over == 1)
        print_data = "Congratulations !!";
    else if (type_over == 2)
        print_data = "Game Over !!";

    Text banner= Text(3.5*LEFT_MARGIN, 0.5*BOTTOM_MARGIN, print_data);//display congratulation or game over

    wait(2);
}
//collision of bubble and shooter
bool collision_bubble_shooter(vector<Bubble> &bubbles , Shooter &shooter)
{
    int x_shooter = shooter.get_head_center_x();  //x coordinate of shooter head
    int y_shooter = shooter.get_head_center_y();   //y coordinate of shooter head
    int x_bubbles,y_bubbles;
    for(unsigned int j=0; j<bubbles.size(); j++)
    {
        bubbles[j].get_coordinate_bubble(x_bubbles,y_bubbles);
        bool y_near = y_bubbles +BUBBLE_DEFAULT_RADIUS >= y_shooter;   //check if bubble and shooter are within a near of each other
        bool x_near = abs(x_shooter - x_bubbles) < 2*BUBBLE_DEFAULT_RADIUS;
        if(x_near && y_near)
        {
            return true;
        }
    }
    return false;
}

void add_smaller_bubbles(Bubble& b, vector<Bubble> &bubbles)
{
    // create smaller bubble  in the game
    int x=0, y=0;
    int radius=b.get_radius()/2;
    int vx= b.get_vx();
    int vy= b.get_vy();
    b.get_coordinate_bubble(x,y);
    bubbles.push_back(Bubble(x, y, radius, -vx, vy, COLOR(255,105,180)));
    bubbles.push_back(Bubble(x, y, radius, vx, vy, COLOR(255,105,180)));
}

int collision_bullet_bubble(vector<Bubble> &bubbles ,vector<Bullet> &bullets)
{
    // create  collision between bullet and bubble
    int counter_score=0;
    int x_bullet ,y_bullet;
    int x_bubbles,y_bubbles;
    vector<Bubble> b;
    for(unsigned int i=0; i<bullets.size(); i++)
    {
        bullets[i].get_coordinate_bullet(x_bullet,y_bullet);
        for(unsigned int j=0; j<bubbles.size(); j++)
        {
            bubbles[j].get_coordinate_bubble(x_bubbles,y_bubbles);
            bool x_near = abs(x_bubbles-x_bullet) <= BUBBLE_DEFAULT_RADIUS; // check if bullet and bubble are within a range of each other in x coordinate
            bool y_near = abs(y_bubbles-y_bullet) <= BUBBLE_DEFAULT_RADIUS;  // check if bullet and bubble are within a range of each other in y coordinate
            if ( x_near && y_near  )
            {
                //save info of bigger bubbles before deleting, to be used for creation of smaller bubbles
                if (bubbles[j].get_radius() > BUBBLE_DEFAULT_RADIUS*0.5)
                {
                    b.push_back(bubbles[j]);
                }
                bullets.erase(bullets.begin()+i);
                bubbles.erase(bubbles.begin()+j);
                counter_score++;
            }
        }
    }
     for(int i=0; i<b.size(); ++i)
        add_smaller_bubbles(b[i], bubbles);
    return counter_score;
}

void move_bullets(vector<Bullet> &bullets){
    // move all bullets
    for(unsigned int i=0; i<bullets.size(); i++){
        if(!bullets[i].nextStep(STEP_TIME)){
            bullets.erase(bullets.begin()+i);  //erase bullet if it move out of canvas
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

    // Intialize the shooter object
    Shooter shooter(SHOOTER_START_X, SHOOTER_START_Y, SHOOTER_VX);

    // Initialize the bubbles with 2 bubbles objects
    vector<Bubble> bubbles = create_bubbles();

    // Initialize the bullets (empty)
    vector<Bullet> bullets;

    // capture keyboard events
    XEvent event;

    string msg_cmd("Cmd: _");
    Text charPressed(LEFT_MARGIN, BOTTOM_MARGIN, msg_cmd);

    string score("Score :  ");
    Text score_box(430,BOTTOM_MARGIN, score);

    double sum_time = 0.0, gap = 1.0; //in seconds
    int timer=0, time_limit = 50; // in seconds
    bool is_time_over = false, is_health_over = false;

    string time("Time : 00/00");
    time[time.length()-1]='0'+time_limit%10;
    time[time.length()-2]='0'+(time_limit/10)%10;

    Text time_box(LEFT_MARGIN, 10, time);

    int counter_collosion=1 ,limit_collsion=1;

    string health="Health : 0/0";
    health[health.length()-1]='0'+limit_collsion;
    health[health.length()-3]= '0'+counter_collosion;

    Text health_box(430, 10, health);

    bool previous_collsion=false;

    int score_value=0;

    // Main game loop
    while (true)
    {
        sum_time=sum_time+ STEP_TIME;

        if (sum_time >= gap) // gap = 1 sec
        {
            timer++; // increase for every 1 sec
            time[time.length()-4]='0'+timer%10;
            time[time.length()-5]='0'+(timer/10)%10;

            time_box.setMessage(time); // update on screen

            if (timer > time_limit)
            {
                is_time_over = 1;
            }
            sum_time = 0; // reset timer count to count next 1 sec
        }

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
        score_value = score_value + collision_bullet_bubble(bubbles , bullets);
        score[score.length()-1]= '0'+(score_value%10); // assuming score_value<99
        score[score.length()-2]= '0'+(score_value/10);
        score_box.setMessage(score);

        //Game over message
        if (bubbles.size() == 0)
        {
            bullets.clear();
            print_banner(1);
            break;
        }

        if (collision_bubble_shooter(bubbles ,shooter))
        {
            if(previous_collsion==false) // for first touch only, it counts
            {
                counter_collosion--;  //health reduces if shooter collides with bubble
                health[health.length()-3]= '0'+counter_collosion;
                health_box.setMessage(health);
                previous_collsion =true;
            }
            if(counter_collosion ==0) // game over if health reduceds to zero
            {
                is_health_over = true;
            }
        }
        else // when bubbles stopped touching shooter, reset
        {
            previous_collsion=false;
        }

        if ( is_time_over || is_health_over )
        {
            print_banner(2);
            break;
        }
        wait(STEP_TIME);
    }
}
