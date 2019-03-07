/**
 *  @dev minakokojima
 *  @copyright Andoromeda
 */
#pragma once
#include <eosiolib/eosio.hpp>
#include <eosiolib/asset.hpp>
#include <eosiolib/singleton.hpp>
// include <eosiolib/transaction.hpp>

#include "config.hpp"
#include "utils.hpp"

using std::string;
using std::vector;
using namespace eosio;
using namespace config;

typedef uint64_t time;

class [[eosio::contract("signature.bp")]] sign : public eosio::contract
{
  public:
    sign(name receiver, name code, datastream<const char *> ds) : 
        contract(receiver, code, ds),
        _signs(receiver, receiver.value),
        _shares(receiver, receiver.value),
        _goods(receiver, receiver.value),
        _orders(receiver, receiver.value) {}

    // 用户表格，记录收入
    // @param scope 为用户账户
    struct [[eosio::table("players")]] player_info
    {
        uint64_t sign_income;    // 签名收入
        uint64_t share_income;   // 分享收入
    };    

    // 签名表格，全局
    // @param scope 为此合约 
    struct[[eosio::table("signs")]] sign_info
    {
        uint64_t id; // 签名 id
        name author; // 作者
        uint64_t fission_factor; // 裂变系数 * 1000
        uint64_t primary_key()const { return id; }
        // EOSLIB_SERIALIZE(sign_info, (id)(author)(fission_factor))
    };

    // 商品表格，全局
    // @param scope 为此合约 
    struct[[eosio::table("goods")]] good_info
    {
        uint64_t id;
        name seller; // 賣家
        uint64_t price; // 一份的價格
        uint64_t referral_bonus; // 推荐返利
        uint64_t fission_bonus;  // 裂变返利
        uint64_t fission_factor; // 裂变系数 * 1000
        uint64_t primary_key()const { return id; }
        // EOSLIB_SERIALIZE(good_info, (id)(seller)(price)(referral_bonus)(fission_bonus)(fission_factor) )
    };

    // 订单表格，全局
    // scope 為此合约的場合，是一般訂單
    // scope 為player的場合，是用來記錄該筆 order 裂变返利 所需的data，
    //                      同時跟 scope 為此合约的同筆data 有所連動
    struct[[eosio::table("orders")]] order_info
    {
        uint64_t id;
        uint64_t good_id;  // 商品, good_id
        uint64_t count; // 数量
        name buyer;     // 买家
        name refer;     // 推荐人
        uint64_t primary_key()const { return id; }
        // EOSLIB_SERIALIZE(order_info, (id)(good_id)(count)(buyer)(refer) )
    };

    // 分享表格，全局
    // @param scope 为此合约
    struct [[eosio::table("shares")]] share_info
    {
        uint64_t id;                // 分享 id
        uint64_t target_sign_id;    // 目标签名 id
        name reader;                // 读者
        uint64_t quota;             // 剩余配额  
        uint64_t primary_key()const { return id; }
        uint64_t get_target_id()const { return target_sign_id; }        
        // EOSLIB_SERIALIZE(order_info, (id)(good_id)(count)(buyer)(refer) )
    };

    typedef singleton<"players"_n, player_info> singleton_players_t;
    typedef eosio::multi_index<"signs"_n, sign_info> index_sign_t;
    typedef eosio::multi_index<"goods"_n, good_info> index_good_t;
    typedef eosio::multi_index<"orders"_n, order_info> index_order_t;    
    // typedef eosio::multi_index<"shares"_n, share_info> index_share_t;
    typedef eosio::multi_index< "shares"_n, share_info, 
                                indexed_by<"bytargetid"_n, const_mem_fun<share_info, uint64_t, &share_info::get_target_id> > 
            > index_share_t;
    index_sign_t _signs;
    index_share_t _shares;
    index_good_t _goods;
    index_order_t _orders;
    
    ACTION init();
    ACTION clean();
    ACTION publish(name from, uint64_t fission_factor);
    ACTION claim(name from);

    ACTION publishgood(name seller, uint64_t price, uint64_t referral_bonus, uint64_t fission_bonus, uint64_t fission_factor);
    ACTION rmorder(const uint64_t id);

    // Log
    ACTION recselling( const uint64_t &good_id, const name &buyer, const uint64_t &quantity ) {
        require_auth(_self);
    }

private:
    void add_share_income(const name &referrer, const asset &quantity);
    inline void check_selling(const name &buyer, asset in, const vector<string> &params);
    void selling(const name &buyer, asset in, const vector<string> &params);
    void superselling(const name &buyer, asset in, const vector<string> &params);
    void shareselling(const name &buyer, asset in, const vector<string> &params);

    void onTransfer(name from, name to,
                    asset in, string memo);
    void share(name from, asset in, const vector<string>& params);

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
                (publishgood)
                (rmorder)
                (recselling)
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