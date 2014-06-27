# About CloudModule for Zabbix

Hybrid Cloud Monitoring Module by using Deltacloud.
This tool is for Zabbix Loadable Module.

Zabbix loadable modules is supported over Zabbix 2.2.0.

# Features

* Automatically registration EC2 instances information.
* Automatically registration EC2 CloudWatch metrics information.

# Requirements

* Setup Deltacloud server. (tested in Deltacloud 1.1.3)
* libdeltacloud customized for this tool
* Zabbix >= 2.2.0

# Installation

* Install Deltacloud
* Apply one patch (https://github.com/ike-dai/deltacloud-core/commit/61b690698c949fb7c92607651f1ctc5e02971df966)
* Start deltacloudd
* Install libdeltacloud_for_cloudmodule
* Compile CloudModule
* Copy cloud_module.conf.example to /etc/zabbix/cloud_module.conf
* Set LoadableModulePath, LoadableModule in zabbix_server.conf(or zabbix_agentd.conf)
* Import CloudModule template to Zabbix server
* Add Cloud monitoring host in Zabbix server

# Contact

Please send feedback to me.

DAISUKE Ikeda
Twitter: @ike_dai
e-mail: <dai.ikd123@gmail.com>

# License

CloudModule for Zabbix is released under the GNU General Public License version2.  
The GPL official full text is published at this [link](http://www.gnu.org/licenses/old-licenses/gpl-2.0.txt).

Copyright 2014 DAISUKE Ikeda. 
