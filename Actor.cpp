#include "Actor.h"
#include "StudentWorld.h"

// Students:  Add code to this file (if you wish), Actor.h, StudentWorld.h, and StudentWorld.cpp

//////////////// Base Actor class for all objects /////////////////////

Actor :: Actor (int imageID, int x, int y, Direction dir, double size, int depth, StudentWorld* world, bool visible) : GraphObject(imageID, x, y, dir, size, depth)
{
    setVisible(visible);
    worldy = world;
}

Actor :: ~Actor()
{
}

void Actor:: getsAnnoyed(int x)
{
    m_health -= x;
}

bool Actor:: isDead() const
{
    return dead;
}

int Actor:: getHealth() const
{
    return m_health;
}

void Actor:: setDead()
{
    dead = true;
}

bool Actor:: cantPass()
{
    return false;
}

bool Actor:: isBaddie()
{
    return false;
}

bool Actor:: Goodie()
{
    return false;
}

void Actor:: setHealth(int x)
{
    m_health = x;
}

StudentWorld* Actor:: getWorld()
{
    return worldy;
}

////////////////////////////////////////////////////////////////////////

///////////////////// FrackMan player class /////////////////////////////
FrackMan :: FrackMan(int x, int y, StudentWorld* world) : Actor(IID_PLAYER, 30,60,right,1,0,world,true) , m_sonar(1), m_water(5), m_gold(0)
{
    setHealth(10);
}

FrackMan :: ~FrackMan()
{
}

void FrackMan:: getsAnnoyed(int x)
{
    Actor::getsAnnoyed(x);
    if (getHealth() <= 0)
        setDead();
}

int FrackMan:: getWater() const
{
    return m_water;
}

int FrackMan:: getSonar() const
{
    return m_sonar;
}

int FrackMan:: getGold() const
{
    return m_gold;
}

void FrackMan:: adjustWater(int x)
{
    m_water += x;
}

void FrackMan:: adjustSonar(int x)
{
    m_sonar+= x;
}

void FrackMan:: adjustGold(int x)
{
    m_gold += x;
}

void FrackMan:: doSomething()
{
    if (isDead())
        return;
    
    bool sound = false;
    
    for (int k = 0; k < 4; k++)     // delete dirt if it is in FrackMan's path
    {
        for (int j = 0; j < 4; j++)
        {
            if(getWorld()->getDirt(getX()+k,getY()+j) && (k!= 1 || k!= 2) && (j != 1 || j!= 2))
            {
                getWorld()->deleteDirt(getX()+k,getY()+j);
                sound = true;
            }
        }
        
    }
    
    if (sound == true)
            {
                getWorld()->playSound(SOUND_DIG);
                sound = false;
            }
    
    int k;
    if (getWorld()->getKey(k))  // if a user enters a key, go into this switch statement to move
    {
        switch (k)
        {
            case KEY_PRESS_LEFT:
                if (getDirection() != left)
                    setDirection(left);
                else if (getX() > 0 && !getWorld()->hitBoulder(getX()-1, getY()))
                    moveTo(getX()-1, getY());
                else if (getX() <= 0)
                    moveTo(getX(), getY());
                break;
            case KEY_PRESS_RIGHT:
                if (getDirection() != right)
                    setDirection(right);
                else if (getX() < 60 && !getWorld()->hitBoulder(getX()+1, getY()))
                    moveTo(getX()+1, getY());
                else if (getX() >= 60)
                    moveTo(getX(), getY());
                break;
            case KEY_PRESS_UP:
                if (getDirection() != up)
                    setDirection(up);
                else if (getY() < 60 &&!getWorld()->hitBoulder(getX(), getY()+1))
                    moveTo(getX(), getY()+1);
                else if (getY() >= 60)
                    moveTo(getX(), getY());
                break;
            case KEY_PRESS_DOWN:
                if (getDirection() != down)
                    setDirection(down);
                else if (getY() > 0 && !getWorld()->hitBoulder(getX(), getY()-1))
                    moveTo(getX(), getY()-1);
                else if (getY() <= 0)
                    moveTo(getX(), getY());
                break;
            case 'z':
                if(m_sonar > 0)
                {getWorld()->scan();
                    getWorld()->adjustSonar(-1);}
                break;
            case KEY_PRESS_SPACE:
                if (m_water > 0){
                if (getDirection() == right)
                    new Squirt(getX()+4,getY(),getDirection(), getWorld());
                else if (getDirection() == up)
                    new Squirt(getX(),getY()+4,getDirection(), getWorld());
                else if(getDirection() == down)
                    new Squirt(getX(),getY()-4,getDirection(), getWorld());
                else
                    new Squirt(getX()-4,getY(),getDirection(), getWorld());
                getWorld()->playSound(SOUND_PLAYER_SQUIRT);
                    getWorld()->adjustWater(-1);}
                break;
            case 'm':
                getWorld()->moveShortPath(getX(), getY(), 60, 60, this);
                break;
            case KEY_PRESS_TAB:
                if (m_gold > 0){
                Gold* goldy = new Gold(getX(),getY(),getWorld());
                goldy->setVisible(true);
                    getWorld()->adjustGold(-1);}
                break;
            
        }
    }
    
    
}


