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
#include "CGameControllerCrapTower.hpp"
#include <ICGameContext.hpp>
#include <iostream>
#include <fstream>
#include <string.h>
#include <winbgim.h>
#include <dirent.h>
using namespace std;


CMap::CMap()
{
}
CMap::~CMap()
{
    std::deque<CRonda*>::iterator it_ = v_Rondas.begin();
    while(it_ != v_Rondas.end())
    {
        delete (*it_);
        (*it_) = 0x0;
        it_ = v_Rondas.erase(it_);
    }
}

bool CMap::LoadMap(const char *filename)
{
    if (strcmp(filename, "") == 0)
        return false;

    char buff[30];
    int col=0,row=0;
    ifstream m_AMapa(filename, ios::in);
    if (!m_AMapa)
        return false;

    while (m_AMapa.getline(buff, 30))
    {
        if (row < 30)
        {
            for (col=0;col<30;col++)
            {
                TILE *m_pTile = new TILE();
                if (buff[col] == '@')
                {
                    m_pTile->m_TypeTile = TILE_START;
                    m_IndexStart = vec2(TILE_SIZE*col, TILE_SIZE*row);
                }
                else if (buff[col] == '$')
                    m_pTile->m_TypeTile = TILE_END;
                else if (buff[col] == 'A' || buff[col] == 'D' || buff[col] == 'W' || buff[col] == 'S')
                {
                    m_pTile->m_TypeTile = TILE_ROAD;

                    if (buff[col] == 'A')
                        m_pTile->m_Dir = LEFT;
                    else if (buff[col] == 'D')
                        m_pTile->m_Dir = RIGHT;
                    else if (buff[col] == 'W')
                        m_pTile->m_Dir = UP;
                    else if (buff[col] == 'S')
                        m_pTile->m_Dir = DOWN;
                }
                else if (buff[col] == 'N')
                    m_pTile->m_TypeTile = TILE_NOTOWER;
                else
                    m_pTile->m_TypeTile = TILE_NORMAL;

                m_pTile->m_Pos = vec2(TILE_SIZE*col, TILE_SIZE*row);
                m_pTile->m_Data = 0x0;
                v_MapTiles[row][col] = *m_pTile;

                delete m_pTile;
                m_pTile = 0x0;
            }
        }
        else
        {
            CRonda *m_pRonda = new CRonda();
            if (row-30 == 0)
                m_pRonda->m_TimeStart = GetTickCount() + 1000*5;
            else
                m_pRonda->m_TimeStart = GetTickCount() + 1000*(row-30)*30;

            for (col=0;col<30;col++)
            {
                if (buff[col] == 'A')
                    m_pRonda->v_Enemies.push_back(new CEnemy(CEnemy::ENEMY_1, GetStartPos(), 0.05f*(col+1)));
                if (buff[col] == 'B')
                    m_pRonda->v_Enemies.push_back(new CEnemy(CEnemy::ENEMY_2, GetStartPos(), 0.01f*(col+1)));
            }

            v_Rondas.push_back(m_pRonda);
        }

        row++;
    }
    m_AMapa.close();

    if (col != 30 && row != 30)
        return false;

    //Analizar Direccion Inicial
    bool found = false;
    for (int row=0; row<30; row++)
    {
        for (int col=0; col<30; col++)
        {
            CMap::TILE *m_pTile = &v_MapTiles[row][col];

            if (m_pTile->m_TypeTile == TILE_START)
            {
                if (row > 0 && v_MapTiles[row-1][col].m_TypeTile == TILE_ROAD)
                    m_pTile->m_Dir = UP;
                else if (row < 30 && v_MapTiles[row+1][col].m_TypeTile == TILE_ROAD)
                    m_pTile->m_Dir = DOWN;
                else if (col < 30 && v_MapTiles[row][col+1].m_TypeTile == TILE_ROAD)
                    m_pTile->m_Dir = RIGHT;
                else
                    m_pTile->m_Dir = LEFT;

                found = true;
                break;
            }
        }

        if (found)
            break;
    }

    if (!found)
        return false;

    return true;
}

