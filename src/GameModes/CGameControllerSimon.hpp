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
#ifndef CGAMECONTROLLERSIMON_HH
#define CGAMECONTROLLERSIMON_HH
#include <CGameController.hpp>
#include <external/math.h>
#include <external/vmath.h>
#include <deque>

class CGameControllerSimon : public CGameController
{
public:
    CGameControllerSimon();
    virtual ~CGameControllerSimon();

    typedef struct
    {
        float m_Timer;
        vec2 m_Pos;
        int m_Radius;
        int m_Dir;
        float m_Start;
        short m_Color;
    } ANIM;

    typedef struct
    {
        float m_Timer;
        bool m_Used;
    } PAD;

    typedef enum
    {
        PLAYER_1,
        PLAYER_CPU
    } PLAYERTYPE;

    enum
    {
        PAD_BLUE=0,
        PAD_RED,
        PAD_YELLOW,
        PAD_GREEN,
        PAD_CYAN,
        NUM_PADS
    };

    virtual void Tick();
    virtual void DrawHUD();
    virtual void EndRound();

private:
    const int dimTW;
    const int dimTH;
    ANIM v_Cursor[4];
    PAD v_Pads[5];
    int m_LastMouseX;
    int m_LastMouseY;
    PLAYERTYPE m_ActivePlayer;
    float m_TimerCPU;

    size_t m_SeqCPUPos;
    size_t m_SeqP1Pos;
    bool m_Started;


    std::deque<short> v_Seq;
    std::deque<ANIM*> v_Anims;

    short GenerateColor();
    short GetPadColor(short pad, bool lightcolor = false);
    void PushPad(short pad);
};

#endif