/////////////////////////////////////////////////////////////////////////
/////////////////////////// Dirt class //////////////////////////////////
Dirt :: Dirt(int x, int y, StudentWorld* world) : Actor(IID_DIRT, x,y,right,.25,3,world,true)
{
}

Dirt :: ~Dirt()
{
}

/////////////////////////////////////////////////////////////////////////

//////////////////////// Boulder class /////////////////////////////////////
Boulder :: Boulder(int x, int y, StudentWorld* world) : Actor(IID_BOULDER, x,y,down,1,1,world,true), stable(true), falling(false), count(0)
{
    getWorld()->addAct(this);
    for (int k = getX(); k < getX()+4; k++)  // remove all the dirt where the boulder is placed
    {for (int j = getY(); j < getY()+4; j++)
    {getWorld()->deleteDirt(k, j);}}
    
}

Boulder :: ~Boulder()
{
}

void Boulder:: doSomething()
{
    if (isDead() == true)
        return;
    
    if (stable && !falling)
    {
        int j = 0;
        for (int k = 0; k < 4; k++) // counts how many empty spaces are under the boulder
        {
            
            if (!getWorld()->getDirt(getX()+k, getY()-1))
            {
                j++;
            }
            if (j == 4) // if its clear under it, make it unstable
                stable = false;
        }
    }
    else if (!stable && count < 30) // while unstable, wait 30 ticks
    {
        count++;
        if (count == 30) // at 30 ticks, start falling
        {
            falling = true;
            getWorld()->playSound(SOUND_FALLING_ROCK);
        }
    }
    
    
    if (falling == true)
    {
        if (getWorld()->annoyProtesters(getX(), getY(), 100))
            getWorld()->increaseScore(400);

        if (getWorld()->getPlayer(getX(), getY()) || getWorld()->getPlayer(getX()+1, getY()) || getWorld()->getPlayer(getX()+2, getY()) || getWorld()->getPlayer(getX()+3, getY())) // if it hits the player, annoy him to death
            {
                getWorld()->annoyPlayer(100);
                setDead();
                return;
            }
        else if (getY() != -1 && !getWorld()->getDirt(getX(),getY()-1) && !getWorld()->getDirt(getX()+1,getY()-1) && !getWorld()->getDirt(getX()+2,getY()-1) && !getWorld()->getDirt(getX()+3,getY()-1)&& !getWorld()->hitBoulder(getX(), getY()-4)) // while under it is clear, fall one tick per second
            {
                moveTo(getX(), getY()-1);
            }
        else
            {
                setDead(); // remove it after it has landed somewhere
            }

        
    }
}

