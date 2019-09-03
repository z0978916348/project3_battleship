#pragma once

#include <BattleShipGame/Board.h>
#include <BattleShipGame/Wrapper/AI.h>
#include <GUI/GUIInterface.h>

#include <iostream>
#include <cassert>
#include <chrono>
#include <cstdarg>
#include <future>
#include <type_traits>
#include <unistd.h>

using namespace std;

namespace TA
{
    class BattleShipGame
    {
    public:
        BattleShipGame(
            int size,
            std::chrono::milliseconds runtime_limit = std::chrono::milliseconds(1000)
        ):
            m_size(size),
            m_runtime_limit(runtime_limit),
            m_P1(nullptr),
            m_P2(nullptr),
            m_P1Board(size),
            m_P2Board(size)
        {
            gui = new ASCII;
            m_ship_size = {3,3,5,7};
        }

        void setPlayer1(AIInterface *ptr) { assert(checkAI(ptr)); m_P1 = ptr; }
        void setPlayer2(AIInterface *ptr) { assert(checkAI(ptr)); m_P2 = ptr; }

        void run()
        {
            gui->title();
            if( !prepareState() ) return ;

            updateGuiGame();

            //Todo: Play Game

            //initailize AI_1 & AI_2
            m_P1Ship = m_P1->init(m_size, m_ship_size, true, m_runtime_limit);
            m_P2Ship = m_P2->init(m_size, m_ship_size, false, m_runtime_limit);

            if(!checkShipPosition(m_P1Ship) || !checkShipPosition(m_P2Ship))return;

            bool round = true; //determine P1 or P2 round
            std::vector <std::pair<int, int>> HitSpots; //recieve hit spots from the last AI
            AIInterface* current_AI = nullptr;
    
            //true for AI_1, false for AI_2
            while(true){
                //Board& my_board = (round == true)? m_P1Board:m_P2Board;
                Board& enemy_board = (round == false)? m_P1Board:m_P2Board;

                current_AI = (round == true)? m_P1:m_P2;
                
                /*updating data stage*/
                call(&AIInterface::callbackReportEnemy, current_AI, HitSpots);
                HitSpots.clear();//clean HitSpots of last AI in order to store HitSpots of current AI 

                /*attack stage*/
                
                //If Ai have K ships, then it can attack K times
                int living_ships = 0;
                std::vector <Ship>& my_Ships = (round == true)? m_P1Ship:m_P2Ship;
                std::vector <Ship>& enemy_Ships = (round == false)? m_P1Ship:m_P2Ship;
                for(auto ship : my_Ships){
                    if(ship.state == Ship::State::Available || ship.state == Ship::State::Hit){
                        living_ships++;
                    }
                }

                for(int i= 0;i< living_ships;i++){
                    //Depends on enemy board, decide where to hit
                    std::pair<int, int> spot = call(&AIInterface::queryWhereToHit, current_AI, enemy_board);
                    HitSpots.push_back(spot);
                }

                //check if attack is available
                for(auto spot : HitSpots){
                    int x = spot.first, y = spot.second;
                    if(enemy_board[x][y] == Board::State::Unknown){
                        bool hit_ship = false;
                        for(auto& ship : enemy_Ships){
                            hit_ship = checkShipStatus(ship, x, y);
                            if(hit_ship)break;
                        }

                        if(hit_ship){
                            putToGui("You hit a ship!\n");
                            enemy_board[x][y] = Board::State::Hit;
                            if(checkGameover()){
                                updateGuiGame();
                                return;
                            }
                        }
                        else enemy_board[x][y] = Board::State::Empty;
                    }
                    else{
                        putToGui("Invalid attack!\n");
                    }
                    updateGuiGame();
                }
				
				std::vector<std::pair<int, int>> plot;
				
                /*movement stage*/
                plot = call(&AIInterface::queryHowToMoveShip, current_AI, my_Ships);

                /*exchange stage*/
                round = !round;
            }
        } 

   private:
        bool checkShipStatus(Ship & ship, int x, int y){
            if(y < ship.y+ ship.size && y >= ship.y && x < ship.x+ ship.size && x >= ship.x){
                if(ship.state == Ship::State::Available){
                    if(y == ship.y+ ship.size/2 && x == ship.x+ ship.size/2)ship.state = Ship::State::Sink;
                    else ship.state = Ship::State::Hit;
                }
                else if(ship.state == Ship::State::Hit){
                    if(y == ship.y+ ship.size/2 && x == ship.x+ ship.size/2)ship.state = Ship::State::Sink;
                }

                return true;
            }
            return false;
        }
        
        void updateGuiGame()
        {
            gui->updateGame(m_P1Board, m_P1Ship, m_P2Board, m_P2Ship);
            usleep(10000);
        }

