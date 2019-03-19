cleos wallet unlock -n www --password PW5JDQJvz73w73eNEtthCRhLn9gieFki5uiZu1JZdjwMftVA3f4cG

cleos -u http://api.eosnewyork.io:80 get info
#production
cleos -u http://api.eosnewyork.io:80 set contract signature.bp ./beta-version/signature.bp -p signature.bp@active

cleos -u http://api-direct.eosasia.one push action signature.bp publish '{"sign":{"id":0, "author":"rukamoemoe51", "fission_factor":"2000", "ipfs_hash":"", "public_key":"EOS5P9HXdVTcAVMph4ZppDKBMkBuT6ihnkLqTUrVFBtGR94cPjykJ", "signature":"SIG_K1_KZU9PyXP8YAePjCfCcmBjGHARkvTVDjKpKvVgS6XL8o2FXTXUdhP3rqrL38dJYgJo2WNBdYubsY9LKTo47RUUE4N3ZHjZQ"}}' -p rukamoemoe51@active

cleos -u http://api.eosnewyork.io:80 push action signature.bp publish '{"sign":{"id":0, "author":"rukamoemoe51", "fission_factor":"2000", "ipfs_hash":"", "public_key":"EOS5P9HXdVTcAVMph4ZppDKBMkBuT6ihnkLqTUrVFBtGR94cPjykJ", "signature":"SIG_K1_KZU9PyXP8YAePjCfCcmBjGHARkvTVDjKpKvVgS6XL8o2FXTXUdhP3rqrL38dJYgJo2WNBdYubsY9LKTo47RUUE4N3ZHjZQ"}}' -p rukamoemoe51@active

cleos -u http://api-direct.eosasia.one push action signature.bp publish '{"sign":{"id":0, "author":"rukamoemoe51", "fission_factor":2000, "ipfs_hash":"QmfJsZmbsFcaNEBejP6HcXQEXycVXKfFwbMM3eju4VdsN3", "public_key":"EOS5P9HXdVTcAVMph4ZppDKBMkBuT6ihnkLqTUrVFBtGR94cPjykJ", "signature":"SIG_K1_KZU9PyXP8YAePjCfCcmBjGHARkvTVDjKpKvVgS6XL8o2FXTXUdhP3rqrL38dJYgJo2WNBdYubsY9LKTo47RUUE4N3ZHjZQ"}}' -p rukamoemoe51@active


cleos -u http://api-direct.eosasia.one push action signature.bp rmorder '["0"]' -p rukamoemoe51@active
cleos -u http://api-direct.eosasia.one push action signature.bp clean '["signs"]' -p signature.bp@active
cleos -u http://api-direct.eosasia.one push action signature.bp clean '["goods"]' -p signature.bp@active
cleos -u http://api-direct.eosasia.one push action signature.bp clean '["shares"]' -p signature.bp@active
#test
cleos -u https://api.eosnewyork.io:80 push action signature.bp testclaim '["rukamoemoe51"]' -p rukamoemoe51@active

cleos -u https://api.eosnewyork.io push action eosio.token transfer '[ "rukamoemoe51", "signature.bp", "0.1000 EOS", "billtest"]' -p rukamoemoe51@active
cleos -u https://api.eosnewyork.io push action eosio.token transfer '[ "megumimegumi", "signature.bp", "0.1000 EOS", "billtest"]' -p megumimegumi@active

# look up table
cleos -u http://api-direct.eosasia.one get table signature.bp signature.bp signs
cleos -u http://api-direct.eosasia.one get table signature.bp rukamoemoe51 players

cleos -u http://api-direct.eosasia.one get table signature.bp signature.bp goods
cleos -u http://api-direct.eosasia.one get table signature.bp signature.bp orders
cleos -u http://api-direct.eosasia.one get table signature.bp signature.bp shares
