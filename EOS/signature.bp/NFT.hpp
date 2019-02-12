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

using std::string;
using eosio::symbol_name;
using eosio::asset;
using eosio::symbol_type;
using eosio::permission_level;
using eosio::action;

namespace NFT {
    
    void create(name from) {
    }

    void transfer(name from, name to) {
    }

    void buy(name from) {
        
    }

    struct token {
        uint64_t     id;
        name owner = 0;
        uint64_t primary_key()const { return id; }
    };    

    struct tradeable_token : public token {
        uint64_t price;        
    };
};