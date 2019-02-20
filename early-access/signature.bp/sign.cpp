/**
 *  @dev minakokojima, yukiexe
 *  @copyright Andoromeda
 */

#include "sign.hpp"

void sign::init()
{
    require_auth(_self);
}

/**
    创建一个签名

    @param from 作者
    @param asset 门槛
    @params params 裂变系数
*/
void sign::create(name from, uint64_t fission_factor)
{
    require_auth(from);
    eosio_assert(1000 <= fission_factor && fission_factor <= 2000, "illegal fission_factor");

    // 写入签名表格
    auto _id = _signs.available_primary_key();
    _signs.emplace(_self, [&](auto &s) {
        s.id = _signs.available_primary_key();
        s.author = from;
        s.fission_factor = fission_factor;
    });
}

/**
    创建一次分享

    @param from 读者
    @param in 赏金
    @params params 签名 ID，上游分享 ID
*/
void sign::share(name from, asset in, const vector<string> &params)
{
    require_auth(from);
    eosio_assert(in.amount >= 1000, "you need at least 0.1 EOS to sponsor a signature"); // 最小打赏 0.1 EOS
    eosio_assert(params.size() >= 1, "No ID found..");

    auto id = string_to_int(params[0]);
    auto sign = _signs.find(id);
    eosio_assert(sign != _signs.end(), "this signature is not exist");

    // 写入分享表格
    auto _id = _shares.available_primary_key();
    _shares.emplace(_self, [&](auto &s) {
        s.id = _shares.available_primary_key();
        s.reader = from;
        s.quota = in;
    });

    // 处理上游读者
    if (params.size() >= 2) {
        auto upstream_share_id = string_to_int(params[1]);
        auto share = _shares.find(upstream_share_id);
        if (share != _shares.end()) {
            auto delta = share->quota < in ? share->quota : in;
            _shares.modify(share, _self, [&](auto &s) {                 
                s.quota -= delta;
            });
            singleton_players_t _player(_self, share->reader.value);
            auto p = _player.get_or_create(_self, player_info{});
            p.share_income += delta;
            _player.set(p, _self);
            in -= delta;
        } 
    }

    // 处理作者
    singleton_players_t _player(_self, sign->author.value);
    auto p = _player.get_or_create(_self, player_info{});
    p.article_income += in;
    _player.set(p, _self);
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

void sign::onTransfer(name from, name to, asset in, string memo)
{
    if (to != _self) return;
    require_auth(from);

    eosio_assert(_code == "eosio.token"_n, "code is not eosio.token, only true EOS token is allowed");
    eosio_assert(in.symbol == EOS_SYMBOL, "symbol is not EOS_SYMBOL, only true EOS token is allowed");
    eosio_assert(in.is_valid(), "Invalid token transfer");
    eosio_assert(in.amount > 0, "must transfer a positive amount");

    auto params = split(memo, ' ');
    eosio_assert(params.size() >= 1, "error memo");    

    if (params[0] == "share")
    {
        share(from, in, params);
        return;
    }
}