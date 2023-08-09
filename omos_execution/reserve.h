/**
 *  reserve.h
 **/

#ifndef _INCLUDE_RESERVE_FUNC_
#define _INCLUDE_RESERVE_FUNC_

//*** エラーコード ***//
#define E_CODE_1201 1201    //予約変更の対象無し

#define E_CODE_1202 1202    //予約可能数超過

#define E_CODE_1203 1203    //店舗番号に問題
#define E_CODE_1204 1204    //日付入力無し
#define E_CODE_1205 1205    //年エラー(桁エラー)
#define E_CODE_1206 1206    //月エラー(桁エラー)
#define E_CODE_1207 1207    //日エラー(桁エラー)
#define E_CODE_1208 1208    //年エラー(当該時刻以前)
#define E_CODE_1209 1209    //月エラー(当該時刻以前)
#define E_CODE_1210 1210    //日エラー(当該時刻以前)
#define E_CODE_1211 1211    //時エラー(桁エラー)
#define E_CODE_1212 1212    //分エラー(桁エラー)
#define E_CODE_1213 1213    //時エラー(当該時刻以前)
#define E_CODE_1214 1214    //分エラー(当該時刻以前)
#define E_CODE_1215 1215    //分エラー(30分以内)
#define E_CODE_1216 1216    //分エラー(15分刻み)
#define E_CODE_1217 1217    //人数エラー(許容値越え)
#define E_CODE_1218 1218    //予約満杯

#define E_CODE_1219 1219    //削除対象無し

#endif