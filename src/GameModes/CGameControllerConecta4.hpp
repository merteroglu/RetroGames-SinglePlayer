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
#ifndef CGAMECONTROLLERCONECTA4_HH
#define CGAMECONTROLLERCONECTA4_HH
#include <ICGameController.hpp>
#include <external/vmath.h>

class CGameControllerConecta4 : public ICGameController
{
public:
    CGameControllerConecta4();
    virtual ~CGameControllerConecta4();

    typedef enum
    {
        PLAYER_NULL,
        PLAYER_1,
        PLAYER_2,
        PLAYER_CPU
    } TYPEPLAYER;


    virtual void Tick();
    virtual void EndRound();
    virtual void DrawHUD();

private:
    struct { int x; int y; int m_Dir; short m_Color; } m_IndicatorChip;
    int m_OffsetX;
    int m_OffsetY;
    TYPEPLAYER v_GameTable[8][8];
    bool m_CPU;
    TYPEPLAYER m_ActivePlayer;
    TYPEPLAYER m_PlayerWin;
};

#endif
