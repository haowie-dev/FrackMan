#ifndef STUDENTWORLD_H_
#define STUDENTWORLD_H_
#include "Actor.h"
#include "GameWorld.h"
#include "GameConstants.h"
#include <cmath>
#include <string>
#include <vector>
#include <cstdlib>
#include <stack>
#include <queue>

using namespace std;

// Students:  Add code to this file, StudentWorld.cpp, Actor.h, and Actor.cpp

class StudentWorld : public GameWorld
{
public:
    StudentWorld(std::string assetDir);
    ~StudentWorld();
    virtual int init();
    virtual int move();
    virtual void cleanUp();
    
    void addAct(Actor* acty);
    void scan();
    
    bool deleteDirt(int x,int y);
    bool getDirt(int x, int y) const;
    bool getBoulder(int x, int y) const;
    bool getPlayer(int x, int y) const;
    bool hitBoulder(int x, int y) const;
    void getPlayerCoord(int &x, int &y);
    void annoyPlayer(int x);
    bool annoyProtesters(int x, int y, int m);
    bool bribe(Actor* acty);
    
    bool isOpen(int x, int y) const;
    void validCoord(int &x, int &y);
    void openCoord(int &x, int &y);
    bool openPath(int x1, int y1, int x2, int y2);
    void moveShortPath(int x1, int y1, int x2, int y2, Actor* acty);
    int stepsToPlayer(int x, int y);
    
    void adjustBarrels(int x);
    void adjustGold(int x);
    void adjustSonar(int x);
    void adjustWater(int x);
    void decrProtest();
    void freshGold(Gold* goldy);
    

private:
    void setDisplayText();
    vector<Actor*> actors;
    FrackMan* player;
    Dirt* dirty[64][64];
    int barrelcount;
    int count;
    int protestcount;
    
    class Coord
    {
    public:
        Coord(int rr, int cc) : m_r(rr), m_c(cc) {}
        ~Coord() {}
        int r() const { return m_r; }
        int c() const { return m_c; }
    private:
        int m_r;
        int m_c;
    };
};

string makePretty(int score, int level, int lives, int health, int squirts, int gold, int sonar, int barrels);
int distance(int x1, int y1 , int x2 , int y2);

#endif // STUDENTWORLD_H_
