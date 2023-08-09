/**
 * table.h
 **/

#ifndef _INCLUDE_TABLE_FUNC_
#define _INCLUDE_TABLE_FUNC_

#define E_CODE_1001 1001    //test_func.cにおいて引数不正

#define E_CODE_1601 1601 //入力エラー（入力された番号が0か1以外である）

#define E_CODE_1602 1602 //入力エラー（入力された番号が0または100以上である）
#define E_CODE_1603 1603 //登録エラー（入力された番号が既に登録されている）
#define E_CODE_1604 1604 //入力エラー（入力された卓番号が存在しない）

#define E_CODE_1605 1605 //入力エラー（入力された番号が0または100以上である）
#define E_CODE_1606 1606 //入力エラー（入力された卓番号がデータベースに存在しない）
#define E_CODE_1607 1607 //登録エラー（入力された番号が既に削除されている）
#define E_CODE_1608 1608 //登録エラー（order_tに注文情報が残っている）
#define E_CODE_1609 1609 //データベースエラー（データベースのアップデートができませんでした）
#define E_CODE_1610 1610 //クエリエラー（order_tから注文情報を取得できなかった）

#endif