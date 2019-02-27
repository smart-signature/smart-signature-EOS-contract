/**
 *  @dev minakokojima, yukiexe
 *  @copyright Andoromeda
 */

#include "sign.hpp"

/**
    合约初始化
*/
void sign::init()
{
    require_auth(_self);
}

/**
    创建一个签名

    @param from 作者
    @param params 裂变系数
*/    
void sign::publish(name from, uint64_t fission_factor)
{
    require_auth(from);
    eosio_assert(1000 <= fission_factor && fission_factor <= 2000, "illegal fission_factor");

    // 写入签名表格
    auto _id = _signs.available_primary_key();
    _signs.emplace(_self, [&](auto &s) {
        s.id = _id;
        s.author = from;
        s.fission_factor = fission_factor;
    });
}

/**
    创建一次分享

    @param from 读者
    @param in 赏金
    @param params 签名 ID，上游分享 ID
*/
void sign::share(name from, asset in, const vector<string> &params)
{
    require_auth(from);
    eosio_assert(in.amount >= 1000, "you need at least 0.1 EOS to sponsor a signature"); // 最小打赏 0.1 EOS
    eosio_assert(params.size() >= 1, "No ID found..");

    auto id = string_to_int(params[1]);
    auto sign = _signs.require_find(id, "this signature is not exist");

    // 写入分享表格
    auto _id = _shares.available_primary_key();
    _shares.emplace(_self, [&](auto &s) {
        s.id = _id;
        s.reader = from;
        s.quota = in.amount * sign->fission_factor / 1000;
    });

    // 处理上游读者
    if (params.size() >= 2)
    {
        auto upstream_share_id = string_to_int(params[2]);
        auto upstream_share = _shares.find(upstream_share_id);
        if (upstream_share != _shares.end())
        {
            auto delta = upstream_share->quota < in.amount ? upstream_share->quota : in.amount;
            _shares.modify(upstream_share, _self, [&](auto &s) {
                s.quota -= delta;
            });
            singleton_players_t _player(_self, upstream_share->reader.value);
            auto p = _player.get_or_create(_self, player_info{});
            p.share_income += delta;
            _player.set(p, _self);
            in.amount -= delta;
        }
    }

    // 处理作者
    singleton_players_t _player(_self, sign->author.value);
    auto p = _player.get_or_create(_self, player_info{});
    p.sign_income += in.amount;
    _player.set(p, _self);
}

/**
    提现

    @param from 发起者
*/
void sign::claim(name from)
{
    require_auth(from);
    singleton_players_t _player(_self, from.value);
    auto p = _player.get_or_create(_self, player_info{});
    uint64_t income = p.sign_income + p.share_income;
    eosio_assert(income == 0, "nothing to claim");

    action(
        permission_level{_self, "active"_n},
        EOS_CONTRACT, "transfer"_n,
        make_tuple(_self, from, asset(income, EOS_SYMBOL),
                   string("claim sign income & share income.")))
        .send();

    p.sign_income = 0;
    p.share_income = 0;
    _player.set(p, from);
}

/**
    转账路由

    @param from 发起者
    @param to 接受者
    @param in 资产
    @param memo 备忘录
*/
void sign::onTransfer(name from, name to, asset in, string memo)
{
    if (to != _self)
        return;
    require_auth(from);

    eosio_assert(_code == EOS_CONTRACT, "code is not eosio.token, only true EOS token is allowed");
    eosio_assert(in.symbol == EOS_SYMBOL, "symbol is not equal EOS_SYMBOL, only true EOS token is allowed");
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