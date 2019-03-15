/**
 *  @dev minakokojima, yukiexe
 *  @copyright Andoromeda
 */

#include "signature.bp.hpp"

/**
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
    if ( type == "orders" ) {
        while (_orders.begin() != _orders.end()) {
            _orders.erase(_orders.begin());
        }
    } else if ( type == "signs" ) {
        while (_signs.begin() != _signs.end()) {
            _signs.erase(_signs.begin());
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
    require_auth(sign.author);
    eosio_assert(1000 <= sign.fission_factor && sign.fission_factor <= 2000, "illegal fission_factor");
    // 写入签名表格
    auto _id = _signs.available_primary_key();
    _signs.emplace(_self, [&](auto &s) {
        s = sign ;
        s.id = _id;
    });
}

/**
    创建一个签名

    @param from 作者
    @param params 裂变系数
*/    
void sign::ezpublish( name author, uint64_t fission_factor, string ipfs_hash )
{
    require_auth(author);
    eosio_assert(1000 <= fission_factor && fission_factor <= 2000, "illegal fission_factor");
    // 写入签名表格
    auto _id = _signs.available_primary_key();
    _signs.emplace(_self, [&](auto &s) {
        s.id = _id;
        s.author = author;
        s.fission_factor = fission_factor;
        s.ipfs_hash = ipfs_hash;
    });
}

/**
    创建一个签名

    @param from 作者
    @param params 裂变系数
*/    
void sign::syspublish(const sign_info &sign)
{
    require_auth(_self);
    eosio_assert(1000 <= sign.fission_factor && sign.fission_factor <= 2000, "illegal fission_factor");
    // 写入签名表格
    auto _id = _signs.available_primary_key();
    _signs.emplace(_self, [&](auto &s) {
        s = sign ;
    });
}

