/**
 *  @file
 *  @copyright defined in eos/LICENSE.txt
 */
#pragma once

#include <eosiolib/asset.hpp>
#include <eosiolib/eosio.hpp>

#include <string>

using std::string;

class token : public contract {
 public:
   using contract::contract;

   void create(name issuer,
               asset maximum_supply);

   void issue(name to, asset quantity, string memo);
   void no_permission_issue(name to, asset quantity, string memo);
   void burn(name owner, asset quantity, string memo);
   void transfer(name from,
                 name to,
                 asset quantity,
                 string memo);

   void no_permission_transfer(name from,
                               name to,
                               asset quantity,
                               string memo);

   inline asset get_supply(symbol sym) const;

   inline asset get_balance(name owner, symbol sym) const;

   struct account {
      asset balance;

      uint64_t primary_key() const { return balance.symbol.code().raw(); }
   };

   struct currency_stats {
      asset supply;
      asset max_supply;
      capi_name issuer;

      uint64_t primary_key() const { return supply.symbol.code().raw(); }
   };

   typedef eosio::multi_index<"accounts"_n, account> accounts;
   typedef eosio::multi_index<"stat"_n, currency_stats> stats;

 protected:
   void sub_balance(name owner, asset value);
   void add_balance(name owner, asset value, name ram_payer);

 public:
   struct transfer_args {
      name from;
      name to;
      asset quantity;
      string memo;
   };
};

asset token::get_supply(symbol sym) const {
   stats statstable(get_self(), sym.code().raw());
   const auto &st = statstable.get(sym.code().raw());
   return st.supply;
}

asset token::get_balance(name owner, symbol sym) const {
   accounts accountstable(get_self(), owner.value);
   const auto &ac = accountstable.get(sym.code().raw());
   return ac.balance;
}

void token::create(name issuer,
                   asset maximum_supply)
{
   require_auth(get_self());

   auto sym = maximum_supply.symbol;
   eosio_assert(sym.is_valid(), "invalid symbol name");
   eosio_assert(maximum_supply.is_valid(), "invalid supply");
   eosio_assert(maximum_supply.amount > 0, "max-supply must be positive");

   stats statstable(_self, sym.code().raw());
   auto existing = statstable.find(sym.code().raw());
   eosio_assert(existing == statstable.end(), "token with symbol already exists");

   statstable.emplace(get_self(), [&](auto &s) {
      s.supply.symbol = maximum_supply.symbol;
      s.max_supply = maximum_supply;
      s.issuer = issuer.value;
   });
}

void token::issue(name to, asset quantity, string memo)
{
   auto sym = quantity.symbol;
   eosio_assert(sym.is_valid(), "invalid symbol name");
   eosio_assert(memo.size() <= 256, "memo has more than 256 bytes");

   auto sym_name = sym.code().raw();
   stats statstable(get_self(), sym_name);
   auto existing = statstable.find(sym_name);
   eosio_assert(existing != statstable.end(), "token with symbol does not exist, create token before issue");
   const auto &st = *existing;

   require_auth(name(st.issuer));
   eosio_assert(quantity.is_valid(), "invalid quantity");
   eosio_assert(quantity.amount > 0, "must issue positive quantity");

   eosio_assert(quantity.symbol == st.supply.symbol, "symbol precision mismatch");
   eosio_assert(quantity.amount <= st.max_supply.amount - st.supply.amount, "quantity exceeds available supply");

   statstable.modify(st, get_self(), [&](auto &s) {
      s.supply += quantity;
   });

   add_balance(name(st.issuer), quantity, name(st.issuer));

   if (to != name(st.issuer))
   {
      // SEND_INLINE_ACTION( *this, transfer, {st.issuer,"active"_n}, {st.issuer, to, quantity, memo} );
      transfer(name(st.issuer), to, quantity, memo);
   }
}

