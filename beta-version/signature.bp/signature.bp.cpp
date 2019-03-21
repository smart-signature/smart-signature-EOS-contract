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
    require_auth(_self);
    if ( type == "signs" ) {
        index_sign_t signs(_self, _self.value);
        while (signs.begin() != signs.end()) {
            signs.erase(signs.begin());
        }
    }

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
    
    // 写入签名表格
    index_sign_t _signs(_self, _self.value);
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
void sign::create_a_share(const name &owner, asset in, const vector<string> &params)
{
    require_auth(owner);
    
    eosio_assert(in.amount >= 1, "you need at least 0.0001 EOS to sponsor a signature");
    // 0 -> false, 1 -> false, 2 -> true
    eosio_assert(params.size() > 1, "No ID found.."); 

    // 首先查 sign 存不存在
    auto sign_id = string_to_int(params[1]);
    index_sign_t _signs(_self, _self.value);    
    auto sign = _signs.require_find(sign_id, "this signature is not exist");
    
    // 查 owner 有沒有生過這 sign 的 share
    index_share_t _shares(_self, owner.value);
    auto share = _shares.find(sign->id);
    // A share can be only created by someone once.
    eosio_assert(share == _shares.end(), "the share was created");

    // 分錢給推薦人，也就是上游读者
    if (params.size() > 2) { // 有推薦人 referral 的情況
        name referral{params[2].c_str()};
        // check 推薦人是不是個人(x) 帳號(o)
        eosio_assert(is_account(referral), "Referral is not an existing account."); 
        
        if (referral != owner) { // 推荐人不是 owner 时
            index_share_t referral_shares(_self, referral.value);
            auto ref_share = referral_shares.find(sign_id);
            if (ref_share != referral_shares.end()) { // 推荐人的 share 存在時
                // 用 asset 啦 qwq
                int64_t delta = ref_share->quota < in.amount ? ref_share->quota : in.amount;
                // 從 quota 扣掉 delta
                // 並增加 delta 到推薦人的 share income
                referral_shares.modify(ref_share, _self, [&](auto &s) {
                    s.quota -= delta;
                });
                add_share_income(referral, asset{delta, EOS_SYMBOL});            
                // asset in 扣掉已發的錢，接著就進作者口袋
                in.amount -= delta;
            }
        }
    }

    // 写入分享表
    _shares.emplace(_self, [&](auto &s) {
        s.id = sign->id;
        s.quota = in.amount * sign->fission_factor / 1000;
    });

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
void sign::add_sign_income(const name &owner, const asset &quantity){
    singleton_players_t _player(_self, owner.value);
    auto p = _player.get_or_create(_self, player_info{});
    p.sign_income += quantity.amount;
    _player.set(p, _self);
    string str{"sign income"};
    SEND_INLINE_ACTION(*this, bill, { _self, "active"_n }, { str, owner, quantity });
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

    if (params[0] == "support") // 打賞
    {
        create_a_share(from, in, params);
        string str{"support expenses"};
        in = -in;
        SEND_INLINE_ACTION(*this, bill, { _self, "active"_n }, { str, from, in });
        return;
    }
    if (params[0] == "billtest") 
    {
        string str{"test income"};
        SEND_INLINE_ACTION(*this, bill, { _self, "active"_n }, { str, from, in });
        action(
            permission_level{_self, "active"_n},
            EOS_CONTRACT, "transfer"_n,
            make_tuple(_self, from, in, string{"test income back"}))
        .send();
        return;
    }   
}