void CMap::Draw()
{
    /** Pintar Mapa **/
    int points[4*2];
    //Fondo
    setcolor(getbkcolor());
    setfillstyle(INTERLEAVE_FILL, GREEN);
    points[0]=0;
    points[1]=0;
    points[2]=points[0]+g_Game.GetScreenW();
    points[3]=points[1];
    points[4]=points[2];
    points[5]=points[1]+g_Game.GetScreenH();
    points[6]=points[0];
    points[7]=points[5];
    fillpoly(4, points);
    //Camino
    for (int row=0; row<30; row++)
    {
        for (int col=0; col<30; col++)
        {
            CMap::TILE *m_pTile = &v_MapTiles[row][col];

            if (m_pTile->m_TypeTile == TILE_NORMAL)
                continue;

            setcolor(WHITE);
            if (m_pTile->m_TypeTile == TILE_ROAD)
            {
                setfillstyle(SOLID_FILL, WHITE);
                points[0]=OFFSET+static_cast<int>(m_pTile->m_Pos.x);
                points[1]=OFFSET+static_cast<int>(m_pTile->m_Pos.y);
                points[2]=points[0]+TILE_SIZE;
                points[3]=points[1];
                points[4]=points[2];
                points[5]=points[1]+TILE_SIZE;
                points[6]=points[0];
                points[7]=points[5];
                fillpoly(4, points);
            }
            else if (m_pTile->m_TypeTile == TILE_START || m_pTile->m_TypeTile == TILE_END)
            {
                setfillstyle(SOLID_FILL, (m_pTile->m_TypeTile == TILE_START)?RED:BLUE);
                points[0]=OFFSET+static_cast<int>(m_pTile->m_Pos.x-2);
                points[1]=OFFSET+static_cast<int>(m_pTile->m_Pos.y-2);
                points[2]=points[0]+TILE_SIZE+4;
                points[3]=points[1];
                points[4]=points[2];
                points[5]=points[1]+TILE_SIZE+4;
                points[6]=points[0];
                points[7]=points[5];
                fillpoly(4, points);
            }
            else if (m_pTile->m_TypeTile == TILE_NOTOWER)
            {
                setcolor(getbkcolor());
                setfillstyle(CLOSE_DOT_FILL, GREEN);
                points[0]=OFFSET+static_cast<int>(m_pTile->m_Pos.x+1);
                points[1]=OFFSET+static_cast<int>(m_pTile->m_Pos.y+1);
                points[2]=points[0]+TILE_SIZE-2;
                points[3]=points[1];
                points[4]=points[2];
                points[5]=points[1]+TILE_SIZE-2;
                points[6]=points[0];
                points[7]=points[5];
                fillpoly(4, points);
            }
        }
    }
}


bool CMap::CanBuildHere(vec2 pos)
{
    TILE *m_pTile = GetTile(pos);

    if (m_pTile->m_Data != NULL
        ||m_pTile->m_TypeTile == CMap::TILE_NOTOWER
        || m_pTile->m_TypeTile == CMap::TILE_ROAD
        || m_pTile->m_TypeTile == CMap::TILE_START
        || m_pTile->m_TypeTile == CMap::TILE_END)
    {
        return false;
    }

    return true;
}


CMap::TILE* CMap::GetTile(vec2 pos)
{
    int index=0;
    vec2 m_PosTile;

    pos.x = clamp(static_cast<int>(pos.x/TILE_SIZE), 0, static_cast<int>((g_Game.GetScreenW()/TILE_SIZE)-1));
    pos.y = clamp(static_cast<int>(pos.y/TILE_SIZE), 0, static_cast<int>((g_Game.GetScreenH()/TILE_SIZE)-1));
    index = static_cast<int>(pos.y * (g_Game.GetScreenW()/TILE_SIZE) + pos.x);
    pos.x = index % (g_Game.GetScreenW()/TILE_SIZE) * TILE_SIZE;
    pos.y = index / (g_Game.GetScreenW()/TILE_SIZE) * TILE_SIZE;

    m_PosTile = vec2(index % (g_Game.GetScreenW()/TILE_SIZE), index / (g_Game.GetScreenW()/TILE_SIZE));
    return &v_MapTiles[(int)m_PosTile.y][(int)m_PosTile.x];
}