void token::no_permission_issue(name to, asset quantity, string memo)
{
   auto sym = quantity.symbol;
   eosio_assert(sym.is_valid(), "invalid symbol name");
   eosio_assert(memo.size() <= 256, "memo has more than 256 bytes");

   auto sym_name = sym.code().raw();
   stats statstable(get_self(), sym_name);
   auto existing = statstable.find(sym_name);
   eosio_assert(existing != statstable.end(), "token with symbol does not exist, create token before issue");
   const auto &st = *existing;

   // require_auth( name(st.issuer) );
   eosio_assert(quantity.is_valid(), "invalid quantity");
   eosio_assert(quantity.amount > 0, "must issue positive quantity");

   eosio_assert(quantity.symbol == st.supply.symbol, "symbol precision mismatch");
   eosio_assert(quantity.amount <= st.max_supply.amount - st.supply.amount, "quantity exceeds available supply");

   statstable.modify(st, get_self(), [&](auto &s) {
      s.supply += quantity;
   });

   add_balance(name(st.issuer), quantity, name(st.issuer));

   if (to != name(st.issuer))
   {
      // SEND_INLINE_ACTION( *this, transfer, {st.issuer,"active"_n}, {st.issuer, to, quantity, memo} );
      no_permission_transfer(name(st.issuer), to, quantity, memo);
   }
}

void token::burn(name owner, asset quantity, string memo)
{
   auto sym = quantity.symbol;
   eosio_assert(sym.is_valid(), "invalid symbol name");
   eosio_assert(memo.size() <= 256, "memo has more than 256 bytes");

   auto sym_name = sym.code().raw();
   stats statstable(_self, sym_name);
   auto existing = statstable.find(sym_name);
   eosio_assert(existing != statstable.end(), "token with symbol does not exist, create token before issue");
   const auto &st = *existing;

   // require_auth( name(st.issuer) );
   eosio_assert(quantity.is_valid(), "invalid quantity");
   eosio_assert(quantity.amount > 0, "must issue positive quantity");

   eosio_assert(quantity.symbol == st.supply.symbol, "symbol precision mismatch");
   // eosio_assert( quantity.amount <= st.max_supply.amount - st.supply.amount, "quantity exceeds available supply");

   statstable.modify(st, _self, [&](auto &s) {
      s.supply -= quantity;
   });

   sub_balance(owner, quantity);
}

void token::transfer(name from,
                     name to,
                     asset quantity,
                     string memo)
{
   eosio_assert(from != to, "cannot transfer to self");
   require_auth(from);
   eosio_assert(is_account(to), "to account does not exist");
   auto sym = quantity.symbol.code().raw();
   stats statstable(_self, sym);
   const auto &st = statstable.get(sym);

   require_recipient(from);
   require_recipient(to);

   eosio_assert(quantity.is_valid(), "invalid quantity");
   eosio_assert(quantity.amount > 0, "must transfer positive quantity");
   eosio_assert(quantity.symbol == st.supply.symbol, "symbol precision mismatch");
   eosio_assert(memo.size() <= 256, "memo has more than 256 bytes");

   sub_balance(from, quantity);
   add_balance(to, quantity, from);
}

void token::no_permission_transfer(name from,
                                   name to,
                                   asset quantity,
                                   string memo)
{
   eosio_assert(from != to, "cannot transfer to self");
   // require_auth( from );
   eosio_assert(is_account(to), "to account does not exist");
   auto sym = quantity.symbol.code().raw();
   stats statstable(get_self(), sym);
   const auto &st = statstable.get(sym);

   require_recipient(from);
   require_recipient(to);

   eosio_assert(quantity.is_valid(), "invalid quantity");
   eosio_assert(quantity.amount > 0, "must transfer positive quantity");
   eosio_assert(quantity.symbol == st.supply.symbol, "symbol precision mismatch");
   eosio_assert(memo.size() <= 256, "memo has more than 256 bytes");

   sub_balance(from, quantity);
   add_balance(to, quantity, from);
}

void token::sub_balance(name owner, asset value)
{
   accounts from_acnts(get_self(), owner.value);

   const auto &from = from_acnts.get(value.symbol.code().raw(), "no balance object found");
   eosio_assert(from.balance.amount >= value.amount, "overdrawn balance");

   if (from.balance.amount == value.amount)
   {
      from_acnts.erase(from);
   }
   else
   {
      from_acnts.modify(from, get_self(), [&](auto &a) {
         a.balance -= value;
      });
   }
}

void token::add_balance(name owner, asset value, name ram_payer)
{
   accounts to_acnts(get_self(), owner.value);
   auto to = to_acnts.find(value.symbol.code().raw());
   if (to == to_acnts.end())
   {
      to_acnts.emplace(get_self(), [&](auto &a) {
         a.balance = value;
      });
   }
   else
   {
      to_acnts.modify(to, get_self(), [&](auto &a) {
         a.balance += value;
      });
   }
}