bool Boulder:: cantPass()
{
    return true;
}

///////////////////////////////////////////////////////////////////////////

///////////////////////// Barrel Class ////////////////////////////////////

Barrel :: Barrel(int x, int y, StudentWorld* world) : Actor(IID_BARREL, x,y,right,1,2,world,false)
{
    getWorld()->addAct(this);
    getWorld()->adjustBarrels(1);
}

Barrel :: ~Barrel()
{
}

void Barrel:: doSomething()
{
    if (isDead())
        return;
    
    int x, y = 0;
    getWorld()->getPlayerCoord(x, y);
    if (sqrt((pow(getX()-x,2))+(pow(getY()-y,2))) <= 4 && sqrt((pow(getX()-x,2))+(pow(getY()-y,2))) > 3)
    {
        setVisible(true);
        return;
    }
    else if (sqrt((pow((getX()-x),2))+(pow((getY()-y),2))) <= 3)
    {
        setDead();
        getWorld()->adjustBarrels(-1);
        getWorld()->playSound(SOUND_FOUND_OIL);
        getWorld()->increaseScore(1000);
        return;
    }
}

/////////////////////////////////////////////////////////////////////////////

/////////////////////////// Gold Class /////////////////////////////////////

Gold :: Gold(int x, int y, StudentWorld* world): Actor(IID_GOLD, x,y,right,1,2,world,false), fresh(false), count(0)
{
    getWorld()->addAct(this);
}

Gold :: ~Gold()
{
}

void Gold:: doSomething()
{
    if (count >= 100 || isDead())
    {
        setDead();
        return;
    }
    
    int x, y = 0;
    getWorld()->getPlayerCoord(x, y);
    if (sqrt((pow(getX()-x,2))+(pow(getY()-y,2))) <= 4 && sqrt((pow(getX()-x,2))+(pow(getY()-y,2))) > 3)
    {
        setVisible(true);
        return;
    }
    else if (sqrt((pow((getX()-x),2))+(pow((getY()-y),2))) <= 3 && fresh)
    {
        setDead();
        getWorld()->adjustGold(1);
        getWorld()->playSound(SOUND_GOT_GOODIE);
        getWorld()->increaseScore(25);
        return;
    }
    
    if (!fresh)
    {
        count++;
    }
}

bool Gold:: Goodie()
{
    if(!fresh)
    return true;
    
    return false;
}

void Gold:: makeFresh()
{
    fresh = true;
}

/////////////////////////////////////////////////////////////////////////////

/////////////////////////// Squirt Class /////////////////////////////////////

Squirt :: Squirt(int x, int y, Direction dir, StudentWorld* world): Actor(IID_WATER_SPURT, x,y,dir,1,2,world,true)
{
    getWorld()->addAct(this);
    count = 0;
}

Squirt :: ~Squirt()
{
}

void Squirt:: doSomething()
{
    
    if (count >= 4 || isDead())
    {
        setDead();
        return;
    }
    
    bool clear = true;
    
    if (getWorld()->annoyProtesters(getX(), getY(), 2))
    {
        setDead();
        return;
    }
    
    switch (getDirection())
    {
        case up:
            if (!getWorld()->openPath(getX(), getY(), getX(), getY()+4))
        {
            clear = false;
            setDead();
        }

            if (clear == true)
                {
                    setVisible(true);
                    moveTo(getX(), getY()+1);
                    count++;
                }
            
            break;
        case down:
            if (!getWorld()->openPath(getX(), getY(), getX(), getY()-4))
            {
                clear = false;
                setDead();
            }

             if(clear == true)
                {
                    setVisible(true);
                    moveTo(getX(), getY()-1);
                    count++;
                }
            break;
        case right:
            if (!getWorld()->openPath(getX(), getY(), getX()+4, getY()))
            {
                clear = false;
                setDead();
            }
            
              if (clear == true)
                {
                    setVisible(true);
                    moveTo(getX()+1, getY());
                    count++;
                }
            break;
        case left:
            if (!getWorld()->openPath(getX(), getY(), getX()-4, getY()))
            {
                clear = false;
                setDead();
            }

            if (clear == true)
                {
                    setVisible(true);
                    moveTo(getX()-1, getY());
                    count++;
                }
            break;
    }
}

