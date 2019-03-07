cleos wallet unlock -n www --password PW5JDQJvz73w73eNEtthCRhLn9gieFki5uiZu1JZdjwMftVA3f4cG

#production
cleos -u http://api-direct.eosasia.one set contract signature.bp ./early-access-sell-key-minakokojima/signature.bp -p signature.bp@active

cleos -u http://api-direct.eosasia.one push action signature.bp rmorder '["0"]' -p rukamoemoe51@active

cleos -u http://api-direct.eosasia.one get table signature.bp signature.bp goods
cleos -u http://api-direct.eosasia.one get table signature.bp signature.bp orders
cleos -u http://api-direct.eosasia.one get table signature.bp rukamoemoe51 players
