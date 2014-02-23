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
#include "CGameControllerArkanoid.hpp"
#include <ICGameContext.hpp>
#include <winbgim.h>
#include <time.h>
#include <stdio.h>
#include <external/math.h>
#include <external/vmath.h>


CBall::CBall(vec2 pos, short color)
:
    ICArkEnt(ENT_BALL)
{
    m_Pos = pos;
    m_Color = color;
    m_Dir = vec2(1, -1);
    m_Vel = 2.5f;
}

CWall::CWall(vec2 pos, int health, short color)
:
    ICArkEnt(ENT_WALL)
{
    m_Pos = pos;
    m_Life = health;
    m_Color = color;
}

//-----------------------------------------

CGameControllerArkanoid::CGameControllerArkanoid()
{
    srand(time(NULL));
    setwindowtitle("ArkanoiD Beta");

    m_Size = 3;
    m_CurPos = vec2(100, g_Game.GetScreenH()-BSIZE-5);
    m_PrevCurDir = m_CurDir = vec2(0, 0);
    m_TimerArkanoid = GetTickCount();
    m_Lifes = 3;
    m_ClockGame = GetTickCount();
    m_Score = 0;
    m_isLost = false;
    m_Level = 15;

    GenerateMap();
}
CGameControllerArkanoid::~CGameControllerArkanoid()
{
    std::deque<ICArkEnt*>::iterator it = v_Entities.begin();
    while(it != v_Entities.end())
    {
        delete (*it);
        (*it) = 0x0;
        it = v_Entities.erase(it);
    }
}

void CGameControllerArkanoid::StartRound()
{
    Reset();
}
void CGameControllerArkanoid::EndRound()
{
    ICGameController::EndRound();

    m_isLost = true;
    setcolor(BLACK);
    setbkcolor(WHITE);
}

void CGameControllerArkanoid::Reset()
{
    m_Size = 3;
    m_CurPos = vec2(100, g_Game.GetScreenH()-BSIZE-5);
    m_PrevCurDir = m_CurDir = vec2(0, 0);
    m_TimerArkanoid = GetTickCount();
    m_Lifes = 3;
    m_ClockGame = GetTickCount();
    m_isLost = false;

    std::deque<ICArkEnt*>::iterator it = v_Entities.begin();
    while(it != v_Entities.end())
    {
        if ((*it) == NULL)
            continue;

        delete (*it);
        (*it) = 0x0;
        it = v_Entities.erase(it);
    }
}

bool CGameControllerArkanoid::CanShootBall()
{
    if (g_Game.m_WorldPaused || m_Lifes <= 0)
        return false;

    for (size_t i=0; i<v_Entities.size(); i++)
    {
        if (v_Entities[i] == NULL)
            continue;

        if (v_Entities[i]->GetType() == ENT_BALL)
            return false;
    }

    return true;
}

short CGameControllerArkanoid::GenerateColor(short notthis)
{
    short result;

    do
    {
        result = rand() % 15 + 1;
    } while (result == notthis);

    return result;
}

void CGameControllerArkanoid::GenerateMap()
{
    vec2 tpos;
    bool error;

    for (int i=0; i<m_Level; i++)
    {
        error = false;
        do
        {
            tpos.x = rand() % (g_Game.GetScreenW() / (BSIZE*CWall::BSIZE));
            tpos.x *= (BSIZE*CWall::BSIZE);
            tpos.y = rand() % ((g_Game.GetScreenH()/2) / BSIZE);
            tpos.y += 3;
            tpos.y *= BSIZE;

            for (size_t i=0; i<v_Entities.size(); i++)
            {
                if (v_Entities[i]->m_Pos == tpos)
                {
                    error = true;
                    break;
                }
            }
        } while(error);

        CWall *m_Wall = new CWall(tpos, rand() % 3 + 1, GenerateColor());
        v_Entities.push_back(m_Wall);
    }
}

void CGameControllerArkanoid::DrawHUD()
{
    char buff[255];
    time_t m_Time;
    struct tm *m_tmTime;

    settextstyle(10, HORIZ_DIR, 2);

    /** Vidas **/
    vec2 pos;
    for (int i=0; i<m_Lifes; i++)
    {
        pos = vec2((BSIZE+3)*1, 0) * i + vec2(6, 6);
        setfillstyle(SOLID_FILL, LIGHTGRAY);
        fillellipse(static_cast<int>(pos.x)+BSIZE/2, static_cast<int>(pos.y)+BSIZE/2, BSIZE/2, BSIZE/2);
    }


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
    outtextxy(g_Game.GetScreenW()/2 - textwidth(buff)/2, 3, buff);

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

    /** Perdio **/
    if (m_isLost)
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

    /** Puede Lanzar la Bola **/
    if (CanShootBall())
    {
        int xt = g_Game.GetScreenW()/2 - textwidth("Haz CLICK para empezar")/2;
        int yt = g_Game.GetScreenH()/2 - textheight("Haz CLICK para empezar")/2;
        outtextxy(xt, yt, "Haz CLICK para empezar");
        rectangle(-1, yt-2, g_Game.GetScreenW()+1, yt-1 + textheight("Haz CLICK para empezar") + 2);
    }
}

