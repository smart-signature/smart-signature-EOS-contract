/**
 *  @dev minakokojima
 *  @copyright Andoromeda
 */
#pragma once
#include <eosiolib/asset.hpp>
#include <eosiolib/eosio.hpp>
#include <eosiolib/singleton.hpp>
// include <eosiolib/transaction.hpp>

#include "config.hpp"
#include "utils.hpp"

using std::string;
using std::vector;
using namespace eosio;
using namespace config;

class [[eosio::contract("signature.bp")]] sign : public eosio::contract
{
  public:
    sign(name receiver, name code, datastream<const char *> ds) : 
        contract(receiver, code, ds) {}

    // 用户表格，记录收入
    // scope 为用户账户
    struct [[eosio::table("players")]] player_info
    {
        uint64_t sign_income;    // 签名收入
        uint64_t share_income;   // 分享收入
    };    

    // 签名表格
    // scope: _self 為此合約
    struct[[eosio::table("signs")]] sign_info
    {
        uint64_t id;             // 签名 id
        name author;             // 作者        
        uint64_t fission_factor; // 裂变系数 * 1000
        string ipfs_hash;
        eosio::public_key public_key;
        eosio::signature signature;
        uint64_t primary_key()const { return id; }
    };

    // 分享表格
    // scope 为读者
    struct [[eosio::table("shares")]] share_info
    {
        uint64_t id;     // 签名 id
        uint64_t quota;  // 剩余配额  
        uint64_t primary_key()const { return id; }        
    };

    typedef singleton<"players"_n, player_info> singleton_players_t;
    typedef eosio::multi_index<"signs"_n, sign_info> index_sign_t;  
    typedef eosio::multi_index<"shares"_n, share_info> index_share_t;;
    
    ACTION init();
    ACTION clean(string type);
    ACTION publish(const sign_info &sign);
    ACTION claim(name from);

    // Log
    ACTION recselling( const uint64_t &good_id, const name &buyer, const uint64_t &quantity ) {
        require_auth(_self);
    }

    ACTION bill( const string &type, const name &owner, const asset &quantity ) {
        require_auth(_self);
        require_recipient(owner);
    }

    // Test
    ACTION testclaim(name account) {
        require_auth(account);
        add_share_income(account, {int64_t{1}, EOS_SYMBOL});
        SEND_INLINE_ACTION(*this, claim, { account, "active"_n }, { account });
    }
    
private:
    inline void add_share_income(const name &owner, const asset &quantity);
    inline void add_sign_income(const name &owner, const asset &quantity);
    void create_a_share(const name &sharer, asset in, const vector<string> &params);

    void onTransfer(name from, name to,
                    asset in, string memo);

  public:
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
                (clean)
                (publish)
                (claim)
                (bill)
                (testclaim)
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