/////////////////////////////////////////////////////////////////////////////

/////////////////////////// Sonar Class /////////////////////////////////////

Sonar :: Sonar(StudentWorld* world) : Actor(IID_SONAR, 0,60,right,1,2,world,true)
{
    getWorld()->addAct(this);
    count = 0;
}

Sonar :: ~Sonar()
{
}

void Sonar:: doSomething()
{
    int T = max(100, 300 - (10 * (int) getWorld()->getLevel()));
    
    if (count >= T || isDead())
    {
        setDead();
        return;
    }
    
    int x,y =0;
    getWorld()->getPlayerCoord(x, y);
    
    if (sqrt((pow((getX()-x),2))+(pow((getY()-y),2))) <= 3)
    {
        setDead();
        getWorld()->adjustSonar(1);
        getWorld()->playSound(SOUND_GOT_GOODIE);
        getWorld()->increaseScore(75);
        return;
    }
    else
        count++;
}

/////////////////////////////////////////////////////////////////////////////

/////////////////////////// Pool Class /////////////////////////////////////

Pool :: Pool(int x, int y, StudentWorld* world) : Actor(IID_WATER_POOL, x,y,right,1,2,world,true)
{
    getWorld()->addAct(this);
    count = 0;
}

Pool:: ~Pool()
{
}

void Pool:: doSomething()
{
    int T = max(100, 300 - (10 * (int) getWorld()->getLevel()));
    
    if (count >= T || isDead())
    {
        setDead();
        return;
    }
    
    int x,y =0;
    getWorld()->getPlayerCoord(x, y);
    
    if (sqrt((pow((getX()-x),2))+(pow((getY()-y),2))) <= 3)
    {
        setDead();
        getWorld()->adjustWater(5);
        getWorld()->playSound(SOUND_GOT_GOODIE);
        getWorld()->increaseScore(100);
        return;
    }
    else
        count++;
}

/////////////////////////////////////////////////////////////////////////////

/////////////////////////// Protester Class /////////////////////////////////////

Protester :: Protester(int imageID, StudentWorld* world) : Actor(imageID, 60,60,left,1,0,world,true), rest(0),yell(0),turnt(200),leaving(false), currentSteps(rand() % 52 +8)
{
    getWorld()->addAct(this);
    setHealth(5);
}

Protester :: ~Protester()
{
}

bool Protester:: isBaddie()
{
    return true;
}

