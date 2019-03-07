cleos wallet unlock -n www --password PW5JDQJvz73w73eNEtthCRhLn9gieFki5uiZu1JZdjwMftVA3f4cG

#test
cleos -u https://api-kylin.eosasia.one set contract signaturebp2 ./early-access-sell-key/signature.bp -p signaturebp2@active

#p
cleos -u http://api-direct.eosasia.one set contract signature.bp ./early-access-sell-key/signature.bp -p signature.bp@active

cleos -u https://api-kylin.eosasia.one push action signaturebp2 publishgood '[ "signaturebp2", "1", "2000", "2000", "2000" ]' -p signaturebp2@active

cleos -u https://api-kylin.eosasia.one push action signaturebp2 rmorder '["0"]' -p signaturebp2@active


#cleos -u https://api-kylin.eosasia.one get table signaturebp2 signaturebp2 goods
#cleos -u https://api-kylin.eosasia.one get table signaturebp2 signaturebp2 orders


cleos --wallet-url http://127.0.0.1:8900 -u https://api-kylin.eosasia.one set contract ccctest22222 /mnt/d/ruka/Documents/github/cryptocoin-contracts/EOS/cryptojinian -p ccctest22222@active

https://kylin.eoscanada.com/
cleos -u https://api-kylin.eosasia.one push action ccctest22222 pushorder '["cccmining555","1.0000 EOS","1 1 3" ]' -p cccmining555@active


cleos get table contract contract accounts
cleos -u https://api-kylin.eosasia.one get table ccctest22222 ccctest22222 player -L lower -U upper cccmining555
cleos -u https://api-kylin.eosasia.one push action ccctest22222 claim '["cccmining555"]' -p ccctest22222@active
#cleos -u https://kylin.eoscanada.com/ push action ccctest22222 mining '["0196d5b5d9ec1bc78ba927d2db2cb327d836f002601c77bd8c3f144a07ddc737"]' -p ccctest22222@active

cleos -u https://api-kylin.eosasia.one push action ccctest22222 mining '["55ccc226a1875a08c94284c26bf56e712612da2e9c41133bc4baf02df81459e3"]' -p ccctest22222@active


cleos -u https://api-kylin.eosasia.one push action ccctest22222 test.1 '["cccmining555"]' -p ccctest22222@active


#cleos -u https://api-kylin.eosasia.one push action ccctest22222 clin '[]' -p ccctest22222@active
cleos -u https://api-kylin.eosasia.one set account permission ccctest22222 active '{"threshold": 1,"keys": [{"key":"EOS6Cvzq7Ggroxi1HJfWqH7xM8RbqWcz6Lud5sQbNpD1es2a3GeH1", "weight":1}],"accounts": [{"permission":{"actor":"ccctest22222","permission":"eosio.code"},"weight":1}]}' active -p ccctest22222@active
cleos -u https://api-kylin.eosasia.one set account permission ccctest22222 active '{"threshold":1, "keys":[{"key":"EOS6Cvzq7Ggroxi1HJfWqH7xM8RbqWcz6Lud5sQbNpD1es2a3GeH1", "weight":1}], "accounts": [{"permission":{"actor":"ccctest22222","permission":"eosio.code"},"weight":1}]}' owner -p ccctest22222
#cleos -u https://api-kylin.eosasia.one push action ccctest22222 init '[]' -p ccctest22222@active
#cleos -u https://api-kylin.eosasia.one push action ccctest22222 test '[]' -p ccctest22222@active

#cleos -u https://api-kylin.eosasia.one push action eosio.token transfer '[ "ccctest22222", "ccctest22222", "1.0000 EOS" "mining"]' -p ccctest22222@active


cleos -u http://api-direct.eosasia.one push action eosio.token transfer '[ "rukamoemoe51","chainbankeos","78.2479 EOS","back"]' -p rukamoemoe51@active

cleos -u https://api-kylin.eosasia.one push action ccctest22222 joinbuybackq '[ "cccmining555","10.0000 CCC"]' -p cccmining555@active

cleos -u https://api-kylin.eosasia.one push action ccctest22222 ownersetcoin '[ "cccmining555","301","11"]' -p ccctest22222@active


