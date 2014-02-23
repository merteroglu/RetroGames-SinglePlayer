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
#include "CGameControllerNormalSnake.hpp"
#include <ICGameContext.hpp>
#include <iostream>
#include <stdio.h>
#include <winbgim.h>
#include <time.h>
#include <external/math.h>
#include <external/vmath.h>
using namespace std;

CGameControllerNormalSnake::CGameControllerNormalSnake()
{
    srand(time(NULL));
    setwindowtitle("SnaKe");

    //Cabeza de la Serpiente
    m_CurPos = vec2(g_Game.GetScreenW()/2, g_Game.GetScreenH()/2);
    m_PrevCurDir = m_CurDir;

    BODYSNAKE *m_pBodySnake = new BODYSNAKE();
    m_pBodySnake->m_Pos = m_CurPos;
    m_pBodySnake->m_ToJoin = false;
    v_SnakeBody.push_back(m_pBodySnake);

    m_BlockType = BLOCK_SQUARE;
    m_GameType = GAMETYPE_SNAKEDOS;
    m_TimerSnake = GetTickCount();
    m_ClockGame = GetTickCount();
    m_Score = 0;
    m_LastKey = 0x0;
    m_SnakeState = STATE_ALIVE;
    m_SnakeColor = LIGHTGREEN;

    m_pBall = new CPickup(GenerateEntityPos());
    m_pBall->m_Color = GenerateColor();
}
CGameControllerNormalSnake::~CGameControllerNormalSnake()
{
    std::deque<BODYSNAKE*>::iterator it = v_SnakeBody.begin();
    while(it != v_SnakeBody.end())
    {
            delete (*it);
            (*it) = 0x0;
            it = v_SnakeBody.erase(it);
    }

    if (m_pBall)
    {
        delete m_pBall;
        m_pBall = 0x0;
    }
}

void CGameControllerNormalSnake::Reset()
{
    std::deque<BODYSNAKE*>::iterator it = v_SnakeBody.begin();
    ++it;
    while(it != v_SnakeBody.end())
    {
            delete (*it);
            (*it) = 0x0;
            it = v_SnakeBody.erase(it);
    }

    m_CurPos = vec2(g_Game.GetScreenW()/2, g_Game.GetScreenH()/2);

    v_SnakeBody[0]->m_Pos = m_CurPos;

    m_TimerSnake = GetTickCount();
    m_ClockGame = GetTickCount();
    m_Score = 0;
    m_LastKey = 0x0;
    m_SnakeState = STATE_ALIVE;

    g_Game.m_WorldPaused = false;
}

int CGameControllerNormalSnake::GetNumColorsInBody(short thiscolor)
{
    int result = 0;

    for (size_t i=0; i<v_SnakeBody.size(); i++)
    {
        if (v_SnakeBody[i]->m_Color == thiscolor)
            result++;
    }

    return result;
}

vec2 CGameControllerNormalSnake::GenerateEntityPos()
{
    vec2 m_Pos = vec2(0, 0);
    bool error = true;
    int index;

    while (error)
    {
        error = false;

        m_Pos.x = rand() % g_Game.GetScreenW() + 1;
        m_Pos.y = rand() % g_Game.GetScreenH() + 1;

        m_Pos.x = clamp(static_cast<int>(m_Pos.x/BODYSNAKE::SIZE), 0, static_cast<int>((g_Game.GetScreenW()/BODYSNAKE::SIZE)-1));
        m_Pos.y = clamp(static_cast<int>(m_Pos.y/BODYSNAKE::SIZE), 0, static_cast<int>((g_Game.GetScreenH()/BODYSNAKE::SIZE)-1));

        index = static_cast<int>(m_Pos.y)*(g_Game.GetScreenW()/BODYSNAKE::SIZE)+static_cast<int>(m_Pos.x);
        m_Pos.x = index % (g_Game.GetScreenW()/BODYSNAKE::SIZE) *BODYSNAKE::SIZE;
        m_Pos.y = index / (g_Game.GetScreenW()/BODYSNAKE::SIZE) *BODYSNAKE::SIZE;

        if (m_Pos.y < 25)
        {
            error = true;
            continue;
        }

        for (size_t i=0; i<v_SnakeBody.size(); i++)
        {
            if (m_Pos == v_SnakeBody[i]->m_Pos)
            {
                error = true;
                break;
            }
        }
    }

    return m_Pos;
}

short CGameControllerNormalSnake::GenerateColor(short notthis)
{
    short color = notthis;

    do
    {
        color = rand() % (LIGHTMAGENTA-LIGHTBLUE+1) + LIGHTBLUE;
    } while (color == notthis);

    return color;
}

