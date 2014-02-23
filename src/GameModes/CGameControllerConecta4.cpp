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
#include "CGameControllerConecta4.hpp"
#include <ICGameContext.hpp>
#include <winbgim.h>
#include <time.h>


//-----

CGameControllerConecta4::CGameControllerConecta4()
{
    srand(time(NULL));
    setwindowtitle("Conecta 4 Beta");
    settextstyle(2, HORIZ_DIR, 5);

    //Limpiar Tablero
    for (int i=0;i<8;i++)
    {
        for (int o=0;o<8;o++)
            v_GameTable[i][o]=PLAYER_NULL;
    }

    m_CPU = false;
    m_ActivePlayer = PLAYER_1;
    m_IndicatorChip.x =  m_IndicatorChip.y = 1;
    m_IndicatorChip.m_Dir = 1;
    m_PlayerWin = PLAYER_NULL;
}
CGameControllerConecta4::~CGameControllerConecta4()
{
}


void CGameControllerConecta4::EndRound()
{
    ICGameController::EndRound();
}
void CGameControllerConecta4::DrawHUD()
{

    /** Usuario Ganador **/
    if (m_PlayerWin != PLAYER_NULL)
    {
        int xt = 0;
        int yt = 0;
        if (m_PlayerWin == PLAYER_1)
        {
            xt = g_Game.GetScreenW()/2 - textwidth("Jugador 1 GANA!")/2;
            yt = g_Game.GetScreenH()/2 - textheight("Jugador 1 GANA!")/2;

            outtextxy(xt, yt, "Jugador 1 GANA!");

            xt = g_Game.GetScreenW()/2 - textwidth("pulsa INTRO para reiniciar")/2;
            yt = g_Game.GetScreenH()/2 - textheight("pulsa INTRO para reiniciar")/2;
            yt += textheight("Jugador 1 GANA!") + 15;
        }
        else if (m_PlayerWin == PLAYER_2)
        {
            xt = g_Game.GetScreenW()/2 - textwidth("Jugador 2 GANA!")/2;
            yt = g_Game.GetScreenH()/2 - textheight("Jugador 2 GANA!")/2;

            outtextxy(xt, yt, "Jugador 2 GANA!");

            xt = g_Game.GetScreenW()/2 - textwidth("pulsa INTRO para reiniciar")/2;
            yt = g_Game.GetScreenH()/2 - textheight("pulsa INTRO para reiniciar")/2;
            yt += textheight("Jugador 2 GANA!") + 15;
        }
        outtextxy(xt, yt, "pulsa INTRO para reiniciar");
        rectangle(-1, yt-2, g_Game.GetScreenW()+1, yt-1 + textheight("pulsa INTRO para reiniciar") + 2);
    }

}

