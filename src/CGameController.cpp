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
#include <CGameContext.hpp>
#include <iostream>
#include <winbgim.h>
using namespace std;

CGameController::CGameController()
{
    m_CurPos = vec2(0, 0);
    m_CurVel = vec2(0, 0);
    m_CurDir = vec2(0, -1);
}
CGameController::~CGameController()
{
}

void CGameController::Reset()
{
}

void CGameController::EndRound()
{
    if (!g_Game.m_WorldPaused)
    {
        g_Game.m_WorldPaused = true;
        g_Game.m_TimePaused = GetTickCount();
        settextstyle(8, HORIZ_DIR, 1);
    }
}

void CGameController::StartRound()
{
}

void CGameController::Tick()
{
}

void CGameController::DrawHUD()
{
}

void CGameController::DrawEntities()
{
}
