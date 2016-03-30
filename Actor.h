#ifndef ACTOR_H_
#define ACTOR_H_

#include "GraphObject.h"


// Students:  Add code to this file, Actor.cpp, StudentWorld.h, and StudentWorld.cpp

class StudentWorld;

class Actor : public GraphObject
{
public:
    Actor(int imageID, int x, int y, Direction dir, double size, int depth, StudentWorld* world, bool visible);
    virtual ~Actor();
    virtual void doSomething() = 0;
    virtual void getsAnnoyed(int x);
    virtual bool cantPass();
    virtual bool isBaddie();
    virtual bool Goodie();
    
    void setHealth(int x);
    bool isDead() const;
    int getHealth() const;
    void setDead();
    
    StudentWorld* getWorld();
    
private:
    int m_health;
    bool dead;
    StudentWorld* worldy;
};


class FrackMan : public Actor
{
public:
    FrackMan(int x, int y, StudentWorld* world);
    virtual ~FrackMan();
    virtual void doSomething();
    virtual void getsAnnoyed(int x);
    
    int getWater() const;
    int getSonar() const;
    int getGold() const;

    void adjustWater(int x);
    void adjustSonar(int x);
    void adjustGold(int x);
    
private:
    int m_water;
    int m_sonar;
    int m_gold;
    int m_health;
};


class Dirt : public Actor
{
public:
    Dirt(int x, int y, StudentWorld* world);
    ~Dirt();
    virtual void doSomething() {};
    
private:
};


class Boulder : public Actor
{
public:
    Boulder(int x, int y, StudentWorld* world);
    virtual ~Boulder();
    virtual void doSomething();
    virtual bool cantPass();
    
private:
    bool stable;
    bool falling;
    int count;
};


class Barrel : public Actor
{
public:
    Barrel(int x, int y, StudentWorld* world);
    ~Barrel();
    virtual void doSomething();
    
private:
};


class Gold : public Actor
{
public:
    Gold(int x, int y, StudentWorld* world);
    virtual ~Gold();
    virtual void doSomething();
    virtual bool Goodie();
    void makeFresh();
    
private:
    bool fresh;
    int count;
};


class Squirt : public Actor
{
public:
    Squirt(int x,int y,Direction dir, StudentWorld* world);
    virtual ~Squirt();
    virtual void doSomething();
private:
    int count;
};

class Sonar : public Actor
{
public:
    Sonar(StudentWorld* world);
    virtual ~Sonar();
    virtual void doSomething();
private:
    int count;
};

class Pool : public Actor
{
public:
    Pool(int x, int y, StudentWorld* world);
    virtual ~Pool();
    virtual void doSomething();
private:
    int count;
};

class Protester : public Actor
{
public:
    Protester(int imageID, StudentWorld* world);
    virtual ~Protester();
    virtual void doSomething();
    virtual void getsAnnoyed(int x);
    virtual bool isBaddie();
    bool canTurn();
    int cRest(int x);
    int cYell(int x);
    int cCurrentSteps(int x);
    int cTurnt(int x);
    bool cLeaving(bool c);
    bool isLeaving();
    
private:
    bool leaving;
    int rest;
    int yell;
    int currentSteps;
    int turnt;
};

class HardCoreProtester : public Protester
{
public:
    HardCoreProtester(StudentWorld* world);
    virtual ~HardCoreProtester();
    virtual void doSomething();
    virtual void getsAnnoyed(int x);
private:
};

#endif // ACTOR_H_
