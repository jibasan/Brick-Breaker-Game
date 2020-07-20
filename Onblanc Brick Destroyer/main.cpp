#include "game.h"

string scoring(int x){ //Converts the int value to a string value
    stringstream ss;
    ss << x;
    return ss.str( );
}

static void key(unsigned char key, int x, int y){
    if (key == ' ' && lives != 0) {
        ballSet->speed = 10;
        gameStart = true;
    }
    if (key == 'r') {  //press r to restart  the level
        firstStage->bars.clear();
        stageOne();
        ballSet->speed = 0;
        ballSet->x = (paddleSet->x + (paddleSet->w / 2));
        ballSet->y = (paddleSet->y + paddleSet->h + 4);
        ballSet->dirY = fabs(ballSet->dirY);
        ballSet->dirX = 0;
        lives = 3;
        score = 0;
    }
    glutPostRedisplay();
}

void screenText(float x, float y, const char* text){
    const char *c;
    glRasterPos2f(x, y);
    for (c = text; *c != '\0'; c++) {
    glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, *c);
    }
}

void render2D(int width, int height){
    glViewport(0, 0, width, height);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0.0f, width, 0.0f, height, 0.0f, 1.0f);
    glMatrixMode (GL_MODELVIEW);
    glLoadIdentity();
}

void mouseButton(int button, int state, int x, int y){
}

void moveMouse(int x, int y){
    if (x < (paddleSet->w / 2)) {
        paddleSet->x = 0;
    } else if ( x > (width - (paddleSet->w / 2))) {
        paddleSet->x = (width - (paddleSet->w));
    } else {
        paddleSet->x = x - (paddleSet->w / 2);
    }
}

void ballUpdate(){
    ballSet->x += ballSet->dirX * ballSet->speed;
    ballSet->y += ballSet->dirY * ballSet->speed;
    
    if (paddleSet->ifHit(ballSet)) {
        float t = ((ballSet->x - paddleSet->x) / paddleSet->w) - 0.5f;
        ballSet->dirY = fabs(ballSet->dirY);
        ballSet->dirX = t;
    }
    int blocksLeft = 0;
    for (vector<bar*>::iterator i = firstStage->bars.begin(); i != firstStage->bars.end(); ++i) {
        if ((*i)->ifHit(ballSet)) {
            if ((*i)->health != 0) {
                float t = ((ballSet->x - (*i)->x) / (*i)->w) - 0.5f;
                float z = ((ballSet->y - (*i)->y) / (*i)->h) - 0.5f;
                if (ballSet->dirY < 0) {
                    ballSet->dirY = -z;
                } else {
                    ballSet->dirY = z;
                }
                
                ballSet->dirX = t;
                if (oneHit) {
                   (*i)->health = 0;
                }
                if ((*i)->health == 0) {
                    score += 100;
                }
            }
        }
        if ((*i)->health != 0) {
            blocksLeft++;
        }
    }
    
    //If Left/Right/Top/Bottom Wall (in that order) have been hit
    if (ballSet->x < 0) {
        ballSet->dirX = fabs(ballSet->dirX);
    }
    if (ballSet->x > width) {
        ballSet->dirX = -fabs(ballSet->dirX);
    }
    if (ballSet->y > height) {
        ballSet->dirY = -fabs(ballSet->dirY);
    }
    if (ballSet->y < 0) {
        --lives;
        ballSet->speed = 0;
        ballSet->x = (paddleSet->x + (paddleSet->w / 2));
        ballSet->y = (paddleSet->y + paddleSet->h + 4);
        ballSet->dirY = fabs(ballSet->dirY);
        ballSet->dirX = 0;
    }
}

void update(int value){
    ballUpdate();
   glutTimerFunc(interval, update, 0);
   glutPostRedisplay();
}

void draw(){ //draws out the objects on the screen
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();
    if (lives > 0) {
        paddleSet->draw();
        ballSet->draw();
        firstStage->draw();
    } else { //resets the stage
        firstStage->bars.clear();
        ballSet->speed = 0;
        ballSet->x = (paddleSet->x + (paddleSet->w / 2));
        ballSet->y = (paddleSet->y + paddleSet->h + 4);
        ballSet->dirY = fabs(ballSet->dirY);
        ballSet->dirX = 0;
        gameStart = false;
        lives = 3;
        score = 0;
    }
    //the amount of lives displayed
    glColor3f(1.0f, 1.0f, 1.0f);
    screenText(7, height - 15,
         ("" + scoring(lives) + "                                                                                             Score: " + scoring(score)).c_str());
    glutSwapBuffers();
}

int main(int argc, char** argv){
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(width, height);
    glutCreateWindow("Onblanc Block Destroyer");
    glutKeyboardFunc(key);
    glutMouseFunc(mouseButton);
    glutMotionFunc(moveMouse);
    glutDisplayFunc(draw);
    glutTimerFunc((1000 / 60), update, 0);
    
    //renders stage one on the screen
    stageOne();
    render2D(width, height);
    
    glutMainLoop();
    return 0;
}

