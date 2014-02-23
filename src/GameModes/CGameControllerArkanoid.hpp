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
#include <CGameController.hpp>
#include <deque>

typedef enum
{
    ENT_BALL,
    ENT_PICKUP,
    ENT_WALL
} ENTITYTYPES;

class ICArkEnt
{
public:
    ICArkEnt(ENTITYTYPES type) { m_ObjType = type; }
    virtual ~ICArkEnt() {}

    ENTITYTYPES GetType()   const { return m_ObjType; }

    vec2 m_Pos;

private:
    ENTITYTYPES m_ObjType;
};

class CBall : public ICArkEnt
{
public:
    CBall(vec2 pos, short color);

    short m_Color;
    vec2 m_Dir;
    float m_Vel;
};

class CWall : public ICArkEnt
{
public:
    CWall(vec2 pos, int health, short color);

    enum
    {
        BSIZE = 3
    };

    vec2 m_Pos;
    short m_Color;
    int m_Life;
};


class CGameControllerArkanoid : public CGameController
{
public:
    CGameControllerArkanoid();
    virtual ~CGameControllerArkanoid();

    enum
    {
        BSIZE = 10
    };

    virtual void Tick();
    virtual void DrawHUD();
    virtual void StartRound();
    virtual void EndRound();
    virtual void Reset();

private:
    short m_Size;
    short m_Lifes;
    vec2 m_PrevCurDir;
    float m_TimerArkanoid;
    float m_ClockGame;
    int m_Score;
    short m_Level;
    bool m_isLost;
    std::deque<ICArkEnt*> v_Entities;

    bool CanShootBall();
    short GenerateColor(short notthis = -1);
    void GenerateMap();
    bool RemoveEntity(ICArkEnt *ent);
};