void CGameControllerArkanoid::Tick()
{
    /** Entrada de Teclado/Raton **/
    //Teclado
    if (g_Game.Keyboard.KeyPressed)
    {
        switch(g_Game.Keyboard.Key)
        {
            case 13:
                if (m_Lifes <= 0 && m_isLost)
                {
                    g_Game.m_GameState = ICGameContext::GAME_MENU;
                    return;
                }
                break;
        }
    }
    //Raton
    if (g_Game.Mouse.m_isPressed && CanShootBall())
    {
        CBall *m_pBall = new CBall(vec2(m_CurPos.x, m_CurPos.y - BSIZE), LIGHTGRAY);
        v_Entities.push_back(m_pBall);
        m_Lifes--;
        clearmouseclick(WM_LBUTTONDOWN);
    }


    if (!g_Game.m_WorldPaused)
    {
        /** Movimiento de la Plataforma **/
        m_CurPos.x = mousex() - ((BSIZE*m_Size)/2);
        m_CurPos.y = g_Game.GetScreenH()-BSIZE-5;
        //Choca contra los Limites
        if (m_CurPos.x < 0)
            m_CurPos.x = 0;
        else if (m_CurPos.x+BSIZE*m_Size > g_Game.GetScreenW())
            m_CurPos.x = g_Game.GetScreenW()-BSIZE*m_Size;

        /** Movimiento de las Entidades **/
        for (size_t i = 0; i<v_Entities.size(); i++)
        {
            if (v_Entities[i] == NULL)
                continue;

            if (v_Entities[i]->GetType() == ENT_BALL)
            {
                //Movimiento
                CBall *m_pBall = static_cast<CBall*>(v_Entities[i]);
                m_pBall->m_Pos += m_pBall->m_Dir * m_pBall->m_Vel;

                //Rebota Contra las Paredes
                if (m_pBall->m_Pos.x <= 0)
                {
                    m_pBall->m_Pos.x = 1;
                    m_pBall->m_Dir.x *= -1;
                }
                else if (m_pBall->m_Pos.x+BSIZE >= g_Game.GetScreenW())
                {
                    m_pBall->m_Pos.x = g_Game.GetScreenW()-BSIZE-1;
                    m_pBall->m_Dir.x *= -1;
                }
                if (m_pBall->m_Pos.y <= 30)
                {
                    m_pBall->m_Pos.y = 31;
                    m_pBall->m_Dir.y *= -1;
                }
                //La pelota se "pierde"
                else if (m_pBall->m_Pos.y+BSIZE >= g_Game.GetScreenH())
                {
                    RemoveEntity(v_Entities[i]);
                    if (m_Lifes == 0)
                        EndRound();
                }

                //Rebota Contra Plataforma
                if (m_pBall->m_Pos.x+BSIZE >= m_CurPos.x && m_pBall->m_Pos.x <= m_CurPos.x + BSIZE*m_Size && m_pBall->m_Pos.y+BSIZE >= m_CurPos.y && m_pBall->m_Pos.y <= m_CurPos.y + BSIZE)
                {
                    m_pBall->m_Dir.y *= -1;
                    if (m_pBall->m_Pos.y+BSIZE > m_CurPos.y)
                    {
                        if (m_pBall->m_Pos.x <= m_CurPos.x)
                            m_pBall->m_Pos.x = m_CurPos.x-2;
                        else if (m_pBall->m_Pos.x >= m_CurPos.x+BSIZE*m_Size)
                            m_pBall->m_Pos.x = m_CurPos.x+BSIZE*m_Size+2;

                        m_pBall->m_Dir.x *= -1;

                        //Aceleracion de la Bola
                        m_pBall->m_Vel += 0.2f;
                        if (m_pBall->m_Vel > 7.0f)
                            m_pBall->m_Vel = 7.0f;
                    }
                }
                //Rebota Contra Bloque
                for (size_t o=0; o<v_Entities.size(); o++)
                {
                    if (!v_Entities[o] || v_Entities[o]->GetType() != ENT_WALL)
                        continue;

                    CWall *m_pWall = static_cast<CWall*>(v_Entities[o]);
                    if (m_pBall->m_Pos.x+BSIZE >= m_pWall->m_Pos.x && m_pBall->m_Pos.x <= m_pWall->m_Pos.x + BSIZE*CWall::BSIZE && m_pBall->m_Pos.y+BSIZE >= m_pWall->m_Pos.y && m_pBall->m_Pos.y <= m_pWall->m_Pos.y + BSIZE)
                    {
                        if (m_pBall->m_Pos.y+BSIZE > m_pWall->m_Pos.y && m_pBall->m_Pos.x <= m_pWall->m_Pos.x)
                        {
                            m_pBall->m_Pos.x = m_pWall->m_Pos.x-2;
                            m_pBall->m_Dir.x *= -1;
                        }
                        else if (m_pBall->m_Pos.y+BSIZE > m_pWall->m_Pos.y && m_pBall->m_Pos.x >= m_pWall->m_Pos.x+BSIZE*CWall::BSIZE)
                        {
                            m_pBall->m_Pos.x = m_pWall->m_Pos.x+BSIZE*CWall::BSIZE+2;
                            m_pBall->m_Dir.x *= -1;
                        }
                        else
                        {
                            if (m_pBall->m_Dir.y < 0)
                                m_pBall->m_Pos.y = m_pWall->m_Pos.y+BSIZE+2;
                            else if (m_pBall->m_Dir.y > 0)
                                m_pBall->m_Pos.y = m_pWall->m_Pos.y-BSIZE-2;

                            m_pBall->m_Dir.y *= -1;
                        }

                        m_Score += 25;

                        m_pWall->m_Life--;
                        if (m_pWall->m_Life <= 0)
                            RemoveEntity(v_Entities[o]);
                    }
                }
            }
        }

        /** Termina Mapa **/
        bool m_isEmpty = true;
        for (size_t o=0; o<v_Entities.size(); o++)
        {
            if (!v_Entities[o] || v_Entities[o]->GetType() != ENT_WALL)
                continue;

            m_isEmpty = false;
            break;
        }

        if (m_isEmpty)
        {
            m_Level += 15;
            m_Score += 555;
            m_Lifes = min (m_Lifes+2, 5);

            //Borrar todas las Bolas
            for (size_t o=0; o<v_Entities.size(); o++)
            {
                if (!v_Entities[o] || v_Entities[o]->GetType() != ENT_BALL)
                    continue;

                RemoveEntity(v_Entities[o]);
            }

            GenerateMap();
        }
    }

    /** Dibujamos **/
    //Entidades
    for (size_t i = 0; i<v_Entities.size(); i++)
    {
        if (v_Entities[i]->GetType() == ENT_BALL)
        {
            CBall *m_pBall = static_cast<CBall*>(v_Entities[i]);

            setfillstyle(SOLID_FILL, LIGHTGRAY);
            fillellipse(static_cast<int>(m_pBall->m_Pos.x)+BSIZE/2, static_cast<int>(m_pBall->m_Pos.y)+BSIZE/2, BSIZE/2, BSIZE/2);
        }
        else if (v_Entities[i]->GetType() == ENT_WALL)
        {
            CWall *m_pWall = static_cast<CWall*>(v_Entities[i]);

            if (m_pWall->m_Life == 1)
                setfillstyle(WIDE_DOT_FILL, m_pWall->m_Color);
            else if (m_pWall->m_Life == 2)
                setfillstyle(CLOSE_DOT_FILL, m_pWall->m_Color);
            else
                setfillstyle(SOLID_FILL, m_pWall->m_Color);

            int points[4*2];
            points[0] = static_cast<int>(m_pWall->m_Pos.x);
            points[1] = static_cast<int>(m_pWall->m_Pos.y);
            points[2] = static_cast<int>(m_pWall->m_Pos.x)+BSIZE*CWall::BSIZE;
            points[3] = static_cast<int>(m_pWall->m_Pos.y);
            points[4] = static_cast<int>(m_pWall->m_Pos.x)+BSIZE*CWall::BSIZE;
            points[5] = static_cast<int>(m_pWall->m_Pos.y)+BSIZE;
            points[6] = static_cast<int>(m_pWall->m_Pos.x);
            points[7] = static_cast<int>(m_pWall->m_Pos.y)+BSIZE;
            fillpoly(4, points);
        }
    }
    //Plataforma
    setfillstyle(LTSLASH_FILL, RED);
    int points[4*2];
    points[0] = static_cast<int>(m_CurPos.x);
    points[1] = static_cast<int>(m_CurPos.y);
    points[2] = static_cast<int>(m_CurPos.x)+BSIZE*m_Size;
    points[3] = static_cast<int>(m_CurPos.y);
    points[4] = static_cast<int>(m_CurPos.x)+BSIZE*m_Size;
    points[5] = static_cast<int>(m_CurPos.y)+BSIZE;
    points[6] = static_cast<int>(m_CurPos.x);
    points[7] = static_cast<int>(m_CurPos.y)+BSIZE;
    fillpoly(4, points);
}


bool CGameControllerArkanoid::RemoveEntity(ICArkEnt *ent)
{
    std::deque<ICArkEnt*>::iterator it = v_Entities.begin();
    while(it != v_Entities.end())
    {
        if ((*it) == ent)
        {
            delete((*it));
            (*it) = 0x0;
            it = v_Entities.erase(it);
            return true;
        }
        else
            ++it;
    }

    return false;
}
