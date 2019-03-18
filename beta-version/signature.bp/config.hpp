#ifndef _CONFIG_HPP

namespace config {

// basic
const auto EOS_SYMBOL = eosio::symbol("EOS", 4);
const auto EOS_CONTRACT = "eosio.token"_n;

// token's setting
// const auto SST_SYMBOL = eosio::symbol("SST", 4) ;
// const auto SST_CONTRACT = "dacincubator"_n ;
// constexpr auto SST_MAX_SUPPLY = 300000000000;

// const auto TOKEN_SYMBOL =  ;
// const auto TOKEN_CONTRACT =  ;

// const auto DIVIDEND_EX_SYMBOL = eosio::extended_symbol( EOS_SYMBOL, EOS_CONTRACT ) ;

// constexpr double_t TRADE_FEE = 0.0175 ; // 1.75%
// constexpr double_t TRADE_COEF = 1 - TRADE_FEE ;

// constexpr int64_t MINING_COEF = 10 ;

constexpr uint64_t PRICE_SCALE = 100000000;

// static constexpr time refund_delay = 3*24*3600;

} // namespace config

#endif