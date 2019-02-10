/**
 *  @dev minakokojima
 *  @copyright Andoromeda
 */
#pragma once
#include <eosiolib/eosio.hpp>
#include <eosiolib/asset.hpp>
#include <eosiolib/singleton.hpp>
#include <eosiolib/transaction.hpp>

#include "config.hpp"
#include "council.hpp"
#include "NFT.hpp"
// #include <cmath>
#include "utils.hpp"
#include "kyubey.hpp"
// #include "eosio.token.hpp"
 
typedef double real_type;

using std::string;
using eosio::symbol_name;
using eosio::asset;
using eosio::symbol_type;
using eosio::permission_level;
using eosio::action;
using eosio::extended_asset;
using eosio::unpack_action_data;


CONTRACT sign : public council {
    public: 
        sign( name receiver, name code, datastream<const char*> ds ) :
        council(self),
        _global(_self, _self),
        _market(_self, _self),
        _sign(_self, _self){}

    struct [[eosio::table("signs")]] sign_info {
        uint64_t id;
        account_name creator = 0;
        account_name owner = 0;
        uint64_t creator_fee;
        uint64_t ref_fee;
        uint64_t k;        
        uint64_t price;
        uint64_t last_anti_bot_fee = 0;
        uint64_t anti_bot_init_fee;
        time anti_bot_timer;
        time last_buy_timer;        
        time st;
        uint64_t primary_key()const { return id; }
        uint64_t next_price() const {
            return price * k / 1000;
        }
    };    
    
    struct [[eosio::table("players")]] player_info {
        int64_t ref_income;
        int64_t staked_income;
        int64_t article_income;
        int64_t sponsor_income;
    };
        
    struct [[eosio::table("global")]] global_info {
        uint64_t defer_id;
        uint64_t total_staked;
        uint64_t global_fee;
        account_name last;
        time st, ed;
    };

    typedef eosio::multi_index<N(signs), sign_info> sign_index;
    sign_index _sign;

    typedef eosio::multi_index<N(market), kyubey::market> market_index;
    market_index _market;    

    typedef singleton<N(global), global_info> singleton_global;
    singleton_global _global;     

    typedef singleton<N(players), player_info> singleton_players;  
    
    // Contract management
    ACTION init();
    ACTION clear();     
    
    ACTION unstake(account_name from, uint64_t amount);
    ACTION claim(account_name from);    

    ACTION airdrop(account_name to, uint64_t amount);

    ACTION transfer(account_name   from,
                  account_name   to,
                  asset          quantity,
                  string         memo);

    ACTION test();

    ACTION receipt(const rec_reveal& reveal) {
        require_auth(_self);
    }

    void apply(uint64_t receiver, uint64_t code, uint64_t action);

    uint64_t get_next_defer_id() {
        auto g = _global.get();    
        g.defer_id += 1;
        _global.set(g, _self);
        return g.defer_id;
    }

    template <typename... Args>
    void send_defer_action(Args&&... args) {
        transaction trx;
        trx.actions.emplace_back(std::forward<Args>(args)...);
        trx.send(get_next_defer_id(), _self, false);
    }

private:
    void onTransfer(account_name from, account_name to,
                    extended_asset quantity, string memo); 

    void create(account_name from, extended_asset in, const vector<string>& params);
    void sponsor(account_name from, extended_asset in, const vector<string>& params);    
    void buy(account_name from, extended_asset in, const vector<string>& params);
    void sell(account_name from, extended_asset in, const vector<string>& params);

};


void sign::apply(uint64_t receiver, uint64_t code, uint64_t action) {   
    auto &thiscontract = *this;
    if (action == ( "transfer"_n ).value) {
        auto transfer_data = unpack_action_data<st_transfer>();
        onTransfer(transfer_data.from, transfer_data.to, extended_asset(transfer_data.quantity, code), transfer_data.memo);               
        return;
    }

    if (code != get_self().value) return;
    switch (action) {
        EOSIO_DISPATCH_HELPER(sign,
                              (init))
    }
}

extern "C" { // renew done
    [[noreturn]] void apply(uint64_t receiver, uint64_t code, uint64_t action) 
    {
        sign p( name(receiver), name(code), datastream<const char*>(nullptr, 0) );
        p.apply(receiver, code, action);
        eosio_exit(0);
    }
}