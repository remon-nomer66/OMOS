BEGIN TRANSACTION;

-- 本部: 0
-- COR: 1~49
-- チェーン: 50~59
-- 店舗: 100~

--
-- 01 user_t
--
CREATE TABLE user_t(
       user_id         integer,                            -- ユーザID
       user_phone      bigint             NOT NULL,        -- 電話番号
       user_name       varchar(100)       NOT NULL,        -- 氏名
       user_pass       varchar(16)        NOT NULL,        -- パスワード
       PRIMARY KEY (user_id)
);

--
-- 02 user_point_t
--
CREATE TABLE user_point_t(
       user_id         integer,                            -- ユーザID
       user_point      integer            DEFAULT 0,       -- ポイント
       user_mag        float(3)           NOT NULL,        -- 倍率
       PRIMARY KEY (user_id)
);

--
-- 03 user_authority_t
--
CREATE TABLE user_authority_t(
       user_id         integer,                            -- ユーザID
       user_authority  integer            DEFAULT 1,       -- 権限
       store_id        integer            DEFAULT 0,       -- ユーザ所属
       PRIMARY KEY (user_id)
);

--
-- 04 store_t
--
CREATE TABLE store_t(
       store_id		integer,                           -- 店舗ID
       store_name           varchar(30)          NOT NULL,     -- 店舗名
       PRIMARY KEY (store_id)
);

--
-- 店舗地域対応関係
--
CREATE TABLE region_t(
       store_id             integer,
       region_id            integer              NOT NULL,
       region_name          varchar(20)          NOT NULL,
       PRIMARY KEY (store_id)
);

--
-- チェーン対応関係!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
--
CREATE TABLE chain_t(
       chain_id             integer              NOT NULL,
       store_id             integer              NOT NULL,
       PRIMARY KEY (chain_id, store_id)
);

--
-- store_tabel_t
--
CREATE TABLE store_table_t(
       store_id	       integer              NOT NULL,
       desk_num		integer              NOT NULL,      -- 卓番号
       desk_max		integer              NOT NULL,      -- 卓上限人数
       desk_use             integer              DEFAULT 0,     -- 卓使用フラグ
       PRIMARY KEY (store_id)
);

--
-- 05 order_t
--
CREATE TABLE order_t(
       store_id    	       integer              NOT NULL,    -- 店舗ID
	desk_num	       integer              NOT NULL,    -- 卓番号
       menu_id     	       integer              NOT NULL,    -- 商品ID 
       order_cnt   	       integer              NOT NULL,    -- 個数
       kitchen_flag  	integer              DEFAULT 0,   -- 確認フラグ
	order_date  	       date                 NOT NULL,    -- 注文日
	order_time  	       time                 NOT NULL,    -- 注文時間
	user_id	       integer              NOT NULL,    -- アカウントID
       PRIMARY KEY (store_id, desk_num, order_time)--,
       --FOREIGN KEY (store_id)
       --  REFERENCES store_t (store_id)
);

--
-- summary_t(総括)
--
CREATE TABLE summary_t(
       store_id    	       integer              NOT NULL,    -- 店舗ID
       menu_id     	       integer              NOT NULL,    -- 商品ID 
       order_cnt   	       integer              NOT NULL,    -- 個数
       order_date	       date	              NOT NULL,    -- 注文日
       order_time	       time	              NOT NULL,    -- 注文時間
       user_id	       integer	       NOT NULL,    -- アカウントID
       PRIMARY KEY (menu_id, order_time)
);

--
-- 07 recipe_t
--
CREATE TABLE recipe_t(
       menu_id              integer       NOT NULL,        -- 商品ID
       menu_name            varchar(50)   NOT NULL,	   -- 商品名
       recipe 	       text          NOT NULL,	   -- レシピ
       f_or_d               integer       NOT NULL,        -- フードかドリンクか
       PRIMARY KEY (menu_id)
);

--
-- 08 menu_price_t
--
CREATE TABLE menu_price_t(
       menu_id              integer       NOT NULL,         -- 商品ID
       price                integer       NOT NULL,         -- 価格
       PRIMARY KEY (menu_id)
);

--
-- 12 menu_charge_t
--
CREATE TABLE menu_charge_t(
       menu_id              integer       NOT NULL,         -- 商品ID
       user_id              integer       NOT NULL,         -- 責任者
       PRIMARY KEY (menu_id)
);

--
-- メニューチェーン，メニュー季節!!!!!!!!!!!!!!!!!!
--
CREATE TABLE menu_detail_t(
       menu_id              integer       NOT NULL,
       layer                integer       NOT NULL,         -- メニューレベル
       id                   integer       NOT NULL,         -- store_id, region_t
       season               integer       DEFAULT 0,        -- 0: 指定無し，1: 春，2: 夏，3: 秋，4: 冬
       PRIMARY KEY(menu_id)
);

--
-- 09 push_t
--
CREATE TABLE push_t(
       menu_id              integer       NOT NULL,         -- 商品ID
       push_hq              integer       NOT NULL,         -- 押しHQ
       push_cor             integer       NOT NULL,         -- 押しCOR
       push_mgr             integer       NOT NULL,         -- 押し店長
       PRIMARY KEY (menu_id)
);

--
-- 11 menu_storage_t
--
CREATE TABLE menu_storage_t(
       menu_id		integer       NOT NULL,    -- 商品ID
       store_id             integer       NOT NULL,    -- 店舗ID
       storage   	       integer       NOT NULL,    -- 在庫個数
       min_storage   	integer       NOT NULL,    -- 在庫下限
       PRIMARY KEY (menu_id)
);

--
-- 13 reserve_t
--
CREATE SCHEMA reserve;
SET search_path to reserve;
CREATE TABLE reserve_t(
       reserve_no           integer       NOT NULL,
       user_id		integer	NOT NULL,
       people_num    	integer	NOT NULL,
       reserve_date		date	       NOT NULL,
       reserve_time		time	       NOT NULL,
       store_id		integer	NOT NULL,
       desk_num		integer	NOT NULL,
       PRIMARY KEY (reserve_no)
       --FOREIGN KEY (store_id)
       --  REFERENCES store_t (store_id)
);
CREATE SEQUENCE reserve_seq MAXVALUE 99999 START 1;
SET search_path to public;

--
-- 14 store_order_t(発注)
--
CREATE TABLE store_order_t(
       store_id           integer,
       menu_id            integer,
       store_order_cnt    integer         NOT NULL,
       store_order_date   date            NOT NULL,
       store_order_time   time            NOT NULL,
       PRIMARY KEY (store_id, store_order_time)
);

COMMIT;