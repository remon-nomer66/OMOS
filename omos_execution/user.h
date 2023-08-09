/*
user.h
*/

#ifndef _INCLUDE_USER_FUNC_
#define _INCLUDE_USER_FUNC_

/*エラーコード*/
#define E_CODE_1101 1101    //入力値不正エラー（入力された電話番号が不正）
#define E_CODE_1102 1102    //入力値不正エラー（入力されたパスワードが不正）
#define E_CODE_1103 1103    //入力値不正エラー（入力された氏名が３０字を超えている）
#define E_CODE_1104 1104    //データベースエラー（ユーザーIDの取得に失敗）
#define E_CODE_1105 1105    //データベースエラー（user_point_tにuser_id,point,point_rateの登録に失敗）
#define E_CODE_1106 1106    //データベースエラー（user_authority_tにuser_id, user_authorityの登録が失敗）

#define E_CODE_1107 1107    //データベースエラー（user_tにuser_id, user_name, user_passwordの登録が失敗）

#define E_CODE_1108 1108    //入力値エラー（入力された値が0,1,2のいずれでもない）
#define E_CODE_1109 1109    //入力値エラー（入力された電話番号がuser_tテーブルにすでに存在する）
#define E_CODE_1110 1110    //データベースエラー（データベースの内容のアップデートに失敗）
#define E_CODE_1111 1111    //データベースエラー（INSERT失敗）
#define E_CODE_1112 1112    //エラー（IDまたはコマンドの再入力）
#define E_CODE_1113 1113    //エラー（パスワードの再入力）
#define E_CODE_1114 1114    //エラー（パスワードの入力回数上限に到達）
#define E_CODE_1115 1115    //エラー（不正なコマンド）
#define E_CODE_1116 1116    //エラー（IDまたはパスワード,あるいはその両方が間違い）

#endif