/**
 *  order.h
 **/

#ifndef _INCLUDE_ORDER_FUNC_
#define _INCLUDE_ORDER_FUNC_

//*** エラーコード ***//
#define E_CODE_1401 1401    //メニューの不存在
#define E_CODE_1402 1402    //チェーン接続無し
#define E_CODE_1403 1403    //地域接続無し

#define E_CODE_1404 1404    //タブ番号エラー

#define E_CODE_1405 1405    //数量超過
#define E_CODE_1406 1406    //同一注文が既に登録済み
#define E_CODE_1407 1407    //タブ内の商品番号でない

#define E_CODE_1408 1408    //注文取消し

#define MENU_OK         10
#define ORDER_END       20
#define ORDER_OK        30
#define QUANTITY_ERR    40
#define ALREADY         50

#endif