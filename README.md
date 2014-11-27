# About

**CloudModule for Zabbix**

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

## 1. Install Deltacloud

Deltacloud is made by Ruby.  
So, please install ruby (>= 1.9.2).

And, install rubygems.

You can install deltacloud by using gem command.

    $ sudo gem install sqlite3 deltacloud-core

If you have installed thin >= 2.0.0, please reinstall thin version 1.6.2

    $ sudo gem uninstall thin
    $ sudo gem install thin --version=1.6.2

If you have installed haml >= 4.0.5, please reinstall haml version 4.0.4

    $ sudo gem uninstall haml
    $ sudo gem install haml --version=4.0.4
 
## 2. Apply one patch to Deltacloud

It is not possible to get AWS CloudWatch metrics from Deltacloud 1.1.3.
So, please apply patch <https://github.com/ike-dai/deltacloud-core/commit/61b690698c949fb7c92607651f1ctc5e02971df966>.

    $ cd <rubygems dircetory>/deltacloud-core-1.1.3
    $ vim lib/deltacloud/drivers/ec2/ec2_driver.rb
        #start_time, end_time, metric_unit_for(p.name), { m.entity => opts[:id]})     <- comment out
        start_time, end_time, metric_unit_for(p.name), {:dimensions => { m.entity => opts[:id]}})


## 3. Start deltacloudd

**Please start Deltacloud daemon.**

    $ sudo deltacloudd -i ec2 -p 3000 &

## 4. Install libdeltacloud-for-cloudmodule

**Clone libdeltacloud-for-cloudmodule git repository from GitHub.**

    $ git clone https://github.com/ike-dai/libdeltacloud-for-cloudmodule.git

**Complile & Install**

    $ sudo yum install libtool
    $ sudo yum install libcurl-devel
    $ cd libdeltacloud-for-cloudmodule
    $ ./autogen.sh
    $ ./configure
    $ make
    $ sudo make install

(As default, this library will be installed in /usr/local/lib.)

**Update shared library path.**

    $ sudo vim /etc/ld.so.conf.d/libdeltacloud.conf
    /usr/local/lib
    $ sudo ldconfig

## 5. Compile & Install CloudModule

**Clone CloudModule git repository from GitHub.**

    $ git clone https://github.com/ike-dai/zabbix-cloud-module.git

Copy Makefile & source code to Zabbix source code directory.  
If you don't have Zabbix source code, please download it.

    $ wget http://jaist.dl.sourceforge.net/project/zabbix/ZABBIX%20Latest%20Stable/2.2.4/zabbix-2.2.4.tar.gz
    $ tar xvzf zabbix-2.2.4.tar.gz
    $ cd zabbix-2.2.4
    $ ./configure --enable-server --with-sqlite3
    $ make

    $ cp -r zabbix-cloud-module ./src/modules/

**Complile**

    $ cd src/modules/zabbix-cloud-module
    $ make

**Error message**  
If you get the following error, please recompile libs/zbxmemory/memalloc.c

    gcc -shared -o cloud_module.so cloud_module.c ../../libs/zbxmemory/memalloc.o -I../../../include -ldeltacloud -fPIC
    /usr/bin/ld: ../../libs/zbxmemory/memalloc.o: relocation R_X86_64_32 against `.rodata.str1.8' can not be used when making a shared object; recompile with -fPIC
    ../../libs/zbxmemory/memalloc.o: could not read symbols: Bad value
    collect2: ld returned 1 exit status

    $ cd src/libs/zbxmemory/
    $ make clean
    $ vi Makefile
    CFLAGS = -g -O2     -I/usr/include -fPIC   <--- Add "-fPIC"
    $ make


**Copy the shared library file.**

    $ mkdir /usr/lib/zabbix/modules
    $ cp cloud_module.so /usr/lib/zabbix/modules/

## 6. Copy configuration file

Please copy cloud_module.conf.example to /etc/zabbix/cloud_module.conf.
And set values.

    $ cp cloud_module.conf.example /etc/zabbix/cloud_module.conf
    $ vim /etc/zabbix/cloud_module.conf
    ModuleTimeout=300
    ModuleCloudCacheSize=4M
    ZabbixFile="/etc/zabbix/zabbix_server.conf"

**Notes: cloud_module.conf must be placed under "/etc/zabbix" directory.**

## 7. Set LoadableModule configurations

Please set LoadableModulePath,LoadableModule in zabbix_server.conf(or zabbix_agentd.conf)  
Loadable Module is used in Zabbix Server or Zabbix Agent.

* When you can use in Zabbix Server(as Simple check item), please edit "zabbix_server.conf".
* When you can use in Zabbix Agent(as Zabbix Agent item), please edit "zabbix_agentd.conf".

    $ vim /etc/zabbix/zabbix_server.conf
    
    LoadableModulePath=/usr/lib/zabbix/modules
    LoadableModule=cloud_module.so

## 8. Import CloudModule template to Zabbix server
 
Please import templates/cloud_module_templates.xml in Zabbix frontend [Configuration] -> [Templates] -> [Import]

By importing this xml, two templates will be added.

* Template Cloud Module
* Template Cloud Module VM


## 9. Add Cloud monitoring host

Add host.

For example:

* Host name: AWS Account A
* Host group: Cloud monitoring
* Interface: Any (This module does not use interface information.)
* Templates: Template Cloud Module
* Macros:
    * {$DELTACLOUD_DRIVER} -> ec2 (Now this module support only ec2)
    * {$DELTACLOUD_PASSWORD} -> AWS API Secret key
    * {$DELTACLOUD_PROVIDER} -> ap-northeast-1 (support any other region name)
    * {$DELTACLOUD_URL} -> http://localhost:3000/api (please set your deltacloud URL)
    * {$DELTACLOUD_USERNAME} -> AWS API Access key

When these settings are finished, monitoring the status of AWS will start automatically.



# Contact

Please send feedback to me.

DAISUKE Ikeda

Twitter: [@ike_dai](https://twitter.com/ike_dai)

e-mail: <dai.ikd123@gmail.com>

# License

CloudModule for Zabbix is released under the GNU General Public License version2.
The GPL official full text is published at this [link](http://www.gnu.org/licenses/old-licenses/gpl-2.0.txt).

Copyright 2014 DAISUKE Ikeda.


