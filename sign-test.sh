cleos wallet unlock -n www --password PW5JDQJvz73w73eNEtthCRhLn9gieFki5uiZu1JZdjwMftVA3f4cG

#cleos wallet import -n www --private-key 

cleos -u https://api-kylin.eosasia.one set account permission signaturebp2 active '{"threshold": 1,"keys": [{"key":"EOS5KjTfz42chCRcGRe7BjnrM4PEyN5kkRH44a5StBs8md3bSGP6X", "weight":1}],"accounts": [{"permission":{"actor":"signaturebp2","permission":"eosio.code"},"weight":1}]}' owner -p signaturebp2@active     


#test
cleos -u https://api-kylin.eosasia.one set contract signaturebp2 ./beta-version/signature.bp -p signaturebp2@active

cleos -u https://api-kylin.eosasia.one push action signaturebp2 publishgood '[ "signaturebp2", "1", "2000", "2000", "2000" ]' -p signaturebp2@active

cleos -u https://api-kylin.eosasia.one push action eosio.token transfer '[ "cccmining555", "signaturebp2", "0.1000 EOS", "billtest 0"]' -p cccmining555@active

# tables
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