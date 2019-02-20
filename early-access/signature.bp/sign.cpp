/**
 *  @dev minakokojima, yukiexe
 *  @copyright Andoromeda
 */

#include "sign.hpp"

void sign::init()
{
    require_auth(_self);
}

void sign::claim(name from)
{
    require_auth(from);
    singleton_players_t _player(_self, from.value);
    auto p = _player.get_or_create(_self, player_info{.article_income = asset(0, EOS_SYMBOL),
                                                      .share_income = asset(0, EOS_SYMBOL)});
    auto _quantity = p.article_income + p.share_income;
    eosio_assert(_quantity.amount == 0, "nothing to claim");

    action(
        permission_level{_self, "active"_n},
        EOS_CONTRACT, "transfer"_n,
        make_tuple(_self, from, _quantity,
                   string("claim article income & share income.")))
        .send();
    p.article_income = asset(0, EOS_SYMBOL);
    p.share_income = asset(0, EOS_SYMBOL);
    _player.set(p, from);
}

void sign::create(name from, asset in, const vector<string> &params)
{
    require_auth(from);
    eosio_assert(in.amount >= 1000, "you need at least 0.1 EOS to create an new signature");
    eosio_assert(params.size() == 2, "need 1 params");

    auto _fission_factor = string_to_price(params[1]);
    eosio_assert(_fission_factor > 1000, "illegal fission_factor"); // 裂变系数还需要一个最大值限定

    auto _id = _signs.available_primary_key();
    _signs.emplace(_self, [&](auto &s) {
        s.id = _signs.available_primary_key();
        s.creator = from;
        s.fission_factor = _fission_factor;
    });
}

void sign::sponsor(name from, asset in, const vector<string> &params)
{
    require_auth(from);
    eosio_assert(in.amount >= 1000, "you need at least 0.1 EOS to sponsor a signature"); // 最小打赏 0.1 EOS
    eosio_assert(params.size() >= 1, "No ID found..");

    auto id = string_to_price(params[0]);
    auto itr = _signs.find(id);
    eosio_assert(itr != _signs.end(), "this article signature is not exist");

    // 处理分享者
    if (params.size() >= 2) {
        auto ref = name(params[1]);
        if (is_account(ref)) {
             share_index_t _shares(_self, ref.value);
             auto s = _shares.find(id);
             if (s != _shares.end()) {
                auto delta = s->quota < in ? s->quota : in;
                _shares.emplace(_self, [&](auto &s) {                 
                    s.quota -= delta;
                });
                /*
                singleton_players_t _player(_self, ref.value);
                auto p = _player.get_or_create(_self, player_info{});
                p.share_income += delta;
                _player.set(p, _self);
                in -= delta;*/
            }
        } 
    }

    // 处理作者
    /*
    singleton_players_t _player(_self, itr->creator);
    auto p = _player.get_or_create(_self, player_info{});
    p.article_income += in;
    _player.set(p, _self);
    */
}

void sign::onTransfer(name from, name to, extended_asset in, string memo)
{
    if (to != _self)
        return;
    require_auth(from);

    eosio_assert(in.quantity.is_valid(), "Invalid token transfer");
    eosio_assert(in.quantity.amount > 0, "must buy a positive amount");
   // eosio_assert(in.contract == "eosio.token"_n, "must be EOS");
    //eosio_assert(in.symbol == "EOS"_n, "must be EOS");
    auto params = split(memo, ' ');
    eosio_assert(params.size() >= 1, "error memo");

    eosio_assert(in.contract == "eosio.token"_n, "only true EOS token is allowed");
    eosio_assert(in.quantity.symbol == EOS_SYMBOL, "only true EOS token is allowed");

    if (params[0] == "sponsor")
    {
        sponsor(from, in.quantity, params);
        return;
    }

    if (params[0] == "create")
    {
        create(from, in.quantity, params);
        return;
    }
}