        bool checkGameover()
        {
            bool flag = true;

            for( Ship s : m_P1Ship )
                if( s.state != Ship::State::Sink )
                    flag = false;
            if( flag )
            {
                putToGui("P2 win!\n");
                return true;
            }
            for( Ship s : m_P2Ship )
                if( s.state != Ship::State::Sink )
                    return false;
            putToGui("P1 win!\n");
            return true;
        }

        bool prepareState()
        {
            std::vector<Ship> initPos;

            putToGui("P1 Prepareing...\n");
            initPos = call(&AIInterface::init, m_P1, m_size, m_ship_size, true, m_runtime_limit);
            if( !checkShipPosition(initPos) )
            {
                putToGui("P1 Init() Invaild...\n");
                return false;
            }
            for( auto &ship : initPos )
                ship.state = Ship::State::Available;
            m_P1Ship = initPos;
            putToGui("Done.\n");

            initPos.clear();

            putToGui("P2 Prepareing...\n");
            initPos = call(&AIInterface::init, m_P2, m_size, m_ship_size, false, m_runtime_limit);
            if( !checkShipPosition(initPos) )
            {
                putToGui("P2 Init() Invaild...\n");
                return false;
            }
            for( auto &ship : initPos )
                ship.state = Ship::State::Available;
            m_P2Ship = initPos;
            putToGui("Done.\n");
            return true;
        }

        template<typename Func ,typename... Args, 
            std::enable_if_t< std::is_void<
                    std::invoke_result_t<Func, AIInterface, Args...>
                >::value , int> = 0 >
        void call(Func func, AIInterface *ptr, Args... args)
        {
            std::future_status status;
            auto val = std::async(std::launch::async, func, ptr, args...);
            status = val.wait_for(std::chrono::milliseconds(m_runtime_limit));

            if( status != std::future_status::ready )
            {
                exit(-1);
            }
            val.get();
        }

        template<typename Func ,typename... Args, 
            std::enable_if_t< std::is_void<
                    std::invoke_result_t<Func, AIInterface, Args...>
                >::value == false, int> = 0 >
        auto call(Func func, AIInterface *ptr, Args... args)
            -> std::invoke_result_t<Func, AIInterface, Args...>
        {
            std::future_status status;
            auto val = std::async(std::launch::async, func, ptr, args...);
            status = val.wait_for(std::chrono::milliseconds(m_runtime_limit));

            if( status != std::future_status::ready )
            {
                exit(-1);
            }
            return val.get();
        }

        void putToGui(const char *fmt, ...)
        {
            va_list args1;
            va_start(args1, fmt);
            va_list args2;
            va_copy(args2, args1);
            std::vector<char> buf(1+std::vsnprintf(nullptr, 0, fmt, args1));
            va_end(args1);
            std::vsnprintf(buf.data(), buf.size(), fmt, args2);
            va_end(args2);

            if( buf.back() == 0 ) buf.pop_back();
            gui->appendText( std::string(buf.begin(), buf.end()) );
        }

        bool checkAI(AIInterface *ptr) 
        {
            return ptr->abi() == AI_ABI_VER;
        }

        bool checkShipPosition(std::vector<Ship> ships)
        {
            
            if( ships.size() != m_ship_size.size() )
            {
                putToGui("Ship number not match : real %zu ,expect %zu,\n",ships.size(), m_ship_size.size());
                return false;
            }
            
            std::sort(ships.begin(), ships.end(), [](Ship a, Ship b){
                return a.size < b.size; 
            });

            std::vector<std::vector<int>> map(m_size, std::vector<int>(m_size));

            int id = -1;
            for( auto [size, x, y, state] : ships )
            {
                id++;
                if( size != m_ship_size[id] )
                {
                    putToGui("Ship %d size not match : real %zu ,expect %zu,\n", id, size, m_ship_size[id]);
                    return false;
                }

                for( int dx = 0 ; dx < size ; dx++ )
                    for( int dy = 0 ; dy < size ; dy++ )
                        {
                            int nx = x + dx;
                            int ny = y + dy;

                            if( nx < 0 || nx >= m_size || ny < 0 || ny >= m_size )
                            {
                                putToGui("Ship %d out of range at (%d,%d),\n", id, nx, ny);
                                return false;
                            }

                            if( map[nx][ny] != 0 )
                            {
                                putToGui("Ship %d and %d overlapping at (%d,%d),\n", id,  map[nx][ny]-1, nx, ny);
                                return false;
                            }
                            map[nx][ny] = id + 1;
                        }
            }
            return true;
        }

        int m_size;
        std::vector<int> m_ship_size;
        std::chrono::milliseconds m_runtime_limit;

        AIInterface *m_P1;
        AIInterface *m_P2;
        GUIInterface *gui;

        std::vector<Ship> m_P1Ship;
        std::vector<Ship> m_P2Ship;
        Board m_P1Board;
        Board m_P2Board;
    } ;
}