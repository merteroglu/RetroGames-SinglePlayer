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
#ifndef ICGAMECONTROLLER_H
#define ICGAMECONTROLLER_H
#include <external/math.h>
#include <external/vmath.h>

enum
{
    GAMETYPE_SNAKEDOS=0,
    GAMETYPE_SNAKEPRO,
    GAMETYPE_MARCIANOS
};

class CGameController
{
public:
    CGameController();
    virtual ~CGameController();

    virtual void Tick();
    virtual void Reset();
    virtual void StartRound();
    virtual void EndRound();
    virtual void DrawHUD();
    virtual void DrawEntities();

    short m_GameType;

public:
    vec2 m_CurPos;
    vec2 m_CurVel;
    vec2 m_CurDir;
};

#endif