//---------------------------------------------

CEnemy::CEnemy(ENEMYTYPE type, vec2 pos, float vel)
{
    m_Type = type;
    m_Pos = pos;
    m_Vel = vel;

    if (type == ENEMY_1)
        m_Health = 30.0f;
    else if (type == ENEMY_2)
        m_Health = 80.0f;

    m_Timer = GetTickCount();
}
CEnemy::~CEnemy()
{
}

void CEnemy::TakeDamage(float dmg)
{
    m_Health -= dmg;
    m_Health = (m_Health < 0)?0:m_Health;
}

void CEnemy::Draw()
{
    if (m_Type == ENEMY_1)
    {
        m_Points[0]=OFFSET+static_cast<int>(m_Pos.x+2);
        m_Points[1]=OFFSET+static_cast<int>(m_Pos.y+2);
        m_Points[2]=m_Points[0]+TILE_SIZE-4;
        m_Points[3]=m_Points[1];
        m_Points[4]=m_Points[2];
        m_Points[5]=m_Points[1]+TILE_SIZE-4;
        m_Points[6]=m_Points[0];
        m_Points[7]=m_Points[5];
    }
    else if (m_Type == ENEMY_2)
    {
        m_Points[0]=OFFSET+static_cast<int>(m_Pos.x+2)+TILE_SIZE/2;
        m_Points[1]=OFFSET+static_cast<int>(m_Pos.y+2);
        m_Points[2]=m_Points[0]+TILE_SIZE-4;
        m_Points[3]=m_Points[1]+TILE_SIZE/2;
        m_Points[4]=m_Points[2]+TILE_SIZE/2;
        m_Points[5]=m_Points[1]+TILE_SIZE-4;
        m_Points[6]=static_cast<int>(m_Pos.x+2);
        m_Points[7]=m_Points[3];
    }

    fillpoly(4, m_Points);
}

//---------------------------------------------

CEntity::CEntity(ENTTYPE type)
{
    m_Type = type;
    m_Timer = GetTickCount();
}
CEntity::~CEntity()
{
}

//---------------------------------------------

CRonda::CRonda()
{
    m_TimeStart = 0.0f;
}
CRonda::~CRonda()
{
}

//---------------------------------------------

CTower::CTower(TOWERLEVEL level, vec2 pos)
:
    CEntity(ENTITY_TOWER)
{
    m_Level = level;
    m_Pos = pos;

    if (level == TOWER_BASIC)
        m_Radius = 30;
    else if (level == TOWER_MEDIUM)
        m_Radius = 40;
    else if (level == TOWER_HIGH)
        m_Radius = 55;
}
CTower::~CTower()
{
}

//---------------------------------------------

CBullet::CBullet(vec2 pos, vec2 dir, CTower *tower, CEnemy *target, short color)
:
    CEntity(ENTITY_BULLET)
{
    m_Pos = pos;
    m_Dir = dir;
    m_TowerOwn = tower;
    m_Target = target;
    m_Color = color;

    if (m_TowerOwn->m_Level == CTower::TOWER_BASIC)
        m_TimerVel = 0.1f;
    else if (m_TowerOwn->m_Level == CTower::TOWER_MEDIUM)
        m_TimerVel = 0.05f;
    else if (m_TowerOwn->m_Level == CTower::TOWER_HIGH)
        m_TimerVel = 0.0f;
}

CBullet::~CBullet()
{
}

//---------------------------------------------

CGameControllerCrapTower::CGameControllerCrapTower()
{
    m_pMap = new CMap();
    m_TimerCrapTower = GetTickCount();
    m_LastTimerRound = m_TimerCrapTower;
    m_Level = 1;
    m_PlayerWin = false;

    //Cargana Nivel 1
    char buffMap[255];
    sprintf(buffMap,"niveles\\CT_nivel%i.map",m_Level);
    m_pMap->LoadMap(buffMap);
}
CGameControllerCrapTower::~CGameControllerCrapTower()
{
    Reset();
}


