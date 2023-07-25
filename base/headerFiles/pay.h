<<<<<<< HEAD
/**
 * pay.h
 **/

#ifndef _INCLUDE_TEST_FUNC_
#define _INCLUDE_TEST_FUNC_

#include <math.h>

#define TEST    "PAY"

#define E_CODE_2301 2301    //テーブルが存在しません
#define E_CODE_2302 2302    //注文が存在しません
#define E_CODE_2303 2303    //まだ提供していない料理がございます。
#define E_CODE_2304 2304    //入力値が不正です。
#define E_CODE_2305 2305    //金額が大きすぎます
#define E_CODE_2306 2306    //お客様から頂戴した金額が合計金額よりも少ないです
#define E_CODE_2307 2307    //お客様から頂戴した金額が合計金額よりも少ないです。


#endif
=======
/*
pay.h
*/

#ifndef _INCLUDE_PAY_FUNC_
#define _INCLUDE_PAY_FUNC_

/*エラーコード*/
#define E_CODE_2301 2301    //データベースエラー（ロールバック失敗）
#define E_CODE_2302 2302    //データベースエラー（SELECT失敗）
#define E_CODE_2303 2303    //入力値エラー（入力されたテーブル番号が存在しない）
#define E_CODE_2304 2304    //エラー（注文が存在しない）
#define E_CODE_2305 2305    //エラー（提供していない商品がある）
#define E_CODE_2306 2306    //入力値エラー（入力された人数が0以下）
#define E_CODE_2307 2307    //入力値エラー（入力された値がy、n以外）
#define E_CODE_2308 2308    //入力値エラー（入力された支払い金額が大きすぎる）
#define E_CODE_2309 2309    //エラー(支払い金額が合計金額よりも少ない)
#define E_CODE_2310 2310    //データベースエラー（データの移行失敗）
#define E_CODE_2311 2311    //エラー（使用しようとしているポイントが保有ポイントを超えている）
#define E_CODE_2312 2312    //データベースエラー（データベースのアップデートに失敗）
>>>>>>> f7982271c9153def53e9b38d26fb7b71ad52b7aa
