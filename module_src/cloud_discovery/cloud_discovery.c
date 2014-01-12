/*
** Copyright (C) 2014 Daisuke Ikeda
**
** This program is free software; you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation; either version 2 of the License, or
** (at your option) any later version.
**
** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
** GNU General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with this program; if not, write to the Free Software
** Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
**/

#include "sysinc.h"
#include "module.h"
#include "zbxjson.h"
#include <curl/curl.h>
#include <stdio.h>
#include <stdlib.h>
#include <libdeltacloud/libdeltacloud.h>

/* the variable keeps timeout setting for item processing */
static int	item_timeout = 0;

int	zbx_module_dummy_ping(AGENT_REQUEST *request, AGENT_RESULT *result);
int	zbx_module_dummy_echo(AGENT_REQUEST *request, AGENT_RESULT *result);
int	zbx_module_dummy_random(AGENT_REQUEST *request, AGENT_RESULT *result);
int	zbx_module_aws_discovery(AGENT_REQUEST *request, AGENT_RESULT *result);

char *name = "hoge";
char *value = "test";

static ZBX_METRIC keys[] =
/*      KEY                     FLAG		FUNCTION        	TEST PARAMETERS */
{
	{"aws.discovery",	CF_HAVEPARAMS,	zbx_module_aws_discovery,"http://hostname/api,ABC1223DE,ZDADQWQ2133"},
	{NULL}
};

/******************************************************************************
 *                                                                            *
 * Function: zbx_module_api_version                                           *
 *                                                                            *
 * Purpose: returns version number of the module interface                    *
 *                                                                            *
 * Return value: ZBX_MODULE_API_VERSION_ONE - the only version supported by   *
 *               Zabbix currently                                             *
 *                                                                            *
 ******************************************************************************/
int	zbx_module_api_version()
{
	return ZBX_MODULE_API_VERSION_ONE;
}

/******************************************************************************
 *                                                                            *
 * Function: zbx_module_item_timeout                                          *
 *                                                                            *
 * Purpose: set timeout value for processing of items                         *
 *                                                                            *
 * Parameters: timeout - timeout in seconds, 0 - no timeout set               *
 *                                                                            *
 ******************************************************************************/
void	zbx_module_item_timeout(int timeout)
{
	item_timeout = timeout;
}

/******************************************************************************
 *                                                                            *
 * Function: zbx_module_item_list                                             *
 *                                                                            *
 * Purpose: returns list of item keys supported by the module                 *
 *                                                                            *
 * Return value: list of item keys                                            *
 *                                                                            *
 ******************************************************************************/
ZBX_METRIC	*zbx_module_item_list()
{
	return keys;
}

/******************************************************************************
 *                                                                            *
 * Function: zbx_module_cloud_discovery                                       *
 *                                                                            *
 * Purpose: Discovering cloud instance lists from deltacloud                  *
 *                                                                            *
 * Parameters: request - structure that contains item key and parameters      *
 *              request->key - item key without parameters                    *
 *              request->nparam - number of parameters                        *
 *              request->timeout - processing should not take longer than     *
 *                                 this number of seconds                     *
 *              request->params[N-1] - pointers to item key parameters        *
 *                                                                            *
 *             result - structure that will contain result                    *
 *                                                                            *
 * Return value: SYSINFO_RET_FAIL - function failed, item will be marked      *
 *                                 as not supported by zabbix                 *
 *               SYSINFO_RET_OK - success                                     *
 *                                                                            *
 * Comment: get_rparam(request, N-1) can be used to get a pointer to the Nth  *
 *          parameter starting from 0 (first parameter). Make sure it exists  *
 *          by checking value of request->nparam.                             *
 *                                                                            *
 ******************************************************************************/
int	zbx_module_aws_discovery(AGENT_REQUEST *request, AGENT_RESULT *result)
{
	struct deltacloud_api api;
	struct deltacloud_instance *instances = NULL;
	struct deltacloud_driver *drivers = NULL;
	struct deltacloud_driver_provider *providers = NULL;
	struct zbx_json json;

	char	*url;
	char	*key;
	char	*secret;
	char	*driver;
	char	*provider;
	char 	*name_macro = "{$INSTANCE.NAME}";
	char 	*interface_macro = "{$INSTANCE.IP}";

	if (request->nparam != 5)
	{
		/* set optional error message */
		SET_MSG_RESULT(result, strdup("Invalid number of parameters e.x.) cloud.discovery[url, key, secret, driver, provider]"));
		return SYSINFO_RET_FAIL;
	}
	url = get_rparam(request, 0);
	key = get_rparam(request, 1);
	secret = get_rparam(request, 2);
	driver = get_rparam(request, 3);
	provider = get_rparam(request, 4);


	deltacloud_initialize(&api, url, key, secret, driver, provider);

	deltacloud_get_instances(&api, &instances);

	// json format init
	zbx_json_init(&json, ZBX_JSON_STAT_BUF_LEN);
	// Add "data":[] for LLD format
	zbx_json_addarray(&json, ZBX_PROTO_TAG_DATA);
	while(1){
		zbx_json_addobject(&json, NULL);
		zbx_json_addstring(&json, name_macro, instances->id, ZBX_JSON_TYPE_STRING);
		if(instances->public_addresses){
			zbx_json_addstring(&json, interface_macro, instances->public_addresses->address, ZBX_JSON_TYPE_STRING);
		}
		zbx_json_close(&json);
		instances = instances->next;
		if(instances == NULL){
			break;
		}
	}

	SET_STR_RESULT(result, strdup(json.buffer));
	zbx_json_free(&json);
	
	return SYSINFO_RET_OK;
}

/******************************************************************************
 *                                                                            *
 * Function: zbx_module_init                                                  *
 *                                                                            *
 * Purpose: the function is called on agent startup                           *
 *          It should be used to call any initialization routines             *
 *                                                                            *
 * Return value: ZBX_MODULE_OK - success                                      *
 *               ZBX_MODULE_FAIL - module initialization failed               *
 *                                                                            *
 * Comment: the module won't be loaded in case of ZBX_MODULE_FAIL             *
 *                                                                            *
 ******************************************************************************/
int	zbx_module_init()
{
	/* initialization for dummy.random */
	srand(time(NULL));

	return ZBX_MODULE_OK;
}

/******************************************************************************
 *                                                                            *
 * Function: zbx_module_uninit                                                *
 *                                                                            *
 * Purpose: the function is called on agent shutdown                          *
 *          It should be used to cleanup used resources if there are any      *
 *                                                                            *
 * Return value: ZBX_MODULE_OK - success                                      *
 *               ZBX_MODULE_FAIL - function failed                            *
 *                                                                            *
 ******************************************************************************/
int	zbx_module_uninit()
{
	return ZBX_MODULE_OK;
}
