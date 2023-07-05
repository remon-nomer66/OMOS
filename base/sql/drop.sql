BEGIN TRANSACTION;

DROP TABLE store_order_t;
DROP TABLE reserve_t;
DROP TABLE menu_storage_t;
DROP TABLE push_t;
DROP TABLE menu_charge_t;
DROP TABLE menu_price_t;
DROP TABLE recipe_t;
DROP TABLE summary_t;
DROP TABLE order_t;
DROP TABLE store_table_t;
DROP TABLE store_t;
DROP TABLE user_authority_t;
DROP TABLE user_point_t;
DROP TABLE user_t;

COMMIT;