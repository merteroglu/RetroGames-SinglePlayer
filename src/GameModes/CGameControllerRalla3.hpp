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
#ifndef CGAMECONTROLLERRALLA3_HH
#define CGAMECONTROLLERRALLA3_HH
#include <ICGameController.hpp>


class CGameControllerRalla3 : public ICGameController
{
public:
    CGameControllerRalla3();
    virtual ~CGameControllerRalla3();

    typedef enum
    {
        PLAYER_NULL,
        PLAYER_1,
        PLAYER_2,
        PLAYER_CPU,
        PLAYER_DRAW
    } PLAYERTYPE;


    virtual void Tick();
    virtual void EndRound();
    virtual void DrawHUD();

private:
    float m_TimerRalla3;
    PLAYERTYPE m_ActivePlayer;
    PLAYERTYPE m_PlayerWin;
    const int dimTX;
    const int dimTY;
    bool m_CPU;

    struct {
        float m_Timer;
        vec2 m_Pos;
    } m_Indicator;

    PLAYERTYPE v_GameTable[3][3];

    void CheckWinner(PLAYERTYPE winner);
};

#endif
