1. user rate detect.   /* 已完成 */

2. user priority set.  /* 已完成 */

3. queue arg set.      /* 进行中 */

    3.1 get80211priv(ifname, IEEE80211_IOCTL_GETWMMPARAMS, param, sizeof(param))   os/linux/tools/wlanconfig.c

    3.2 IEEE80211_IOCTL_GETWMMPARAMS

        3.2.1 ieee80211_ioctl_getwmmparams(struct net_device *dev, 
              struct iw_request_info *info, void *w, char *extra)                    os/linux/src/ieee80211_wireless.c

            3.2.1.1 wlan_get_wmm_param(vap, WLAN_WME_CWMIN, bss, ac)
                    {
                        chanParams->cap_wmeParams[ac].wmep_logcwmin;
                    }

   用iwpriv ath0 去设置

   set,  有些值设置时提示invalid argument.

4. how to test.
