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
#include <iostream>
#include <conio.h>
#include <CGameContext.hpp>
#include <GameModes/CGameControllerNormalSnake.hpp>
#include <GameModes/CGameControllerArkanoid.hpp>
#include <GameModes/CGameControllerConecta4.hpp>
#include <GameModes/CGameControllerRalla3.hpp>
#include <GameModes/CGameControllerSimon.hpp>
#include <GameModes/CGameControllerCrapTower.hpp>
#include <winbgim.h>
using namespace std;

CGameContext g_Game;
CGameContext::CGameContext()
{
    m_Exit = false;
    m_TimerGame = GetTickCount();
    m_WorldPaused = false;
    m_TimePaused = 0;
    m_GameState = GAME_MENU;
    m_MenuIndex = MENU_INIT;
    m_MenuSelGame = GAME_SNAKE;
    m_ScreenWidth = SCREEN_W;
    m_ScreenHeight = SCREEN_H;

    m_pController = 0x0;

    s_AnimCredits.m_Pos = vec2(GetScreenW(), 0);
    s_AnimCredits.m_Timer = GetTickCount();

    Keyboard.KeyPressed = false;
    Keyboard.Key = 0x0;

    m_pRows[0] = new CRow(CRow::LEFT);
    m_pRows[1] = new CRow(CRow::RIGHT);
}
CGameContext::~CGameContext()
{
    if (m_pController)
        delete m_pController;

    for (int i=0; i<2; delete m_pRows[i++]);
}

void CGameContext::StartGame()
{
    while (!m_Exit)
    {
        m_ScreenWidth = getmaxx();
        m_ScreenHeight = getmaxy();

        if (GetTickCount() >= m_TimerGame)
        {
            /** Obtenemos informacion del teclado **/
            Keyboard.KeyPressed = false;
            if (kbhit())
            {
                Keyboard.KeyPressed  = true;
                Keyboard.Key = getch();

                //Si pulsa ESC se sale del juego este donde este!
                if (Keyboard.Key == 27)
                {
                    g_Game.m_Exit = true;
                    break;
                }
            }
            /** Obtenemos informacion del raton **/
            Mouse.m_isPressed = false;
            if (ismouseclick(WM_LBUTTONDOWN))
                Mouse.m_isPressed = true;

            /** Dibujamos el Juego **/
            setvisualpage(1);
            setactivepage(0);

            cleardevice();

            if (m_GameState == GAME_MENU)
            {
                ShowMenu(m_MenuIndex);
            }
            else if (m_GameState == GAME_PLAY)
            {
                m_pController->DrawEntities();   //Dibujo de las Entidades (Premios, Enemigos, etc..)
                m_pController->Tick();          //Calculos+Dibujo del tipo de Juego (Movimiento y representacion del Personaje con el Entorno)
                m_pController->DrawHUD();       //Dibujo del HUD propio del tipo de Juego (Puntuaciones, Tiempo transcurrido, etc..)
            }

            setvisualpage(0);

            //~60fps
            m_TimerGame = GetTickCount()+1000/60.0f;
        }

        delay(1);
    }

    EndGame();
}

