/*
Copyright(C) 2011 Alexandre Díaz Cuadrado
This file is part of RetroJuegos.

RetroJuegos is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License.

RetroJuegos is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with RetroJuegos.  If not, see <http://www.gnu.org/licenses/>.
*/
#ifndef CRAPTOWER_HH
#define CRAPTOWER_HH
#include <ICGameController.hpp>
#include <winbgim.h>
#include <vector>
#include <deque>

#define MAP_SIZE    400
#define TILE_SIZE   (MAP_SIZE/30)
#define OFFSET      (TILE_SIZE/2)

using namespace std;

class CEnemy
{
public:
    typedef enum
    {
        ENEMY_1,
        ENEMY_2
    } ENEMYTYPE;


    CEnemy(ENEMYTYPE type, vec2 pos, float vel);
    virtual ~CEnemy();

    vec2 m_Pos;
    vec2 m_Dir;
    float m_Vel;
    float m_Health;
    float m_Timer;
    ENEMYTYPE m_Type;
    int m_Points[4*2];

    void TakeDamage(float dmg);
    void Draw();
};

//----------------

class CRonda
{
public:
    CRonda();
    virtual ~CRonda();

    float m_TimeStart;
    std::deque<CEnemy*> v_Enemies;
};

//----------------

class CMap
{
public:
    CMap();
    ~CMap();

    enum
    {
        TILE_NORMAL=0,
        TILE_ROAD,
        TILE_START,
        TILE_END,
        TILE_NOTOWER,
        NUM_TILES
    };

    typedef enum
    {
        STOP,
        LEFT,
        RIGHT,
        UP,
        DOWN
    } DIR;

    typedef struct
    {
        short m_TypeTile;
        vec2 m_Pos;
        DIR m_Dir;
        void *m_Data;
    } TILE;

    bool LoadMap(const char *filename);
    void Draw();
    TILE* GetTile(vec2 pos);
    bool CanBuildHere(vec2 pos);

    vec2 GetStartPos() const { return m_IndexStart; }


    TILE v_MapTiles[30][30];
    std::deque<CRonda*> v_Rondas;
private:
    vec2 m_IndexStart;
};

//----------------

class CEntity
{
public:
    typedef enum
    {
        ENTITY_NULL,
        ENTITY_TOWER,
        ENTITY_WATER,
        ENTITY_BULLET
    } ENTTYPE;

    CEntity(ENTTYPE type);
    virtual ~CEntity();

    vec2 m_Pos;
    float m_Timer;

    ENTTYPE GetType()   const { return m_Type; }

private:
    ENTTYPE m_Type;
};

//----------------

class CTower : public CEntity
{
public:
    typedef enum
    {
        TOWER_BASIC,
        TOWER_MEDIUM,
        TOWER_HIGH
    } TOWERLEVEL;

    CTower(TOWERLEVEL level, vec2 pos);
    ~CTower();

    TOWERLEVEL m_Level;
    short m_Color;
    int m_Radius;
};

class CBullet : public CEntity
{
public:
    CBullet(vec2 pos, vec2 dir, CTower *tower, CEnemy *target, short m_Color = RED);
    ~CBullet();

    short m_Color;
    vec2 m_Dir;
    float m_TimerVel;
    CTower *m_TowerOwn;
    CEnemy *m_Target;
};

//----------------

class CGameControllerCrapTower : public ICGameController
{
public:
    CGameControllerCrapTower();
    virtual ~CGameControllerCrapTower();

    virtual void Tick();
    virtual void DrawHUD();
    virtual void EndRound();
    virtual void Reset();

private:
    float m_TimerCrapTower;
    float m_LastTimerRound;
    unsigned short m_Level;
    bool m_PlayerWin;
    CMap *m_pMap;
    short m_SelEntity;
    std::vector<CEnemy*> v_Enemies;
    std::vector<CEntity*> v_Entities;

    void RemoveEntity(CEntity *ent);
    void RemoveEnemy(CEnemy *ent);
    void RemoveRound(CRonda *round);
};

#endif
