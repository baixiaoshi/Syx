/*
  +----------------------------------------------------------------------+
  | Yet Another Framework                                                |
  +----------------------------------------------------------------------+
  | This source file is subject to version 3.01 of the PHP license,      |
  | that is bundled with this package in the file LICENSE, and is        |
  | available through the world-wide-web at the following url:           |
  | http://www.php.net/license/3_01.txt                                  |
  | If you did not receive a copy of the PHP license and are unable to   |
  | obtain it through the world-wide-web, please send a note to          |
  | license@php.net so we can mail you a copy immediately.               |
  +----------------------------------------------------------------------+
  | Author: Xinchen Hui  <laruence@php.net>                              |
  +----------------------------------------------------------------------+
*/

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "php.h"
#include "Zend/zend_interfaces.h"

#include "php_syx.h"
#include "syx_namespace.h"
#include "syx_exception.h"
#include "syx_config.h"

#include "configs/syx_config_simple.h"

zend_class_entry *syx_config_simple_ce;

#ifdef HAVE_SPL
extern PHPAPI zend_class_entry *spl_ce_Countable;
#endif

/** {{{ ARG_INFO
 */
ZEND_BEGIN_ARG_INFO_EX(syx_config_simple_void_arginfo, 0, 0, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(syx_config_simple_construct_arginfo, 0, 0, 1)
	ZEND_ARG_INFO(0, config_file)
	ZEND_ARG_INFO(0, section)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(syx_config_simple_get_arginfo, 0, 0, 0)
	ZEND_ARG_INFO(0, name)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(syx_config_simple_rget_arginfo, 0, 0, 1)
	ZEND_ARG_INFO(0, name)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(syx_config_simple_set_arginfo, 0, 0, 2)
	ZEND_ARG_INFO(0, name)
	ZEND_ARG_INFO(0, value)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(syx_config_simple_isset_arginfo, 0, 0, 1)
	ZEND_ARG_INFO(0, name)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(syx_config_simple_unset_arginfo, 0, 0, 1)
	ZEND_ARG_INFO(0, name)
ZEND_END_ARG_INFO()
/* }}} */

syx_config_t *syx_config_simple_instance(syx_config_t *this_ptr, zval *values, zval *readonly) /* {{{ */ {
	switch (Z_TYPE_P(values)) {
		case IS_ARRAY:
			if (Z_ISUNDEF_P(this_ptr)) {
				object_init_ex(this_ptr, syx_config_simple_ce);
			}
			zend_update_property(syx_config_simple_ce, this_ptr, ZEND_STRL(SYX_CONFIG_PROPERT_NAME), values);
			if (readonly) {
				convert_to_boolean(readonly);
				zend_update_property_bool(syx_config_simple_ce, this_ptr, ZEND_STRL(SYX_CONFIG_PROPERT_NAME_READONLY),
						Z_TYPE_P(readonly) == IS_TRUE ? 1 : 0);
			}
			return this_ptr;
		default:
			syx_trigger_error(SYX_ERR_TYPE_ERROR, "Invalid parameters provided, must be an array");
			return NULL;
	}
}
/* }}} */

zval *syx_config_simple_format(syx_config_t *instance, zval *pzval, zval *rv) /* {{{ */ {
	zval *readonly, *ret;
	readonly = zend_read_property(syx_config_simple_ce, instance, ZEND_STRL(SYX_CONFIG_PROPERT_NAME_READONLY), 1, NULL);
	ret = syx_config_simple_instance(rv, pzval, readonly);
	return ret;
}
/* }}} */

/** {{{ proto public Syx_Config_Simple::__construct(mixed $array, string $readonly)
*/
PHP_METHOD(syx_config_simple, __construct) {
	zval *values, *readonly = NULL;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "z|z", &values, &readonly) == FAILURE) {
		zval prop;

		array_init(&prop);
		zend_update_property(syx_config_simple_ce, getThis(), ZEND_STRL(SYX_CONFIG_PROPERT_NAME), &prop);
		zval_ptr_dtor(&prop);

		return;
	}

	(void)syx_config_simple_instance(getThis(), values, readonly);
}
/** }}} */

