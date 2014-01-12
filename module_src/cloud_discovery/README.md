# cloud_discovery

This is a Zabbix loadable module which is discovering cloud instances.

This module depends on Deltacloud.
This module collects cloud instance data through Deltacloud by using libdeltacloud.

## Usage

1. Download Zabbix source code (over 2.2.0).
2. Install libdeltacloud
3. Clone this source code.
4. Copy this code to under the Zabbix src/modules directory.
5. Execute 'make' command
6. Copy cloud_discovery.so to under the LoadModulePath directory.
7. Restart Zabbix Agent
8. Set LLD rule 'cloud.discovery[url,key,secret,driver,provider]'
 
