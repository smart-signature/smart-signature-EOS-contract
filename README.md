# Introduction
Yet Another EOS Contract

## 開發須知
目前開發主要在 early-access 裡，注意

新來的 contributor ，請開個人 branch 維護，ex: xxx/xxxx

流程圖預計用 md 做一份

測試歡迎使用

https://github.com/yukiexe/eos_contract_test

- 關於 action 中的 require_auth
  建議統一寫在開頭的 inline documentation ，在哪一塊用了哪種，並且寫上最後 checked 的日期和執行人

## 權限管理
cleos -u http://api.eosbeijing.one set account permission signature.bp active '{"threshold": 1,"keys": [{"key": "EOS5P9HXdVTcAVMph4ZppDKBMkBuT6ihnkLqTUrVFBtGR94cPjykJ","weight":1},{"key":"EOS5Xmv3vA9dxjqmduHCnUZNuKboUQqQTmN8DtpJx6gHUbsNWjMTy","weight": 1}],"accounts":[{"permission":{"actor":"signature.bp","permission":"eosio.code"},"weight":1}]}' owner -p signature.bp@owner

## 部署合约
cleos -u http://api.eosbeijing.one set contract signature.bp signature.bp -p signature.bp@active

## 查询表格
cleos -u http://api.eosbeijing.one get table signature.bp signature.bp orders

## 清理表格
cleos -u http://api.eosbeijing.one push action signature.bp clean '[]' -p signature.bp@active

## 參考資料
- [Dapp Rosetta, EOS Contract](https://github.com/Dapp-rosetta/dapp-rosetta-contract/tree/master/Contract/EOS)


