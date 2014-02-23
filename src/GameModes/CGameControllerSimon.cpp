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
#include "CGameControllerSimon.hpp"
#include <ICGameContext.hpp>
#include <winbgim.h>
#include <stdio.h>

CGameControllerSimon::CGameControllerSimon()
:
    dimTW(200),
    dimTH(200)
{

    m_LastMouseX = mousex();
    m_LastMouseY = mousey();
    m_TimerCPU = GetTickCount();
    m_SeqCPUPos = 0;
    m_SeqP1Pos = 0;
    m_ActivePlayer = PLAYER_CPU;
    m_Started = false;

    for (int i=0;i<4;i++)
    {
        v_Cursor[i].m_Radius = 10;
        v_Cursor[i].m_Dir = -1;
        v_Cursor[i].m_Start = GetTickCount() + 1000 * 0.5f * i;
        v_Cursor[i].m_Pos = vec2(mousex(), mousey());
    }

    for (int i=0;i<NUM_PADS;i++)
    {
        v_Pads[i].m_Timer = GetTickCount();
        v_Pads[i].m_Used = false;
    }

    //Primer Color de la Secuencia
    v_Seq.push_back(rand()%NUM_PADS);
}
CGameControllerSimon::~CGameControllerSimon()
{
    std::deque<ANIM*>::iterator it = v_Anims.begin();
    while(it != v_Anims.end())
    {
            delete (*it);
            (*it) = 0x0;
            it = v_Anims.erase(it);
    }
}

void CGameControllerSimon::PushPad(short pad)
{
    v_Pads[pad].m_Timer = GetTickCount() + 1000 * 0.3f;
    v_Pads[pad].m_Used = true;

    if (m_ActivePlayer == PLAYER_1)
    {
        if (v_Seq[m_SeqP1Pos] != pad)
            EndRound();

        m_SeqP1Pos++;
        if (m_SeqP1Pos >= v_Seq.size()-1)
        {
            m_SeqP1Pos=0;
            m_ActivePlayer = PLAYER_CPU;
        }
    }
    else
    {
        ANIM *m_pAnim = new ANIM();
        m_pAnim->m_Dir=-1;
        m_pAnim->m_Pos = vec2(g_Game.GetScreenW()/2 - dimTW/2*2+(dimTW/3)*3, g_Game.GetScreenH()/2);
        m_pAnim->m_Radius = dimTH/2 + (dimTH/3)/2;
        m_pAnim->m_Timer = GetTickCount();
        m_pAnim->m_Color = GetPadColor(pad);
        v_Anims.push_back(m_pAnim);
    }
}

short CGameControllerSimon::GenerateColor()
{
    return rand()%WHITE;
}

short CGameControllerSimon::GetPadColor(short pad, bool lightcolor)
{
    if (pad == PAD_BLUE)
    {
        if (lightcolor)
            return LIGHTBLUE;

        return BLUE;
    }
    else if (pad == PAD_RED)
    {
        if (lightcolor)
            return LIGHTRED;

        return RED;
    }
    else if (pad == PAD_YELLOW)
    {
        if (lightcolor)
            return LIGHTMAGENTA;

        return MAGENTA;
    }
    else if (pad == PAD_GREEN)
    {
        if (lightcolor)
            return LIGHTGREEN;

        return GREEN;
    }
    else if (pad == PAD_CYAN)
    {
        if (lightcolor)
            return LIGHTCYAN;

        return CYAN;
    }

    return BLACK;
}

void CGameControllerSimon::EndRound()
{
    ICGameController::EndRound();
}

