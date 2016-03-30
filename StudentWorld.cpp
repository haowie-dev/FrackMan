#include "StudentWorld.h"
#include <string>
using namespace std;

// Students:  Add code to this file (if you wish), StudentWorld.h, Actor.h and Actor.cpp

GameWorld* createStudentWorld(string assetDir)
{
	return new StudentWorld(assetDir);
}


StudentWorld :: StudentWorld(string assetDir): GameWorld(assetDir)
{
    count = 0;
    barrelcount = 0;
    protestcount = 1;
}


StudentWorld :: ~StudentWorld()
{
    for (int k = 0; k < 64; k++)
    {
        for (int j = 0; j < 60; j++)
        {
            deleteDirt(k, j);
        }
    }
    
    delete player;
    
    for (int k = 0; k < actors.size(); k++)
    {
        delete actors[k];
    }
    actors.clear();
    
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////// Main Functions //////////////////////////////////////////

int StudentWorld :: init()
{
    player = new FrackMan(30,60, this);
    for (int k = 0; k < 64; k++) // fills the field with dirt
    {
        for (int j = 0; j < 60; j++)
        {
            if (j < 4)                // creates a mineshaft in the middle
            dirty[k][j] = new Dirt(k, j, this);
            else if (k < 30 || k > 33)
            dirty[k][j] = new Dirt(k, j, this);
            else
            dirty[k][j] = nullptr;
        }
    }
    
    int level = getLevel();  // use current level
    
    int B = min(level / 2 + 2, 6);
    int G = max(level / 2, 2);
    int L = min(2 + level, 20);
    
    int x,y = 0;
    
       for (int k = 0; k < B;k++)
    {
        validCoord(x, y);
        new Boulder(x,y,this);
    }
  
    for (int k = 0; k < L;k++)
    {
        validCoord(x, y);
        new Barrel(x,y,this);
    }

    for (int k = 0; k < G;k++)
    {
        validCoord(x, y);
        Gold* goldy = new Gold(x,y,this);
        goldy->makeFresh();
    }
    if (rand() % 4 +1 >0)
        new HardCoreProtester(this);
    else
        new Protester(IID_PROTESTER, this);

    
    return GWSTATUS_CONTINUE_GAME;
}


int StudentWorld:: move()
{
    setDisplayText();
    
    if (player->isDead())
    {
        decLives();
        playSound(SOUND_PLAYER_GIVE_UP);
        return GWSTATUS_PLAYER_DIED;
    }
    
    player->doSomething();
    for (int k = 0; k < actors.size(); k++)
    {
        actors[k]->doSomething();
    }
    
    if (barrelcount == 0)
    {
        return GWSTATUS_FINISHED_LEVEL;
    }
    
    int level = getLevel();
    int G = level * 25 + 300;
    int x,y = 0;
    
    if ((rand()% G + 1) == 3)
    {
        if ((rand()% 5+1) == 2)
            new Sonar(this);
        else
        {
            openCoord(x, y);
            new Pool(x,y,this);
        }
            
    }
    
    int T = max(25, 200 - level);
    int P = min((int)1.5*level+2,15);
    int H = min(90, level* 10 + 30);
    
    if (count >= T && protestcount < P)
    {
        if (rand() % H + 90 == 92)
            new HardCoreProtester(this);
        else
            new Protester(IID_PROTESTER, this);
        protestcount++;
        count = -1;
    }
    
    vector<Actor*>::iterator next = actors.begin();
    while (next != actors.end())
    {
        if ((*next)->isDead())
        {
            delete (*next);
            actors.erase(next);
            
        }
        else
            next++;
    }
    
    
    count++;
    return GWSTATUS_CONTINUE_GAME;
}


void StudentWorld:: cleanUp()
{
    
    for (int k = 0; k < 64; k++)
    {
        for (int j = 0; j < 60; j++)
        {
            deleteDirt(k, j);
        }
    }
    
    player->Actor::getsAnnoyed(player->getHealth()-10);
    
    delete player;
    player = nullptr;
    
    for (int k = 0; k < actors.size(); k++)
    {
        delete actors[k];
    }
    actors.clear();

    count = 0;
    barrelcount = 0;
    protestcount = 1;
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////// Helping Functions ////////////////////////////////////////////////

void StudentWorld:: setDisplayText() {
    int score = getScore();
    int level = getLevel();
    int lives = getLives();
    int health = player->getHealth();
    int squirts = player->getWater();
    int gold = player->getGold();
    int sonar = player->getSonar();
    int barrelsleft = barrelcount;

    string s = makePretty(score, level, lives, health, squirts, gold, sonar, barrelsleft);

    setGameStatText(s); // calls our provided GameWorld::setGameStatText
}

void StudentWorld:: scan()
{
    for (int k= 0; k < actors.size();k++)
    {
        if (distance(player->getX(),player->getY(), actors[k]->getX(), actors[k]->getY())<= 12)
        {
            actors[k]->setVisible(true);
        }
    }
    playSound(SOUND_SONAR);
}



void StudentWorld:: addAct(Actor* acty)
{
    actors.push_back(acty);
}

bool StudentWorld:: deleteDirt(int x,int y)
{
    if (dirty[x][y] != nullptr)
    {
        delete dirty[x][y];
        dirty[x][y] = nullptr;
        return true;
    }
    return false;
}

bool StudentWorld:: getDirt(int x, int y) const
{
    if (dirty[x][y] == nullptr)
        return false;
    return true;
}

bool StudentWorld:: getBoulder(int x, int y) const
{
    for (int z = 0; z < actors.size(); z++)
    {
        if (actors[z]->cantPass())
        {
            if (x >= actors[z]->getX()  && x < actors[z]->getX()+4 && y >= actors[z]->getY() && y < actors[z]->getY()+4)
                return true;
        }
    }
    return false;
}

bool StudentWorld:: getPlayer(int x, int y) const
{
    for (int k = 0; k < 4; k++)
    {
        for (int j = 0; j < 4; j++)
        {
            if (player->getX()+k == x && player->getY()+j == y)
                return true;
        }
    }
    return false;
}

bool StudentWorld:: hitBoulder(int x, int y) const
{
    int xmin = x;
    int xmax = x+4;
    int ymin = y;
    int ymax = y+4;
    for (int z = 0; z < actors.size(); z++)
    {
        if (actors[z]->cantPass())
        {
        if (xmax > actors[z]->getX() && xmax <= actors[z]->getX()+4)
        {
            if(ymax > actors[z]->getY() && ymax <= actors[z]->getY()+4)
                return true;
            else if (ymin >= actors[z]->getY() && ymin < actors[z]->getY()+4)
                return true;
        }
        else if (xmin >= actors[z]->getX() && xmin < actors[z]->getX()+4)
        {
            if(ymax > actors[z]->getY() && ymax <= actors[z]->getY()+4)
                return true;
            else if (ymin >= actors[z]->getY() && ymin < actors[z]->getY()+4)
                return true;
        }
        }
    }
    return false;
}

bool StudentWorld:: isOpen(int x, int y) const
{
    for (int k = 0; k < 4; k++)
    {for (int j = 0; j <4; j++)
    {if (getDirt(x+k,y+j) || hitBoulder(x+k, y+j) || x > 60 || y > 60 || x < 0 || y < 0)
    {return false;}}}
    
    return true;
}

void StudentWorld:: getPlayerCoord(int &x, int &y)
{
    x = player->getX();
    y = player->getY();
}

void StudentWorld:: annoyPlayer(int x)
{
    player->getsAnnoyed(x);
}

bool StudentWorld:: annoyProtesters(int x, int y, int m)
{
    int xmin = x;
    int xmax = x+4;
    int ymin = y;
    int ymax = y+4;
    for (int k = 0; k < actors.size(); k++)
    {
        if (actors[k]->isBaddie())
        {
            if (xmax > actors[k]->getX() && xmax <= actors[k]->getX()+4)
            {
                if(ymax > actors[k]->getY() && ymax <= actors[k]->getY()+4)
                {actors[k]->getsAnnoyed(m); return true;}
                else if (ymin >= actors[k]->getY() && ymin < actors[k]->getY()+4)
                {actors[k]->getsAnnoyed(m); return true;}
            }
            else if (xmin >= actors[k]->getX() && xmin < actors[k]->getX()+4)
            {
                if(ymax > actors[k]->getY() && ymax <= actors[k]->getY()+4)
                {actors[k]->getsAnnoyed(m); return true;}
                else if (ymin >= actors[k]->getY() && ymin < actors[k]->getY()+4)
                {actors[k]->getsAnnoyed(m); return true;}
            }

        }
    }
    return false;
}

bool StudentWorld:: bribe(Actor* acty)
{
    int x = acty->getX();
    int y = acty->getY();
    
    for (int k = 0; k < actors.size();k++)
    {
        if (actors[k]->Goodie()){
            if (distance(x,y,actors[k]->getX(), actors[k]->getY()) <= 3)
            {
                actors[k]->setDead();
                return true;
            }
            else
            return false;
        }
    }
    return false;
}

void StudentWorld:: adjustBarrels(int x)
{
    barrelcount += x;
}

void StudentWorld:: adjustGold(int x)
{
    player->adjustGold(x);
}

void StudentWorld:: adjustSonar(int x)
{
    player->adjustSonar(x);
}

void StudentWorld:: adjustWater(int x)
{
    player->adjustWater(x);
}

void StudentWorld:: decrProtest()
{
    protestcount--;
}

void StudentWorld:: freshGold(Gold* goldy)
{
    goldy->makeFresh();
}

void StudentWorld:: openCoord(int &x, int &y)
{
    bool valid = false;
    
    while (!valid)
    {
        x = (int) rand() % 60 + 1; // pick random coordinates within the valid range
        y = (int) rand() % 41 + 20;
        if (isOpen(x, y))
            valid = true;
    }
}

bool StudentWorld:: openPath(int x1, int y1, int x2, int y2)
{
    int xtemp;
    int ytemp;
    if (x1 > x2)
    {
        xtemp = x2;
        x2 = x1;
        x1 = xtemp;
    }
    if (y1 > y2)
    {
        ytemp = y2;
        y2 = y1;
        y1 = ytemp;
    }
    
    for (int k = x1; k <= x2; k++)
    {
        for (int j = y1; j <= y2; j++)
    {
        if (!isOpen(k,j))
    {
        return false;}}}
    
    return true;
}

void StudentWorld:: validCoord(int &x, int &y)
{
    bool valid = false;
    
    
    while (!valid)
    {
        int z = 0;
        x = (int) rand() % 60 + 1; // pick random coordinates within the valid range
        y = (int) rand() % 36 + 20;
        for (int j = 0; j < actors.size(); j++)
        {
               if (x < 27 || x > 33) // clears the mineshaft of actors
               {
                   if (distance(x,y,actors[j]->getX(), actors[j]->getY()) >= 6 ) // check to see if its within 6 of any other actor
                    {
                        z++;
                    }
               }
        }
        
        if (z >= (actors.size())) // if its far enough from all the actors, put it in
        {
            valid = true;
        }
    }
    return;
}

void StudentWorld:: moveShortPath(int x1, int y1, int x2, int y2, Actor* acty)
{
    
    int Map[64][64];
    for (int k = 0; k < 64; k++)
    {
        for (int j = 0; j < 64; j++)
        {
            if (isOpen(k, j))
                Map[k][j] = 999;
            else
                Map[k][j] = -1;
        }
    }
    
    queue<Coord> QQ;
    stack<Coord> stacky;
    
    QQ.push(Coord(x1,y1));
    Map[x1][y1] = 0;
    
    while (!QQ.empty())
    {
        Coord current = QQ.front();
        QQ.pop();
        
        if (current.r() == x2 && current.c() == y2)
        {
            break;
        }
        
        if (Map[current.r()+1][current.c()] > Map[current.r()][current.c()]+1)
        {
            QQ.push(Coord(current.r()+1, current.c()));
            Map[current.r()+1][current.c()]= min(Map[current.r()][current.c()]+1, Map[current.r()+1][current.c()]);
        }
        if (Map[current.r()][current.c()+1] > Map[current.r()][current.c()]+1)
        {
            QQ.push(Coord(current.r(), current.c()+1));
            Map[current.r()][current.c()+1]= min(Map[current.r()][current.c()]+1, Map[current.r()][current.c()+1]);
        }
        if (Map[current.r()-1][current.c()] > Map[current.r()][current.c()]+1)
        {
            QQ.push(Coord(current.r()-1, current.c()));
            Map[current.r()-1][current.c()]= min(Map[current.r()][current.c()]+1, Map[current.r()-1][current.c()]);
        }
        if (Map[current.r()][current.c()-1] > Map[current.r()][current.c()]+1)
        {
            QQ.push(Coord(current.r(), current.c()-1));
            Map[current.r()][current.c()-1]= min(Map[current.r()][current.c()]+1, Map[current.r()][current.c()-1]);
        }
    }
    
    bool found = false;
    Coord firstStep = Coord(60,60);
    
    stacky.push(Coord(x2,y2));
    
    while (!found && !stacky.empty())
    {
        Coord current = stacky.top();
        stacky.pop();
        
        if (current.r() == x1 && current.c() == y1)
        {
            found = true;
            break;
        }
        
        if (Map[current.r()-1][current.c()] == Map[current.r()][current.c()]-1)
        {
            stacky.push(Coord(current.r()-1, current.c()));
            firstStep = Coord(current.r(), current.c());
        }
        if (Map[current.r()][current.c()+1] == Map[current.r()][current.c()]-1)
        {
            stacky.push(Coord(current.r(), current.c()+1));
            firstStep = Coord(current.r(), current.c());
        }
        if (Map[current.r()+1][current.c()] == Map[current.r()][current.c()]-1)
        {
            stacky.push(Coord(current.r()+1, current.c()));
            firstStep = Coord(current.r(), current.c());
        }
        if (Map[current.r()][current.c()-1] == Map[current.r()][current.c()]-1)
        {
            stacky.push(Coord(current.r(), current.c()-1));
            firstStep = Coord(current.r(), current.c());
        }
        
    }
    
    Actor::Direction dir;
    int x = acty->getX();
    int y = acty->getY();
    if (firstStep.r() > acty->getX())
    {
        dir = Actor::right;
        acty->setDirection(dir);
        acty->moveTo(x+1, y);
    }
    else if (firstStep.c() > acty->getY())
    {
        dir = Actor::up;
        acty->setDirection(dir);
        acty->moveTo(x, y+1);
    }
    else if (firstStep.r() < acty->getX())
    {
        dir = Actor::left;
        acty->setDirection(dir);
        acty->moveTo(x-1, y);
    }
    else if (firstStep.c() < acty->getY())
    {
        dir = Actor::down;
        acty->setDirection(dir);
        acty->moveTo(x, y-1);
    }
}

int StudentWorld:: stepsToPlayer(int x, int y)
{
    int Map[64][64];
    for (int k = 0; k < 64; k++)
    {
        for (int j = 0; j < 64; j++)
        {
            if (isOpen(k, j))
                Map[k][j] = 999;
            else
                Map[k][j] = -1;
        }
    }
    
    queue<Coord> QQ;
    int x2 = player->getX();
    int y2 = player->getY();
    
    QQ.push(Coord(x,y));
    Coord current = QQ.front();
    Map[x][y] = 0;
    
    while (!QQ.empty())
    {
        current = QQ.front();
        QQ.pop();
        
        if (current.r() == x2 && current.c() == y2)
        {
            break;
        }
        
        if (Map[current.r()+1][current.c()] > Map[current.r()][current.c()]+1)
        {
            QQ.push(Coord(current.r()+1, current.c()));
            Map[current.r()+1][current.c()]= min(Map[current.r()][current.c()]+1, Map[current.r()+1][current.c()]);
        }
        if (Map[current.r()][current.c()+1] > Map[current.r()][current.c()]+1)
        {
            QQ.push(Coord(current.r(), current.c()+1));
            Map[current.r()][current.c()+1]= min(Map[current.r()][current.c()]+1, Map[current.r()][current.c()+1]);
        }
        if (Map[current.r()-1][current.c()] > Map[current.r()][current.c()]+1)
        {
            QQ.push(Coord(current.r()-1, current.c()));
            Map[current.r()-1][current.c()]= min(Map[current.r()][current.c()]+1, Map[current.r()-1][current.c()]);
        }
        if (Map[current.r()][current.c()-1] > Map[current.r()][current.c()]+1)
        {
            QQ.push(Coord(current.r(), current.c()-1));
            Map[current.r()][current.c()-1]= min(Map[current.r()][current.c()]+1, Map[current.r()][current.c()-1]);
        }
    }
    
    return Map[x2][y2];
}

////////////////////////////////////////// Extra Functions //////////////////////////////////////////

string makePretty(int score, int level, int lives, int health, int squirts, int gold, int sonar, int barrels)
{
    string temp = "000000" + to_string(score);
    string s1 = temp.substr(temp.length()-6,6);
    string l1 = to_string(level);
    string l2 = to_string(lives);
    string h = to_string(health*10) + "%";
    string s2 = to_string(squirts);
    string g = to_string(gold);
    string s3 = to_string(sonar);
    string b = to_string(barrels);
    
    string f = "Scr: " +  s1 + " Lvl: " + l1 + " Lives: " + l2 + " Hlth: " + h + " Water: " + s2 + " Gld: " + g + " Sonar " + s3 + " Oil Left: " + b;
    return f;
}

int distance(int x1, int y1, int x2, int y2)
{
    return sqrt(pow(x1-x2,2)+pow(y1-y2,2));
}