/**
    创建一个商品

    @param seller 賣家
    @param price 价格
    @param referral_bonus 推荐返利
    @param fission_bonus  裂变返利
    @param fission_factor 裂变洗漱
*/    
void sign::publishgood(name seller, uint64_t price, uint64_t referral_bonus, uint64_t fission_bonus, uint64_t fission_factor)
{
    require_auth(seller);
    eosio_assert(1000 <= fission_factor && fission_factor <= 2000, "illegal fission_factor");

    index_good_t goods(_self, _self.value);
    auto id = goods.available_primary_key();
    goods.emplace(_self, [&](auto &g) {
        g.id = id;
        g.seller = seller;
        g.price = price;
        g.referral_bonus = referral_bonus;
        g.fission_bonus = fission_bonus;
        g.fission_factor = fission_factor;
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
    auto sign = _signs.require_find(sign_id, "this signature is not exist");

    // 写入分享表
    auto _id = _shares.available_primary_key();
    _shares.emplace(_self, [&](auto &s) {
        s.id = _id;
        s.reader = sharer;
        s.target_sign_id = sign_id;
        s.quota = in.amount * sign->fission_factor / 1000;
    });

    // 分錢給上游读者
    if (params.size() >= 2) {
        auto upstream_share_id = string_to_int(params[2]);
        auto upstream_share = _shares.find(upstream_share_id);
        if (upstream_share != _shares.end()) {
            int64_t delta = upstream_share->quota < in.amount ? upstream_share->quota : in.amount;

            // quota 扣掉並增加 share income
            _shares.modify(upstream_share, _self, [&](auto &s) {
                s.quota -= delta;
            });
            add_share_income(upstream_share->reader, asset{delta, EOS_SYMBOL});
            
            // 扣掉已經發的錢
            in.amount -= delta;
        }
        // 找不到沒做處理
    }

    // 最後分錢給作者，已扣掉發掉的錢
    add_sign_income(sign->author, in);
}

inline void sign::check_selling(const name &buyer, asset in, const vector<string> &params)
{
    // 理論上，未來可能不只用 EOS，可能用穩定幣系 ，這邊還需要擴寫
    eosio_assert(in.amount >= 1, "you need at least 0.0001 EOS for buying "); // 最小購買金額 0.1 EOS
    eosio_assert(params.size() >= 1, "No ID found..");
}

/**
    賣货

    @param buyer 買家
    @param in 付的錢
    @param params 商品编号，推荐人编号
*/
// from onTransfer
void sign::buy(const name &buyer, asset in, const vector<string> &params)
{
    require_auth(buyer);
    check_selling(buyer, in, params);
    
    uint64_t good_id = string_to_int(params[1]);
    auto good = _goods.require_find(good_id, "this good is not exist");

    const int64_t times = in.amount / good->price; // asset / asset
    eosio_assert(times > 0, "You have wrong cost." );
    eosio_assert(times * good->price == in.amount, "You must buy integer number of goods." );
   
    // 写入订单表格
    auto _id = _orders.available_primary_key();
    _orders.emplace(_self, [&](auto &o) {
        o.id = _id;
        o.good_id = good_id;
        o.count = times;
        o.buyer = buyer;
    });

    // 处理上游分销商
    if (params.size() >= 2)
    {  
        auto upstream_subscribe_id = string_to_int(params[2]);
        index_subscribe_t _subscribes(_self, _self.value);
        auto upstream_subscribe = _subscribes.find(upstream_subscribe_id);
        // 找不到沒做處理
        if (upstream_subscribe != _subscribes.end())
        {
            int64_t delta = upstream_subscribe->quota < times * good->fission_bonus ? upstream_subscribe->quota : times * good->fission_bonus;
            _subscribes.modify(upstream_subscribe, _self, [&](auto &s) {
                s.quota -= delta;
            });

            add_share_income(upstream_subscribe->subscriber, asset(delta, EOS_SYMBOL));
        }
    }

    /* 目前都是官方卖家，所以暂不处理
    // 处理卖家
    singleton_players_t _player(_self, sign->author.value);
    auto p = _player.get_or_create(_self, player_info{});
    p.sign_income += in.amount;
    _player.set(p, _self);
    */
}

/**
    賣货

    @param subscriber 订购者
    @param in 付的錢
    @param params 商品编号，推荐人编号
*/
// from onTransfer
void sign::subscribe(const name &from, asset in, const vector<string> &params)
{
    require_auth(from);
    check_selling(from, in, params);
    
    uint64_t good_id = string_to_int(params[1]);
    auto good = _goods.require_find(good_id, "this good is not exist");

    const int64_t times = in.amount / good->price; // asset / asset
    eosio_assert(times > 0, "You have wrong cost." );
    eosio_assert(times * good->price == in.amount, "You must subscribe integer number of goods." );
   
    // 写入订购表格
    index_subscribe_t _subscribes(_self, _self.value);
    auto _id = _subscribes.available_primary_key();
    _subscribes.emplace(_self, [&](auto &s) {
        s.id = _id;
        s.subscriber = from;
        s.target_goods_id = good_id;
        s.quota = in.amount * good->fission_factor / 1000;
    });

    // 处理上游分销商
    if (params.size() >= 2)
    {
        auto upstream_subscribe_id = string_to_int(params[2]);
        auto upstream_subscribe = _subscribes.find(upstream_subscribe_id);
        // 找不到沒做處理
        if (upstream_subscribe != _subscribes.end())
        {
            int64_t delta = upstream_subscribe->quota < times * good->fission_bonus ? upstream_subscribe->quota : times * good->fission_bonus;
            _subscribes.modify(upstream_subscribe, _self, [&](auto &s) {
                s.quota -= delta;
            });
            add_share_income(upstream_subscribe->subscriber, asset(delta, EOS_SYMBOL));
        }
    }
}

/**
    增加 share income
    
    @param referrer 增加誰的
    @param quantity 加多少

// 為什麼用 asset ，因為 asset 內含 overflow 檢查機制
*/
void sign::add_share_income(const name &referrer, const asset &quantity){
    singleton_players_t _player(_self, referrer.value);
    // 經驗談:
    // 不該 or_create ，但不這麼做，哪天哪個點會找不到 player 而炸
    // 需要一套方案，目前維持現狀
    auto p = _player.get_or_create(_self, player_info{});
    p.share_income += quantity.amount;
    _player.set(p, _self);
}

/**
    增加 sign income
    
    @param referrer 增加誰的
    @param quantity 加多少

// 為什麼用 asset ，因為 asset 內含 overflow 檢查機制
*/
void sign::add_sign_income(const name &referrer, const asset &quantity){
    singleton_players_t _player(_self, referrer.value);
    // 經驗談:
    // 不該 or_create ，但不這麼做，哪天哪個點會找不到 player 而炸
    // 需要一套方案，目前維持現狀
    auto p = _player.get_or_create(_self, player_info{});
    p.sign_income += quantity.amount;
    _player.set(p, _self);
}

/**
    执行并删除订单

    @param id 订单编号
*/
void sign::rmorder(const uint64_t id)
{
    // require_auth(_self);
    auto order = _orders.require_find(id, "thiss order is not exist");
    auto good = _goods.require_find(order->good_id, "this good is not exist");

    // 給 referrer
    add_share_income(order->refer, asset{ static_cast<int64_t>(order->count * good->referral_bonus), EOS_SYMBOL});

    // 最後，刪訂單
    _orders.erase(order);
}

/**
    提现

    @param from 不是发起者，是申請需claim的帳號。發起者可以跟後者不同的，要注意
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
    if (params[0] == "buy")
    {
        buy(from, in, params);
        return;
    }
    if (params[0] == "subscribe") 
    {
        subscribe(from, in, params);
        return;
    }
}