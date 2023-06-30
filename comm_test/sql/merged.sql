--
-- 01 user_t
--
CREATE TABLE user_t(
       user_id         integer,                            -- ユーザID
       user_phone      integer            NOT NULL,        -- 電話番号
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
       user_aff        integer            NOT NULL,        -- ユーザ所属
       PRIMARY KEY (user_id)
);

--
-- 04 store_t
--
CREATE TABLE store_t(
       store_id		integer,                           -- 店舗ID
       store_name           varchar(30)          NOT NULL,     -- 店舗名
       region			integer	       NOT NULL,     -- 地域
       PRIMARY KEY (store_id)
);

--
-- store_tabel_t
--
CREATE TABLE store_region_t(		--store_region_tではなく，store_table_t
       store_id	       integer              NOT NULL,
       desk_num		integer              NOT NULL,     -- 卓番号
       desk_max		integer              NOT NULL,     -- 卓上限人数
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
	account_id	       integer              NOT NULL,    -- アカウントID
       PRIMARY KEY (store_id, desk_num, order_time),
       FOREIGN KEY (store_id)
         REFERENCES store_t (store_id)
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
       account_id	       integer	       NOT NULL,    -- アカウントID
       PRIMARY KEY (menu_id, order_time)
);

--
-- 07 recipe_t
--
CREATE TABLE recipe_t(
       menu_id              integer       NOT NULL,        -- 商品ID
       menu_name            varchar(50)   NOT NULL,	   -- 商品名
       recipe 	       text          NOT NULL,	   -- レシピ
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
       account_id           integer       NOT NULL,         -- 責任者
       PRIMARY KEY (menu_id)
);

--
-- 09 push_t
--
CREATE TABLE push_t(
       menu_id              integer       NOT NULL,         -- 商品ID
       push                 integer       NOT NULL,         -- 押し
       layer                integer       NOT NULL,         -- メニューレベル
       PRIMARY KEY (menu_id)
);

--
-- 11 menu_storage_t
--
CREATE TABLE menu_storage_t(
       menu_id		integer       NOT NULL,    -- 商品ID
       storage   	       integer       NOT NULL,    -- 在庫個数
       min_storage   	integer       NOT NULL,    -- 在庫下限
       PRIMARY KEY (menu_id)
);

--
-- 13 reserve_t
--
CREATE TABLE reserve_t(
       account_id		integer	NOT NULL,
       number_of_people	integer	NOT NULL,
       reserve_date		date	       NOT NULL,
       reserve_time		time	       NOT NULL,
       store_id		integer	NOT NULL,
       desk_num		integer	NOT NULL,
       desk_max		integer	NOT NULL,
       PRIMARY KEY (account_id, reserve_date),
       FOREIGN KEY (store_id)
         REFERENCES store_t (store_id)
);

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
