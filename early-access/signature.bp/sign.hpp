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
#include "NFT.hpp"
// include "council.hpp"
#include "kyubey.hpp"

using std::string;
using std::vector;
using namespace eosio;
using namespace config;

typedef uint64_t time;

CONTRACT sign : public eosio::contract
{
  public:
    sign(name receiver, name code, datastream<const char *> ds) : contract(receiver, code, ds)                                                                  
    {
    }

    struct [[eosio::table("players")]] player_info
    {
        asset article_income;
        asset share_income;
    };    

    struct[[eosio::table("signs")]] sign_info : NFT::tradeable_token
    {
        uint64_t id; // 签名 id
        name creator;
        uint64_t fission_factor; // 裂变系数
    };

    struct [[eosio::table("shares")]] share_info
    {
        uint64_t id; // 对应的签名的 id
        asset sponsor; // 打赏了多少钱
        asset quota; // 还剩多少配额
    };    

    typedef singleton<"players"_n, player_info> singleton_players_t;
    typedef eosio::multi_index<"signs"_n, sign_info> sign_index_t;
    typedef eosio::multi_index<"shares"_n, share_info> share_index_t;

    ACTION init();
    ACTION airdrop(name to, uint64_t amount);

    void onTransfer(name from, name to,
                    extended_asset in, string memo); 

    void create(name from, extended_asset in, const vector<string>& params);
    void sponsor(name from, extended_asset in, const vector<string>& params);
    void buy(name from, extended_asset in, const vector<string>& params);
    void sell(name from, extended_asset in, const vector<string>& params);



    void apply(uint64_t receiver, uint64_t code, uint64_t action)
    {
        auto &thiscontract = *this;
        if (action == ("transfer"_n).value)
        {
            auto transfer_data = unpack_action_data<st_transfer>();
            onTransfer(transfer_data.from, transfer_data.to, extended_asset(transfer_data.quantity, name(code)), transfer_data.memo);
            return;
        }

        if (code != _self.value) return;
        switch (action)
        {
            EOSIO_DISPATCH_HELPER(sign,
                (init)
                (airdrop)
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