void CGameControllerCrapTower::DrawHUD()
{
    if (!m_pMap)
        return;

    int points[4*2];

    /** INDICADOR DE RATON **/
    vec2 m_MousePos = vec2(mousex(), mousey());
    vec2 m_MouseIPos, m_PosTile;
    int index;

    m_MouseIPos.x = clamp((int)m_MousePos.x/TILE_SIZE, 0, (g_Game.GetScreenW()/TILE_SIZE)-1);
    m_MouseIPos.y = clamp((int)m_MousePos.y/TILE_SIZE, 0, (g_Game.GetScreenH()/TILE_SIZE)-1);
    index = static_cast<int>(m_MouseIPos.y * (g_Game.GetScreenW()/TILE_SIZE) + m_MouseIPos.x);
    m_MouseIPos.x = index % (g_Game.GetScreenW()/TILE_SIZE) * TILE_SIZE;
    m_MouseIPos.y = index / (g_Game.GetScreenW()/TILE_SIZE) * TILE_SIZE;

    m_PosTile = vec2(index % (g_Game.GetScreenW()/TILE_SIZE), index / (g_Game.GetScreenW()/TILE_SIZE));
    if (!m_pMap->CanBuildHere(m_MousePos))
    {
        setcolor(RED);
        setfillstyle(SOLID_FILL, RED);
    }
    else
    {
        setcolor(WHITE);
        setfillstyle(SOLID_FILL, LIGHTGREEN);
    }
    rectangle(static_cast<int>(OFFSET+m_MouseIPos.x), static_cast<int>(OFFSET+m_MouseIPos.y), static_cast<int>(OFFSET+m_MouseIPos.x+TILE_SIZE), static_cast<int>(OFFSET+m_MouseIPos.y+TILE_SIZE));

    /* Rondas Indicador */
    int offset = MAP_SIZE/2 - 150/2;
    setfillstyle(SOLID_FILL, BLACK);
    setcolor(WHITE);
    points[0]=offset;
    points[1]=10;
    points[2]=points[0]+150;
    points[3]=points[1];
    points[4]=points[2];
    points[5]=points[1]+15;
    points[6]=points[0];
    points[7]=points[5];
    fillpoly(4, points);
    CRonda *p_Ronda = static_cast<CRonda*>(m_pMap->v_Rondas[0]);
    if (p_Ronda)
    {
        float sizeBar = 150.0f - ((GetTickCount()-m_LastTimerRound)*150.f)/(p_Ronda->m_TimeStart-m_LastTimerRound);
        sizeBar = (sizeBar < 0.0f)?0.0f:sizeBar;
        setfillstyle(SOLID_FILL, RED);
        setcolor(RED);
        points[0]=offset+2;
        points[1]=10+2;
        points[2]=points[0]+static_cast<int>(sizeBar)-4;
        points[3]=points[1];
        points[4]=points[2];
        points[5]=points[1]+15-4;
        points[6]=points[0];
        points[7]=points[5];
        fillpoly(4, points);
    }

    //Ver Informacion de las Entidades
    for (size_t i=0; i<v_Entities.size(); i++)
    {
        if (v_Entities[i]->GetType() == CEntity::ENTITY_TOWER)
        {
            CTower *m_pTower = static_cast<CTower*>(v_Entities[i]);
            if (!m_pTower)
                continue;

            if (m_MouseIPos == m_pTower->m_Pos)
            {
                setcolor(YELLOW);
                circle(OFFSET+static_cast<int>(m_pTower->m_Pos.x+TILE_SIZE/2), OFFSET+static_cast<int>(m_pTower->m_Pos.y+TILE_SIZE/2), m_pTower->m_Radius);
            }
        }
    }

    //Jugador Gana Mapa
    if (m_PlayerWin)
    {
        setbkcolor(WHITE);
        setcolor(BLACK);
        int xt = g_Game.GetScreenW()/2 - textwidth("MAPA COMPLETADO!")/2;
        int yt = g_Game.GetScreenH()/2 - textheight("MAPA COMPLETADO!")/2;

        outtextxy(xt, yt, "MAPA COMPLETADO!");

        xt = g_Game.GetScreenW()/2 - textwidth("pulsa INTRO para siguiente mapa")/2;
        yt = g_Game.GetScreenH()/2 - textheight("pulsa INTRO para siguiente mapa")/2;
        yt += textheight("MAPA COMPLETADO!") + 15;
        outtextxy(xt, yt, "pulsa INTRO para siguiente mapa");
        rectangle(-1, yt-2, g_Game.GetScreenW()+1, yt-1 + textheight("pulsa INTRO para siguiente mapa") + 2);
    }
}

