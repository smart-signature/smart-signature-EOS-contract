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
#include "model/Contract/EOS/util/util.hpp"
#include "model/Contract/EOS/nft/nft.hpp"

using std::string;
using std::vector;
using namespace eosio;
using namespace config;

typedef uint64_t time;

CONTRACT sign : public eosio::contract
{
  public:
    using contract::contract;

    typedef uint64_t id_t;

    struct [[eosio::table("players")]] player_info
    {
        asset article_income;
        asset share_income;
        vector<id_t> signs;
    };    

    struct[[eosio::table("signs")]] sign_info : nft::st_nft 
    {
        name creator;
        uint64_t fission_factor; // 裂变系数
    };

    struct [[eosio::table("shares")]] share_info
    {
        uint64_t id; // share 自己的id
        id_t sign_id; // 对应的签名的 id
        asset sponsor; // 打赏了多少钱
        asset quota; // 还剩多少配额
    };    

    typedef singleton<"players"_n, player_info> singleton_players_t;
    typedef eosio::multi_index<"signs"_n, sign_info> sign_index_t;
    typedef eosio::multi_index<"shares"_n, share_info> share_index_t;

    // Actions
    // Contract management
    ACTION init();
    ACTION airdrop(name to, uint64_t amount);

    ACTION signcreate(name owner) {
        require_auth(_self);

        //two-way binding.
        // auto itr_p = _players.require_find( owner, "Unable to find player" );
        auto itr_newsign = _signs.emplace(_self, [&](auto &s) {
            s.id = _signs.available_primary_key();
            // s.creator
            s.owner = owner;

        });

        /*
        _players.modify(itr_p, _self, [&](auto &p) {
            p.signs.push_back(itr_newsign->id);
        });
        */
    }

    ACTION signtransfer(name from, name to, uint64_t id, string memo) {
        nft::transfer<sign_index_t>({_self, _self.value}, from, to, id, memo);

        singleton_players_t players_from(_self, from.value);
        auto p_from = players_from.get();
        p_from.signs.push_back(id);
        players_from.set(p_from, _self);

        singleton_players_t players_to(_self, to.value);
        auto p_to = players_to.get();

        p_to.signs.erase(std::find(p_to.signs.begin(), p_to.signs.end(), id));
        players_to.set(p_to, _self);
    }

    void onTransfer(name from, name to,
                    asset quantity, string memo); 
    void apply(uint64_t receiver, uint64_t code, uint64_t action);

  private:
    void create(name from, extended_asset in, const vector<string>& params);
    void sponsor(name from, extended_asset in, const vector<string>& params);
    void buy(name from, extended_asset in, const vector<string>& params);
    void sell(name from, extended_asset in, const vector<string>& params);
};

void sign::apply(uint64_t receiver, uint64_t code, uint64_t action) // Warning no EOS check
{
    auto &thiscontract = *this;
    if (action == ("transfer"_n).value)
    {
        auto transfer_data = unpack_action_data<kyubeyutil::st_transfer>();
        onTransfer(transfer_data.from, transfer_data.to, transfer_data.quantity, transfer_data.memo);
        return;
    }

    if (code != _self.value) return;
    switch (action)
    {
        EOSIO_DISPATCH_HELPER(sign,
                              (init)(airdrop))
    }
}

extern "C"
{
    [[noreturn]] void apply(uint64_t receiver, uint64_t code, uint64_t action) {
        sign p(name(receiver), name(code), datastream<const char *>(nullptr, 0));
        p.apply(receiver, code, action);
        eosio_exit(0);
    }
}