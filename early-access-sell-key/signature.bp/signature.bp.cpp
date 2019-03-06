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
void sign::clean()
{
    require_auth(_self);
    while (_orders.begin() != _orders.end()) {
        _orders.erase(_orders.begin());
    }
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
        s.target_sign_id = id;
        s.quota = in.amount * sign->fission_factor / 1000;
    });

    // 处理上游读者
    if (params.size() >= 2)
    {
        auto upstream_share_id = string_to_int(params[2]);
        auto upstream_share = _shares.find(upstream_share_id);
        // 找不到沒做處理
        if (upstream_share != _shares.end())
        {
            int64_t delta = upstream_share->quota < in.amount ? upstream_share->quota : in.amount;
            _shares.modify(upstream_share, _self, [&](auto &s) {
                s.quota -= delta;
            });

            add_share_income(upstream_share->reader, asset(delta, EOS_SYMBOL));
            in.amount -= delta;
        }
    }

    // 处理作者
    singleton_players_t _player(_self, sign->author.value);
    auto p = _player.get_or_create(_self, player_info{});
    p.sign_income += in.amount;
    _player.set(p, _self);
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
void sign::selling(const name &buyer, asset in, const vector<string> &params)
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

    /* SEND_INLINE_ACTION(*this, recselling, { _self, "active"_n }, { good->id, buyer, times }); */
}

/**
    賣货

    @param buyer 買家
    @param in 付的錢
    @param params 商品编号，推荐人编号
*/
// from onTransfer
void sign::superselling(const name &buyer, asset in, const vector<string> &params)
{
    require_auth(buyer);
    check_selling(buyer, in, params);
    
    uint64_t good_id = string_to_int(params[1]);
    auto good = _goods.require_find(good_id, "this good is not exist");

    const int64_t times = in.amount / good->price; // asset / asset
    eosio_assert(times > 0, "You have wrong cost." );
    eosio_assert(times * good->price == in.amount, "You must buy integer number of goods." );

    index_order_t p_orders(_self, buyer.value);

    // 写入订单表格
    auto _id = _orders.available_primary_key();
    _orders.emplace(_self, [&](auto &o) {
        o.id = _id;
        o.good_id = good_id;
        o.count = times;
        o.buyer = buyer;
    });

    /*
    // id      同總表的
    // good_id 同總表的
    // 這個 ram_payer 應該 buyer 付，不過一樣會造成各種問題，總之一律先 _self
    * 因為同步新增的關係，不會有重復，不做檢查
    * 刪除時務必一起刪
    */
    p_orders.emplace(_self, [&](auto &o) {
        o.id = _id;
        o.good_id = good_id;
        o.count = 0; // 已賣出數量
        o.buyer = buyer; // 這裡應該叫 owner
    });

    // 写入 shares 表格
    /* id 自動
     * target_sign_id order_id
     * 
    */
    auto _id2 = _shares.available_primary_key();
    _shares.emplace(_self, [&](auto &s) {
        s.id = _id2;
        s.reader = buyer;
        s.target_sign_id = _id;
        s.quota = in.amount * good->fission_factor / 1000;
    });

    /* SEND_INLINE_ACTION(*this, recselling, { _self, "active"_n }, { good->id, buyer, times }); */
}

/**
    透過返利連結的賣貨

    @param buyer 買家
    @param in 付的錢
    @param params 商品编号，order_id
*/
void sign::shareselling(const name &buyer, asset in, const vector<string> &params)
{
    require_auth(buyer);
    check_selling(buyer, in, params);

    uint64_t good_id = string_to_int(params[1]);
    uint64_t order_id = string_to_int(params[2]);
    auto good = _goods.require_find(good_id, "this good is not exist");
    auto order = _orders.require_find(order_id, "this order is not exist");
    const name seller = order->buyer;
    index_order_t p_orders(_self, seller.value);
    auto p_order = p_orders.require_find(order_id, "this order is not exist");
    auto target_id_index_shares = _shares.get_index<"bytargetid"_n>();
    auto share = target_id_index_shares.require_find(order_id, "this share is not exist");

    const int64_t times = in.amount / good->price; // asset / asset
    eosio_assert(times > 0, "You have wrong cost." );
    eosio_assert(times * good->price == in.amount, "You must buy integer number of goods." );

    // 推荐返利
    add_share_income(seller, asset{ static_cast<int64_t>(good->referral_bonus), EOS_SYMBOL});
    // 裂变返利
    // 賣貨數量++
    p_orders.modify(p_order, _self, [&](auto &o) {
        ++o.count;
    });

    // quota 扣除
    int64_t delta = share->quota < in.amount ? share->quota : in.amount;
    target_id_index_shares.modify(share, _self, [&](auto &s) {
        s.quota -= delta;
    });

    // 返利++
    add_share_income(seller, asset{ static_cast<int64_t>(good->fission_bonus), EOS_SYMBOL});
}

// 為什麼用 asset ，因為 asset 內含 overflow 檢查機制
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
    执行并删除订单

    @param id 订单编号
*/
void sign::rmorder(const uint64_t id)
{
    // require_auth(_self);
    auto order = _orders.require_find(id, "thiss order is not exist");
    auto good = _goods.require_find(order->good_id, "this good is not exist");
    index_order_t p_orders(_self, order->buyer.value);
    auto p_order = p_orders.require_find(id, "thiss p_order is not exist");

    // 給 referrer
    add_share_income( order->refer, asset{ static_cast<int64_t>(order->count * good->referral_bonus), EOS_SYMBOL});

    // 最後，刪訂單
    _orders.erase(order);
    p_orders.erase(p_order); // 照目前的邏輯，得一起刪
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
        share(from, in, params);
        return;
    }
    if (params[0] == "buy")
    {
        selling(from, in, params);
        return;
    }
    if (params[0] == "superbuy")
    {
        superselling(from, in, params);
        return;
    }
    if (params[0] == "sharebuy")
    {
        shareselling(from, in, params);
        return;
    }
}