void CGameControllerSimon::DrawHUD()
{
    /** Teclado **/
    if (g_Game.Keyboard.KeyPressed)
    {
        switch(g_Game.Keyboard.Key)
        {
            case 13:
                if (g_Game.m_WorldPaused)
                {
                    g_Game.m_GameState = ICGameContext::GAME_MENU;
                    return;
                }

                break;
        }
    }
    /** Raton **/
    vec2 m_MousePos = vec2(mousex(), mousey());
    //Captar Acciones
    if (!g_Game.m_WorldPaused)
    {
        if (g_Game.Mouse.m_isPressed)
        {
            if (!m_Started)
                m_Started = true;

            if (m_ActivePlayer == PLAYER_1 && m_Started)
            {

                for (int i=0; i<3;i++)
                {
                    if (m_MousePos.x >= g_Game.GetScreenW()/2-dimTW/2+dimTH/3*i && m_MousePos.x <= g_Game.GetScreenW()/2-dimTW/2+dimTH/3*i+(dimTW/3)
                        && m_MousePos.y >= g_Game.GetScreenH()/2-dimTH/2+dimTH/3 && m_MousePos.y <= g_Game.GetScreenH()/2-dimTH/2+dimTH/3+(dimTH/3))
                        {
                            if (i == 0)
                                PushPad(PAD_RED);
                            else if (i==1)
                                PushPad(PAD_YELLOW);
                            else
                                PushPad(PAD_GREEN);

                            m_TimerCPU = GetTickCount() + 1000 * 0.7f;
                            break;
                        }
                }

                for (int i=0;i<3;i+=2)
                {

                    if (m_MousePos.x >= g_Game.GetScreenW()/2-dimTW/2+dimTH/3 && m_MousePos.x <= g_Game.GetScreenW()/2-dimTW/2+dimTH/3+(dimTW/3)
                        && m_MousePos.y >= g_Game.GetScreenH()/2-dimTH/2+dimTH/3*i && m_MousePos.y <= g_Game.GetScreenH()/2-dimTH/2+dimTH/3*i+(dimTH/3))
                        {
                            if (i==0)
                                PushPad(PAD_BLUE);
                            else
                                PushPad(PAD_CYAN);

                            m_TimerCPU = GetTickCount() + 1000 * 0.7f;
                            break;
                        }
                }
            }
            clearmouseclick(WM_LBUTTONDOWN);
        }
    }


    //* PERDIO? *//
    if (g_Game.m_WorldPaused)
    {
        setbkcolor(WHITE);
        setcolor(BLACK);
        int xt = g_Game.GetScreenW()/2 - textwidth("Eres un Noob!!!")/2;
        int yt = g_Game.GetScreenH()/2 - textheight("Eres un Noob!!!")/2;

        outtextxy(xt, yt, "Eres un Noob!!!");

        xt = g_Game.GetScreenW()/2 - textwidth("pulsa INTRO para reiniciar")/2;
        yt = g_Game.GetScreenH()/2 - textheight("pulsa INTRO para reiniciar")/2;
        yt += textheight("Eres un Noob!") + 15;
        outtextxy(xt, yt, "pulsa INTRO para reiniciar");
        rectangle(-1, yt-2, g_Game.GetScreenW()+1, yt-1 + textheight("pulsa INTRO para reiniciar") + 2);
    }
    else
    {
        if (!m_Started)
        {
            int xt = g_Game.GetScreenW()/2 - textwidth("Haz CLICK para empezar")/2;
            int yt = g_Game.GetScreenH()/2 - textheight("Haz CLICK para empezar")/2;
            yt += 15;
            outtextxy(xt, yt, "Haz CLICK para empezar");
            rectangle(-1, yt-2, g_Game.GetScreenW()+1, yt-1 + textheight("Haz CLICK para empezar") + 2);
        }
    }

    //Marcadores
    vec2 m_SBPos;
    vec2 m_SBEPos;
    const char *m_pTexto;
    char m_ExText[25];

    if (m_ActivePlayer == PLAYER_CPU)
    {
        m_pTexto = "CPU";
        sprintf(m_ExText, "%i/%i", m_SeqCPUPos, v_Seq.size()-1);
    }
    else
    {
        m_pTexto = "JUGADOR 1";
        sprintf(m_ExText, "%i/%i", m_SeqP1Pos, v_Seq.size()-1);
    }

    m_SBPos = vec2(g_Game.GetScreenW() - textwidth(m_pTexto) - 10, 10);
    m_SBEPos = vec2(g_Game.GetScreenW() - textwidth(m_ExText) - 10, 15+textheight(m_pTexto));
    outtextxy(static_cast<int>(m_SBPos.x), static_cast<int>(m_SBPos.y), m_pTexto);
    outtextxy(static_cast<int>(m_SBEPos.x), static_cast<int>(m_SBEPos.y), m_ExText);

    //Effecto cola...
    setcolor(DARKGRAY);
    setfillstyle(SOLID_FILL, DARKGRAY);
    for (int i=1; i<4; i++)
    {
        ANIM *a_Esfera = &v_Cursor[i];

        if (GetTickCount() < a_Esfera->m_Start)
            continue;

        if (a_Esfera->m_Radius > 10)
            a_Esfera->m_Dir = -1;
        else if (a_Esfera->m_Radius <= 0)
        {
            a_Esfera->m_Pos = m_MousePos;
            a_Esfera->m_Radius = 10;
        }
        a_Esfera->m_Radius += a_Esfera->m_Dir;

        if (m_LastMouseX != mousex() || m_LastMouseY != mousey())
            fillellipse(static_cast<int>(a_Esfera->m_Pos.x), static_cast<int>(a_Esfera->m_Pos.y), a_Esfera->m_Radius, a_Esfera->m_Radius);
    }
    //Cursor
    setcolor(LIGHTGRAY);
    setfillstyle(SOLID_FILL, LIGHTGRAY);
    v_Cursor[0].m_Pos = m_MousePos;
    fillellipse(static_cast<int>(v_Cursor[0].m_Pos.x), static_cast<int>(v_Cursor[0].m_Pos.y), v_Cursor[0].m_Radius, v_Cursor[0].m_Radius);

    m_LastMouseX = mousex();
    m_LastMouseY = mousey();
}