void CGameControllerNormalSnake::DrawHUD()
{
    char buff[255];
    time_t m_Time;
    struct tm *m_tmTime;

    settextstyle(10, HORIZ_DIR, 2);

    /** Tiempo **/
    if (!g_Game.m_WorldPaused)
        m_Time = static_cast<time_t>((GetTickCount()-m_ClockGame)/1000);
    else
       m_Time = static_cast<time_t>((g_Game.m_TimePaused-m_ClockGame)/1000);
    m_tmTime = localtime(&m_Time);

    //TODO: Esto esta algo sucio... :S
    if (m_tmTime->tm_min < 10 && m_tmTime->tm_sec < 10)
        sprintf(buff, "0%i:0%i", m_tmTime->tm_min, m_tmTime->tm_sec);
    else if (m_tmTime->tm_min < 10)
        sprintf(buff, "0%i:%i", m_tmTime->tm_min, m_tmTime->tm_sec);
    else if (m_tmTime->tm_sec < 10)
        sprintf(buff, "%i:0%i", m_tmTime->tm_min, m_tmTime->tm_sec);
    else
        sprintf(buff, "%i:%i", m_tmTime->tm_min, m_tmTime->tm_sec);
    outtextxy(3, 3, buff);

    /** Puntuacion **/
    if (m_Score < 10)
        sprintf(buff, "00%i", m_Score);
    else if (m_Score < 100)
        sprintf(buff, "0%i", m_Score);
    else
       sprintf(buff, "%i", m_Score);
    outtextxy(g_Game.GetScreenW()-textwidth(buff)-3, 3, buff);
    moveto(0, textheight(buff)+5);
    lineto(g_Game.GetScreenW(), textheight(buff)+5);

    /** Perdio? **/
    if (m_SnakeState == STATE_DEAD)
    {
        int xt = g_Game.GetScreenW()/2 - textwidth("Eres un Noob!!!")/2;
        int yt = g_Game.GetScreenH()/2 - textheight("Eres un Noob!!!")/2;

        outtextxy(xt, yt, "Eres un Noob!!!");

        xt = g_Game.GetScreenW()/2 - textwidth("pulsa INTRO para reiniciar")/2;
        yt = g_Game.GetScreenH()/2 - textheight("pulsa INTRO para reiniciar")/2;
        yt += textheight("Eres un Noob!") + 15;
        outtextxy(xt, yt, "pulsa INTRO para reiniciar");
        rectangle(-1, yt-2, g_Game.GetScreenW()+1, yt-1 + textheight("pulsa INTRO para reiniciar") + 2);
    }
}

