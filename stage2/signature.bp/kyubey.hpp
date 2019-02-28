/**
 *  @dev minakokojima
 *  @copyleft ／人◕ ‿‿ ◕人＼
 *  Want to make a Contract with me?
 *  I am the linear version of kyubey protocol.
 */
#pragma once
#include "token.hpp"

#include <eosiolib/eosio.hpp>
#include <eosiolib/asset.hpp>
// include <eosiolib/transaction.hpp>
// include <cmath>
typedef double real_type;

using namespace eosio ;

constexpr uint64_t K = 10000000000;

class kyubey : public token {
    public:
        using token::token ;

        /*
        void buy(name account, asset in) {    
            asset out;
            _market.modify(_market.begin(), 0, [&](auto &m) {
                out = m.buy(in.amount);
            }); 

           // static char msg[20];
           // sprintf(msg, "delta: %llu", out.amount);
           // eosio_assert(false, msg);

            issue(account, out, "");
        }
        
        void sell(name account, asset in) {
            
            sub_balance(account, in);          
            asset out;
            _market.modify(_market.begin(), 0, [&](auto &m) {
                out = m.sell(in.amount);
            });    
            
            action(
                permission_level{_self, "active"_n},
                N(eosio.token), "transfer"_n,
                make_tuple(_self, account, out, std::string(""))
            ).send();
        }

        struct market {
            uint64_t id = 0;        
            asset supply;
            asset balance;
            uint64_t progress;                         
            
            uint64_t fee(uint64_t x) {
                return x * progress / 10000;
            }

            void update_progress(uint64_t new_progress) {
                eosio_assert(new_progress <= 10000, "out of range");                                
                progress = new_progress;
            }
            
            asset buy(uint64_t in) {
                in -= fee(in);
                balance.amount += in;
                uint64_t new_supply = sqrt((real_type)balance.amount * 2 * K) * 100;
                uint64_t delta_supply = new_supply - supply.amount;

                supply.amount = new_supply;
                balance.amount = ((real_type)supply.amount * supply.amount) / 2 / K / 10000;
                return asset(delta_supply, supply.symbol);
            } 

            asset sell(uint64_t in) {
                supply.amount -= in;
                uint64_t new_balance = ((real_type)supply.amount * supply.amount) / 2 / K / 10000;
                uint64_t delta_balance = balance.amount - new_balance;

                balance.amount = new_balance;
                delta_balance -= fee(delta_balance);
                return asset(delta_balance, balance.symbol);
            }
            
            uint64_t primary_key() const { return id; }
            EOSLIB_SERIALIZE(market, (id)(supply)(balance)(progress))
        };

        typedef eosio::multi_index<"market"_n, market> market_t;*/
};