void CGameControllerCrapTower::Tick()
{
    /** Captar Teclado **/
    if (m_PlayerWin && g_Game.Keyboard.KeyPressed && g_Game.Keyboard.Key == 13 )
    {
        Reset();

        //Cargar Siguiente Mapa
        char buffMap[255];
        sprintf(buffMap,"niveles\\CT_nivel%i.map",++m_Level);
        m_pMap = new CMap();
        m_pMap->LoadMap(buffMap);
    }

    if (!g_Game.m_WorldPaused)
    {
        /** Captar Raton **/
        if (g_Game.Mouse.m_isPressed)
        {
            vec2 m_MousePos = vec2(mousex(), mousey());
            CMap::TILE *m_pTile = m_pMap->GetTile(m_MousePos);

            if (m_pMap->CanBuildHere(m_MousePos))
            {
                CTower *m_pTower = new CTower(CTower::TOWER_MEDIUM, m_pTile->m_Pos);
                m_pTile->m_Data = static_cast<void*>(m_pTower);
                v_Entities.push_back(m_pTower);
            }

            clearmouseclick(WM_LBUTTONDOWN);
        }

        /** Rondas **/
        CRonda *p_Ronda = static_cast<CRonda*>(m_pMap->v_Rondas[0]);
        if (p_Ronda)
        {
            float timeElap = p_Ronda->m_TimeStart - GetTickCount();
            if (timeElap <= 0)
            {
                for (size_t o=0; o<p_Ronda->v_Enemies.size(); o++)
                {
                    CEnemy *p_Enemy = static_cast<CEnemy*>(p_Ronda->v_Enemies[o]);
                    if (!p_Enemy)
                        continue;

                    v_Enemies.push_back(p_Enemy);
                }
                m_LastTimerRound = p_Ronda->m_TimeStart;
                RemoveRound(p_Ronda);
            }
        }

        /** MAPA COMPLETADO **/
        if (!m_PlayerWin && !p_Ronda && !v_Enemies[0])
            m_PlayerWin = true;

        /** Mover Enemigos **/
        for (size_t i=0; i<v_Enemies.size(); i++)
        {
            CEnemy *m_pEnemy = v_Enemies[i];
            if (!m_pEnemy || GetTickCount() < m_pEnemy->m_Timer)
                continue;

            CMap::TILE *m_pTile = m_pMap->GetTile(m_pEnemy->m_Pos);

            if (m_pEnemy->m_Dir == vec2(-1, 0))
                 m_pTile = m_pMap->GetTile(m_pEnemy->m_Pos+vec2(TILE_SIZE-1, 0));
            else if (m_pEnemy->m_Dir == vec2(0, -1))
                 m_pTile = m_pMap->GetTile(m_pEnemy->m_Pos+vec2(TILE_SIZE-1, TILE_SIZE-1));

            if (m_pTile->m_TypeTile == CMap::TILE_ROAD || m_pTile->m_TypeTile == CMap::TILE_START)
            {
                if (m_pTile->m_Dir == CMap::LEFT)
                    m_pEnemy->m_Dir = vec2(-1, 0);
                else if (m_pTile->m_Dir == CMap::RIGHT)
                    m_pEnemy->m_Dir = vec2(1, 0);
                else if (m_pTile->m_Dir == CMap::UP)
                    m_pEnemy->m_Dir = vec2(0, -1);
                else if (m_pTile->m_Dir == CMap::DOWN)
                    m_pEnemy->m_Dir = vec2(0, 1);
            }
            else
                m_pEnemy->m_Dir = vec2(0, 0);

            m_pEnemy->m_Pos += m_pEnemy->m_Dir;
            m_pEnemy->m_Timer = GetTickCount()+1000* m_pEnemy->m_Vel;
        }
    }
    /** INTERACCION ENTIDADES **/
    for (size_t i=0; i<v_Entities.size(); i++)
    {
        CEntity *m_pEntity = v_Entities[i];
        if (!m_pEntity)
            continue;

        if (m_pEntity->GetType() == CEntity::ENTITY_TOWER)
        {
            CTower *m_pTower = static_cast<CTower*>(m_pEntity);
            if (!m_pTower)
                continue;

            for (size_t o=0; o<v_Enemies.size(); o++)
            {
                CEnemy *m_pEnemy = v_Enemies[o];
                if (!m_pEnemy)
                    continue;

                vec2 PosT = m_pEnemy->m_Pos-m_pTower->m_Pos;
                if (sqrtf(PosT.x*PosT.x + PosT.y*PosT.y) < m_pTower->m_Radius && GetTickCount() >= m_pTower->m_Timer)
                {
                    vec2 dir = m_pEnemy->m_Pos - m_pTower->m_Pos;
                    dir = normalize(dir);

                    v_Entities.push_back(new CBullet(vec2(OFFSET+m_pTower->m_Pos.x+TILE_SIZE/2,OFFSET+m_pTower->m_Pos.y+TILE_SIZE/2) , dir, m_pTower, m_pEnemy, rand()%8));

                    float val=0.0f;
                    if (m_pTower->m_Level == CTower::TOWER_BASIC)
                        val = 0.35f;
                    else if (m_pTower->m_Level == CTower::TOWER_MEDIUM)
                        val = 0.25f;
                    else if (m_pTower->m_Level == CTower::TOWER_HIGH)
                        val = 0.15f;

                    m_pTower->m_Timer = GetTickCount() + 1000 * val;
                }
            }
        }
        if (m_pEntity->GetType() == CEntity::ENTITY_BULLET)
        {
            CBullet *m_pBullet = static_cast<CBullet*>(m_pEntity);

            if (m_pBullet && GetTickCount() >= m_pBullet->m_Timer)
            {
                m_pBullet->m_Dir = vec2(m_pBullet->m_Target->m_Pos.x+TILE_SIZE/2, m_pBullet->m_Target->m_Pos.y+TILE_SIZE/2) - m_pBullet->m_Pos;
                m_pBullet->m_Dir = normalize(m_pBullet->m_Dir);
                m_pBullet->m_Pos+=m_pBullet->m_Dir;

                //Quitar vida al Enemigo
                vec2 PosT = vec2(m_pBullet->m_Pos.x+2, m_pBullet->m_Pos.y+2) - vec2(m_pBullet->m_Target->m_Pos.x + TILE_SIZE/2, m_pBullet->m_Target->m_Pos.y + TILE_SIZE/2);
                if (sqrtf(PosT.x*PosT.x + PosT.y*PosT.y) < TILE_SIZE/2)
                {
                    m_pBullet->m_Target->TakeDamage(5);
                    if (m_pBullet->m_Target->m_Health <= 0)
                    {
                        //Borrar todas las balas del enemigo muerto
                        for (size_t e=0; e<v_Entities.size(); e++)
                        {
                            if (v_Entities[e]->GetType() == CEntity::ENTITY_BULLET)
                            {
                                CBullet *m_pBulletT = static_cast<CBullet*>(v_Entities[e]);
                                if (m_pBulletT == m_pBullet)
                                    continue;

                                if (m_pBulletT->m_Target == m_pBullet->m_Target)
                                    RemoveEntity(m_pBulletT);
                            }
                        }

                        RemoveEnemy(m_pBullet->m_Target);
                    }

                    RemoveEntity(m_pBullet);
                }

                m_pBullet->m_Timer = GetTickCount() + 1000 * m_pBullet->m_TimerVel;
                m_pBullet->m_TimerVel-= 0.01f;
                if (m_pBullet->m_TimerVel < 0.0f)
                    m_pBullet->m_TimerVel = 0.0f;
            }
        }
    }


    /** DIBUJAMOS **/
    //Mapa
    if (m_pMap)
        m_pMap->Draw();
    //Enemigos
    setcolor(BLACK);
    setfillstyle(SOLID_FILL, BROWN);
    for (size_t i=0; i<v_Enemies.size(); i++)
    {
        CEnemy *m_pEnemy = v_Enemies[i];
        if (!m_pEnemy)
            continue;

        m_pEnemy->Draw();
    }
    //Entidades
    for (size_t i=0; i<v_Entities.size(); i++)
    {
        CEntity *m_pEntity = v_Entities[i];
        if (!m_pEntity)
            continue;

        if (m_pEntity->GetType() == CEntity::ENTITY_TOWER)
        {
            int points[4*2];

            setfillstyle(SOLID_FILL, LIGHTGRAY);
            points[0]=OFFSET+static_cast<int>(m_pEntity->m_Pos.x);
            points[1]=OFFSET+static_cast<int>(m_pEntity->m_Pos.y);
            points[2]=points[0]+TILE_SIZE;
            points[3]=points[1];
            points[4]=points[2];
            points[5]=points[1]+TILE_SIZE;
            points[6]=points[0];
            points[7]=points[5];
            fillpoly(4, points);

            setfillstyle(SOLID_FILL, DARKGRAY);
            points[0]=OFFSET+static_cast<int>(m_pEntity->m_Pos.x+2);
            points[1]=OFFSET+static_cast<int>(m_pEntity->m_Pos.y+2);
            points[2]=points[0]+TILE_SIZE-4;
            points[3]=points[1];
            points[4]=points[2];
            points[5]=points[1]+TILE_SIZE-4;
            points[6]=points[0];
            points[7]=points[5];
            fillpoly(4, points);
        }
        else if (m_pEntity->GetType() == CEntity::ENTITY_BULLET)
        {
            CBullet *m_pBullet = static_cast<CBullet*>(m_pEntity);
            setfillstyle(SOLID_FILL, m_pBullet->m_Color);
            fillellipse(static_cast<int>(OFFSET+m_pEntity->m_Pos.x), static_cast<int>(OFFSET+m_pEntity->m_Pos.y), 2, 2);
        }
    }
}

