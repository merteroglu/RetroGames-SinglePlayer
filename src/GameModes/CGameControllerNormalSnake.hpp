/*
SnakeDOS
Funcion: Mitico snake para terminal windows.

Copyright© 2011 - Alexandre Diaz Cuadrado
GNU License v3
*/

#ifndef CGAMECONTROLLERNORMALSNAKE_H
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
#define CGAMECONTROLLERNORMALSNAKE_H
#include <CGameController.hpp>
#include <winbgim.h>
#include <deque>

class CGameControllerNormalSnake : public CGameController
{
public:
    CGameControllerNormalSnake();
    virtual ~CGameControllerNormalSnake();

    class BODYSNAKE
    {
        public:
            BODYSNAKE() { m_Color = LIGHTGREEN; m_ToJoin = true; }
            vec2 m_Pos;
            int m_Color;
            bool m_ToJoin;

            enum { SIZE=10 };
    };
    class CPickup
    {
        public:
            CPickup(vec2 pos) { m_Pos = pos; }
            vec2 m_Pos;
            short m_Color;
    };

    enum
    {
        BLOCK_SQUARE=0,
        BLOCK_CIRCLE,

        STATE_ALIVE=0,
        STATE_DEAD
    };

    virtual void Tick();
    virtual void Reset();
    virtual void DrawHUD();
    virtual void DrawEntities();
    virtual void EndRound();

private:
    std::deque<BODYSNAKE*> v_SnakeBody;
    float m_TimerSnake;
    float m_ClockGame;
    short m_BlockType;
    short m_SnakeColor;
    CPickup *m_pBall;
    int m_Score;
    char m_LastKey;
    short m_SnakeState;
    vec2 m_PrevCurDir;

    vec2 GenerateEntityPos();
    short GenerateColor(short notthis = 0);
    int GetNumColorsInBody(short thiscolor);
};

#endif
