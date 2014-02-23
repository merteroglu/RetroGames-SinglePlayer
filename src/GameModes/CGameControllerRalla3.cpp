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
#include "CGameControllerRalla3.hpp"
#include <ICGameContext.hpp>
#include <time.h>
#include <winbgim.h>

CGameControllerRalla3::CGameControllerRalla3()
:
    dimTX(200),
    dimTY(200)
{
    srand(time(NULL));
    setwindowtitle("3 en Ralla");
    settextstyle(2, HORIZ_DIR, 5);

    m_TimerRalla3= GetTickCount();
    m_ActivePlayer = PLAYER_1;
    m_PlayerWin = PLAYER_NULL;
    m_CPU = true;
    m_Indicator.m_Timer = GetTickCount();

    //Tablero Vacio
    for (int i=0;i<3;i++)
        for (int o=0;o<3;o++)
            v_GameTable[i][o]=PLAYER_NULL;
}
CGameControllerRalla3::~CGameControllerRalla3()
{
}


void CGameControllerRalla3::EndRound()
{
    ICGameController::EndRound();
}

void CGameControllerRalla3::DrawHUD()
{
    if (m_PlayerWin == PLAYER_NULL)
    {
        if (m_ActivePlayer == PLAYER_1)
        {
            circle(mousex(), mousey(), 10);
            outtextxy(g_Game.GetScreenW()/2-textwidth("TURNO: JUGADOR 1")/2, dimTY+60, "TURNO: JUGADOR 1");
        }
        else
        {
            moveto(mousex()-10, mousey()-10);
            lineto(mousex()+10, mousey()+10);
            moveto(mousex()+10, mousey()-10);
            lineto(mousex()-10, mousey()+10);

            if (m_CPU)
                outtextxy(g_Game.GetScreenW()/2-textwidth("TURNO: CPU")/2, dimTY+120, "TURNO: CPU");
            else
                outtextxy(g_Game.GetScreenW()/2-textwidth("TURNO: JUGADOR 2")/2, dimTY+60, "TURNO: JUGADOR 2");
        }
    }
    else
    {
        const char *sBuff;
        if (m_PlayerWin == PLAYER_1)
            sBuff = "Gana el JUGADOR 1";
        else if (m_PlayerWin == PLAYER_2)
            sBuff = "Gana el JUGADOR 2";
        else if (m_PlayerWin == PLAYER_CPU)
            sBuff = "Gana la CPU";
        else
            sBuff = "EMPATE!";

        int xt = g_Game.GetScreenW()/2 - textwidth(sBuff)/2;
        int yt = g_Game.GetScreenH()/2 - textheight(sBuff)/2;

        outtextxy(xt, yt, sBuff);

        xt = g_Game.GetScreenW()/2 - textwidth("pulsa INTRO para reiniciar")/2;
        yt = g_Game.GetScreenH()/2 - textheight("pulsa INTRO para reiniciar")/2;
        yt += textheight(sBuff) + 15;
        outtextxy(xt, yt, "pulsa INTRO para reiniciar");
        rectangle(-1, yt-2, g_Game.GetScreenW()+1, yt-1 + textheight("pulsa INTRO para reiniciar") + 2);
    }
}

void CGameControllerRalla3::CheckWinner(PLAYERTYPE winner)
{
    /** Verificacion Lineas **/
    if ((v_GameTable[0][0] == winner && v_GameTable[1][0] == winner && v_GameTable[2][0] == winner)
        || (v_GameTable[0][1] == winner && v_GameTable[1][1] == winner && v_GameTable[2][1] == winner)
        || (v_GameTable[0][2] == winner && v_GameTable[1][2] == winner && v_GameTable[2][2] == winner)
        || (v_GameTable[0][0] == winner && v_GameTable[0][1] == winner && v_GameTable[0][2] == winner)
        || (v_GameTable[1][0] == winner && v_GameTable[1][1] == winner && v_GameTable[1][2] == winner)
        || (v_GameTable[2][0] == winner && v_GameTable[2][1] == winner && v_GameTable[2][2] == winner)
        || (v_GameTable[2][0] == winner && v_GameTable[1][1] == winner && v_GameTable[0][2] == winner)
        || (v_GameTable[0][0] == winner && v_GameTable[1][1] == winner && v_GameTable[2][2] == winner)
        )
    {
        m_PlayerWin = winner;
        EndRound();
    }
}

