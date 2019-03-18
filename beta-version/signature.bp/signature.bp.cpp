/**
 *  @dev minakokojima, yukiexe
 *  @copyright Andoromeda
 */

#include "signature.bp.hpp"

/*
    合约初始化
*/
void sign::init()
{
    require_auth(_self);
}

/**
    清理表格
*/
void sign::clean( string type )
{
}

/**
    创建一个签名

    @param from 作者
    @param params 裂变系数
*/    
void sign::publish(const sign_info &sign)
{
    // require_auth(sign.author);
    require_auth(_self);
    eosio_assert(1000 <= sign.fission_factor && sign.fission_factor <= 2000, "illegal fission_factor");
    index_sign_t _signs(_self, _self.value);
    // 写入签名表格
    _signs.emplace(_self, [&](auto &s) {
        s = sign;
    });
}

/**
    创建一次分享並分錢給作者和上游讀者

    @param sharer 读者
    @param in 赏金
    @param params 签名 ID，上游分享 ID
*/
void sign::create_a_share(const name &sharer, asset in, const vector<string> &params)
{
    require_auth(sharer);
    
    eosio_assert(in.amount >= 1000, "you need at least 0.1 EOS to sponsor a signature"); // 最小打赏 0.1 EOS
    eosio_assert(params.size() >= 1, "No ID found..");

    auto sign_id = string_to_int(params[1]);
    index_sign_t _signs(_self, _self.value);    
    auto sign = _signs.require_find(sign_id, "this signature is not exist");

    // 写入分享表
    index_share_t _shares(_self, sharer.value);
    _shares.emplace(_self, [&](auto &s) {
        s.id = sign_id;
        s.quota = in.amount * sign->fission_factor / 1000;
    });

    // 分錢給上游读者
    if (params.size() >= 2) {
        name referral(params[2].c_str() ) ;
        eosio_assert(is_account(referral), "Referral is not an existing account."); // sponsor 存在 check
        
        index_share_t referral_shares(_self, referral.value);
        auto referral_share = referral_shares.find(sign_id);
        if (referral_share != referral_shares.end()) {
            int64_t delta = referral_share->quota < in.amount ? referral_share->quota : in.amount;
            // quota 扣掉並增加 share income
            referral_shares.modify(referral_share, _self, [&](auto &s) {
                s.quota -= delta;
            });
            add_share_income(referral, asset{delta, EOS_SYMBOL});            
            // 扣掉已經發的錢
            in.amount -= delta;
        }
        // 找不到沒做處理
    }
    // 最後分錢給作者，已扣掉發掉的錢
    add_sign_income(sign->author, in);
}

/**
    增加 share income
    
    @param referrer 增加誰的
    @param quantity 加多少
*/
void sign::add_share_income(const name &owner, const asset &quantity){
    singleton_players_t _player(_self, owner.value);
    auto p = _player.get_or_create(_self, player_info{});
    p.share_income += quantity.amount;
    _player.set(p, _self);
    string str{"share income"};
    SEND_INLINE_ACTION(*this, bill, { _self, "active"_n }, { str, owner, quantity });
}

/**
    增加 sign income
    
    @param referrer 增加誰的
    @param quantity 加多少
*/
void sign::add_sign_income(const name &referrer, const asset &quantity){
    singleton_players_t _player(_self, referrer.value);
    auto p = _player.get_or_create(_self, player_info{});
    p.sign_income += quantity.amount;
    _player.set(p, _self);
    string str{"sign income"};
    SEND_INLINE_ACTION(*this, bill, { _self, "active"_n }, { str, referrer, quantity });
}

/**
    提现

    @param from 不是发起者，是申請需 claim 的帳號。發起者可以跟後者不同的，要注意
*/
void sign::claim(name from)
{
    require_auth(from);
    
    singleton_players_t _player(_self, from.value);
    auto p = _player.get_or_create(_self, player_info{});
    int64_t income = p.sign_income + p.share_income;
    eosio_assert(income != 0, "nothing to claim");

    action(
        permission_level{_self, "active"_n},
        EOS_CONTRACT, "transfer"_n,
        make_tuple(_self, from, asset{income, EOS_SYMBOL},
                   string{"claim sign income & share income."}))
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
    if (to != _self) return;
    require_auth(from);

    eosio_assert(_code == EOS_CONTRACT, "code is not eosio.token, only true EOS token is allowed");
    eosio_assert(in.symbol == EOS_SYMBOL, "symbol is not equal EOS_SYMBOL, only true EOS token is allowed");
    eosio_assert(in.is_valid(), "Invalid token transfer");
    eosio_assert(in.amount > 0, "must transfer a positive amount");

    auto params = split(memo, ' ');
    eosio_assert(params.size() >= 1, "error memo");

    if (params[0] == "share")
    {
        create_a_share(from, in, params);
        return;
    }
    if (params[0] == "billtest") 
    {
        string str{"test income"};
        SEND_INLINE_ACTION(*this, bill, { _self, "active"_n }, { str, from, in });
        action(
            permission_level{_self, "active"_n},
            EOS_CONTRACT, "transfer"_n,
            make_tuple(_self, from, in,
                   string{"test income back"}))
        .send();
        return;
    }   
}

