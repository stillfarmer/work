openwrt_trunk用户管理ipk设计

version: 1.0

author: wudi


一： 方案设计：

    用户管理作用:管理lan侧有线及无线设备。

    功能划分：

    （1） 用户接入离开管理
    （2） 限速
    （3） portal
    （4） 用户其他配置（如无线黑名单， 用户host别名）


二： 用户管理


    （1） 用户接入离开管理


2.1 相关结构体

    2.1.1 内核空间结构体

    struct user_info_kernel
    {
        unsigned char   is_active;                          /* 标识该用户是否活跃， 网桥mac学习时 置1， 老化时置0, 只由用户空间下发 */
        unsigned char   mac[SIX_MAC_ADDR_LEN];              /* 用户mac */
        unsigned char   devname[IFNAMSIZ];                  /* 用户接入的 intfname, ra0, eth0.1 */
        unsigned char   mac_bwlimit_type;                   /* 限速， 场景1， 2下， 限速是否要区分intf */
        unsigned int    up_bwlimit_bits;                    /*  上行限速值， bit */
        unsigned int    down_bwlimit_bits;                  /*  下行限速值， bit  */
        unsigned int    up_total_bits_count;                /*  用户上行流量统计， bits */
        unsigned int    down_total_bits_count;              /*  用户下行流量统计， bits */
        UserPktHead     up_link;                            /*  用户上行限速实现链表 */
        UserPktHead     down_link;                          /*  用户下行限速实现链表 */
        unsigned int    time_long;                          /*  用户认证时长到期jiffies值 */

        /* 为iphone portal 保留 start */
        unsigned int      neediphoneportalsend;  
        unsigned int      issendiphoneportal;
        unsigned char     iphonequeryurl[64];
        /* 为iphone portal 保留 end */
    } user_info_kernel;


    2.2.2 用户空间结构体

        2.2.2.1 用户信息表

        struct auth_time_type
        {
            unsigned long auth_time;                                   /* 用户认证成功的时间 */
            unsigned long allowed_time;                                /* 服务器下发的用户上网时长 */
            unsigned long time_long;                                   /* 用户上网时长到期时刻点 */
        };

        typedef struct user_info
        {
            unsigned char  is_active;                               /* 是否在线 */
            unsigned char  user_mac[SIX_MAC_ADDR_LEN];                  /* 用户mac地址 */
            unsigned char  bssid[SIX_MAC_ADDR_LEN];                     /* 待确定是否需要保留 */
            unsigned char  devname[IFNAMESIZ];                          /* e.g. eth0, ath0, ath1 */
            unsigned char  is_auth;                                     /* 是否认证 */
            unsigned char  is_2_4G;                                     /* 是否2.4g */
            char           device_type[DEVICE_TYPE_LEN];                /* 设备类型 xiaomi, ipad, iphone */
            unsigned long  login_time;                                  /* 用户login时的时间 */
#if 0
            struct  auth_time_type auth_time_table[PORTAL_GROUP_NUM];  /* auth_time_table index 同portal模块 */
#endif
            struct  auth_time_type auth_type_entry;  /* auth_time_table index 同portal模块 */
            unsigned long  leave_time;                                  /* 用户离开时间， 用作表滿之后， 新加用户覆盖entry 的判断 */
            unsigned int   up_bwlimit_bits;                             /* 上行带宽限制速率 */
            unsigned int   down_bwlimit_bits;                           /* 下行带宽限制速率 */
            unsigned int   up_bits_count;                               /* 上行速率统计 */
            unsigned int   down_bits_count;                             /* 下行速率统计 */
            unsigned int   rssi;                                        /* 无线驱动参数， recv rssi ? 待研究 */
            char           host_name[HOST_NAME_LEN];                    /* 用户主机名 */
            char           ip_addr[IP_ADDR_LEN];                        /* 路由模式下， 用户ip地址， arp 更新条目时更新 */
            char           user_config_host_name[HOST_NAME_LEN]         /* 用户配置的 主机别名*/
            char           option60[64];                                /* dhcp option 60 字段 */
            unsigned long  reg;                                         /* 暂不清除用途， 保留 */
            unsigned char  connection_flag;                             /* 标识是否允许用户上网， ebtable rule */

            /* 为iphone portal 保留 start */
            unsigned int      neediphoneportalsend;  
            unsigned int      issendiphoneportal;
            unsigned char   iphonequeryurl[64];
            /* 为iphone portal 保留 end */
        }user_info;


        2.2.2.2 无线黑名单表

        struct wireless_black_list
        {
            unsigned char devname[IFNAMSIZ];
            unsigned char mac[SIX_MAC_ADDR_LEN];
        }


