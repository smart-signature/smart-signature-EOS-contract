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
#include <cmath>
typedef double real_type;

using namespace eosio ;

constexpr uint64_t K = 10000000000;

class kyubey : public token {
    public:
     using token::token;

        /*
        template <typename T>
        void transfer(T table, name from, name to, const uint64_t &id, string memo)
        {
            // 驗證權限
            eosio_assert(from != to, "cannot transfer to self");
            require_auth(from);
            eosio_assert(is_account(to), "to account does not exist");

            // 回執
            require_recipient(from);
            require_recipient(to);

            // 驗證 token
            auto itr = table.require_find(id, "Unable to find NFT");
            
            eosio_assert(quantity.is_valid(), "invalid quantity");
            eosio_assert(token.amount > 0, "must transfer positive quantity");
            eosio_assert(quantity.symbol == st.supply.symbol, "symbol precision mismatch");
            
            eosio_assert(memo.size() <= 256, "memo has more than 256 bytes");

            // 誰付 ram 錢
            auto payer = has_auth(to) ? to : from;

            // 表操作
            table.modify(itr, payer, [&](auto &t) {
                t.owner = to;
            });
        }*/

        void buy(name account, asset in) {
            market_t _market( _self, _self.value);
            asset out;
            
            _market.modify(_market.begin(), _self, [&](auto &m) {
                out = m.buy(in.amount);
            }); 

           // static char msg[20];
           // sprintf(msg, "delta: %llu", out.amount);
           // eosio_assert(false, msg);

            issue(account, out, "");
        }
        
        void sell(name account, asset in) {
            market_t _market( _self, _self.value);
            sub_balance(account, in);          
            asset out;
            _market.modify(_market.begin(), _self, [&](auto &m) {
                out = m.sell(in.amount);
            });
            
            action(
                permission_level{_self, "active"_n},
                config::EOS_CONTRACT, "transfer"_n,
                make_tuple(_self, account, out, std::string(""))
            ).send();
        }
    
    private:
        struct st_market {
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
            EOSLIB_SERIALIZE(st_market, (id)(supply)(balance)(progress))
        };

        typedef eosio::multi_index<"market"_n, st_market> market_t;
};