void Protester:: doSomething()
{
    rest++;
    int level = getWorld()->getLevel();
    int wait = max(0, 3 - level/4);
    
    if (isDead() || rest < wait)
    {
        return;
    }
    rest = 0;
    yell++;
    turnt++;
    
    if (leaving == true)
    {
        getWorld()->moveShortPath(getX(), getY(), 60, 60, this);
        if (getX() == 60 && getY() == 60)
        {
            setDead();
            getWorld()->decrProtest();
            return;
        }
        return;
    }
    
    if (getWorld()->bribe(this))
    {
        getWorld()->playSound(SOUND_PROTESTER_FOUND_GOLD);
        getWorld()->increaseScore(25);
        leaving = true;
        return;
    }
    
    
    int x,y;
    getWorld()->getPlayerCoord(x, y);
    if (distance(getX(), getY(), x,y) <= 4 && yell >= 15)
    {
        if ((getDirection()==down && getY() > y) || (getDirection()==up && getY() < y) || (getDirection()==right && getX() < x) || (getDirection()==left && getX() > x))
        {
            getWorld()->playSound(SOUND_PROTESTER_YELL);
            getWorld()->annoyPlayer(2);
            yell = 0;
            rest = -15;
            return;
        }
    }
    else if(((getX() == x) || (getY() == y)) && getWorld()->openPath(x, y, getX(), getY()) )
    {
        if (getX() > x)
        {
            setDirection(left);
            moveTo(getX()-1, getY());
        }
        else if (getX() < x)
        {
            setDirection(right);
            moveTo(getX()+1, getY());
        }
        else if (getY() > y)
        {
            setDirection(down);
            moveTo(getX(), getY()-1);
        }
        else if (getY() < y)
        {
            setDirection(up);
            moveTo(getX(), getY()+1);
        }
        currentSteps = 0;
        return;
    }
    else
    {
        currentSteps--;
        if (currentSteps <= 0)
        {
            int open = false;
            while (!open)
            {
            int newDir = rand() % 3 +1;
            switch (newDir){
                case 1:
                    if (getWorld()->isOpen(getX()+1, getY()))
                    {
                        setDirection(right);
                        open = true;
                    }
                    break;
                case 2:
                    if (getWorld()->isOpen(getX()-1, getY()))
                    {
                        setDirection(left);
                        open = true;
                    }
                    break;
                case 3:
                    if (getWorld()->isOpen(getX(), getY()+1))
                    {
                        setDirection(up);
                        open = true;
                    }
                    break;
                case 4:
                    if (getWorld()->isOpen(getX(), getY()-1))
                    {
                        setDirection(down);
                        open = true;
                    }
                break;}
            }
            currentSteps = rand() % 52 +8;
        }
        
        if (canTurn() && turnt >= 200)
        {
            switch (getDirection()){
                case left:
                    if (getWorld()->isOpen(getX(), getY()-1))
                    {
                        setDirection(down);
                        currentSteps = rand() % 52 +8;
                    }
                    else
                    {
                        setDirection(up);
                        currentSteps = rand() % 52 +8;
                    }
                    break;
                case right:
                    if (getWorld()->isOpen(getX(), getY()+1))
                    {
                        setDirection(up);
                        currentSteps = rand() % 52 +8;
                    }
                    else
                    {
                        setDirection(down);
                        currentSteps = rand() % 52 +8;
                    }
                    break;
                case down:
                    if (getWorld()->isOpen(getX()-1, getY()))
                    {
                        setDirection(left);
                        currentSteps = rand() % 52 +8;
                    }
                    else
                    {
                        setDirection(right);
                       currentSteps = rand() % 52 +8;
                    }
                    break;
                case up:
                    if (getWorld()->isOpen(getX()+1, getY()))
                    {
                        setDirection(right);
                        currentSteps = rand() % 52 +8;
                    }
                    else
                    {
                        setDirection(left);
                        currentSteps = rand() % 52 +8;
                    }
                break;}
            turnt = 0;
        }
        
        switch (getDirection()){
    case left:
                if (getWorld()->isOpen(getX()-1, getY()))
                    moveTo(getX()-1, getY());
                else
                    currentSteps = 0;
                break;
    case right:
                if (getWorld()->isOpen(getX()+1, getY()))
                    moveTo(getX()+1, getY());
                else
                    currentSteps = 0;
                break;
    case down:
                if (getWorld()->isOpen(getX(), getY()-1))
                    moveTo(getX(), getY()-1);
                else
                    currentSteps = 0;
                break;
    case up:
                if (getWorld()->isOpen(getX(), getY()+1))
                    moveTo(getX(), getY()+1);
                else
                    currentSteps = 0;
            break;}
        
    }
    
}

bool Protester:: canTurn()
{
    switch (getDirection()){
        case left:
            if ( getWorld()->isOpen(getX(), getY()-1) || getWorld()->isOpen(getX(), getY()+1))
            {
                return true;
            }
            break;
        case right:
            if ( getWorld()->isOpen(getX(), getY()-1) || getWorld()->isOpen(getX(), getY()+1))
            {
                return true;
            }
            break;
        case down:
            if (getWorld()->isOpen(getX()-1, getY()) || getWorld()->isOpen(getX()+1, getY()))
            {
               return true;
            }
            break;
        case up:
            if (getWorld()->isOpen(getX()-1, getY()) || getWorld()->isOpen(getX()+1, getY()))
            {
               return true;
            }
        break;}
    return false;
}

