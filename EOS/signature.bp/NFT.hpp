/**
 *  @dev minakokojima, yukiexe
 *  @copyright Andoromeda
 */
#pragma once
#include <eosiolib/eosio.hpp>
#include <eosiolib/asset.hpp>
#include <eosiolib/singleton.hpp>
#include <eosiolib/transaction.hpp>
 
typedef double real_type;
typedef uint8_t card ;

using std::string;
using eosio::asset;
using eosio::permission_level;
using eosio::action;

namespace NFT {
    
    using std::string;
    using std::vector;
    using namespace eosio;

    void create(name from) {
    }

    void transfer(name from, name to) {
    }

    void buy(name from) {
        
    }

    struct token {
        uint64_t     id;
        name owner;
        uint64_t primary_key()const { return id; }
    };    

    struct tradeable_token : public token {
        uint64_t price;        
    };
};