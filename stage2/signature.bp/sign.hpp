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
#include "util.hpp"
#include "kyubey.hpp"

using std::string;
using std::vector;
using namespace eosio;
using namespace config;
using namespace kyubeyutil;

typedef uint64_t time;

CONTRACT sign : virtual public eosio::contract, public kyubey
{
public:
    sign(name receiver, name code, datastream<const char *> ds) : 
        contract(receiver, code, ds),
        kyubey(receiver, code, ds),
        _signs(receiver, receiver.value),
        _shares(receiver, receiver.value) {}

    TABLE accounts : token::account {};

    // 用户表格，记录收入，scope 为用户账户
    struct [[eosio::table("players")]] player_info
    {
        uint64_t sign_income;    // 签名收入
        uint64_t share_income;   // 分享收入
    };

    // 签名表格，全局，scope 为此合约
    struct [[eosio::table("signs")]] sign_info
    {
        uint64_t id; // 签名 id
        name author; // 作者
        uint64_t fission_factor; // 裂变系数 * 1000
        uint64_t primary_key()const { return id; }
    };

    // 分享表格，全局，scope 为此合约
    struct [[eosio::table("shares")]] share_info
    {   
        uint64_t id;                // 分享 id
        uint64_t target_sign_id;    // 目标签名 id
        name reader;                // 读者
        uint64_t quota;             // 剩余配额  
        uint64_t primary_key()const { return id; }  
    };

    typedef singleton<"players"_n, player_info> singleton_players_t;
    typedef eosio::multi_index<"signs"_n, sign_info> index_sign_t;
    typedef eosio::multi_index<"shares"_n, share_info> index_share_t;
    index_sign_t _signs;
    index_share_t _shares;
    
    ACTION init();
    ACTION publish(name from, uint64_t fission_factor);
    ACTION claim(name from);

    // Token management by token issueer player
    /*
     * @auth: in token::create using require_auth(_self);
    */
    ACTION tcreate(name issuer, asset maximum_supply) {
        token::create(issuer, maximum_supply);
    }

    /*
     * @auth: in token::issue using require_auth(st.issuer);
    */    
    ACTION tissue(name to, asset quantity, string memo) {
        token::issue(to, quantity, memo);
    }

private:
    /* Using by onTransfer */
    void token_buy(name account, asset in) {
        kyubey::buy(account, in);
    }
 
public:
    /*
     * @auth: in tsell using require_auth(account);
    */
    ACTION tsell(name account, asset in) {
        require_auth(account);
        kyubey::sell(account, in);
    }

    ACTION transfer(name from, name to, asset quantity, string memo) {
        require_auth(from);
        // nft::transfer<sign_index_t>( {_self,_self.value}, from, to, id, memo );
        // _contract_kyubey.transfer(from, to, quantity, memo);
        // _contract_dividend.unstake(from, quantity, memo);
        // _contract_dividend.stake(to, quantity);
    }

    void onTransfer(name from, name to,
                    asset in, string memo);
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
                (publish)
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