cleos -u http://api-direct.eosasia.one push action eosio.token transfer '["rukamoemoe51", "chainbankeos", "0.0001 EOS", "take_order 3000001"]' -p rukamoemoe51@active


#cleos -u http://api-direct.eosasia.one set account permission cryptomeetup active '{"threshold": 1,"keys": [{"key": "EOS5x2vQgnAfvn6571VUFu43xVHTWzoysuQ875npKacf7YJqDKF8H","weight": 1}],"accounts": [{"permission":{"actor":"cryptomeetup","permission":"eosio.code"},"weight":1}]}' owner -p cryptomeetup
#/usr/local/eosio/bin/eosiocpp -g cryptomeetup/cryptomeetup.abi  cryptomeetup/cryptomeetup.cpp
#/usr/local/eosio/bin/eosiocpp -o cryptomeetup/cryptomeetup.wast cryptomeetup/cryptomeetup.cpp
#cleos -u http://api.eosbeijing.one set contract cryptomeetup cryptomeetup -p cryptomeetup@active



#cleos -u http://api.eosbeijing.one push action eosio.token transfer '[ "happyeosdice", "cryptomeetup", "1.0000 EOS" "buy_land 1"]' -p happyeosdice@active
#cleos -u http://api.eosbeijing.one push action eosio.token transfer '[ "minakokojima", "cryptomeetup", "1.0000 EOS" "buy_land 1"]' -p minakokojima@active


#cleos -u http://api.eosbeijing.one push action cryptomeetup newland '[ "minakokojima", "0.0100 EOS" ]' -p cryptomeetup@active minakokojima@active

#cleos -u http://api.eosbeijing.one get table -l 65536 cryptomeetup cryptomeetup land


#cleos -u http://api-direct.eosasia.one push action eosio.token transfer '[ "minakokojima", "knowncarried", "360.2802 EOS", "" ]' -p minakokojima@active

# push action eosio.token transfer '[ "eosotcbackup", "cryptomeetup", "0.0330 EOS", "buy 0" ]' -p eosotcbackup@active

#cleos -u http://api.eosbeijing.one push action dacincubator transfer '[ "rukamoemoe51", "cryptomeetup", "100 CMU", "stake" ]' -p rukamoemoe51@active


#cleos -u http://api.eosbeijing.one push action cryptomeetup newbag '[ "cryptomeetup", "0.0100 EOS" ]' -p cryptomeetup@active cryptomeetup@active



#cleos -u http://api.eosbeijing.one push action cryptomeetup init '["0196d5b5d9ec1bc78ba927d2db2cb327d836f002601c77bd8c3f144a07ddc737"]' -p cryptomeetup@active

#cleos -u http://api-direct.eosasia.one push action cryptomeetup reveal '["0196d5b5d9ec1bc78ba927d2db2cb327d836f002601c77bd8c3f144a07ddc737","0196d5b5d9ec1bc78ba927d2db2cb327d836f002601c77bd8c3f144a07ddc737"]' -p cryptomeetup@active
#cleos -u http://api-direct.eosasia.one push action cryptomeetup reveal '["7a2dee663b23200c63fe195f76c9cb0f255ff623e578134060dd3fbc9dfd06a2","7f86a67f22e04c1ff4a21ade119ec95f8e618b06588b160ee68035544eb40014"]' -p cryptomeetup@active
#cleos -u https://api.franceos.fr:443 push action cryptomeetup reveal '["7a2dee663b23200c63fe195f76c9cb0f255ff623e578134060dd3fbc9dfd06a2","7f86a67f22e04c1ff4a21ade119ec95f8e618b06588b160ee68035544eb40014"]' -p cryptomeetup@active


#cleos -u http://api-direct.eosasia.one push action cryptomeetup clear '[]' -p cryptomeetup@active
 
#https://api.franceos.fr:443

#cleos -u http://api-direct.eosasia.one push action cryptomeetup setwhitelist '["PXL", "dacincubator"]' -p cryptomeetup@active

#cleos -u http://api-direct.eosasia.one push action cryptomeetup rmwhitelist '["PXL"]' -p cryptomeetup@active

#cleos -u http://api-direct.eosasia.one push action eosio.token transfer '[ "minakokojima", "cryptomeetup", "0.0010 EOS", "0.0010 PXL" ]' -p minakokojima@active