/** {{{ proto public Syx_Config_Simple::get(string $name = NULL)
*/
PHP_METHOD(syx_config_simple, get) {
	zval *ret, *pzval;
	zend_string *name = NULL;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "|S", &name) == FAILURE) {
		return;
	}

	if (!name) {
		RETURN_ZVAL(getThis(), 1, 0);
	} else {
		zval *properties;
		HashTable *hash;
		long lval;
		double dval;

		properties = zend_read_property(syx_config_simple_ce, getThis(), ZEND_STRL(SYX_CONFIG_PROPERT_NAME), 1, NULL);
		hash  = Z_ARRVAL_P(properties);

		if (is_numeric_string(ZSTR_VAL(name), ZSTR_LEN(name), &lval, &dval, 0) != IS_LONG) {
			if ((pzval = zend_hash_find(hash, name)) == NULL) {
				RETURN_FALSE;
			}
		} else {
			if ((pzval = zend_hash_index_find(hash, lval)) == NULL) {
				RETURN_FALSE;
			}
		}

		if (Z_TYPE_P(pzval) == IS_ARRAY) {
			zval rv = {{0}};
			if ((ret = syx_config_simple_format(getThis(), pzval, &rv))) {
				RETURN_ZVAL(ret, 1, 1);
			} else {
				RETURN_NULL();
			}
		} else {
			RETURN_ZVAL(pzval, 1, 0);
		}
	}

	RETURN_FALSE;
}
/* }}} */

/** {{{ proto public Syx_Config_Simple::toArray(void)
*/
PHP_METHOD(syx_config_simple, toArray) {
	zval *properties = zend_read_property(syx_config_simple_ce, getThis(), ZEND_STRL(SYX_CONFIG_PROPERT_NAME), 1, NULL);
	RETURN_ZVAL(properties, 1, 0);
}
/* }}} */

/** {{{ proto public Syx_Config_Simple::set($name, $value)
*/
PHP_METHOD(syx_config_simple, set) {
	zval *readonly = zend_read_property(syx_config_simple_ce,
			getThis(), ZEND_STRL(SYX_CONFIG_PROPERT_NAME_READONLY), 1, NULL);

	if (Z_TYPE_P(readonly) == IS_FALSE) {
		zend_string *name;
		zval *value, *props;
		if (zend_parse_parameters(ZEND_NUM_ARGS(), "Sz", &name, &value) == FAILURE) {
			return;
		}

		props = zend_read_property(syx_config_simple_ce, getThis(), ZEND_STRL(SYX_CONFIG_PROPERT_NAME), 1, NULL);
		if (zend_hash_update(Z_ARRVAL_P(props), name, value) != NULL) {
			Z_TRY_ADDREF_P(value);
			RETURN_TRUE;
		}
	}

	RETURN_FALSE;
}
/* }}} */

/** {{{ proto public Syx_Config_Simple::__isset($name)
*/
PHP_METHOD(syx_config_simple, __isset) {
	zend_string *name;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "S", &name) == FAILURE) {
		return;
	} else {
		zval *prop = zend_read_property(syx_config_simple_ce, getThis(), ZEND_STRL(SYX_CONFIG_PROPERT_NAME), 1, NULL);
		RETURN_BOOL(zend_hash_exists(Z_ARRVAL_P(prop), name));
	}
}
/* }}} */

/** {{{ proto public Syx_Config_Simple::offsetUnset($index)
*/
PHP_METHOD(syx_config_simple, offsetUnset) {
	zval *readonly = zend_read_property(syx_config_simple_ce,
			getThis(), ZEND_STRL(SYX_CONFIG_PROPERT_NAME_READONLY), 1, NULL);

	if (Z_TYPE_P(readonly) == IS_FALSE) {
		zval *props;
		zend_string *name;
		if (zend_parse_parameters(ZEND_NUM_ARGS(), "S", &name) == FAILURE) {
			return;
		}

		props = zend_read_property(syx_config_simple_ce, getThis(), ZEND_STRL(SYX_CONFIG_PROPERT_NAME), 1, NULL);
		if (UNEXPECTED(Z_TYPE_P(props) != IS_ARRAY)) {
			RETURN_FALSE;
		}
		if (zend_hash_del(Z_ARRVAL_P(props), name) == SUCCESS) {
			RETURN_TRUE;
		}
	}

	RETURN_FALSE;
}
/* }}} */

