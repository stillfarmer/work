1.1
typedef struct ieee80211vap {
    TAILQ_ENTRY(ieee80211vap)         iv_next;    /* list of vap instances */

#if WAR_DELETE_VAP
    void                              *iv_athvap; /* opaque ath vap pointer */
#endif

    struct ieee80211com               *iv_ic;     /* back ptr to common state */
//    struct net_device_stats           iv_devstats; /* interface statistics */
    u_int                             iv_unit;    /* virtual AP unit */

    void                              *iv_priv;   /* for extending vap functionality */
    os_if_t                           iv_ifp;     /* opaque handle to OS-specific interface */
    wlan_event_handler_table          *iv_evtable;/* vap event handlers */
    
    os_handle_t                       iv_mlme_arg[IEEE80211_MAX_VAP_MLME_EVENT_HANDLERS];  /* opaque handle used for mlme event handler */
    wlan_mlme_event_handler_table     *iv_mlme_evtable[IEEE80211_MAX_VAP_MLME_EVENT_HANDLERS];  /* mlme event handlers */
    
    os_handle_t                       iv_misc_arg[IEEE80211_MAX_MISC_EVENT_HANDLERS];
    wlan_misc_event_handler_table     *iv_misc_evtable[IEEE80211_MAX_MISC_EVENT_HANDLERS];

    void*                             iv_event_handler_arg[IEEE80211_MAX_VAP_EVENT_HANDLERS];
    ieee80211_vap_event_handler       iv_event_handler[IEEE80211_MAX_VAP_EVENT_HANDLERS];

    os_if_t                           iv_ccx_arg;  /* opaque handle used for ccx handler */
    wlan_ccx_handler_table            *iv_ccx_evtable;  /* ccx handlers */

    u_int32_t                         iv_debug;   /* debug msg flags */

    u_int8_t                          iv_myaddr[IEEE80211_ADDR_LEN]; /* current mac address */
    u_int8_t                          iv_my_hwaddr[IEEE80211_ADDR_LEN]; /* mac address from EEPROM */

    enum ieee80211_opmode             iv_opmode;  /* operation mode */
    int                               iv_scan_priority_base;  /* Base value used to determine priority of scans requested by this VAP */
    u_int32_t                         iv_create_flags;   /* vap create flags */

    u_int32_t                         iv_flags;   /* state flags */
    u_int32_t                         iv_flags_ext;   /* extension of state flags */

/* not multiple thread safe */
    u_int32_t                         iv_deleted:1,  /* if the vap deleted by user */
                                      iv_active:1,   /* if the vap is active */
                                      iv_scanning:1, /* if the vap is scanning */
                                      iv_smps:1,     /* if the vap is in static mimo ps state */
                                      iv_ready:1,    /* if the vap is ready to send receive data */
                                      iv_standby:1,  /* if the vap is temporarily stopped */
                                      iv_cansleep:1, /* if the vap can sleep*/
                                      iv_sw_bmiss:1, /* use sw bmiss timer */
                                      iv_copy_beacon:1, /* enable beacon copy */
                                      iv_wapi:1,
                                      iv_sta_fwd:1, /* set to enable sta-fws fweature */
                                      iv_dynamic_mimo_ps, /* dynamic mimo ps enabled */
                                      iv_doth:1,     /* 802.11h enabled */
                                      iv_country_ie:1,  /* Country IE enabled */
                                      iv_wme:1, /* wme enabled */
                                      iv_dfswait:1,   /* if the vap is in dfswait state */
                                      iv_erpupdate:1, /* if the vap has erp update set */
                                      iv_needs_scheduler:1, /* this vap needs scheduler for off channel operation */
                                      iv_vap_ind:1,   /* if the vap has wds independance set */
                                      iv_forced_sleep:1,        /*STA in forced sleep set PS bit for all outgoing frames */
                                      iv_ap_reject_dfs_chan:1,  /* SoftAP to reject resuming in DFS channels */
                                      iv_mfp_test:1;  /* test flag for MFP */
        
    enum ieee80211_state    iv_state;    /* state machine state */


/* multiple thread safe */
    u_int32_t                         iv_caps;    /* capabilities */
    u_int16_t                         iv_ath_cap; /* Atheros adv. capablities */
    u_int8_t                          iv_chanchange_count; /* 11h counter for channel change */
    int                               iv_mcast_rate; /* Multicast rate (Kbps) */
    int                               iv_mcast_fixedrate; /* fixed rate for multicast */
    u_int32_t                         iv_node_count; /* node count */
    atomic_t                          iv_rx_gate; /* pending rx threads */
    struct ieee80211_stats            iv_stats; /* for backward compatibility */
    struct ieee80211_mac_stats        iv_unicast_stats;   /* mac statistics for unicast frames */
    struct ieee80211_mac_stats        iv_multicast_stats; /* mac statistics for multicast frames */
    struct tkip_countermeasure        iv_unicast_counterm;  /* unicast tkip countermeasure */
    struct tkip_countermeasure        iv_multicast_counterm;  /* unicast tkip countermeasure */

    spinlock_t                        iv_lock; /* lock to protect access to vap object data */
    u_int32_t                         *iv_aid_bitmap; /* association id map */
    u_int16_t                         iv_max_aid;
    u_int16_t                         iv_sta_assoc;   /* stations associated */
    u_int16_t                         iv_ps_sta;  /* stations in power save */
    u_int16_t                         iv_ps_pending;  /* ps sta's w/ pending frames */
    u_int8_t                          iv_dtim_period; /* DTIM period */
    u_int8_t                          iv_dtim_count;  /* DTIM count from last bcn */
    u_int8_t                          iv_atim_window; /* ATIM window */
    u_int8_t                          *iv_tim_bitmap; /* power-save stations w/ data*/
    u_int16_t                         iv_tim_len;     /* ic_tim_bitmap size (bytes) */
                                      /* set/unset aid pwrsav state */
    void                              (*iv_set_tim)(struct ieee80211_node *, int,bool isr_context);
    struct ieee80211_node             *iv_bss;    /* information for this node */
    struct ieee80211_rateset          iv_op_rates[IEEE80211_MODE_MAX]; /* operational rate set by os */
    struct ieee80211_fixed_rate       iv_fixed_rate;  /* 802.11 rate or -1 */
    u_int32_t                         iv_fixed_rateset;  /* 802.11 rate set or -1(invalid) */
    u_int32_t                         iv_fixed_retryset; /* retries */
    u_int16_t                         iv_rtsthreshold;
    u_int16_t                         iv_fragthreshold;
    u_int16_t                         iv_def_txkey;   /* default/group tx key index */
#if ATH_SUPPORT_AP_WDS_COMBO
    u_int8_t                          iv_no_beacon;   /* VAP does not transmit beacon/probe resp. */
#endif
    struct ieee80211_key              iv_nw_keys[IEEE80211_WEP_NKID];

    struct ieee80211_rsnparms         iv_rsn;         /* rsn information */
    ieee80211_privacy_exemption       iv_privacy_filters[IEEE80211_MAX_PRIVACY_FILTERS];    /* privacy filters */
    u_int32_t                         iv_num_privacy_filters;
    ieee80211_pmkid_entry             iv_pmkid_list[IEEE80211_MAX_PMKID];
    u_int16_t                         iv_pmkid_count;
    u_int8_t                          iv_wps_mode;    /* WPS mode */
    u_int8_t                          iv_wep_mbssid;    /* force multicast wep keys in first 4 entries 0=yes, 1=no */

    enum ieee80211_phymode            iv_des_mode; /* desired wireless mode for this interface. */
    u_int16_t                         iv_des_modecaps;   /* set of desired phy modes for this VAP */
    enum ieee80211_phymode            iv_cur_mode; /* current wireless mode for this interface. */
    struct ieee80211_channel          *iv_des_chan[IEEE80211_MODE_MAX]; /* desired channel for each PHY */
    struct ieee80211_channel          *iv_bsschan;   /* bss channel */
    u_int8_t                          iv_des_ibss_chan;   /* desired ad-hoc channel */
    u_int8_t                          iv_rateCtrlEnable;  /* enable rate control */
    int                               iv_des_nssid;       /* # desired ssids */
    ieee80211_ssid                    iv_des_ssid[IEEE80211_SCAN_MAX_SSID];/* desired ssid list */

    int                               iv_bmiss_count;
    int                               iv_bmiss_count_for_reset;
    int                               iv_bmiss_count_max;
    systime_t                         iv_last_beacon_time;         /* absolute system time, not TSF */
    systime_t                         iv_last_directed_frame;      /* absolute system time; not TSF */
    systime_t                         iv_last_ap_frame;            /* absolute system time; not TSF */
    systime_t                         iv_last_traffic_indication;  /* absolute system time; not TSF */
    systime_t                         iv_lastdata;                 /* absolute system time; not TSF */
    u_int64_t                         iv_txrxbytes;                /* No. of tx/rx bytes  */
    ieee80211_power_t                 iv_power;                    /* handle private to power module */
    ieee80211_sta_power_t             iv_pwrsave_sta;
    ieee80211_pwrsave_smps_t          iv_pwrsave_smps;
    u_int16_t                         iv_keep_alive_timeout;       /* keep alive time out */
    u_int16_t                         iv_inact_count;               /* inactivity count */
    u_int8_t                          iv_smps_rssithresh;
    u_int8_t                          iv_smps_datathresh;

    u_int8_t                          iv_lastbcn_phymode_mismatch;        /* Phy mode mismatch between scan entry, BSS */
    /* NEW APP IE implementation. Note, we need to obselete the old one */
    APP_IE_LIST_HEAD                  iv_app_ie_list[IEEE80211_APPIE_MAX_FRAMES];

    /* TODO: we need to obselete the use of these 2 fields */
    /* app ie buffer */
    struct ieee80211_app_ie_t         iv_app_ie[IEEE80211_APPIE_MAX_FRAMES];
    u_int16_t                         iv_app_ie_maxlen[IEEE80211_APPIE_MAX_FRAMES];

    IEEE80211_VEN_IE                 *iv_venie;
    
    /* opt ie buffer - currently used for probereq and assocreq */
    struct ieee80211_app_ie_t         iv_opt_ie;
    u_int16_t                         iv_opt_ie_maxlen;

    /* Copy of the beacon frame */
    u_int8_t                          *iv_beacon_copy_buf;
    int                               iv_beacon_copy_len;

    /* country ie data */
    u_int16_t                         iv_country_ie_chanflags;
    struct ieee80211_ie_country       iv_country_ie_data; /* country info element */

    u_int8_t                      iv_mlmeconnect;     /* Assoc state machine roaming mode */

    /* U-APSD Settings */
    u_int8_t                          iv_uapsd;
    u_int8_t                          iv_wmm_enable;
    u_int8_t                          iv_wmm_power_save;

    ieee80211_mlme_priv_t             iv_mlme_priv;    /* opaque handle to mlme private information */ 

    ieee80211_scan_table_t            iv_scan_table;          /* bss list */
    ieee80211_aplist_config_t         iv_aplist_config;

    ieee80211_candidate_aplist_t      iv_candidate_aplist;    /* opaque handle to aplist private information */ 

    ieee80211_resmgr_vap_priv_t       iv_resmgr_priv;         /* opaque handle with resource manager private info */ 

    ieee80211_vap_pause_info          iv_pause_info;          /* pause private info */ 

    ieee80211_vap_state_info          iv_state_info;          /* vap private state information */ 
    ieee80211_txrx_event_info         iv_txrx_event_info;     /* txrx event handler private data */
    ieee80211_acl_t                   iv_acl;   /* opaque handle to acl */ 

    ieee80211_vap_ath_info_t          iv_vap_ath_info_handle; /* opaque handle for VAP_ATH_INFO */

    u_int8_t                          iv_ht40_intolerant;
    u_int8_t                          iv_chwidth;
    u_int8_t                          iv_chextoffset;
    u_int8_t                          iv_disable_HTProtection;
    u_int32_t                         iv_chscaninit;

#if UMAC_SUPPORT_TDLS
    /* TDLS information */
    u_int32_t                         iv_tdls_macaddr1;
    u_int32_t                         iv_tdls_macaddr2;
    u_int32_t                         iv_tdls_action;

    struct ieee80211_tdls             *iv_tdlslist;
    ieee80211_tdls_ops_t              iv_tdls_ops;	
#endif /* UMAC_SUPPORT_TDLS */

    /* Optional feature: mcast enhancement */
#if ATH_SUPPORT_IQUE
    struct ieee80211_ique_me          *iv_me;
    struct ieee80211_hbr_list         *iv_hbr_list;
#endif

    struct ieee80211_ique_ops         iv_ique_ops; 
    /* channel_change_done is a flag value used to indicate that a channel 
     * change happend for this VAP. This information (for now) is used to update 
     * the beacon information and then reset. This is needed in case of DFS channel change 
     */
    u_int8_t channel_change_done;  

    ieee80211_vap_input_mgmt_filter   iv_input_mgmt_filter;   /* filter  input mgmt frames */
    ieee80211_vap_output_mgmt_filter  iv_output_mgmt_filter;  /* filter outpur mgmt frames */
    int                               (*iv_up)(struct ieee80211vap *);
    int                               (*iv_join)(struct ieee80211vap *);
    int                               (*iv_down)(struct ieee80211vap *);
    int                               (*iv_listen)(struct ieee80211vap *);
    int                               (*iv_stopping)(struct ieee80211vap *);
    void                              (*iv_update_ps_mode)(struct ieee80211vap *);
                                
    int                               (*iv_key_alloc)(struct ieee80211vap *,
                                                      struct ieee80211_key *);
    int                               (*iv_key_delete)(struct ieee80211vap *, 
                                                       const struct ieee80211_key *,
                                                       struct ieee80211_node *);
    int                               (*iv_key_set)(struct ieee80211vap *,
                                                    const struct ieee80211_key *,
                                                    const u_int8_t mac[IEEE80211_ADDR_LEN]);
    void                              (*iv_key_update_begin)(struct ieee80211vap *);
    void                              (*iv_key_update_end)(struct ieee80211vap *);
    void                              (*iv_update_node_txpow)(struct ieee80211vap *, u_int16_t , u_int8_t *);
    
    int                               (*iv_reg_vap_ath_info_notify)(struct ieee80211vap *,
                                                                    ath_vap_infotype, 
                                                                    ieee80211_vap_ath_info_notify_func,
                                                                    void *);
    int                               (*iv_vap_ath_info_update_notify)(struct ieee80211vap *,
                                                                       ath_vap_infotype);
    int                               (*iv_dereg_vap_ath_info_notify)(struct ieee80211vap *);
    int                               (*iv_vap_ath_info_get)(struct ieee80211vap *, 
                                                             ath_vap_infotype, 
                                                             u_int32_t *, u_int32_t *);
    
#if ATH_WOW
    u_int8_t                          iv_wowflags;         /* Flags for wow */
#define IEEE80211_F_WOW_DEAUTH             1               /* Deauth Pattern */
#define IEEE80211_F_WOW_DISASSOC           2               /* Disassoc Pattern */
    struct ieee80211_wowpattern       iv_patterns[8];      /* Patterns status */
#endif
    u_int8_t                          iv_ccmpsw_seldec;  /* Enable/Disable encrypt/decrypt of frames selectively ( frames with KEYMISS) */    
    u_int16_t                         iv_mgt_rate;       /* rate to be used for management rates */  
#if UMAC_SUPPORT_NAWDS
    struct ieee80211_nawds            iv_nawds;
#endif
    ieee80211_vap_tsf_offset          iv_tsf_offset;    /* TSF-related data utilized for concurrent multi-channel operations */
#if defined(ATH_CWMIN_WORKAROUND) && defined(ATH_SUPPORT_HTC) && defined(ATH_USB)
    u_int8_t                          iv_vi_need_cwmin_workaround;
#endif /*ATH_CWMIN_WORKAROUND ,ATH_SUPPORT_HTC ,ATH_USB*/
#ifdef ATH_SUPPORT_QUICK_KICKOUT
    u_int8_t						  iv_sko_th;        /* station kick out threshold */ 
#endif /*ATH_SUPPORT_QUICK_KICKOUT*/   
    /* add flag to enable/disable auto-association */
    u_int8_t    auto_assoc;
#if ATH_SW_WOW
    u_int8_t                          iv_sw_wow;           /* Flags for sw wow */
#endif
#ifdef ATH_SUPPORT_TxBF
    u_int8_t    iv_txbfmode;
    u_int8_t    iv_autocvupdate;
    u_int8_t    iv_cvupdateper;
#endif
    struct ieee80211_node             *iv_ni;
    struct ieee80211_channel          *iv_cswitch_chan;
    u_int8_t                          iv_cswitch_rxd;
    os_timer_t                        iv_cswitch_timer;
#ifdef MAGPIE_HIF_GMAC
    u_int8_t    iv_chanswitch;
#endif    
#if ATH_SUPPORT_WAPI
    u32    iv_wapi_urekey_pkts;/*wapi unicast rekey packets, 0 for disable*/
    u32    iv_wapi_mrekey_pkts;/*wapi muiticast rekey packets, 0 for disable*/	
#endif
#if ATH_SUPPORT_IBSS_DFS
    struct ieee80211_ibssdfs_ie iv_ibssdfs_ie_data; /* IBSS DFS element */
    u_int8_t                    iv_measrep_action_count_per_tbtt;
    u_int8_t                    iv_csa_action_count_per_tbtt;
    u_int8_t                    iv_ibssdfs_recovery_count;
    u_int8_t                    iv_ibssdfs_state;
    u_int8_t                    iv_ibss_dfs_csa_threshold;
    u_int8_t                    iv_ibss_dfs_csa_measrep_limit;
    u_int8_t                    iv_ibss_dfs_enter_recovery_threshold_in_tbtt;
#endif /* ATH_SUPPORT_IBSS_DFS */
#if ATH_SUPPORT_IBSS_NETLINK_NOTIFICATION
#define IBSS_RSSI_CLASS_MAX 7
    u_int8_t			iv_ibss_rssi_monitor;
    u_int8_t			iv_ibss_rssi_class[IBSS_RSSI_CLASS_MAX];
    u_int8_t			iv_ibss_rssi_hysteresis;
#endif
#if ATH_SUPPORT_FLOWMAC_MODULE
    int                         iv_dev_stopped;
    int                         iv_flowmac;
#endif
    u_int8_t iv_wep_keycache;/*forstatic wep keys are allocated in first four slots in keycahe */
#if ATH_SUPPORT_WPA_SUPPLICANT_CHECK_TIME
	int 				iv_rejoint_attemp_time;
#endif	
    struct ieee80211_tim_set iv_tim_infor;
    rwlock_t            iv_tim_update_lock;
	char	if_name[16];
} IEEE80211VAP, *PIEEE80211VAP;

1.2