void CGameContext::ShowMenu(short index)
{
    /** Teclado **/
    if (Keyboard.KeyPressed)
    {
        switch (Keyboard.Key)
        {
            case KEY_LEFT:
                m_MenuSelGame--;
                if (m_MenuSelGame<0)
                    m_MenuSelGame = 0;
                break;

            case KEY_RIGHT:
                m_MenuSelGame++;
                if (m_MenuSelGame>NUM_GAMES-1)
                    m_MenuSelGame = NUM_GAMES-1;
                break;

            case 32:
            {
                if (m_pController)
                    delete m_pController;

                m_WorldPaused = false;
                m_GameState = GAME_PLAY;

                if (m_MenuSelGame == GAME_SNAKE)
                    m_pController = new CGameControllerNormalSnake();
                else if (m_MenuSelGame == GAME_ARKANOID)
                    m_pController = new CGameControllerArkanoid();
                else if (m_MenuSelGame == GAME_CONECTA4)
                    m_pController = new CGameControllerConecta4();
                else if (m_MenuSelGame == GAME_RALLA3)
                    m_pController = new CGameControllerRalla3();
                else if (m_MenuSelGame == GAME_SIMON)
                    m_pController = new CGameControllerSimon();
                else if (m_MenuSelGame == GAME_CRAPTOWER)
                    m_pController = new CGameControllerCrapTower();
                else
                    return;

                s_AnimCredits.m_Pos = vec2(GetScreenW(), 0);
                return;
            } break;
        }
    }

    switch(index)
    {
        case MENU_INIT:
            setwindowtitle("RetroJuegos 1.0 - Menu de Seleccion");
            settextstyle(8, HORIZ_DIR, 1);
            setcolor(WHITE);
            setbkcolor(BLACK);

            const char *m_sGame = "";
            if (m_MenuSelGame == GAME_SNAKE)
                m_sGame = "SnaKe";
            else if (m_MenuSelGame == GAME_ARKANOID)
                m_sGame = "ArkanoiD ~Beta~";
            else if (m_MenuSelGame == GAME_CONECTA4)
                m_sGame = "Conecta 4 ~Beta~";
            else if (m_MenuSelGame == GAME_RALLA3)
                m_sGame = "Tres en Ralla";
            else if (m_MenuSelGame == GAME_SIMON)
                m_sGame = "Simon";
            else if (m_MenuSelGame == GAME_CRAPTOWER)
                m_sGame = "Crap Tower";

            m_pRows[0]->offsetx += static_cast<int>(m_pRows[0]->m_Dir.x);
            if (m_pRows[0]->offsetx < -10 || m_pRows[0]->offsetx > 0)
                m_pRows[0]->m_Dir *= -1;
            m_pRows[1]->offsetx += static_cast<int>(m_pRows[1]->m_Dir.x);
            if (m_pRows[1]->offsetx < 0 || m_pRows[1]->offsetx > 10)
                m_pRows[1]->m_Dir *= -1;

            if (m_MenuSelGame > 0)
            {
                int points[3*2];
                points[0] = GetScreenW()/2 - textwidth(m_sGame)/2 - 15 + m_pRows[0]->offsetx;
                points[1] = 10 + textheight(m_sGame)/2;
                points[2] = GetScreenW()/2 - textwidth(m_sGame)/2 - 5 + m_pRows[0]->offsetx;
                points[3] = 10;
                points[4] = GetScreenW()/2 - textwidth(m_sGame)/2 - 5 + m_pRows[0]->offsetx;
                points[5] = 20 + textheight(m_sGame)/2;
                setfillstyle(SOLID_FILL, DARKGRAY);
                fillpoly(3, points);
            }
            if (m_MenuSelGame < NUM_GAMES-1)
            {
                int points[3*2];
                points[0] = GetScreenW()/2 + textwidth(m_sGame)/2 + 2 + m_pRows[1]->offsetx;
                points[1] = 10;
                points[2] = GetScreenW()/2 + textwidth(m_sGame)/2 + 12 + m_pRows[1]->offsetx;
                points[3] = 10 + textheight(m_sGame)/2;
                points[4] = GetScreenW()/2 + textwidth(m_sGame)/2 + 2 + m_pRows[1]->offsetx;
                points[5] = 20 + textheight(m_sGame)/2;
                setfillstyle(SOLID_FILL, DARKGRAY);
                fillpoly(3, points);
            }

            outtextxy(GetScreenW()/2 - textwidth(m_sGame)/2, 10, m_sGame);

            outtextxy(GetScreenW()/2 - textwidth("Pulsa ESPACIO para iniciar")/2, GetScreenH()/2 - textheight("Pulsa ESPACIO para iniciar")/2, "Pulsa ESPACIO para iniciar");

            char credits[] = "Creado por Alexandre Díaz Cuadrado                                        Creado por Alexandre Díaz Cuadrado                                                      Creado por Alexandre Díaz Cuadrado                                          Toc!Toc!! ¿Alguien mirando?? O_o...................                                                          Tutututututututututututututututututututututututututututututututututututu";
            if (GetTickCount() >= s_AnimCredits.m_Timer)
            {
                s_AnimCredits.m_Pos = vec2(s_AnimCredits.m_Pos.x, GetScreenH() - textheight("Alexandre Díaz Cuadrado")-3) - vec2(3, 0);
                s_AnimCredits.m_Timer = GetTickCount() + 1000 * 0.07f;

                if (s_AnimCredits.m_Pos.x+textwidth(credits) < 0)
                    s_AnimCredits.m_Pos.x = GetScreenW();
            }
            outtextxy(static_cast<int>(s_AnimCredits.m_Pos.x), GetScreenH() - textheight(credits)-3, credits);
            break;
    }
}

void CGameContext::EndGame()
{
}