void Protester:: getsAnnoyed(int x)
{
    if (leaving == true)
        return;
    
    Actor::getsAnnoyed(x);
    int N = max(50, 100 - (int)getWorld()->getLevel()* 10);
    rest = -N + max(0, 3 - (int)getWorld()->getLevel()/4);
    getWorld()->playSound(SOUND_PROTESTER_ANNOYED);
    
    if (getHealth() <= 0)
    {
        getWorld()->increaseScore(100);
        leaving = true;
        getWorld()->playSound(SOUND_PROTESTER_GIVE_UP);
        rest = max(0, 3 - (int)getWorld()->getLevel()/4)+10;
    }
}

bool Protester:: isLeaving()
{
    return leaving;
}

int Protester:: cRest(int x)
{
    rest += x;
    return rest;
}

int Protester:: cYell(int x)
{
    yell += x;
    return yell;
}

int Protester:: cCurrentSteps(int x)
{
    currentSteps += x;
    return currentSteps;
}

int Protester:: cTurnt(int x)
{
    turnt += x;
    return turnt;
}

bool Protester:: cLeaving(bool c)
{
    leaving = c;
    return leaving;
}

///////////////////////////////////////////////////////////////////////////////////

/////////////////////////// HardCoreProtester Class /////////////////////////////////////

HardCoreProtester :: HardCoreProtester(StudentWorld* world) : Protester(IID_HARD_CORE_PROTESTER,world)
{  
    setHealth(20);
}

HardCoreProtester :: ~HardCoreProtester()
{
}