/** {{{ proto public Syx_Config_Simple::count($name)
*/
PHP_METHOD(syx_config_simple, count) {
	zval *prop = zend_read_property(syx_config_simple_ce, getThis(), ZEND_STRL(SYX_CONFIG_PROPERT_NAME), 1, NULL);
	RETURN_LONG(zend_hash_num_elements(Z_ARRVAL_P(prop)));
}
/* }}} */

/** {{{ proto public Syx_Config_Simple::rewind(void)
*/
PHP_METHOD(syx_config_simple, rewind) {
	zval *prop = zend_read_property(syx_config_simple_ce, getThis(), ZEND_STRL(SYX_CONFIG_PROPERT_NAME), 1, NULL);
	zend_hash_internal_pointer_reset(Z_ARRVAL_P(prop));
}
/* }}} */

/** {{{ proto public Syx_Config_Simple::current(void)
*/
PHP_METHOD(syx_config_simple, current) {
	zval *prop, *pzval, *ret;

	prop = zend_read_property(syx_config_simple_ce, getThis(), ZEND_STRL(SYX_CONFIG_PROPERT_NAME), 1, NULL);
	if ((pzval = zend_hash_get_current_data(Z_ARRVAL_P(prop))) == NULL) {
		RETURN_FALSE;
	}

	if (Z_TYPE_P(pzval) == IS_ARRAY) {
		zval rv = {{0}};
		if ((ret = syx_config_simple_format(getThis(), pzval, &rv))) {
			RETURN_ZVAL(ret, 1, 1);
		} else {
			RETURN_NULL();
		}
	} else {
		RETURN_ZVAL(pzval, 1, 0);
	}
}
/* }}} */

/** {{{ proto public Syx_Config_Simple::key(void)
*/
PHP_METHOD(syx_config_simple, key) {
	zval *prop;
	zend_string *string;
	ulong index;

	prop = zend_read_property(syx_config_simple_ce, getThis(), ZEND_STRL(SYX_CONFIG_PROPERT_NAME), 1, NULL);
	zend_hash_get_current_key(Z_ARRVAL_P(prop), &string, &index);
	switch(zend_hash_get_current_key_type(Z_ARRVAL_P(prop))) {
		case HASH_KEY_IS_LONG:
			RETURN_LONG(index);
			break;
		case HASH_KEY_IS_STRING:
			RETURN_STR(zend_string_copy(string));
			break;
		default:
			RETURN_FALSE;
	}
}
/* }}} */

/** {{{ proto public Syx_Config_Simple::next(void)
*/
PHP_METHOD(syx_config_simple, next) {
	zval *prop = zend_read_property(syx_config_simple_ce, getThis(), ZEND_STRL(SYX_CONFIG_PROPERT_NAME), 1, NULL);
	zend_hash_move_forward(Z_ARRVAL_P(prop));
	RETURN_TRUE;
}
/* }}} */

/** {{{ proto public Syx_Config_Simple::valid(void)
*/
PHP_METHOD(syx_config_simple, valid) {
	zval *prop = zend_read_property(syx_config_simple_ce, getThis(), ZEND_STRL(SYX_CONFIG_PROPERT_NAME), 1, NULL);
	RETURN_BOOL(zend_hash_has_more_elements(Z_ARRVAL_P(prop)) == SUCCESS);
}
/* }}} */

/** {{{ proto public Syx_Config_Simple::readonly(void)
*/
PHP_METHOD(syx_config_simple, readonly) {
	zval *readonly = zend_read_property(syx_config_simple_ce,
			getThis(), ZEND_STRL(SYX_CONFIG_PROPERT_NAME_READONLY), 1, NULL);
	RETURN_BOOL(Z_TYPE_P(readonly) == IS_TRUE);
}
/* }}} */