void CGameControllerRalla3::Tick()
{
    if (g_Game.Keyboard.KeyPressed)
    {
        switch(g_Game.Keyboard.Key)
        {
            case 13:
                if (g_Game.m_WorldPaused && m_PlayerWin != PLAYER_NULL)
                {
                    g_Game.m_GameState = ICGameContext::GAME_MENU;
                    return;
                }
                break;
        }
    }

    if (!g_Game.m_WorldPaused)
    {
        /** Captar Raton **/
        if (g_Game.Mouse.m_isPressed)
        {
            vec2 m_MousePos = vec2(mousex(), mousey());

            for (int i=0; i<3;i++)
            {
                for (int o=0;o<3;o++)
                {
                    if (m_MousePos.x > g_Game.GetScreenW()/2-dimTX/2 + dimTX/3*o && m_MousePos.x <= g_Game.GetScreenW()/2-dimTX/2 + dimTX/3*(o+1)
                        && m_MousePos.y >= 20+((dimTY/3)*i) && m_MousePos.y <= 20+((dimTY/3)*(i+1)))
                    {
                        if (v_GameTable[i][o] != PLAYER_NULL)
                        {
                            m_Indicator.m_Timer = GetTickCount() + 1000 * 0.15f;
                            m_Indicator.m_Pos = vec2(g_Game.GetScreenW()/2-dimTX/2 + dimTX/3*o, 20+((dimTY/3)*i));
                            continue;
                        }

                        v_GameTable[i][o] = m_ActivePlayer;
                        CheckWinner(m_ActivePlayer);

                        if (m_CPU)
                            m_ActivePlayer = (m_ActivePlayer==PLAYER_1)?PLAYER_CPU:PLAYER_1;
                        else
                            m_ActivePlayer = (m_ActivePlayer==PLAYER_1)?PLAYER_2:PLAYER_1;
                    }
                }
            }
            clearmouseclick(WM_LBUTTONDOWN);
        }

        //Comprobar Empate
        if (m_PlayerWin == PLAYER_NULL)
        {
            bool draw=true;
            for (int i=0; i<3; i++)
            {
                for (int o=0; o<3; o++)
                    if (v_GameTable[i][o] == PLAYER_NULL)
                    {
                        draw=false;
                        break;
                    }
            }
            if (draw)
            {
                m_PlayerWin = PLAYER_DRAW;
                EndRound();
            }
        }

        //Jugada de la Makina
        if (m_PlayerWin == PLAYER_NULL && m_ActivePlayer == PLAYER_CPU)
        {
            int i, o;
            bool error = true;
            for (int i=rand()%3; i<3; i++)
            {
                for (int o=rand()%3; o<3; o++)
                {
                    if (v_GameTable[i][o] == PLAYER_NULL)
                    {
                        if ((i-1 > 0 && (v_GameTable[i-1][o] == PLAYER_NULL || v_GameTable[i-1][o] == PLAYER_CPU))
                            || (i+i<3 && (v_GameTable[i+1][o] == PLAYER_NULL || v_GameTable[i+1][o] == PLAYER_CPU))
                            || (o-1 > 0 && (v_GameTable[i][o-1] == PLAYER_NULL || v_GameTable[i][o-1] == PLAYER_CPU))
                            || (o+1 > 0 && (v_GameTable[i][o+1] == PLAYER_NULL || v_GameTable[i][o+1] == PLAYER_CPU))
                            || (i-1>0 && o-1>0 && (v_GameTable[i-1][o-1] == PLAYER_NULL || v_GameTable[i-1][o-1] == PLAYER_CPU))
                            || (i+1<3 && o-1>0 && (v_GameTable[i+1][o-1] == PLAYER_NULL || v_GameTable[i+1][o-1] == PLAYER_CPU))
                            || (i+1<3 && o+1<3 && (v_GameTable[i+1][o+1] == PLAYER_NULL || v_GameTable[i+1][o+1] == PLAYER_CPU))
                            || (i-1>0 && o+1<3 && (v_GameTable[i-1][o+1] == PLAYER_NULL || v_GameTable[i-1][o+1] == PLAYER_CPU)))
                        {
                            v_GameTable[i][o]=PLAYER_CPU;
                            error = false;
                            break;
                        }
                    }
                }

                if (!error)
                    break;
            }

            if (error)
            {
                do {
                    i=rand() % 3;
                    o=rand() % 3;
                } while (v_GameTable[i][o] != PLAYER_NULL);

                v_GameTable[i][o]=PLAYER_CPU;
            }

            CheckWinner(PLAYER_CPU);
            m_ActivePlayer = PLAYER_1;
        }

        //Resetear Color Indicador
        if (GetTickCount() < m_Indicator.m_Timer)
        {
            setfillstyle(SOLID_FILL, RED);
            int points[4*2];
            points[0]=static_cast<int>(m_Indicator.m_Pos.x);
            points[1]=static_cast<int>(m_Indicator.m_Pos.y);
            points[2]=static_cast<int>(m_Indicator.m_Pos.x) + dimTX/3;
            points[3]=points[1];
            points[4]=points[2];
            points[5]=static_cast<int>(m_Indicator.m_Pos.y) + dimTY/3;
            points[6]=points[0];
            points[7]=points[5];
            fillpoly(4, points);
        }
    }

    /** Dibujar Tablero **/
    int offsetx = dimTX/3;
    int offsety = dimTY/3;

    setcolor(DARKGRAY);
    for (int i=offsety; i<dimTY-dimTY/3; i+=offsety)
    {
        moveto(g_Game.GetScreenW()/2-dimTX/2, i+20);
        lineto(g_Game.GetScreenW()/2-dimTX/2+dimTX, i+20);
    }
    for (int i=offsetx; i<dimTX-dimTX/3; i+=offsetx)
    {
        moveto(g_Game.GetScreenW()/2-dimTX/2+i, 1+20);
        lineto(g_Game.GetScreenW()/2-dimTX/2+i, dimTY+20);
    }
    setcolor(WHITE);
    //Fichas
    for (int i=0;i<3;i++)
    {
        for (int o=0;o<3;o++)
        {
            if (v_GameTable[i][o] == PLAYER_NULL)
                continue;

            if (v_GameTable[i][o] == PLAYER_1)
            {
                circle(g_Game.GetScreenW()/2-dimTX/2 + dimTX/3*(o+1) - ((dimTX/3)/2), 20+dimTY/3*(i+1) - (dimTY/3)/2, 25);
            }
            else
            {
                moveto(g_Game.GetScreenW()/2-dimTX/2 + dimTX/3*(o+1)-((dimTX/3)/2) -25, 20+dimTY/3*(i+1)-(dimTY/3)/2 -25);
                lineto(g_Game.GetScreenW()/2-dimTX/2 + dimTX/3*(o+1)-((dimTX/3)/2) +25, 20+dimTY/3*(i+1)-(dimTY/3)/2 +25);
                moveto(g_Game.GetScreenW()/2-dimTX/2 + dimTX/3*(o+1)-((dimTX/3)/2) +25, 20+dimTY/3*(i+1)-(dimTY/3)/2 -25);
                lineto(g_Game.GetScreenW()/2-dimTX/2 + dimTX/3*(o+1)-((dimTX/3)/2) -25, 20+dimTY/3*(i+1)-(dimTY/3)/2 +25);
            }
        }
    }
}
