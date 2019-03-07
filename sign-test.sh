cleos wallet unlock -n www --password PW5JDQJvz73w73eNEtthCRhLn9gieFki5uiZu1JZdjwMftVA3f4cG

#cleos wallet import -n www --private-key 

#test
cleos -u https://api-kylin.eosasia.one set contract signaturebp2 ./early-access-sell-key/signature.bp -p signaturebp2@active

cleos -u https://api-kylin.eosasia.one push action signaturebp2 publishgood '[ "signaturebp2", "1", "2000", "2000", "2000" ]' -p signaturebp2@active

cleos -u https://api-kylin.eosasia.one get table signaturebp2 signaturebp2 goods
cleos -u https://api-kylin.eosasia.one get table signaturebp2 signaturebp2 orders