2.2 用户接入离开

    2.2.1 新用户添加

        2.2.1.1 bridge, mac学习阶段添加。

            用户空间user_info更新以下字段：
            (1)   is_associated;                                            /* 是否在线 */
           （2）  user_mac[SIX_MAC_ADDR_LEN];                               /* 用户mac地址 */
           （3）  devname                                                   /* eth0, ath0, ath1 */
            (4)   login_time;                                               /* 用户login时的时间 */
            (5)   up_bwlimit_bits;                                          /* 上行带宽限制速率 限速场景为1，2时， 从规则表中获取 */
            (6)   down_bwlimit_bits;                                        /* 下行带宽限制速率 限速场景为1，2时， 从规则表中获取 */
            (7)   auth_time_table;

        2.2.1.2 路由模式时， dhcp msg， 会更新以下字段
            (7)  device_type。                                              /* 设备类型 xiaomi, ipad, iphone */
            (8)  host_name。                                                /* 用户主机名 */                           
            (9)  option60。                                                 /*  option 60 字段 */

        2.2.1.3 路由模式时， arp msg， 会更新以下字段
            (10) ip_addr                                                    /* 用户ip_addr */ 

   
        2.2.1.4 内核空间更新以下字段：
           （1） is_active;
            (2） mac[SIX_MAC_ADDR_LEN];
           （3） devname[IFNAMSIZ];
           （4） up_bwlimit_bits;
           （5） down_bwlimit_bits;
           （6） time_long;

            注： （a）当用户空间表滿时， 以is_associated字段为0的条目中， leave_time最早的entry作为覆盖的条目。
       
       
  2.2.2 用户离开

    bridge, mac老化


    用户空间：
    （1）is_associated置0。
    （2）leave_time更新。


    内核空间：
    （1） is_active字段置0。



三： 限速

    限速四种场景：
    （1） 根据mac的限速， 区分intf, ra0, ra1, (如252cp).
    （2） 根据mac的限速， 不区分intf。 (如302tv2).

    （3） 根据intf的限速， 参照252cpa， 根据ssid的限速, 新加，2.4G与5G的配置(ath0, ath16)在同一个限速group中.
    （4） 根据intf的限速， 参照252cpa， 根据用户的限速。

     (5)  有线限速可以关闭。

    只能处于以下3种场景，
    （1）存在。
    （2）存在。
    （3）（4）存在， 可共存也可单独存在。

    (1) (2) 与（3）（4）不可共存。


配置项：1. 场景配置     ----  g_bwlimit_type
        2. 有线无线配置 --    dev.c 限速intf配置


    3.1 用户空间数据

        3.1.1 全局变量

        int g_bwlimit_type_int;       
          /* value 0x00 设备不开启限速功能，
                   0x11  场景（1), 有线也限。
                   0x12. 场景（2), 有线也限。
                   0x14. 场景（3)（4）, 有线也限  

                   0x01, 场景（1）， 有线不限。
                   0x02, 场景（2）， 有线不限。
                   0x04, 场景（3）(4)， 有线不限。*/
                   

        3.1.2 结构体

        struct dev_name_group_entry
        {
            char dev_name[BW_LIMIT_GROUP_INF_NUM][IFNAMSIZ];
        };

        struct dev_name_group_entry dev_name_group_table[DEV_NAME_GROUP_NUM]; /* DEV_NAME_GROUP_NUM 应是2.4G intface的number ?*/

        struct bwlimit_user_rule                                             /* 该结构类型在场景1， 2下使用 */
        {
            unsigned char       is_used;                                     /* 标识该规则条目是否已用 */
            unsigned char       user_mac[SIX_MAC_ADDR_LEN];                  /* 用户mac */
            unsigned char       dev_name[IFNAMSIZ];                           /* e.g. eth0, ath1, ath2， 当设备处于场景1时， 标识该限速group中的intfname。 
                                                                              * 在设备中, 除有线外， 应是成对存在, 如ath0, ath16.  一个场景1下的限速group中的
                                                                                intf 最多只有3个成员？ lan侧有线如eth1， ath0, ath16 
                                                                                config 文件中(不可改动)应存在场景1下， group中的intf的配对情况， 如
                                                                                eth0, ath0, ath16
                                                                                eth0, ath1, ath17, 
                                                                                eth0可共用， 标识lan侧有线intfname， */
                                                                                
            unsigned int        up_bwlimit_bits;                             /* 上行限速, bits */
            unsigned int        down_bwlimit_bits;                           /* 下行限速, bits */
        }; 

        struct bwlimit_user_rule bwlimit_user_rule_table[BWLIMIT_USER_RULE_NUM];

        struct bwlimit_intf_rule
        {
            unsigned char       devname[IFNAMSIZ];                           /* g_bwlimit_type_int 为2时用到 */
            unsigned char       bwlimit_type;                                /* g_bwlimit_type_int 为2时用到， 用以区分252cpa, ssid的限速, 用户的限速 */
            unsigned int        up_bwlimit_bits                              /* bits */
            unsigned int        down_bwlimit_bits;                           /* bits */
        };

        struct bwlimit_intf_rule bwlimit_inf_rule_table[BWLIMIT_INTF_RULE_NUM];   /**/

        bwlimit_user_rule   bwlimit_user_rule_table[BWLIMIT_USER_RULE_NUM];
        bwlimit_intf_rule   bwlimit_intf_rule_table[BWLIMIT_INTFACE_RULE_NUM];   MAX_BWLIMIT_INTFACE_RULE_NUM 为intf;



    3.2 内核空间数据

        3.2.1  dev.c 可配置项：

            限速intf可配， 如eth0.1, ath2等。链表

            name          len
            ath            3
            eth0.1         6


        3.2.2. 全局变量：

            g_bwlimit_type_int， 同用户空间同名全局变量。


        3.2.3 结构体

            3.2.3.1 内核, 场景（3），（ 4）下的的intf rule 规则表， 相比用户空间， 多了uplink, downlink用于限速。

            struct bwlimit_intf_rule_kernel
            {
                unsigned char       devname[IFNAMSIZ];               /* e.g. ath1, eth2 */
                unsigned char       bwlimit_type;                    /* g_bwlimit_type_int 为2时用到， 用以区分252cpa, ssid的限速, 用户的限速 */
                unsigned int        up_bwlimit_bits;                 /* 上行限速， bits */
                unsigned int        down_bwlimit_bits;               /* 下行限速， bits */
                UserPktHead         uplink;
                UserPktHead         downlink;
            };

            struct bwlimit_intf_rule_kernel intf_info_table[BWLIMIT_INTF_NUM];


            3.2.3.2  内核， 用户表， 同2.1.1

            typedef struct user_info_kernel
            {
                unsigned char   is_active;                          /* 标识该用户是否活跃， 网桥mac学习时 置1， 老化时置0, 只由用户空间下发 */
                unsigned char   mac[SIX_MAC_ADDR_LEN];              /* 用户mac */
                unsigned char   mac_bwlimit_type;                   /* 限速， 场景1， 2下， 限速是否要区分intf */
                unsigned char   devname[IFNAMSIZ];                  /* 用户接入的 intfname, ra0, eth0.1 */
                unsigned int    up_bwlimit_bits;                    /*  上行限速值， bit */
                unsigned int    down_bwlimit_bits;                  /*  下行限速值， bit  */
                unsigned int    up_total_bits_count;                /*  用户上行流量统计， bits */
                unsigned int    down_total_bits_count;              /*  用户下行流量统计， bits */
                UserPktHead     up_link;                            /*  用户上行限速实现链表 */
                UserPktHead     down_link;                          /*  用户下行限速实现链表 */
                unsigned int    time_long;                          /*  用户认证时长到期jiffies值 */

                /* 为iphone portal 保留 start */
                unsigned int      neediphoneportalsend;  
                unsigned int      issendiphoneportal;
                unsigned char   iphonequeryurl[64];
                /* 为iphone portal 保留 end */
            } user_info_kernel;


四： portal

    对于用户管理模块， 需要记录用户的认证情况

    4.1 内核空间， 以下结构体中的time_long字段标识该用户认证情况

    typedef struct user_info_kernel
    {
        unsigned char   is_active;                          /* 标识该用户是否活跃， 网桥mac学习时 置1， 老化时置0, 只由用户空间下发 */
        unsigned char   mac[SIX_MAC_ADDR_LEN];              /* 用户mac */
        unsigned char   mac_bwlimit_type;                   /* 限速， 场景1， 2下， 限速是否要区分intf */
        unsigned char   devname[IFNAMSIZ];                  /* 用户接入的 intfname, ra0, eth0.1 */
        unsigned int    up_bwlimit_bits;                    /*  上行限速值， bit */
        unsigned int    down_bwlimit_bits;                  /*  下行限速值， bit  */
        unsigned int    up_total_bits_count;                /*  用户上行流量统计， bits */
        unsigned int    down_total_bits_count;              /*  用户下行流量统计， bits */
        UserPktHead     up_link;                            /*  用户上行限速实现链表 */
        UserPktHead     down_link;                          /*  用户下行限速实现链表 */
        unsigned int    time_long;                          /*  用户认证时长到期jiffies值 */

        /* 为iphone portal 保留 start */
        unsigned int      neediphoneportalsend;  
        unsigned int      issendiphoneportal;
        unsigned char   iphonequeryurl[64];
        /* 为iphone portal 保留 end */
    } user_info_kernel;


    4.2 用户空间    auth_time, allowed_time, time_long字段标识用户认证情况

    typedef struct user_info
    {
        unsigned char  is_associated;                               /* 是否在线 */
        unsigned char  user_mac[SIX_MAC_ADDR_LEN];                  /* 用户mac地址 */
        unsigned char  bssid[SIX_MAC_ADDR_LEN];                     /* 待确定是否需要保留 */
        unsigned char  devname[IFNAMESIZ];                          /* e.g. eth0, ath0, ath1 */
        int            is_auth;                                     /* 是否认证 */
        int            is_2_4G;                                     /* 是否2.4g */
        char           device_type[DEVICE_TYPE_LEN];                /* 设备类型 xiaomi, ipad, iphone */
        unsigned long  login_time;                                  /* 用户login时的时间 */
        unsigned long  auth_time;                                   /* user auth time */
        unsigned long  allowed_time;                                /*  服务器下发的上网时长 */
        unsigned long  time_long;                                   /*  用户认证到期时间 */
        unsigned long  leave_time;                                  /* 用户离开时间， 用作表滿之后， 新加用户覆盖entry 的判断 */
        int            up_bwlimit_bits;                             /* 上行带宽限制速率 */
        int            down_bwlimit_bits;                           /* 下行带宽限制速率 */
        unsigned int   up_bits_count;                               /* 上行速率统计 */
        unsigned int   down_bits_count;                             /* 下行速率统计 */
        unsigned int   rssi;                                        /* 无线驱动参数， recv rssi ? 待研究 */
        char           host_name[HOST_NAME_LEN];                    /* 用户主机名 */
        char           ip_addr[HOST_NAME_LEN];                      /*  路由模式下， 用户ip地址， arp 更新条目时更新 */
        char           user_config_host_name[HOST_NAME_LEN]         /* 用户配置的 主机别名*/
        char           option60[64];                                /* dhcp option 60 字段 */
        unsigned long  reg;                                         /* 暂不清除用途， 保留 */
        unsigned char  connection_flag;                             /* 标识是否允许用户上网， ebtable rule */

        /* 为iphone portal 保留 start */
        unsigned int      neediphoneportalsend;  
        unsigned int      issendiphoneportal;
        unsigned char   iphonequeryurl[64];
        /* 为iphone portal 保留 end */
    }user_info;


五： 其他配置

5.1 无线黑名单

        场景：1. AP上的无线黑名单
              2. intf的无线黑名单

    5.1.1 相关结构体

        struct wireless_black_list
        {
            unsigned char devname[IFNAMSIZ];
            unsigned char mac[SIX_MAC_ADDR_LEN];
        };


5.2 禁止用户上网

    用户空间， 用户信息表中的connection_flag字段

         


六. 用户空间消息事件

6.1  用户相关

    6.1.1 mac学习, 老化
          
          msg_mac_update

          int user_mac_update(unsigned char update_flag, unsigned char *mac, unsigned char *devname);

          args: update_flag = 0时标识老化，删除;
                update_flag = 1时标识学习, 更新;


    6.1.2 arp更新

         msg_arp_update

         int user_ip_update(unsigned char *mac, char *ip_addr);


    6.1.3 dhcp msg更新

        msg_dhcp_update

         int user_dhcp_update( unsigned char *mac, char *hostname, char *option60);
         /* 可以仅更新一项 */

    6.1.4 用户信息获取

        msg_user_info_get


6.2 限速

    6.2.1 设置 bwlimit 类型（0, 不限速； 1， 根据mac的; 2, 根据intf的）

          msg_bwlimit_type_set
    
          int user_bwlimit_type_set(int g_bwlimit_type);

    6.2.1 限速规则更新

          msg_bwlimit_rule_update

          int user_bwlimit_rule_update(int flag, unsigned char *devname, unsigned char *mac, unsigned int bwlimit_bits_value);

          args:  flag:                 0 删除， 1 添加 更新；
                 devname:              e.g. ra0, eth0, 根据mac的限速时可以为null。
                 mac：                 根据intf的限速时可以为null。
                 bwlimit_bits_value:   限速值， bits value；

   6.2.2 限速规则获取

        msg_bwlimit_rule_get;


6.4 portal

    6.4.1 更新用户portal信息

        msg_portal_update

        int user_portal_update(unsigned char *mac, unsigned int time_long);

        args: time_long为0时表示踢出用户。


6.5 其他配置

    6.5.1 更新用户无线黑名单信息

        msg_wireless_blacklist_update

        int user_wireless_blacklist_update(int flag, unsigned char *mac, unsigned char *devname);

        args: flag = 0, 删除该条目。
              flag = 1, 添加该条目。

              devname， 无线黑名单interface。


    6.5.2 更新禁止用户上网信息

        msg_ebtable_rule_update

        int user_ebtable_rule_update(int flag, unsigned char *mac, unsigned char *devname);

        args: flag = 0, 删除该条目。
              flag = 1, 添加该条目。


    6.5.3 更新用户hostname别名

        msg_user_config_host_name

        int user_config_host_name( int flag, unsigned char *mac, char *user_config_host_name);

        args: flag = 0, 删除该别名。
              flag = 1, 添加该别名。


七： 用户空间与内核接口

7.1 内核 netlink dev

   /dev/usrmgmt




