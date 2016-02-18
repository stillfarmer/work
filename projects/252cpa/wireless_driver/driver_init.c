rc.wlan

1. insmod $MODULE_PATH/adf.ko

2. insmod $MODULE_PATH/asf.ko

3. insmod $MODULE_PATH/ath_hal.ko

4. insmod $MODULE_PATH/ath_rate_atheros.ko

#  insmod $MODULE_PATH/ath_spectral.ko $SPECTRAL_ARGS
if [ "${AP_NO_A_BAND}" != "1" ]; then
#load DFS if A band is supported,default is supported and set AP_NO_A_BAND=1 if not supported

5. insmod $MODULE_PATH/ath_dfs.ko $DFS_ARGS
fi

6. insmod $MODULE_PATH/ath_dev.ko

7. insmod $MODULE_PATH/umac.ko

8. insmod $MODULE_PATH/ath_pktlog.ko

10. rm -f /tmp/.apup

#
# If Country Code has been specified, it needs to be set here using
# the iwpriv command.  This will be common for all VAPs
#

if [ "$ATH_countrycode" != "" ]; then

11. iwpriv wifi0 setCountryID $ATH_countrycode

##
## Check the result of this command.  If the country code
## is invalid, it will return a non-zero result.  We should
## die a horrible death in this case
##

if [ $? != 0 ]; then
_ath_unload
exit 255
fi

12. iwpriv wifi1 setCountryID $ATH_countrycode




1. insmod adf.ko

   /* 该模块装载时并没有做任何事， 只是export symbol供其他模块使用 */

2. insmod asf.ko

    /* 该模块装载时没有做任何事， 只是export symbol供其他模块使用 */

3. insmod ath_hal.ko

   /* 该模块装载时， 只是打印了 ath_hal.ko 的build option, export symbol*/
   ath_hal: 0.9.17.1 (AR5416, AR9380, REGOPS_FUNC, WRITE_EEPROM, 11D)
            0.9.17.1 ath_hal ko version

4. insmod ath_rate_atheros.ko

    /* 该模块装载时， 只是打印了一些信息， export symbol */
    ath_rate_atheros: Copyright (c) 2001-2005 Atheros Communications, Inc, All Rights Reserved

5. insmod ath_dfs.ko $DFS_ARGS

    /* 该模块装载时， 只是打印了一些信息， export symbol */
    ath_dfs: Version 2.0.0
    Copyright (c) 2005-2006 Atheros Communications, Inc. All Rights Reserved


6. insmod ath_dev.ko

    /* 该模块装载时， 打印了一些信息， export symbol */
    ath_dev: Copyright (c) 2001-2007 Atheros Communications, Inc, All Rights Reserved

    6.1 ath_create_proc_debug();
    6.2 ath_create_proc_version();
    6.3 no_buf_work_run_proc_init();
    #if ATH_DEBUG   /*该编译选项已打开 */
    6.4 ath_create_proc_rtscts();
    6.5 ath_create_proc_nodefixedrate();
    #endif

    6.1 static void ath_create_proc_debug(void)
    {
        parent_entry = create_proc_entry("athdebug", 0644, NULL);
        parent_entry->nlink = 1;
        parent_entry->data = (void *)&ath_ioctl_debug;
        parent_entry->read_proc = ath_proc_read;
        parent_entry->write_proc = ath_proc_write;
    }

    6.2 static void ath_create_proc_version(void)
        {
            parent_version_entry = create_proc_entry("athversion", 0644, NULL);
            parent_version_entry->nlink = 1;
            parent_version_entry->data = (void *)NULL;
            parent_version_entry->read_proc = ath_proc_read_version;
            parent_version_entry->write_proc = ath_proc_write_version;
        }

   6.3 int no_buf_work_run_proc_init(void) /* lmac/ath_dev/ath_xmit.c */
       {
           if(procRegDir == NULL)
               procRegDir = proc_mkdir("privdir", NULL);

           if(procRegDir)
           {    
               entry_no_buf_time = create_proc_entry("nobuftime", 0, procRegDir);
               if(entry_no_buf_time)
               {    
                   entry_no_buf_time->proc_fops = &no_buf_time_ops; 
               }    
           }    

           return 0;
       }

   6.4 static void ath_create_proc_rtscts(void)
       {
           parent_rtscts_entry = create_proc_entry("athrtscts", 0644, NULL);
           parent_rtscts_entry->nlink = 1;
           parent_rtscts_entry->data = (void *)&ath_rtscts_enable;
           parent_rtscts_entry->read_proc = ath_proc_read_rtscts;
           parent_rtscts_entry->write_proc = ath_proc_write_rtscts;
       }

   6.5 static void ath_create_proc_nodefixedrate(void)
       {
            parent_nodefixedrate_entry = create_proc_entry("athnodefixedrate", 0644, NULL);
            parent_nodefixedrate_entry->nlink = 1;
            parent_nodefixedrate_entry->data = (void *)NULL;
            parent_nodefixedrate_entry->read_proc = ath_proc_read_nodefixedrate;
            parent_nodefixedrate_entry->write_proc = ath_proc_write_nodefixedrate;
       }


7. insmod umac.ko