void HardCoreProtester:: doSomething()
{
    cRest(1);
    int level = getWorld()->getLevel();
    int wait = max(0, 3 - level/4);
    
    if (isDead() || (cRest(0) < wait))
    {
        return;
    }
    cRest(-cRest(0));
    cYell(1);
    cTurnt(1);
    
    if (isLeaving())
    {
        getWorld()->moveShortPath(getX(), getY(), 60, 60, this);
        if (getX() == 60 && getY() == 60)
        {
            setDead();
            getWorld()->decrProtest();
            return;
        }
        return;
    }
    
    if (getWorld()->bribe(this))
    {
        getWorld()->playSound(SOUND_PROTESTER_FOUND_GOLD);
        getWorld()->increaseScore(50);
        cRest(-cRest(0)- max(50, 100 - (int)(getWorld()->getLevel() * 10)));
        return;
    }
    
    int x,y;
    getWorld()->getPlayerCoord(x, y);
    
    if (distance(getX(), getY(), x,y) <= 4 && cYell(0) >= 15)
    {
        if ((getDirection()==down && getY() >= y) || (getDirection()==up && getY() <= y) || (getDirection()==right && getX() <= x) || (getDirection()==left && getX() >= x))
        {
            getWorld()->playSound(SOUND_PROTESTER_YELL);
            getWorld()->annoyPlayer(2);
            cYell(-cYell(0));
            cRest(cRest(0)-20);
            return;
        }
    }
    else if (getWorld()->stepsToPlayer(getX(), getY()) <= (16+(getWorld()->getLevel()*2)) && getWorld()->stepsToPlayer(getX(), getY()) > 0)
    {
        getWorld()->moveShortPath(getX(), getY(), x, y, this);
    }
    else if(((getX() == x) || (getY() == y)) && getWorld()->openPath(x, y, getX(), getY()))
    {
        if (getX() > x)
        {
            setDirection(left);
            moveTo(getX()-1, getY());
        }
        else if (getX() < x)
        {
            setDirection(right);
            moveTo(getX()+1, getY());
        }
        else if (getY() > y)
        {
            setDirection(down);
            moveTo(getX(), getY()-1);
        }
        else if (getY() < y)
        {
            setDirection(up);
            moveTo(getX(), getY()+1);
        }
        cCurrentSteps(-cCurrentSteps(0));
        return;
    }
    else
    {
        cCurrentSteps(-1);
        if (cCurrentSteps(0) <= 0)
        {
            int open = false;
            while (!open)
            {
                int newDir = rand() % 3 +1;
                switch (newDir){
                    case 1:
                        if (getWorld()->isOpen(getX()+1, getY()))
                        {
                            setDirection(right);
                            open = true;
                        }
                        break;
                    case 2:
                        if (getWorld()->isOpen(getX()-1, getY()))
                        {
                            setDirection(left);
                            open = true;
                        }
                        break;
                    case 3:
                        if (getWorld()->isOpen(getX(), getY()+1))
                        {
                            setDirection(up);
                            open = true;
                        }
                        break;
                    case 4:
                        if (getWorld()->isOpen(getX(), getY()-1))
                        {
                            setDirection(down);
                            open = true;
                        }
                    break;}
            }
            cCurrentSteps(-cCurrentSteps(0)+rand() % 52 +8);
        }
        
        if (canTurn() && cTurnt(0) >= 200)
        {
            switch (getDirection()){
                case left:
                    if (getWorld()->isOpen(getX(), getY()-1))
                    {
                        setDirection(down);
                        cCurrentSteps(-cCurrentSteps(0)+rand() % 52 +8);
                    }
                    else
                    {
                        setDirection(up);
                        cCurrentSteps(-cCurrentSteps(0)+rand() % 52 +8);
                    }
                    break;
                case right:
                    if (getWorld()->isOpen(getX(), getY()+1))
                    {
                        setDirection(up);
                        cCurrentSteps(-cCurrentSteps(0)+rand() % 52 +8);
                    }
                    else
                    {
                        setDirection(down);
                        cCurrentSteps(-cCurrentSteps(0)+rand() % 52 +8);
                    }
                    break;
                case down:
                    if (getWorld()->isOpen(getX()-1, getY()))
                    {
                        setDirection(left);
                        cCurrentSteps(-cCurrentSteps(0)+rand() % 52 +8);
                    }
                    else
                    {
                        setDirection(right);
                        cCurrentSteps(-cCurrentSteps(0)+rand() % 52 +8);
                    }
                    break;
                case up:
                    if (getWorld()->isOpen(getX()+1, getY()))
                    {
                        setDirection(right);
                        cCurrentSteps(-cCurrentSteps(0)+rand() % 52 +8);
                    }
                    else
                    {
                        setDirection(left);
                        cCurrentSteps(-cCurrentSteps(0)+rand() % 52 +8);
                    }
                break;}
            cTurnt(-cTurnt(0));
        }
        
        switch (getDirection()){
            case left:
                if (getWorld()->isOpen(getX()-1, getY()))
                    moveTo(getX()-1, getY());
                else
                    cCurrentSteps(-cCurrentSteps(0));
                break;
            case right:
                if (getWorld()->isOpen(getX()+1, getY()))
                    moveTo(getX()+1, getY());
                else
                    cCurrentSteps(-cCurrentSteps(0));
                break;
            case down:
                if (getWorld()->isOpen(getX(), getY()-1))
                    moveTo(getX(), getY()-1);
                else
                    cCurrentSteps(-cCurrentSteps(0));
                break;
            case up:
                if (getWorld()->isOpen(getX(), getY()+1))
                    moveTo(getX(), getY()+1);
                else
                    cCurrentSteps(-cCurrentSteps(0));
            break;}
        
    }
    
}

void HardCoreProtester:: getsAnnoyed(int x)
{
    if (isLeaving())
    {
        return;
    }
    
    Protester::getsAnnoyed(x);
    if(isLeaving())
    getWorld()->increaseScore(150);
}