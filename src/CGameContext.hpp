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
#ifndef IGAMECONTEXT_H
#define IGAMECONTEXT_H
#include <CGameController.hpp>
#include <windows.h>
#include <external/vmath.h>

#define SCREEN_H    400
#define SCREEN_W    400


class CGameContext
{
public:
    CGameContext();
    ~CGameContext();

    enum
    {
        MENU_INIT=0,
        NUM_MENUS,

        GAME_MENU=0,
        GAME_PLAY
    };

    enum
    {
        GAME_SNAKE=0,
        GAME_ARKANOID,
        GAME_CONECTA4,
        GAME_RALLA3,
        GAME_SIMON,
        GAME_CRAPTOWER,
        NUM_GAMES
    };

    class CWINDOW
    {
        public:
            HWND m_HWND;
            int m_Apage;
            int m_Vpage;
    } c_Window;

    struct
    {
        bool KeyPressed;
        char Key;
        char m_LastKey;
    } Keyboard;
    struct
    {
        bool m_isPressed;
        short m_ClickType;
    } Mouse;

    struct
    {
        float m_Timer;
        vec2 m_Pos;
    } s_AnimCredits;

    class CRow
    {
        public:
            typedef enum
            {
                LEFT,
                RIGHT,
            } ROWDIR;

            CRow(ROWDIR dir) { m_PosDir = dir; offsetx=0; m_Dir=(dir=LEFT)?vec2(-1,0):vec2(1, 0); }

            ROWDIR m_PosDir;
            float m_Timer;
            int offsetx;
            vec2 m_Dir;
    };




    void ShowMenu(short);

    void SetWinHWND(HWND hwnd) { m_HWND = hwnd; }
    HWND GetWinHWND() const { return m_HWND; }

    int GetScreenW()  const { return m_ScreenWidth; }
    int GetScreenH()  const { return m_ScreenHeight; }

    void StartGame();
    void EndGame();

    CGameController *m_pController;
    bool m_Exit;
    bool m_WorldPaused;
    float m_TimePaused;
    short m_GameState;

private:
    HWND m_HWND;
    float m_TimerGame;
    int m_ScreenWidth;
    int m_ScreenHeight;
    short m_MenuIndex;
    int m_MenuSelGame;
    CRow *m_pRows[2];
};
extern class CGameContext g_Game;

#endif