void CGameControllerCrapTower::EndRound()
{
}

void CGameControllerCrapTower::Reset()
{
    setbkcolor(BLACK);
    setcolor(WHITE);
    m_TimerCrapTower = GetTickCount();
    m_PlayerWin = false;

    {
        std::vector<CEnemy*>::iterator it = v_Enemies.begin();
        while(it != v_Enemies.end())
        {
                delete (*it);
                (*it) = 0x0;
                it = v_Enemies.erase(it);
        }
    }
    {
        std::vector<CEntity*>::iterator it_ = v_Entities.begin();
        while(it_ != v_Entities.end())
        {
                delete (*it_);
                (*it_) = 0x0;
                it_ = v_Entities.erase(it_);
        }
    }

    if (m_pMap)
    {
        delete m_pMap;
        m_pMap = 0x0;
    }
}


void CGameControllerCrapTower::RemoveEntity(CEntity *ent)
{
    std::vector<CEntity*>::iterator it = v_Entities.begin();
    while(it != v_Entities.end())
    {
        if ((*it) == ent)
        {
            delete((*it));
            (*it) = 0x0;
            it = v_Entities.erase(it);
            return;
        }
        else
            ++it;
    }
}

void CGameControllerCrapTower::RemoveEnemy(CEnemy *ent)
{
    std::vector<CEnemy*>::iterator it = v_Enemies.begin();
    while(it != v_Enemies.end())
    {
        if ((*it) == ent)
        {
            delete((*it));
            (*it) = 0x0;
            it = v_Enemies.erase(it);
            return;
        }
        else
            ++it;
    }
}

void CGameControllerCrapTower::RemoveRound(CRonda *round)
{
    std::deque<CRonda*>::iterator it = m_pMap->v_Rondas.begin();
    while(it != m_pMap->v_Rondas.end())
    {
        if ((*it) == round)
        {
            delete((*it));
            (*it) = 0x0;
            it = m_pMap->v_Rondas.erase(it);
            return;
        }
        else
            ++it;
    }
}
