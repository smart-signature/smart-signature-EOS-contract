cleos wallet unlock -n www --password PW5JDQJvz73w73eNEtthCRhLn9gieFki5uiZu1JZdjwMftVA3f4cG

#cleos wallet import -n www --private-key 

#test
cleos -u https://api-kylin.eosasia.one set contract signaturebp2 ./beta-version/signature.bp -p signaturebp2@active

cleos -u https://api-kylin.eosasia.one push action signaturebp2 publishgood '[ "signaturebp2", "1", "2000", "2000", "2000" ]' -p signaturebp2@active

cleos -u https://api-kylin.eosasia.one get table signaturebp2 signaturebp2 goods
cleos -u https://api-kylin.eosasia.one get table signaturebp2 signaturebp2 orders
cleos -u https://api-kylin.eosasia.one get table signaturebp2 signaturebp2 signs
cleos -u https://api-kylin.eosasia.one get table signaturebp2 signaturebp2 shares
cleos -u https://api-kylin.eosasia.one get table signaturebp2 signaturebp2 subscribes

cleos -u https://api-kylin.eosasia.one push action signaturebp2 clean '["orders"]' -p signaturebp2@active
cleos -u https://api-kylin.eosasia.one push action signaturebp2 clean '["signs"]' -p signaturebp2@active
cleos -u https://api-kylin.eosasia.one push action signaturebp2 clean '["goods"]' -p signaturebp2@active
cleos -u https://api-kylin.eosasia.one push action signaturebp2 clean '["shares"]' -p signaturebp2@active
cleos -u https://api-kylin.eosasia.one push action signaturebp2 clean '["subscribes"]' -p signaturebp2@active