void CGameControllerSimon::Tick()
{
    /** Control Iluminacion Pads**/
    for (int i=0; i<NUM_PADS;i++)
    {
        if (v_Pads[i].m_Used && GetTickCount() >= v_Pads[i].m_Timer)
            v_Pads[i].m_Used = false;
    }
    /** Control de Animaciones **/
    for(size_t i=0; i<v_Anims.size(); i++)
    {
        if (GetTickCount() >= v_Anims[i]->m_Timer)
        {
            if (v_Anims[i]->m_Radius <= 0)
                continue;

            v_Anims[i]->m_Radius += v_Anims[i]->m_Dir;
            v_Anims[i]->m_Timer = GetTickCount() + 1000 * 0.0f;

            if (v_Anims[i]->m_Radius <= 0)
            {
            }
        }
    }


    /** Dibujar Animaciones **/
    for(size_t i=0; i<v_Anims.size(); i++)
    {
        setcolor(v_Anims[i]->m_Color);
        circle(static_cast<int>(v_Anims[i]->m_Pos.x), static_cast<int>(v_Anims[i]->m_Pos.y), v_Anims[i]->m_Radius);
    }

    /** Dibujar Tablero **/
    setcolor(WHITE);
    int offsetx = dimTW/3;
    int offsety = dimTH/3;

    for (int i=offsety; i<dimTH-dimTH/3; i+=offsety)
    {
        moveto(g_Game.GetScreenW()/2-dimTW/2, i+g_Game.GetScreenH()/2-dimTH/2);
        lineto(g_Game.GetScreenW()/2-dimTW/2+dimTW, i+g_Game.GetScreenH()/2-dimTH/2);
    }
    for (int i=offsetx; i<dimTW-dimTW/3; i+=offsetx)
    {
        moveto(g_Game.GetScreenW()/2-dimTW/2+i, 1+g_Game.GetScreenH()/2-dimTH/2);
        lineto(g_Game.GetScreenW()/2-dimTW/2+i, dimTH+g_Game.GetScreenH()/2-dimTH/2);
    }

    circle(g_Game.GetScreenW()/2 - dimTW/2*2+(dimTW/3)*3, g_Game.GetScreenH()/2, dimTH/2 + (dimTH/3)/2);
    arc(g_Game.GetScreenW()/2-dimTW/2, dimTH/3+(dimTH/3)/2+g_Game.GetScreenH()/2-dimTH/2, 90, 270, (dimTH/3)/2);
    arc(g_Game.GetScreenW()/2-dimTW/2+(dimTW/3)*3, dimTH/3+(dimTH/3)/2+g_Game.GetScreenH()/2-dimTH/2, 270, 90, (dimTH/3)/2);
    arc(g_Game.GetScreenW()/2-dimTW/2+(dimTW/3)/2+dimTW/3, g_Game.GetScreenH()/2-dimTH/2+1, 0, 180, (dimTH/3)/2);
    arc(g_Game.GetScreenW()/2-dimTW/2+(dimTW/3)/2+dimTW/3, (dimTH/3)*3+g_Game.GetScreenH()/2-dimTH/2+1, 180, 0, (dimTH/3)/2);

    //Cuadros Luminosos
    int points[4*2];
    //--Derecha\Centro\Izquierda
    for (int i=0;i<3;i++)
    {
        if (i==0)
            setfillstyle(SOLID_FILL, GetPadColor(PAD_RED, v_Pads[PAD_RED].m_Used));
        else if (i == 1)
            setfillstyle(SOLID_FILL, GetPadColor(PAD_YELLOW, v_Pads[PAD_YELLOW].m_Used));
        else
            setfillstyle(SOLID_FILL, GetPadColor(PAD_GREEN, v_Pads[PAD_GREEN].m_Used));

        points[0]=g_Game.GetScreenW()/2-dimTW/2+dimTH/3*i;
        points[1]=g_Game.GetScreenH()/2-dimTH/2+dimTH/3;
        points[2]=points[0]+(dimTW/3);
        points[3]=points[1];
        points[4]=points[2];
        points[5]=points[1]+(dimTH/3);
        points[6]=points[0];
        points[7]=points[5];
        fillpoly(4, points);
    }
    //--Arriba\Abajo
    for (int i=0;i<3;i+=2)
    {
        if (i==0)
            setfillstyle(SOLID_FILL, GetPadColor(PAD_BLUE, v_Pads[PAD_BLUE].m_Used));
        else
            setfillstyle(SOLID_FILL, GetPadColor(PAD_CYAN, v_Pads[PAD_CYAN].m_Used));

        points[0]=g_Game.GetScreenW()/2-dimTW/2+dimTH/3;
        points[1]=g_Game.GetScreenH()/2-dimTH/2+dimTH/3*i;
        points[2]=points[0]+(dimTW/3);
        points[3]=points[1];
        points[4]=points[2];
        points[5]=points[1]+(dimTH/3);
        points[6]=points[0];
        points[7]=points[5];
        fillpoly(4, points);
    }


    //La makina Juega
    //** Generar Pad Iluminati **/
    if (!g_Game.m_WorldPaused && m_Started && m_ActivePlayer == PLAYER_CPU && GetTickCount() > m_TimerCPU)
    {
        if (m_SeqCPUPos == 0)
            v_Seq.push_back(rand()%NUM_PADS);

        PushPad(v_Seq[m_SeqCPUPos]);
        m_SeqCPUPos++;

        if (m_SeqCPUPos >= v_Seq.size()-1)
        {
            m_SeqCPUPos = 0;
            m_ActivePlayer = PLAYER_1;
        }
        m_TimerCPU = GetTickCount() + 1000 * 0.7f;
    }
}