void CGameControllerConecta4::Tick()
{
    /** Teclado **/
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

        if (!g_Game.m_WorldPaused)
        {
            switch(g_Game.Keyboard.Key)
            {
                case 'a':
                case 'A':
                case KEY_LEFT:
                        m_IndicatorChip.y = 1;
                        m_IndicatorChip.x = max(m_IndicatorChip.x-1, 1);
                    break;

                case 'd':
                case 'D':
                case KEY_RIGHT:
                    m_IndicatorChip.y = 1;
                    m_IndicatorChip.x = min(m_IndicatorChip.x+1, 8);
                    break;

                case 32:
                    m_IndicatorChip.y = 1;

                    for (int i=7;i>=0;i--)
                    {
                        if (v_GameTable[m_IndicatorChip.x-1][i] == PLAYER_NULL)
                        {
                            v_GameTable[m_IndicatorChip.x-1][i] = m_ActivePlayer;
                            break;
                        }
                    }

                    if (m_CPU)
                        m_ActivePlayer = (m_ActivePlayer==PLAYER_1)?PLAYER_CPU:PLAYER_1;
                    else
                        m_ActivePlayer = (m_ActivePlayer==PLAYER_1)?PLAYER_2:PLAYER_1;
                    break;
            }
        }
    }

    /** Comprobar Lineas **/
    if (m_PlayerWin == PLAYER_NULL)
    {
        int m_TeamChip[2] = {0, 0};

        //Comprobar la Vertical
        for (int i=0; i<8; i++)
        {
            m_TeamChip[0] = 0;
            m_TeamChip[1] = 0;
            for (int o=7; o>=0; o--)
            {
                if (v_GameTable[i][o] == PLAYER_1)
                {
                    m_TeamChip[1]=0;
                    m_TeamChip[0]++;

                    if (m_TeamChip[0] >= 4)
                    {
                        m_PlayerWin = PLAYER_1;
                        EndRound();
                        break;
                    }
                }
                else if (v_GameTable[i][o] == PLAYER_2 || v_GameTable[i][o] == PLAYER_CPU)
                {
                    m_TeamChip[0]=0;
                    m_TeamChip[1]++;

                    if (m_TeamChip[1] >= 4)
                    {
                        if (m_CPU)
                            m_PlayerWin = PLAYER_CPU;
                        else
                            m_PlayerWin = PLAYER_2;

                        EndRound();
                        break;
                    }
                }
                else
                {
                    m_TeamChip[0]=0;
                    m_TeamChip[1]=0;
                }
            }
        }

        //Comprobar la Horizontal
        for (int i=0; i<8; i++)
        {
            m_TeamChip[0] = 0;
            m_TeamChip[1] = 0;
            for (int o=7; o>=0; o--)
            {
                if (v_GameTable[o][i] == PLAYER_1)
                {
                    m_TeamChip[1]=0;
                    m_TeamChip[0]++;

                    if (m_TeamChip[0] >= 4)
                    {
                        m_PlayerWin = PLAYER_1;
                        EndRound();
                        break;
                    }
                }
                else if (v_GameTable[o][i] == PLAYER_2 || v_GameTable[o][i] == PLAYER_CPU)
                {
                    m_TeamChip[0]=0;
                    m_TeamChip[1]++;

                    if (m_TeamChip[1] >= 4)
                    {
                        if (m_CPU)
                            m_PlayerWin = PLAYER_CPU;
                        else
                            m_PlayerWin = PLAYER_2;

                        EndRound();
                        break;
                    }
                }
                else
                {
                    m_TeamChip[0]=0;
                    m_TeamChip[1]=0;
                }
            }
        }

        //Comprobar Diagonales
        /*for (int i=7; i>=0; i--)
        {
            m_TeamChip[0] = 0;
            m_TeamChip[1] = 0;

            for (int o=7; o>=0; o--)
            {
                if (v_GameTable[i][8*o] == PLAYER_1)
                {
                    m_TeamChip[0]++;
                    m_TeamChip[1]=0;

                    if (m_TeamChip[0] >= 4)
                    {
                        m_PlayerWin = PLAYER_1;
                        EndRound();
                        break;
                    }
                }
                else if (v_GameTable[i][8*o] == PLAYER_2 || v_GameTable[o*8][i] == PLAYER_CPU)
                {
                    m_TeamChip[1]++;
                    m_TeamChip[0]=0;

                    if (m_TeamChip[1] >= 4)
                    {
                        if (m_CPU)
                            m_PlayerWin = PLAYER_CPU;
                        else
                            m_PlayerWin = PLAYER_2;

                        EndRound();
                        break;
                    }
                }
                else
                {
                    m_TeamChip[0]=0;
                    m_TeamChip[1]=0;
                }
            }
        }*/
    }



    /** Dibujamos**/
    m_OffsetX = g_Game.GetScreenW() - g_Game.GetScreenW()/2 - 10*8 - 10*6;
    m_OffsetY = g_Game.GetScreenH() - g_Game.GetScreenH()/2 - 10*7 - 10*5;

    //Ficha Indicadora
    if (!g_Game.m_WorldPaused && m_ActivePlayer != PLAYER_CPU)
    {
        m_IndicatorChip.m_Color = (m_ActivePlayer==PLAYER_1)?RED:BLUE;

        m_IndicatorChip.y += m_IndicatorChip.m_Dir;
        if (m_IndicatorChip.y >= 15 || m_IndicatorChip.y <= 0)
            m_IndicatorChip.m_Dir *=-1;

        const char *buff;
        if (m_ActivePlayer == PLAYER_1)
            buff = "JUGADOR 1";
        else
            buff = "JUGADOR 2";

        outtextxy(m_OffsetX+30*m_IndicatorChip.x-textwidth(buff)/2, m_OffsetY+10+m_IndicatorChip.y-textheight(buff)-15, buff);
        setfillstyle(SOLID_FILL, m_IndicatorChip.m_Color+8);
        fillellipse(m_OffsetX+30*m_IndicatorChip.x,m_OffsetY+10+m_IndicatorChip.y, 10, 10);
        setfillstyle(SOLID_FILL, m_IndicatorChip.m_Color);
        fillellipse(m_OffsetX+30*m_IndicatorChip.x,m_OffsetY+10+m_IndicatorChip.y, 7, 7);
    }

    //Tablero
    setfillstyle(SOLID_FILL, DARKGRAY);
    int points[4*2];
    points[0]=m_OffsetX+10;
    points[1]=m_OffsetY+30;
    points[2]=m_OffsetX+10 + 20*8+10*9;
    points[3]=m_OffsetY+30;
    points[4]=m_OffsetX+10 + 20*8+10*9;
    points[5]=m_OffsetY + 20*8+10*8;
    points[6]=m_OffsetX+10;
    points[7]=m_OffsetY + 20*8+10*8;
    fillpoly(4, points);

    int m_PosCol=1;
    int m_PosRow=1;
    setfillstyle(SOLID_FILL, BLACK);
    for (int i=0; i<8*8; i++, m_PosRow++)
    {
        if (i%8 == 0) { m_PosCol++; m_PosRow=1; }
        fillellipse(m_OffsetX+30*m_PosRow,m_OffsetY+25*m_PosCol, 10, 10);
    }

    //Fichas Colocadas
    for (int i=0; i<8;i++)
    {
        for (int o=0; o<8; o++)
        {
            if (v_GameTable[i][o] == PLAYER_NULL)
                continue;

            if (v_GameTable[i][o] == PLAYER_1)
            {
                setfillstyle(SOLID_FILL, LIGHTRED);
                fillellipse(m_OffsetX+30*(i+1),m_OffsetY+25*(o+2), 10, 10);
                setfillstyle(SOLID_FILL, RED);
                fillellipse(m_OffsetX+30*(i+1),m_OffsetY+25*(o+2), 7, 7);
            }
            else if (v_GameTable[i][o] == PLAYER_2)
            {
                setfillstyle(SOLID_FILL, LIGHTBLUE);
                fillellipse(m_OffsetX+30*(i+1),m_OffsetY+25*(o+2), 10, 10);
                setfillstyle(SOLID_FILL, BLUE);
                fillellipse(m_OffsetX+30*(i+1),m_OffsetY+25*(o+2), 7, 7);
            }
        }
    }
}