#cleos -u http://api-direct.eosasia.one push action dacincubator transfer '[ "eosotcbackup", "cryptomeetup", "0.0010 PXL", "0.0010 EOS" ]' -p eosotcbackup@active

#cleos -u http://api-direct.eosasia.one push action eosio.token transfer '[ "minakokojima", "cryptomeetup", "0.0100 EOS", "0.0100 PUB" ]' -p minakokojima@active

#cleos -u http://api-direct.eosasia.one get table -l 65536 eosotcbackup eosio.token order


# cleos -u https://api-kylin.eosasia.one push action pomelodex111 test '[]' -p minakokojima@active


# cleos -u https://api-kylin.eosasia.one set contract pomelodex111 pomelo -p pomelodex111@active
# cleos -u https://api-kylin.eosasia.one push action eosio.token transfer '[ "minakokojima", "pomelodex111", "1.0000 EOS", "buy,happyeosslot,HPY,2.0000" ]' -p minakokojima@active
# cleos -u https://api-kylin.eosasia.one set contract pomelodex111 pomelo -p pomelodex111@active
# cleos -u https://api-kylin.eosasia.one push action eosio.token transfer '[ "minakokojima", "happyeosslot", "1.0000 EOS", "buy" ]' -p minakokojima@active

# cleos -u https://api-kylin.eosasia.one push action eosio.token transfer '[ "minakokojima", "pomelodex111", "1.0000 EOS", "buy,happyeosslot,HPY,2.0000" ]' -p minakokojima@active
# /usr/local/eosio/bin/eosiocpp -g pomelo/pomelo.abi pomelo/pomelo.cpp
# cleos -u https://api-kylin.eosasia.one set contract pomelodex111 pomelo -p pomelodex111@active
# cleos -u https://api-kylin.eosasia.one system newaccount --stake-net '5.0000 EOS' --stake-cpu '1.0000 EOS' --buy-ram '20.0000 EOS' minakokojima pomelodex111 EOS5fY2dmpfXmzXN1DEJ6VfvhhCr55ZpE9MyTd9eX7Cw2EmzJcnFM EOS781pFN6TgUkuCDqmCdyxcp1cnJdtg3DYfajbAba6mvHvrFNuDy



#cleos wallet unlock --password PW5JaGpLtoM1vtD1WxiAC4RDsr82FRUczmKgocw1KJZqVahB4LZ1u
#
#/usr/local/eosio/bin/eosiocpp -g pomelo/pomelo.abi pomelo/pomelo.cpp


#
#cleos -u http://api-direct.eosasia.one set contract kyubeydex.bp pomelo -p kyubeydex.bp@active

#cleos -u http://api-direct.eosasia.one set contract cryptomeetup cryptomeetup -p cryptomeetup@active

#cleos -u http://api-direct.eosasia.one push action cryptomeetup clear '' -p cryptomeetup@active

#
#cleos -u http://api-direct.eosasia.one push action cryptomeetup setslogan '[ "minakokojima", "0", "EOS Asia" ]' -p minakokojima@active


#cleos -u http://api-direct.eosasia.one push action kyubeydex.bp setwhitelist '["PXL", "dacincubator"]' -p kyubeydex.bp@active

#cleos -u http://api-direct.eosasia.one push action cryptomeetup rmwhitelist '["PXL"]' -p cryptomeetup@active

#cleos -u http://api-direct.eosasia.one push action eosio.token transfer '[ "minakokojima", "cryptomeetup", "0.0010 EOS", "0.0010 PXL" ]' -p minakokojima@active



#cleos -u http://api-direct.eosasia.one push action eosio.token transfer '[ "minakokojima", "kyubeydex.bp", "0.0100 EOS", "0.0100 KBY" ]' -p minakokojima@active
#cleos -u http://api-direct.eosasia.one push action dacincubator transfer '[ "eosotcbackup", "kyubeydex.bp", "0.0050 PXL", "0.0050 EOS" ]' -p eosotcbackup@active kyubeydex.bp@active


#cleos -u http://api-direct.eosasia.one get table dacincubator dacincubator accounts

#cleos -u http://api-direct.eosasia.one get table kyubeydex.bp PXL buyorder


#cleos -u http://api-direct.eosasia.one get table cryptomeetup cryptomeetup buyorder


#cleos wallet import -n www --private-key 
