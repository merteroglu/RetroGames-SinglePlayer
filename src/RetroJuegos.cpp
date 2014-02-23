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
#define _WIN32_WINNT 0x0500
#include <ICGameContext.hpp>
#include <winbgim.h>

int WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
    //FreeConsole();
    initwindow(SCREEN_W, SCREEN_H);
    g_Game.SetWinHWND(GetActiveWindow());
    ShowCursor(false);

    SetWindowLong(g_Game.GetWinHWND(), GWL_EXSTYLE, WS_EX_TOOLWINDOW);
    SetWindowPos(g_Game.GetWinHWND(), HWND_TOP, SCREEN_W, SCREEN_H, 0, 0, SWP_NOSIZE|SWP_FRAMECHANGED);

    g_Game.StartGame();

    SetWindowLong(g_Game.GetWinHWND(), GWL_EXSTYLE, GetWindowLong(g_Game.GetWinHWND(), GWL_EXSTYLE) | WS_EX_LAYERED);
    SetLayeredWindowAttributes(g_Game.GetWinHWND(), 0, 255, LWA_ALPHA);
    delay(100);
    for (int i=255;i>=0; i--)
    {
        SetLayeredWindowAttributes(g_Game.GetWinHWND(), 0, i, LWA_ALPHA);
        Sleep(5);
    }

    closegraph();
    return 0;
}