/** {{{ proto public Syx_Config_Simple::__destruct
*/
PHP_METHOD(syx_config_simple, __destruct) {
}
/* }}} */

/** {{{ proto private Syx_Config_Simple::__clone
*/
PHP_METHOD(syx_config_simple, __clone) {
}
/* }}} */

/** {{{ syx_config_simple_methods
*/
zend_function_entry syx_config_simple_methods[] = {
	PHP_ME(syx_config_simple, __construct, syx_config_simple_construct_arginfo, ZEND_ACC_PUBLIC|ZEND_ACC_CTOR)
	/* PHP_ME(syx_config_simple, __destruct,	NULL, ZEND_ACC_PUBLIC|ZEND_ACC_DTOR) */
	PHP_ME(syx_config_simple, __isset, syx_config_simple_isset_arginfo, ZEND_ACC_PUBLIC)
	PHP_ME(syx_config_simple, get, syx_config_simple_get_arginfo, ZEND_ACC_PUBLIC)
	PHP_ME(syx_config_simple, set, syx_config_simple_set_arginfo, ZEND_ACC_PUBLIC)
	PHP_ME(syx_config_simple, count, syx_config_simple_void_arginfo, ZEND_ACC_PUBLIC)
	PHP_ME(syx_config_simple, offsetUnset,	syx_config_simple_unset_arginfo, ZEND_ACC_PUBLIC)
	PHP_ME(syx_config_simple, rewind, syx_config_simple_void_arginfo, ZEND_ACC_PUBLIC)
	PHP_ME(syx_config_simple, current, syx_config_simple_void_arginfo, ZEND_ACC_PUBLIC)
	PHP_ME(syx_config_simple, next,	syx_config_simple_void_arginfo, ZEND_ACC_PUBLIC)
	PHP_ME(syx_config_simple, valid, syx_config_simple_void_arginfo, ZEND_ACC_PUBLIC)
	PHP_ME(syx_config_simple, key, syx_config_simple_void_arginfo, ZEND_ACC_PUBLIC)
	PHP_ME(syx_config_simple, readonly,	syx_config_simple_void_arginfo, ZEND_ACC_PUBLIC)
	PHP_ME(syx_config_simple, toArray, syx_config_simple_void_arginfo, ZEND_ACC_PUBLIC)
	PHP_MALIAS(syx_config_simple, __set, set, syx_config_simple_set_arginfo, ZEND_ACC_PUBLIC)
	PHP_MALIAS(syx_config_simple, __get, get, syx_config_simple_get_arginfo, ZEND_ACC_PUBLIC)
	PHP_MALIAS(syx_config_simple, offsetGet, get, syx_config_simple_rget_arginfo, ZEND_ACC_PUBLIC)
	PHP_MALIAS(syx_config_simple, offsetExists, __isset, syx_config_simple_isset_arginfo, ZEND_ACC_PUBLIC)
	PHP_MALIAS(syx_config_simple, offsetSet, set, syx_config_simple_set_arginfo, ZEND_ACC_PUBLIC)
	{NULL, NULL, NULL}
};
/* }}} */

/** {{{ SYX_STARTUP_FUNCTION
*/
SYX_STARTUP_FUNCTION(config_simple) {
	zend_class_entry ce;

	SYX_INIT_CLASS_ENTRY(ce, "Syx\\Config\\Simple", syx_config_simple_methods);
	syx_config_simple_ce = zend_register_internal_class_ex(&ce, syx_config_ce);

#ifdef HAVE_SPL
	zend_class_implements(syx_config_simple_ce, 3, zend_ce_iterator, zend_ce_arrayaccess, spl_ce_Countable);
#else
	zend_class_implements(syx_config_simple_ce, 2, zend_ce_iterator, zend_ce_arrayaccess);
#endif
	zend_declare_property_bool(syx_config_simple_ce, ZEND_STRL(SYX_CONFIG_PROPERT_NAME_READONLY), 0, ZEND_ACC_PROTECTED);

	syx_config_simple_ce->ce_flags |= ZEND_ACC_FINAL;

	return SUCCESS;
}
/* }}} */

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
