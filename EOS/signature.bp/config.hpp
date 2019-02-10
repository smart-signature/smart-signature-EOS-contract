#ifndef _CONFIG_HPP
#include <eosiolib/asset.hpp>
#include "utils.hpp"

#define TOKEN_CONTRACT N(dacincubator)

namespace config {

constexpr auto CCC_MAX_SUPPLY = 300000000000;

const auto EOS_SYMBOL = eosio::symbol("EOS", 4);
const auto EOS_CONTRACT = "eosio.token"_n;

const auto SST_SYMBOL = eosio::symbol("SST", 4);
// const auto CCC_CONTRACT = get_self();

const auto TOKEN_SYMBOL = SST_SYMBOL ;
// const auto TOKEN_CONTRACT =

// const auto DIVIDEND_EX_SYMBOL = eosio::extended_symbol( EOS_SYMBOL, EOS_CONTRACT ) ;


// constexpr double_t TRADE_FEE = 0.0175 ; // 1.75%
// constexpr double_t TRADE_COEF = 1 - TRADE_FEE ;

// constexpr int64_t MINING_COEF = 10 ;

constexpr uint64_t PRICE_SCALE = 100000000;

} // namespace config

#endif