void CGameControllerNormalSnake::Tick()
{
    /** Actualizar Posiciones **/
    if (g_Game.Keyboard.KeyPressed)
    {
        //Teclas Especiales del Modo de Juego
        switch(g_Game.Keyboard.Key)
        {
            case 13:
                if (m_SnakeState == STATE_DEAD)
                {
                    g_Game.m_GameState = ICGameContext::GAME_MENU;
                    return;
                }
                break;

            case '0':
                m_BlockType = (m_BlockType==BLOCK_SQUARE)?BLOCK_CIRCLE:BLOCK_SQUARE;
                break;

            case 32:
                if (m_SnakeState == STATE_DEAD)
                    break;

                g_Game.m_WorldPaused = (g_Game.m_WorldPaused==true)?false:true;
                if (!g_Game.m_WorldPaused)
                    m_ClockGame += GetTickCount() - g_Game.m_TimePaused;
                else
                    g_Game.m_TimePaused = GetTickCount();
                break;
        }

        if (!g_Game.m_WorldPaused)
        {
            //Movimientos de la Serpiente
            switch(g_Game.Keyboard.Key)
            {
                case 'a':
                case 'A':
                case KEY_LEFT:
                    if (m_CurDir == vec2(0, 1) || m_CurDir == vec2(0, -1))
                        m_PrevCurDir = vec2(-1, 0);
                    break;

                case 'd':
                case 'D':
                case KEY_RIGHT:
                    if (m_CurDir == vec2(0, 1) || m_CurDir == vec2(0, -1))
                        m_PrevCurDir = vec2(1, 0);
                    break;

                case 'w':
                case 'W':
                case KEY_UP:
                    if (m_CurDir == vec2(1, 0) || m_CurDir == vec2(-1, 0))
                        m_PrevCurDir = vec2(0, -1);
                    break;

                case 's':
                case 'S':
                case KEY_DOWN:
                    if (m_CurDir == vec2(1, 0) || m_CurDir == vec2(-1, 0))
                        m_PrevCurDir = vec2(0, 1);
                    break;
            }
        }

        m_LastKey = g_Game.Keyboard.Key;
    }

    /** Mover Serpiente **/
    if (!g_Game.m_WorldPaused && GetTickCount() > m_TimerSnake)
    {
        int index;

        m_CurDir = m_PrevCurDir;
        //Obtenemos Posicion de la Casilla
        m_CurPos += m_CurVel + m_CurDir * BODYSNAKE::SIZE;
        m_CurPos.x /= BODYSNAKE::SIZE;
        m_CurPos.y /= BODYSNAKE::SIZE;

        //La serpiente no choca contra los bordes de la ventana
        if (m_CurPos.x < 0)
            m_CurPos.x = (g_Game.GetScreenW()/BODYSNAKE::SIZE)-1;
        else if (m_CurPos.x > (g_Game.GetScreenW()/BODYSNAKE::SIZE)-1)
            m_CurPos.x = 0;
        if (m_CurPos.y < 3)
            m_CurPos.y = (g_Game.GetScreenH()/BODYSNAKE::SIZE)-1;
        else if (m_CurPos.y > (g_Game.GetScreenH()/BODYSNAKE::SIZE)-1)
            m_CurPos.y = 3;

        //Generar posicion en px de la casilla
        index = static_cast<int>(m_CurPos.y)*(g_Game.GetScreenW()/BODYSNAKE::SIZE)+static_cast<int>(m_CurPos.x);
        m_CurPos.x = index % (g_Game.GetScreenW()/BODYSNAKE::SIZE) * BODYSNAKE::SIZE;
        m_CurPos.y = index / (g_Game.GetScreenW()/BODYSNAKE::SIZE) * BODYSNAKE::SIZE;


        std::deque<BODYSNAKE*>::iterator it = v_SnakeBody.begin();
        if (!(*it)->m_ToJoin)
        {
            //Cuerpo de la Serpiente Color Verde Claro
            (*it)->m_Color = m_SnakeColor;

            //Cabeza de la Serpiente
            BODYSNAKE *m_pBodySnake = new BODYSNAKE();
            m_pBodySnake->m_Pos = m_CurPos;
            m_pBodySnake->m_Color = m_SnakeColor-8;
            m_pBodySnake->m_ToJoin = false;
            v_SnakeBody.insert(v_SnakeBody.begin(), m_pBodySnake);
            it = v_SnakeBody.end();
            delete (*it);
            (*it) = 0x0;
            v_SnakeBody.pop_back();
        }
        else
        {
            //Cabeza de la Serpiente
            (*it)->m_Color = m_SnakeColor-8;
            (*it)->m_ToJoin = false;
            (*it)->m_Pos = m_CurPos;
        }

        m_TimerSnake = GetTickCount() + 1000*0.1f;
    }

    /** Colision? **/
    for (size_t i=1; i<v_SnakeBody.size(); i++)
    {
        if (v_SnakeBody[0]->m_ToJoin)
            break;

        if (v_SnakeBody[0]->m_Pos == v_SnakeBody[i]->m_Pos)
        {
            m_SnakeState = STATE_DEAD;
            v_SnakeBody[i]->m_Color = BROWN;
            EndRound();
        }
    }

    /** Dibujamos **/
    for (size_t i=0; i<v_SnakeBody.size(); i++)
    {
        if (v_SnakeBody[i]->m_ToJoin)
            continue;

        vec2 pos = v_SnakeBody[i]->m_Pos;
        setfillstyle(SOLID_FILL, v_SnakeBody[i]->m_Color);

        if (m_BlockType == BLOCK_SQUARE)
        {
            int points[4*2];
            points[0]=static_cast<int>(pos.x);
            points[1]=static_cast<int>(pos.y);
            points[2]=static_cast<int>(pos.x)+BODYSNAKE::SIZE;
            points[3]=static_cast<int>(pos.y);
            points[4]=static_cast<int>(pos.x)+BODYSNAKE::SIZE;
            points[5]=static_cast<int>(pos.y)+BODYSNAKE::SIZE;
            points[6]=static_cast<int>(pos.x);
            points[7]=static_cast<int>(pos.y)+BODYSNAKE::SIZE;
            fillpoly(4, points);
        }
        else if (m_BlockType == BLOCK_CIRCLE)
            fillellipse(static_cast<int>(pos.x)+BODYSNAKE::SIZE/2, static_cast<int>(pos.y)+BODYSNAKE::SIZE/2, BODYSNAKE::SIZE/2, BODYSNAKE::SIZE/2);
    }

    /** Interaccion con las Entidades **/
    //Aumento de Cuerpo
    if (m_pBall && m_CurPos == m_pBall->m_Pos)
    {
        std::deque<BODYSNAKE*>::iterator it = v_SnakeBody.begin();
        (*it)->m_Color = LIGHTGRAY;
        BODYSNAKE *m_pBodySnake = new BODYSNAKE();
        m_pBodySnake->m_Pos = m_CurPos;
        v_SnakeBody.insert(v_SnakeBody.begin(), m_pBodySnake);

        m_SnakeColor = m_pBall->m_Color;
        m_pBall->m_Pos = GenerateEntityPos();
        m_pBall->m_Color = GenerateColor(m_pBall->m_Color);
        m_Score += 25 * GetNumColorsInBody(LIGHTGRAY);
    }
}

void CGameControllerNormalSnake::DrawEntities()
{
    //Bola de Incremento de Tamaño
    if (m_pBall)
    {
        setfillstyle(INTERLEAVE_FILL, m_pBall->m_Color);
        fillellipse(static_cast<int>(m_pBall->m_Pos.x)+BODYSNAKE::SIZE/2, static_cast<int>(m_pBall->m_Pos.y)+BODYSNAKE::SIZE/2, BODYSNAKE::SIZE/2, BODYSNAKE::SIZE/2);
    }
}

void CGameControllerNormalSnake::EndRound()
{
    ICGameController::EndRound();

    setcolor(BLACK);
    setbkcolor(WHITE);
}
