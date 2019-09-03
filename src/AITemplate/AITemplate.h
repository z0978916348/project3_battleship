#pragma once

#include <BattleShipGame/Wrapper/AI.h>
#include <BattleShipGame/Ship.h>
#include <algorithm>
#include <random>
#include <ctime>
#include <BattleShipGame/Board.h>

class AI : public AIInterface
{
    std::vector<std::pair<int,int>> way;
    TA::Board map{20};
    bool Order;
public:
    virtual std::vector<TA::Ship> init(int size ,std::vector<int> ship_size, bool order, std::chrono::milliseconds runtime) override
    {
        (void)ship_size;
        (void)runtime;
        Order = order;
        std::vector<TA::Ship> tmp;
        //old
        tmp.push_back({3, 0,  6,TA::Ship::State::Available});
        tmp.push_back({3, 1,  11,TA::Ship::State::Available});
        tmp.push_back({5, 7,  4,TA::Ship::State::Available});
        tmp.push_back({7, 11, 13,TA::Ship::State::Available});
        
        /*tmp.push_back({3, 0,  0,TA::Ship::State::Available});
        tmp.push_back({3, 5,  0,TA::Ship::State::Available});
        tmp.push_back({5, 0,  5,TA::Ship::State::Available});
        tmp.push_back({7, 10, 10,TA::Ship::State::Available});*/
        for(int i=0; i<size; i++)
            way.emplace_back(i,0);

        for(int j=1; j<size; j++)
            way.emplace_back(0,j);

        for(int i=1; i<size; i++)
            way.emplace_back(i,19);

        for(int j=1; j<size-1; j++)
            way.emplace_back(19,j);


        for(int i=1;i<size-1;++i)
            for(int j=1;j<size-1;++j)
                if (j%3!=1 && i%3!=1) way.emplace_back(i, j);
                
        for (int i=1;i<size-1;i++)
            for (int j=1;j<size-1;j++)
                if (i%3==1 && j%3!=1) way.emplace_back(i, j);
        for (int i=1;i<size-1;i++)
            for (int j=1;j<size-1;j++)
                if (i%3!=1 && j%3==1) way.emplace_back(i, j);   
        for (int i=1;i<size-1;i++)
            for (int j=1;j<size-1;j++)
                if (i%3==1 && j%3==1) way.emplace_back(i, j);
        
        way.emplace_back(1,1);
        way.emplace_back(6,1);
        way.emplace_back(2,7);
        way.emplace_back(13,13);

        /*for(int i= 0;i< size;i++){
            for(int j= 0;j< size;j++){
                way.emplace_back(i, j);
            }
        }*/

        for(int i=0; i<size; i++)
            for(int j=0; j<size; j++)
                map[i][j] = TA::Board::State::Unknown;

        /*std::mt19937 mt;
        mt.seed( std::time(nullptr) + 7122 + (order?1:0) );
        std::shuffle(way.begin(), way.end(), mt);*/
        return tmp;
    }

    virtual void callbackReportEnemy(std::vector<std::pair<int,int>> v) override
    {
        if(v.empty())
            return;

        for(auto point : v){
            int x = point.first;
            int y = point.second;
            int i, j;
            if(map[x][y] == TA::Board::State::Unknown){
                for(i=0; i<3; i++){
                    for(j=6; j<9; j++){
                        if(i==x && j==y) {
                            map[x][y] = TA::Board::State::Hit;
                        }
                    }
                }
                for(i= 1; i<4; i++){
                    for(j=11; j<14; j++){
                        if(i==x && j==y) {
                            map[x][y] = TA::Board::State::Hit;
                        }
                    }
                }

                for(i=7; i<12; i++){
                    for(j= 4; j<9; j++){
                        if(i==x && j==y) {
                            map[x][y] = TA::Board::State::Hit;
                        }
                    }
                }

                for(i=11; i<18; i++){
                    for(j=13; j<20; j++){
                        if(i==x && j==y) {
                            map[x][y] = TA::Board::State::Hit;
                        }
                    }
                }

                map[x][y] = TA::Board::State::Empty;
            }
        }     
    }

    virtual std::pair<int,int> queryWhereToHit(TA::Board) override
    {
        auto res = way.back();
        way.pop_back();
        return res;
    }

    virtual void callbackReportHit(bool)  override
    {

    }

    virtual std::vector<std::pair<int,int>> queryHowToMoveShip(std::vector<TA::Ship> ships) override
    {
        std::vector<std::pair<int, int>> res;
	    for( auto ship : ships )
		    res.emplace_back(ship.x, ship.y);
			
         return res;
    }
};