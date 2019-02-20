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
// #include "NFT.hpp"
// #include "council.hpp"
#include "kyubey.hpp"

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
        _signs(receiver, receiver.value) {}

    struct [[eosio::table("players")]] player_info
    {
        asset article_income;
        asset share_income;
    };    

    struct[[eosio::table("signs")]] sign_info
    {
        uint64_t id; // 签名 id
        name creator;
        uint64_t fission_factor; // 裂变系数*1000
        uint64_t primary_key()const { return id; }
    };

    struct [[eosio::table("shares")]] share_info
    {
        uint64_t id; // 对应的签名的 id
        asset sponsor; // 打赏了多少钱
        asset quota; // 还剩多少配额
    
    };    

    typedef singleton<"players"_n, player_info> singleton_players_t;
    typedef eosio::multi_index<"signs"_n, sign_info> sign_index_t;
    sign_index_t _signs;
    typedef singleton<"shares"_n, share_info> singleton_share_t;
    

    ACTION init();
    ACTION airdrop(name to, uint64_t amount);

    // 服务器定时调用，同步树状结构数据。比如现在是第100玩家进场，
    // 假设前20个可以拿到share_income，那么将前20个的share_income同步进来。
    ACTION settle(name to, uint64_t amount, const vector<string>& params);
    ACTION claim(name from);         

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
                (settle)
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