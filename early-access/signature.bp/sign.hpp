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
#include "utils.hpp"

using std::string;
using std::vector;
using namespace eosio;
using namespace config;

typedef uint64_t time;

CONTRACT sign : public eosio::contract
{
  public:
    sign(name receiver, name code, datastream<const char *> ds) : 
        contract(receiver, code, ds),
        _signs(receiver, receiver.value),
        _shares(receiver, receiver.value) {}

    struct [[eosio::table("players")]] player_info
    {
        asset article_income;
        asset share_income;
    };    

    struct[[eosio::table("signs")]] sign_info
    {
        uint64_t id; // 签名 id
        name author; // 作者
        uint64_t fission_factor; // 裂变系数 * 1000
        uint64_t primary_key()const { return id; }
    };

    struct [[eosio::table("shares")]] share_info
    {   
        uint64_t id;                // 分享 id
        uint64_t target_sign_id;    // 目标签名 id
        name reader;                // 读者
        asset quota;                // 剩余配额  
        uint64_t primary_key()const { return id; }  
    };

    typedef singleton<"players"_n, player_info> singleton_players_t;
    typedef eosio::multi_index<"signs"_n, sign_info> index_sign_t;
    typedef eosio::multi_index<"shares"_n, share_info> index_share_t;
    index_sign_t _signs;
    index_share_t _shares;
    
    ACTION init();
    ACTION claim(name from);         

    void onTransfer(name from, name to,
                    asset in, string memo); 

    void create(name from, asset in, const vector<string>& params);
    void share(name from, asset in, const vector<string>& params);
    
    void apply(uint64_t receiver, uint64_t code, uint64_t action)
    {
        auto &thiscontract = *this;
        if (action == ("transfer"_n).value)
        {
            auto transfer_data = unpack_action_data<st_transfer>();
            onTransfer(transfer_data.from, transfer_data.to, transfer_data.quantity, transfer_data.memo);
            return;
        }

        if (code != _self.value) return;
        switch (action)
        {
            EOSIO_DISPATCH_HELPER(sign,
                (init)
                (claim)
            )
        }
    }
};

extern "C"
{
    [[noreturn]] void apply(uint64_t receiver, uint64_t code, uint64_t action) {
        sign p(name(receiver), name(code), datastream<const char *>(nullptr, 0));
        p.apply(receiver, code, action);
        eosio_exit(0);
    }
}