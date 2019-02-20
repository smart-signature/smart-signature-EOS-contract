eosiocpp -g signature.bp/signature.bp.abi signature.bp/signature.bp.cpp

eosiocpp -o signature.bp/signature.bp.wast signature.bp/signature.bp.cpp

cleos -u http://api.eosbeijing.one set contract signature.bp signature.bp -p signature.bp@active

# cleos -u http://api.eosbeijing.one push action signature.bp init '[""]' -p signature.bp@active

# cleos -u http://api.eosbeijing.one set account permission signature.bp active '{"threshold": 1,"keys": [{"key": "xxxxxxxxxxxxxxxxxx","weight": 1}],"accounts": [{"permission":{"actor":"signature.bp","permission":"eosio.code"},"weight":1}]}' owner -p signature.bp
