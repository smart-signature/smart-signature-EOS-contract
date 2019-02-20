cleos wallet unlock --password PW5JaGpLtoM1vtD1WxiAC4RDsr82FRUczmKgocw1KJZqVahB4LZ1u
/usr/local/eosio/bin/eosiocpp -g signature.bp/signature.bp.abi  signature.bp/signature.bp.cpp
/usr/local/eosio/bin/eosiocpp -o signature.bp/signature.bp.wast signature.bp/signature.bp.cpp
# cleos -u http://api.eosbeijing.one set contract signature.bp signature.bp -p signature.bp@active

# in test net:
# testallthing is the account deployed signature.bp contract. bookwilllook is stand for user.
# cleos -u http://dev.cryptolions.io:38888 push action eosio.token transfer '["bookwilllook","testallthing","0.5000 EOS", "create 20 50 350 1000 1543154069"]' -p bookwilllook@active

# in main net:
# cleos -u http://api.eosbeijing.one push action eosio.token transfer '["xxxxyyyyxxxx","signature.bp","0.5000 EOS", "create 20 50 350 1000 1543154069"]' -p xxxxyyyyxxxx@active
# attenion:
# 1543154069 should change into the unix time of tx. 
