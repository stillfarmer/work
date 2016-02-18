1. struct 

    1.1 
/*
 * Data transmit queue state.  One of these exists for each
 * hardware transmit queue.  Packets sent to us from above
 * are assigned to queues based on their priority.  Not all
 * devices support a complete set of hardware transmit queues.
 * For those devices the array sc_ac2q will map multiple
 * priorities to fewer hardware queues (typically all to one
 * hardware queue).
 */
struct ath_txq {
    u_int           axq_qnum;       /* hardware q number */
    u_int32_t       *axq_link;      /* link ptr in last TX desc */
    TAILQ_HEAD(, ath_buf)   axq_q;  /* transmit queue */
    spinlock_t      axq_lock;       /* lock on q and link */
    unsigned long   axq_lockflags;  /* intr state when must cli */
    u_int           axq_depth;      /* queue depth */
#if ATH_TX_BUF_FLOW_CNTL
    u_int           axq_minfree;    /* Number of free tx_bufs required in common buf pool */
    u_int           axq_num_buf_used; /* Number of used tx_buf for this q */
#endif
    u_int8_t        axq_aggr_depth; /* aggregates queued */
    u_int32_t       axq_totalqueued;/* total ever queued */
    u_int           axq_intrcnt;	/* count to determine if descriptor
                                     * should generate int on this txq.
                                     */
	/*
	 * State for patching up CTS when bursting.
	 */
	struct	ath_buf     *axq_linkbuf;   /* virtual addr of last buffer*/
	struct	ath_desc    *axq_lastdsWithCTS; /* first desc of the last descriptor
                                             * that contains CTS
                                             */
	struct	ath_desc    *axq_gatingds;	/* final desc of the gating desc
                                         * that determines whether lastdsWithCTS has
                                         * been DMA'ed or not
                                         */
    /*
     * Staging queue for frames awaiting a fast-frame pairing.
     */
    TAILQ_HEAD(axq_headtype, ath_buf) axq_stageq;
    TAILQ_HEAD(,ath_atx_ac) axq_acq;

#ifdef ATH_SUPERG_COMP
    /* scratch compression buffer */
    char                    *axq_compbuf;   /* scratch comp buffer */
    dma_addr_t              axq_compbufp;  /* scratch comp buffer (phys)*/
    u_int                   axq_compbufsz; /* scratch comp buffer size */
    OS_DMA_MEM_CONTEXT(axq_dmacontext)
#endif
#if defined(ATH_SWRETRY) && defined(ATH_SWRETRY_MODIFY_DSTMASK)
    u_int                   axq_destmask:1; /*Destination mask for this queue*/
#endif
#if ATH_TX_POLL
    int            axq_lastq_tick; /* ticks when last packet queued */
#endif
    TAILQ_HEAD(, ath_buf)   axq_fifo[HAL_TXFIFO_DEPTH];
    u_int8_t                axq_headindex;
    u_int8_t                axq_tailindex;
    u_int                   irq_shared:1; /* This queue is shared by the irq_handler() */
#if ATH_HW_TXQ_STUCK_WAR
    u_int8_t               tx_done_stuck_count;
#endif
};




    1.2 

struct ath_softc {
#ifdef ATH_SUPPORT_HTC
    void                    *sc_host_wmi_handle;
#endif
    struct ath_ops          sc_ath_ops;

    ieee80211_handle_t      sc_ieee;
    struct ieee80211_ops    *sc_ieee_ops;

    osdev_t                 sc_osdev;   /* Opaque handle to OS-specific device */
    spinlock_t              sc_lock;    /* softc-level lock */

    struct ath_reg_parm     sc_reg_parm;/* per-instance attach-time parameters */
    struct ath_config       sc_config;  /* per-instance load-time parameters */
    struct ath_stats        sc_stats;   /* private statistics 此处就是 athstats打出的值， 默认wifi0 */   
#ifdef ATH_SUPPORT_DFS
    struct ath_dfs          *sc_dfs;
    struct dfs_hang_war     sc_dfs_hang;
#endif
#ifdef ATH_TX99_DIAG
    struct ath_tx99         *sc_tx99;
#ifndef ATH_SUPPORT_HTC    
    TAILQ_HEAD(,ath_buf)    tx99_tmpq;
#endif
#endif
#if ATH_SUPPORT_SPECTRAL
    struct ath_spectral     *sc_spectral;
    int                      sc_spectral_scan;
    int                      sc_spectral_full_scan;
    u_int32_t                sc_spectral_samp_count;
#endif
#if ATH_SUPPORT_RAW_ADC_CAPTURE
    int 				sc_raw_adc_capture_enabled;
    HAL_OPMODE 			sc_save_current_opmode;
    HAL_CHANNEL 		sc_save_current_chan;
    u_int8_t            sc_save_rx_chainmask;
    u_int16_t           sc_adc_num_chains;     /* number of chains */
    u_int16_t           sc_adc_chain_mask;     /* chain mask (lsb=ch0) */
    u_int16_t           sc_adc_freq;         /* setting in Mhz */
    u_int16_t           sc_adc_ieee;         /* IEEE channel number */
    u_int32_t           sc_adc_chan_flags;   /* channel flags */
	u_int32_t			sc_adc_capture_flags;
#endif
    /* Green AP support structure */
    struct ath_green_ap     *sc_green_ap;
    spinlock_t   green_ap_ps_lock;
    u_int32_t green_ap_dbg_rx_cnt;    
    /* private PHY statisitics for each PHY mode */
    struct ath_phy_stats    sc_phy_stats[WIRELESS_MODE_MAX];

    int                     sc_debug;
    SetDefAntenna_callback  sc_setdefantenna;
    void                    *sc_bdev;   /* associated bus device */
    
    struct ath_hal          *sc_ah;     /* Atheros HAL */
    struct atheros_softc    *sc_rc;     /* tx rate control support */

    u_int32_t               sc_intrstatus; /* XXX HAL_STATUS */
    spinlock_t              sc_intrstatus_lock;

    atomic_t                sc_inuse_cnt;

    /* Properties */
    unsigned int
        sc_invalid             : 1, /* being detached */
        sc_needmib             : 1, /* enable MIB stats intr */
        sc_hasdiversity        : 1, /* rx diversity available */
        sc_diversity           : 1, /* enable rx diversity */
        sc_olddiversity        : 1, /* diversity setting before XR enable */
        sc_haswme              : 1, /* wme priority queueing support */
        sc_hascompression      : 1, /* compression support */
        sc_hasveol             : 1, /* tx VEOL support */
        sc_hastpc              : 1, /* per-packet TPC support */
        sc_dturbo              : 1, /* dynamic turbo capable */
        sc_dturbo_switch       : 1, /* turbo switch mode*/
        sc_rate_recn_state     : 1, /* dynamic turbo state recmded by ratectrl */
        sc_ignore_ar           : 1, /* ignore AR during transision*/
        sc_hasbmask            : 1, /* bssid mask support */
        sc_hastsfadd           : 1, /* tsf adjust support */
        sc_scanning            : 1, /* scanning active */
        sc_nostabeacons        : 1, /* no beacons for station */
        sc_xrgrppoll           : 1, /* xr group polls are active */
        sc_hasclrkey           : 1, /* CLR key supported */
        sc_devstopped          : 1, /* stopped 'cuz no tx bufs */
        sc_stagbeacons         : 1, /* use staggered beacons */
        sc_rtasksched          : 1, /* radar task is scheduled */
        sc_hasrfkill           : 1, /* RfKill is enabled */
#ifdef ATH_RFKILL
        sc_rfkillpollenabled   : 1, /* RFKill poll timer is enabled */
#endif	//ifdef ATH_RFKILL
        sc_hw_phystate         : 1, /* hardware (GPIO) PHY state */
        sc_sw_phystate         : 1, /* software PHY state */
        sc_txaggr              : 1, /* enable 11n tx aggregation */
        sc_rxaggr              : 1, /* enable 11n rx aggregation */
        sc_hasautosleep        : 1, /* automatic sleep after TIM */
        sc_waitbeacon          : 1, /* waiting for first beacon after waking up */
        sc_no_tx_3_chains      : 1, /* user, hardware, regulatory or country
                                     * may disallow transmit on three chains. */
        sc_cus_cts_set         : 1; /* customer specified cts rate */

    unsigned int
        sc_update_chainmask    : 1, /* change chain mask */
        sc_rx_chainmask_detect : 1, /* enable rx chain mask detection */
        sc_rx_chainmask_start  : 1, /* start rx chain mask detection */
        sc_txrifs              : 1, /* enable 11n tx rifs */
        sc_hashtsupport        : 1, /* supports 11n */
        sc_ht20sgisupport      : 1, /* supports short GI in ht20 mode */
        sc_txstbcsupport       : 1,
        sc_rxstbcsupport       : 2,
        sc_log_rcfind          : 1, /* log rcfind */
        sc_txamsdu             : 1, /* enable 11n tx amsdu */
        sc_swRetryEnabled      : 1, /* support for sw retrying mechanism */
        sc_full_reset          : 1, /* force full reset */
        sc_uapsdsupported      : 1, /* supports UAPSD */
#if ATH_WOW
        sc_hasWow              : 1, /* Is WoW feature present in HW */
        sc_wow_sleep           : 1, /* In the middle of Wow Sleep? */
        sc_wow_bmiss_intr      : 1, /* Beacon Miss workaround during WOW sleep. */
        sc_wowenable           : 1, /* plt support */
#endif
        sc_slowAntDiv          : 1, /* enable slow antenna diversity */
        sc_removed             : 1, /* card has been physically removed */
        sc_wpsgpiointr         : 1, /* WPS Push button GPIO interrupt enabled */
#ifdef ATH_BT_COEX
        sc_hasbtcoex           : 1,
#endif
#ifdef ATH_GEN_TIMER
        sc_hasgentimer         : 1,
#endif
#if ATH_ANT_DIV_COMB
        sc_antDivComb          : 1, /* enable antenna diversity and combining feature for KITE */
#endif
#if ATH_SUPPORT_WIRESHARK
        sc_tapmonenable        : 1, /* tap monitor mode */
#endif

        sc_wpsbuttonpushed     : 1, /* WPS Push Button status */
        sc_enhanceddmasupport  : 1, /* Enhanced DMA supported */
#ifdef  ATH_SUPPORT_TxBF
        sc_txbfsupport         : 1, /* Tx BF supported */
#endif
        sc_bbpanicsupport      : 1, /* BB Panic supported */
        sc_ldpcsupport         : 1,
        sc_fastabortenabled    : 1, /* fast rx/tx abort */
        sc_hwuapsdtrig         : 1, /* HW Uapsd trigger */
        sc_edmarxdpc           : 1 /* flag to indicate edma rxdpc */
#if ATH_SUPPORT_DYN_TX_CHAINMASK
        ,sc_dyn_txchainmask     : 1 /* dynamic tx chain support */
#endif /* ATH_SUPPORT_DYN_TX_CHAINMASK */
        ,sc_enableapm           : 1 /* apm enabled */
#if ATH_SUPPORT_FLOWMAC_MODULE
        ,sc_osnetif_flowcntrl   : 1 /* flow control with os network stack enabled */
        ,sc_ethflowmac_enable   : 1 /* enable flow control between ethernet and 802.11 interface */
#endif
        ; /* end of the bit fields here, see ',' above and last one should have a ';' */
      
    u_int32_t       sc_3stream_sigflag1 ;  /* signature flag for 3 stream fix */
    u_int32_t       sc_3stream_sigflag2 ;  /* signature flag for 3 stream fix */
    u_int32_t       sc_3streamfix_engaged ;  /* flag for stating 3 stream fix is engaged */      
    ATH_RESET_TYPE          sc_reset_type; /* reason for calling ath_internal_reset */
#if ATH_VOW_EXT_STATS
    unsigned int  sc_vowext_stats:1; /* enable/disable extended stats */
#endif

    atomic_t                sc_in_reset;    /* reset in progress */
    atomic_t                sc_rx_hw_en;    /* Rx Hw is enabled and running */

#ifdef ATH_RB
    ath_rb_mode_t           sc_rxrifs;  /* enable/disable rb detection */
    u_int16_t               sc_rxrifs_timeout; /* rb timeout value*/
    u_int8_t                sc_rxrifs_skipthresh; /* rb skip thresh value*/
#endif
    HAL_OPMODE              sc_opmode;  /* current operating mode */
    u_int8_t                sc_coverageclass;

    /* rate tables */
    const HAL_RATE_TABLE    *sc_rates[WIRELESS_MODE_MAX];
    const HAL_RATE_TABLE    *sc_currates;   /* current rate table */
    const HAL_RATE_TABLE    *sc_xr_rates;   /* XR rate table */
    const HAL_RATE_TABLE    *sc_half_rates; /* half rate table */
    const HAL_RATE_TABLE    *sc_quarter_rates; /* quarter rate table */
    WIRELESS_MODE           sc_curmode;     /* current phy mode */
    u_int16_t               sc_curtxpow;    /* current tx power limit */
    u_int16_t               sc_curaid;      /* current association id */
    HAL_CHANNEL             sc_curchan;     /* current h/w channel */
    HAL_CHANNEL             sc_extchan;     /* extension h/w channel */
    u_int8_t                sc_curbssid[IEEE80211_ADDR_LEN];
    u_int8_t                sc_myaddr[IEEE80211_ADDR_LEN]; /* current mac address */
    u_int8_t                sc_my_hwaddr[IEEE80211_ADDR_LEN]; /* mac address from EEPROM */
    u_int8_t                sc_rixmap[256]; /* IEEE to h/w rate table ix */
    struct {
        u_int32_t           rateKbps;       /* transfer rate in kbs */
        u_int8_t            ieeerate;       /* IEEE rate */
        u_int8_t            flags;          /* radiotap flags */
        u_int16_t           ledon;          /* softled on time */
        u_int16_t           ledoff;         /* softled off time */
    } sc_hwmap[256];         /* h/w rate ix mappings */
    u_int32_t               sc_max_wep_tkip_ht20_tx_rateKbps;
    u_int32_t               sc_max_wep_tkip_ht40_tx_rateKbps;
    u_int32_t               sc_max_wep_tkip_ht20_rx_rateKbps;
    u_int32_t               sc_max_wep_tkip_ht40_rx_rateKbps;

#if ATH_SUPPORT_IQUE
    struct {
        u_int8_t            per_threshold;
        u_int8_t            probe_interval;
    } sc_rc_params[2];
    struct {
        u_int8_t            use_rts;
        u_int8_t            aggrsize_scaling;
        u_int32_t           min_kbps;
    } sc_ac_params[WME_NUM_AC];
    struct {
        u_int8_t            hbr_enable;
        u_int8_t            hbr_per_low;
    } sc_hbr_params[WME_NUM_AC];
    u_int                   sc_retry_duration;
    u_int8_t                sc_hbr_per_high;
    u_int8_t                sc_hbr_per_low;    
#endif

#if  ATH_SUPPORT_AOW
    ath_aow_t   sc_aow;
#endif  /* ATH_SUPPORT_AOW */

    u_int8_t                sc_minrateix;   /* min h/w rate index */
    u_int8_t                sc_protrix;     /* protection rate index */
    PROT_MODE               sc_protmode;    /* protection mode */
    u_int8_t                sc_mcastantenna;/* Multicast antenna number */
    u_int8_t                sc_txantenna;   /* data tx antenna (fixed or auto) */
    u_int8_t                sc_cur_txant;   /* current tx antenna */
    u_int8_t                sc_nbcnvaps;    /* # of vaps sending beacons */
    u_int8_t                sc_nslots;      /* # of beacon slots allocated for staggered beacons case */
    ath_beacon_lock_t       sc_beacon_lock;    /* lock to be used by beacon module */
    u_int16_t               sc_nvaps;       /* # of active virtual ap's */
    atomic_t                sc_nsta_vaps_up;/* # of active STA virtual macs */
    atomic_t                sc_nap_vaps_up;/* # of active AP virtual macs */
    struct ath_vap          *sc_vaps[ATH_BCBUF]; /* interface id to avp map */
#if WAR_DELETE_VAP
    struct ath_vap          *sc_vap_del_pend_head;  /* List of vaps that are hanging due to a node refcount being non-zero */
    struct ath_vap          *sc_vap_del_pend_tail;  /* List of vaps that are hanging due to a node refcount being non-zero */
    u_int32_t               sc_vap_del_pend_count;  /* Number of vaps that are in limbo */
#endif

    u_int                   sc_fftxqmin;    /* aggregation threshold */
    HAL_INT                 sc_imask;       /* interrupt mask copy */
    u_int                   sc_keymax;      /* size of key cache */
    u_int8_t                sc_keymap[ATH_KEYBYTES];/* key use bit map */
    struct ieee80211_node   *sc_keyixmap[ATH_KEYMAX];/* key ix->node map */
    u_int8_t                sc_bssidmask[IEEE80211_ADDR_LEN];

    u_int8_t                sc_rxrate;      /* current rx rate for LED */
    u_int8_t                sc_txrate;      /* current tx rate for LED */

    ATH_LED_CONTROL_OBJ(sc_led_control);     /* ath led control */

    struct ath_force_ppm    sc_ppm_info;    /* PPM info */

    int                     sc_rxbufsize;   /* rx size based on mtu */
    struct ath_descdma      sc_rxdma;       /* RX descriptors */
    ath_bufhead             sc_rxbuf;       /* receive buffer */
    struct ath_buf *        sc_rxfreebuf;   /* free athbuf -head - need buffers to be allocated */
    struct ath_buf *        sc_rxfreetail;  /* free athbuf -tail-need buffers to be allocated */
    u_int32_t               sc_num_rxbuf;
#if USE_MULTIPLE_BUFFER_RCV || AP_MULTIPLE_BUFFER_RCV
    wbuf_t                  sc_rxpending;   /* pending 1st half of 2 buf rx */
    struct ath_buf *        sc_bfpending;   /* athbuf corresponding to sc_rxpending */
#if AP_MULTIPLE_BUFFER_RCV
    u_int32_t               sc_prebuflen;   /* record the current size */
    u_int32_t               sc_mergecheck; /* check the which fram to start merge use only for TX BF delay report*/
#endif
#endif /* USE_MULTIPLE_BUFFER_RCV || AP_MULTIPLE_BUFFER_RCV*/
    u_int8_t                sc_rxtimeout[WME_NUM_AC];
    u_int32_t               *sc_rxlink;     /* link ptr in last RX desc */
    spinlock_t              sc_rxflushlock; /* lock of RX flush */
    int32_t                 sc_rxflush;     /* rx flush in progress */
    spinlock_t              sc_rxbuflock;
    unsigned long           sc_rxbuflockflags;
    u_int8_t                sc_defant;      /* current default antenna */
    u_int8_t                sc_rxotherant;  /* rx's on non-default antenna*/
    u_int16_t               sc_cachelsz;    /* cache line size */

    /* Enhanced DMA support */
    u_int32_t               sc_num_txmaps;
    ATH_RX_EDMA_CONTROL_OBJ(sc_rxedma);
    spinlock_t              sc_txstatuslock;/* tx status ring lock (edma) */
    struct ath_descdma      sc_txsdma;
    u_int32_t               sc_txstatuslen; /* Tx descriptor status length */
    u_int32_t               sc_rxstatuslen; /* Rx descriptor status length */
    struct ath_buf          *sc_rxbufptr;
    struct ath_rx_status    *sc_rxsptr;

    struct ath_descdma      sc_txdma;       /* TX descriptors */
    ath_bufhead             sc_txbuf;       /* transmit buffer */
#if ATH_RESET_SERIAL
    struct ath_defer_txbuf  sc_queued_txbuf[HAL_NUM_TX_QUEUES]; /*queued txbuf */
    atomic_t                sc_hold_reset; /* reset or not */
    atomic_t                sc_dpc_stop;
    atomic_t                sc_reset_queue_flag; /*queue buf or not*/
    atomic_t                sc_tx_add_processing;
    atomic_t                sc_rx_return_processing;
    atomic_t                sc_dpc_processing;
    atomic_t                sc_intr_processing;
    OS_MUTEX                sc_reset_mutex;
    OS_MUTEX                sc_reset_hal_mutex;
#endif
#if ATH_TX_BUF_FLOW_CNTL
    u_int                   sc_txbuf_free;  /* No. of free tx buffers */
#endif
    spinlock_t              sc_txbuflock;   /* txbuf lock */
    unsigned long           sc_txbuflockflags;
    u_int                   sc_txqsetup;    /* h/w queues setup */
    u_int                   sc_txintrperiod;/* tx interrupt batching */
    struct ath_txq          sc_txq[HAL_NUM_TX_QUEUES];   /* HAL_NUM_TX_QUEUES 10 hw tx queues */
    int                     sc_haltype2q[HAL_WME_AC_VO+1]; /* HAL WME AC -> h/w qnum */
    HAL_TXQ_INFO            sc_beacon_qi;   /* adhoc only: beacon queue parameters */
    u_int32_t               sc_txdesclen;   /* Tx descriptor length */
#ifdef ATH_SUPPORT_UAPSD
    struct ath_descdma      sc_uapsdqnuldma;    /* U-APSD QoS NULL TX descriptors */
    ath_bufhead             sc_uapsdqnulbf;     /* U-APSD QoS NULL free buffers */
    spinlock_t              sc_uapsdirqlock;    /* U-APSD IRQ lock */
    unsigned long           sc_uapsdirqlockflags;
    int                     sc_uapsdqnuldepth;  /* U-APSD delivery queue depth */
    bool                    sc_uapsd_pause_in_progress;
    bool                    sc_uapsd_trigger_in_progress;
#endif

    u_int8_t                sc_grppoll_str[GRPPOLL_RATE_STR_LEN];
    struct ath_descdma      sc_bdma;        /* beacon descriptors */
    ath_bufhead             sc_bbuf;        /* beacon buffers */
    u_int                   sc_bhalq;       /* HAL q for outgoing beacons */
    u_int                   sc_bmisscount;  /* missed beacon transmits */
    u_int                   sc_consecutive_gtt_count;  /* consecutive gtt intr count */
    u_int                   sc_noise;       /* noise detected on channel */
    u_int                   sc_toggle_immunity; /* toggle immunity parameters */
    u_int32_t               sc_ant_tx[8];   /* recent tx frames/antenna */
    struct ath_txq          *sc_cabq;       /* tx q for cab frames */
    struct ath_txq          sc_grpplq;      /* tx q for XR group polls */
    struct ath_txq          *sc_xrtxq;      /* tx q for XR data */
    struct ath_descdma      sc_grppolldma;  /* TX descriptors for grppoll */
    ath_bufhead             sc_grppollbuf;  /* transmit buffers for grouppoll  */
    u_int16_t               sc_xrpollint;   /* xr poll interval */
    u_int16_t               sc_xrpollcount; /* xr poll count */
    struct ath_txq          *sc_uapsdq;     /* tx q for uapsd */
    struct ath_txq          sc_burstbcn_cabq;/* staging q for mcast frames when bursted beaconing */
#if ATH_SUPPORT_CFEND
    struct ath_txq          *sc_cfendq;       /* tx q for CF-END frame */
    struct ath_descdma      sc_cfenddma;    /* CF-END Tx descriptor */
    ath_bufhead             sc_cfendbuf;    /* CF-END free buffer */
#endif
//PAPRD related vars
    int8_t                  sc_paprd_rate;           /* PAPRD TX rate */
    int8_t                  sc_paprd_enable;         /* PAPRD enabled */
    int8_t                  sc_paprd_done_chain_mask;      /* PAPRD Done on chain num */
    int8_t                  sc_paprd_chain_num;      /* chain num - current PAPRD frame to be sent*/
    int8_t                  sc_paprd_abort;          /* PAPRD abort */
    int8_t                  sc_paprd_failed_count;   /* PAPRD failed count */
    int8_t                  sc_paprd_retrain_count;  /* PAPRD re-train failed count */
    int8_t                  sc_paprd_cal_started;    /* PAPRD CAL started */
    int16_t                 sc_paprd_bufcount;       /* Total paprd buf need to send for 1 paprd CAL */
    spinlock_t              sc_paprd_lock;
    struct ath_txq          *sc_paprdq;             /* tx q for PAPRD frame */
    struct ath_descdma      sc_paprddma;            /* PAPRD Tx descriptor */
    ath_bufhead             sc_paprdbuf;            /* PAPRD free buffer */
    u_int16_t               sc_paprd_lastchan_num;  /* Last chan PAPRD Cal completed */
    int8_t                  sc_paprd_thermal_packet_not_sent;       /* PAPRD thermal packet */
//End PAPRD vars
    sc_updateslot_t         sc_updateslot;  /* slot time update fsm */
    int                     sc_slotupdate;  /* slot to next advance fsm */
    int                     sc_slottime;    /* slot time */
    int                     sc_bslot[ATH_BCBUF];/* beacon xmit slots */
    int                     sc_bnext;       /* next slot for beacon xmit */

    struct ath_timer        sc_cal_ch;      /* calibration timer */
    struct ath_timer        sc_up_beacon;       /* beacon config timer after vap up */
    u_int8_t                sc_up_beacon_purge; /* purge beacon config after vap up */
    HAL_NODE_STATS          sc_halstats;    /* station-mode rssi stats */

    u_int16_t               sc_reapcount;   /* # of tx buffers reaped after net dev stopped */
    int16_t                 sc_noise_floor; /* signal noise floor in dBm */
    u_int8_t                sc_noreset;
    u_int8_t                sc_limit_legacy_frames;

#ifdef ATH_SUPERG_DYNTURBO
    os_timer_t              sc_dturbo_switch_mode;  /* AP scan timer */
    u_int32_t               sc_dturbo_tcount;       /* beacon intval count */
    u_int32_t               sc_dturbo_hold_max;     /* hold count before switching to base*/
    u_int16_t               sc_dturbo_hold_count;   /* hold count before switching to base*/
    u_int16_t               sc_dturbo_turbo_tmin;   /* min turbo count */
    u_int32_t               sc_dturbo_bytes;        /* bandwidth stats */ 
    u_int32_t               sc_dturbo_base_tmin;    /* min time in base */
    u_int32_t               sc_dturbo_turbo_tmax;   /* max time in turbo */
    u_int32_t               sc_dturbo_bw_base;      /* bandwidth threshold */
    u_int32_t               sc_dturbo_bw_turbo;     /* bandwidth threshold */
#endif

    HAL_HT_EXTPROTSPACING   sc_ht_extprotspacing;

    u_int8_t                *sc_mask2chains;
    u_int8_t                sc_tx_numchains;
    u_int8_t                sc_rx_numchains;

#ifdef ATH_TX_INACT_TIMER
    os_timer_t              sc_inact;      /* inactivity timer */
    u_int8_t                sc_tx_inact;
    u_int8_t                sc_max_inact;
#endif

#if ATH_HW_TXQ_STUCK_WAR
    os_timer_t              sc_qstuck;      /* q stuck monitoring timer */
#endif

    u_int8_t                sc_tx_chainmask;
    u_int8_t                sc_rx_chainmask;
    u_int8_t                sc_rxchaindetect_ref;
    u_int8_t                sc_rxchaindetect_thresh5GHz;
    u_int8_t                sc_rxchaindetect_thresh2GHz;
    u_int8_t                sc_rxchaindetect_delta5GHz;
    u_int8_t                sc_rxchaindetect_delta2GHz;
    u_int8_t                sc_cfg_txchainmask;     /* initial chainmask config */
    u_int8_t                sc_cfg_rxchainmask;
    u_int8_t                sc_cfg_txchainmask_leg;
    u_int8_t                sc_cfg_rxchainmask_leg;
    
    u_int32_t               sc_rtsaggrlimit; /* Chipset specific aggr limit */
    ATH_PWRSAVE_CONTROL_OBJ(sc_pwrsave);     /* power save control */
    u_int32_t               sc_pktlog_enable;
    systime_t               sc_shortcal_timer;
    systime_t               sc_longcal_timer;
    systime_t               sc_ani_timer;
    systime_t               sc_olpc_timer;
    HAL_BOOL                sc_Caldone;
    u_int32_t               sc_ent_min_pkt_size_enable : 1,
                            sc_ent_spectral_mask : 2,
                            sc_ent_rtscts_delim_war : 1;
#ifdef ATH_USB
    usblock_t               sc_resetlock;
#else
    spinlock_t              sc_resetlock;
#endif
    unsigned long           sc_resetlock_flags;
#ifdef ATH_USB
    usblock_t               sc_reset_serialize;
#else
    spinlock_t              sc_reset_serialize;
#endif
#if ATH_WOW
    struct wow_info         *sc_wowInfo;
#endif

#ifdef ATH_RFKILL
    struct ath_rfkill_info  sc_rfkill;      /* RfKill settings */
#endif

#ifdef ATH_BT_COEX
    struct ath_bt_info      sc_btinfo;      /* BT coexistence settings */
#endif

#ifdef ATH_GEN_TIMER
    struct ath_gen_timer_table  sc_gen_timers;
#endif

#ifndef REMOVE_PKT_LOG
    struct ath_pktlog_info  *pl_info;
#endif
    hal_hw_hangs_t          sc_hang_war;
    HAL_BOOL                sc_hang_check;
#if ATH_SUPPORT_VOWEXT
    int                     sc_phyrestart_disable;
    int                     sc_keysearch_always_enable;
#endif
#ifdef ATH_RB
    ath_rb_t                sc_rb;
    u_int8_t                sc_do_rb_war;
#endif

#ifdef ATH_SWRETRY
    u_int32_t                   sc_num_swretries;
    ATH_LIST_HEAD(, ath_node)   sc_nt;          /* node table list */
    spinlock_t                  sc_nt_lock;     /* node table lock */
#endif

#if ATH_SLOW_ANT_DIV
    /* Slow antenna diversity */
    struct ath_antdiv       sc_antdiv;
#endif
    u_int8_t        sc_lastdatarix;    /* last data frame rate index */
    u_int8_t        sc_lastrixflags;/* last data frame rate flags */

#ifdef ATH_MIB_INTR_FILTER
    struct ath_intr_filter  sc_intr_filter;
#endif

#ifdef ATH_SUPPORT_HTC
#define NUM_TX_EP           12

    /* Host HTC/HIF/WMI related data structure */
    HTC_HANDLE              sc_host_htc_handle;

    /* Host HTC Endpoint IDs */
    HTC_ENDPOINT_ID         sc_wmi_command_ep;
    HTC_ENDPOINT_ID         sc_beacon_ep;
    HTC_ENDPOINT_ID         sc_cab_ep;
    HTC_ENDPOINT_ID         sc_uapsd_ep;
    HTC_ENDPOINT_ID         sc_mgmt_ep;
    HTC_ENDPOINT_ID         sc_data_VO_ep;
    HTC_ENDPOINT_ID         sc_data_VI_ep;
    HTC_ENDPOINT_ID         sc_data_BE_ep;
    HTC_ENDPOINT_ID         sc_data_BK_ep;

    adf_os_handle_t         sc_hdl;
    u_int32_t               sc_rxepid;

    struct ath_txep         sc_txep[NUM_TX_EP];
    struct ath_txep         *sc_ac2ep[WME_NUM_AC]; /* BE & BK are muxed */  /* WME AC -> h/w qnum */
    struct ath_txep         *sc_cabep;
    struct ath_txep         *sc_beaconep;
    struct ath_txep         *sc_uapsdep;
    u_int32_t               sc_ep2qnum[NUM_TX_EP];
#ifdef MAGPIE_HIF_GMAC    
    adf_nbuf_queue_t        sc_rxwbufhead;
#ifdef ATH_HTC_SG_SUPPORT    
    struct ath_htc_rxsg      sc_sgrx;
#endif    
#endif    
#endif

#ifdef ATH_SUPPORT_HTC
    u_int8_t                sc_usb_invalid;
    u_int8_t                sc_qosDropIpFrag[WME_NUM_AC];
    u_int32_t               sc_qosDropIpFragNum[WME_NUM_AC];

    /* Need to rewrite */
    u_int32_t               flags;
    unsigned long           kevent_flags;
    u_int32_t               kevent_ready;
#ifdef __linux__
    struct semaphore        ioctl_sem;
    struct work_struct      kevent;
#endif  
    
#ifdef ATH_HTC_TX_SCHED
    struct ath_buf   *sc_ath_memtxbuf; /* buffer to be freed during cleanup */
    struct ath_buf   *sc_ath_membbuf; /* buffer to be freed during cleanup */
#endif

#endif

#ifdef ATH_SUPPORT_LINUX_STA
#ifdef CONFIG_SYSCTL
    struct ctl_table_header *sc_sysctl_header;
    struct ctl_table        *sc_sysctls;
#endif
#endif

#if ATH_SUPPORT_WIRESHARK
    struct {
        unsigned char
            filter_phy_err : 1;
	} sc_wireshark;
#endif /* ATH_SUPPORT_WIRESHARK */

#if ATH_SUPPORT_CFEND
    spinlock_t              sc_cfend_lock;
#endif
#if ATH_SUPPORT_VOWEXT
    u_int32_t               sc_vowext_flags;
    /* aggregate control related variables*/
    u_int32_t               agtb_tlim;
    u_int32_t               agtb_blim;
    u_int32_t               agthresh;
    /* RCA variables */
    u_int8_t 		    rca_aggr_uptime;
    u_int8_t 		    aggr_aging_factor;
    u_int8_t 		    per_aggr_thresh;
    u_int8_t 		    excretry_aggr_red_factor; 
    u_int8_t 		    badper_aggr_red_factor;
    u_int32_t vsp_enable;				/* enable/disable vsp algo */
    u_int32_t vsp_threshold;			/* VSP uses this threshold value (in mbps) to differenatite the good/bad vi streams */
    u_int32_t vsp_evalinterval;			/* Time interval in ms after which vsp will be triggered if required */
    u_int32_t vsp_prevevaltime;			/* Stores the previous time stamp of vsp algo run */
    u_int32_t vsp_vicontention;         /* Counter indicating no of vi contention occurances (incoming video packet sees buffer full cond) */
    u_int32_t vsp_bependrop;            /* Counter indicating no of be packets(bufs) penalized for incoming vi packet */
    u_int32_t vsp_bkpendrop;            /* Counter indicating no of bk packets(bufs) penalized for incoming vi packet */
    u_int32_t vsp_vipendrop;            /* Counter indicating no of low priority (decided b vsp) vi packets(bufs) penalized for incoming vi packet of high priority */
#endif

#ifdef ATH_C3_WAR
    atomic_t                sc_pending_tx_data_frames;
    bool                    sc_fifo_underrun;
    bool                    c3_war_timer_active;
    spinlock_t              sc_c3_war_lock;
    struct ath_gen_timer    *sc_c3_war_timer;
#endif

#if ATH_ANT_DIV_COMB
    /* antenna diversity & combining */
    struct ath_antcomb      sc_antcomb;
#endif
    HAL_CALIBRATION_TIMER   *sc_cals;
	int                     sc_num_cals;
	u_int32_t               sc_min_cal;
	u_int32_t               sc_sched_cals;
#ifdef ATH_SUPPORT_TxBF
#ifdef TXBF_TODO
    struct ath_timer        sc_imbf_cal_short;
    u_int8_t                only_bf_cal_allow;
    u_int8_t                radio_cal_process;
    u_int8_t                sc_rccaling        :1,          
                            sc_40m_rc_valid    :1,
                            sc_20m_rc_valid    :1;

    struct ieee80211_node   *pos3_node;
    u_int8_t                *pos2_data;
    u_int16_t               pos2_data_len;
    struct ath_rx_status    pos2_rx_status;
    u_int8_t                join_macaddr[6];
#endif
    u_int8_t                sc_txbfsounding    :1,
                            sc_txbfcalibrating :1;              
    u_int8_t                last_rx_type;
    struct ieee80211_node   *v_cv_node;
    u_int8_t                do_node_check;
    u_int8_t                last_h_invalid;
    u_int8_t                last_be_aggr;
    wbuf_t                  sc_rx_wbuf_waiting;
    struct ath_buf	        *sc_bf_waiting;
    struct ath_rx_status    *sc_rxs_waiting;
    u_int32_t               sounding_rx_kbps;
    struct ath_timer        sc_selfgen_timer;
#endif
    atomic_t                sc_has_tx_bcn_notify;   /* Enable flag for this feature */
    ath_notify_tx_bcn_t     sc_notify_tx_bcn;       /* Opaque handle */
#ifdef VOW_TIDSCHED
    u_int32_t tidsched;
    TAILQ_HEAD(,ath_atx_tid)tid_q;      /* queue of TIDs with buffers */
    u_int32_t acqw[4];
    u_int32_t acto[4];
#endif
#ifdef ATH_SUPPORT_IQUE
    u_int32_t total_delay_timeout;
#endif
#ifdef VOW_LOGLATENCY
    u_int32_t loglatency;
#endif
#if ATH_VAP_PAUSE_SUPPORT
    spinlock_t              sc_vap_pause_lock;    /* lock to serialize vap pause operations */
    u_int32_t               sc_txq_use_cnt;       /* number of txq users */
    u_int32_t               sc_vap_pause_in_progress; /* flag indicating vap pause is in progress */
#endif

#if UMAC_SUPPORT_VI_DBG
    u_int32_t               sc_en_vi_dbg_log;
    u_int32_t               sc_vi_dbg_start;
#endif
#if ATH_TRAFFIC_FAST_RECOVER
    struct ath_timer        sc_traffic_fast_recover_timer;
#endif
    u_int16_t tx_power;
#if ATH_SUPPORT_FLOWMAC_MODULE
    void                    *sc_flowmac_dev;
    int                     sc_flowmac_debug;
#endif
#if UMAC_SUPPORT_INTERNALANTENNA
    u_int8_t   sc_smartant_enable;  /* software configuration to enable smart antenna 
                                     * if hardware is capable of smart antenna then only enable smart antenna
                                     */
#endif    
#if ATH_SUPPORT_AGGR_BURST
#define ATH_BURST_DURATION 5100
    bool                    sc_aggr_burst;
    u_int32_t               sc_aggr_burst_duration;
#endif
#if ATH_SUPPORT_RX_PROC_QUOTA    
    u_int32_t               sc_rx_work;
    u_int32_t               sc_process_rx_num; /*No of packets to process per rx schedule */
#endif    

#if ATH_SUPPORT_VOW_DCS
    struct ath_dcs_params      sc_dcs_params;
    u_int16_t                  sc_phyerr_rate_thresh;
    u_int8_t                   sc_coch_intr_thresh;
    u_int8_t                   sc_tx_err_thresh;
    u_int8_t                   sc_dcs_enable;
#endif
    bool		       sc_dfs_radar_detected[WME_NUM_TID];
};

    1.3 
/*
 * per TID aggregate tx state for a destination
 */
typedef struct ath_atx_tid {
    int               tidno;      /* TID number */
    u_int16_t         seq_start;  /* starting seq of BA window */
    u_int16_t         seq_next;   /* next seq to be used */
    u_int16_t         baw_size;   /* BA window size */
#ifdef ATH_HTC_TX_SCHED
    u_int8_t          tid_buf_cnt;
    u_int8_t          pad0;
#endif
    int               baw_head;   /* first un-acked tx buffer */
    int               baw_tail;   /* next unused tx buffer slot */
    u_int16_t         sched:1,    /* TID is scheduled */
                      filtered:1, /* TID has filtered pkts */
                      min_depth:2;/* num pkts that can be queued to h/w */
    int               paused;     /* TID is paused */
    int               cleanup_inprogress; /* this TID's aggr being torn down */
    TAILQ_HEAD(ath_tid_bq,ath_buf) buf_q;      /* pending buffers */
    TAILQ_ENTRY(ath_atx_tid)       tid_qelem;  /* round-robin tid entry */
    TAILQ_HEAD(,ath_buf)           fltr_q;     /* filtered buffers */
    TAILQ_ENTRY(ath_atx_tid)       fltr_qelem; /* handle hwq filtering */
    struct ath_node   *an;        /* parent node structure */
    struct ath_atx_ac *ac;        /* parent access category */
    u_int32_t         tx_buf_bitmap[TX_BUF_BITMAP_WORDS]; /* active tx frames */

    /*
     * ADDBA state
     */
    u_int32_t               addba_exchangecomplete:1,
                            addba_amsdusupported:1;
    int32_t                 addba_exchangeinprogress;
    struct ath_timer        addba_requesttimer;
    int                     addba_exchangeattempts;
    u_int16_t               addba_exchangestatuscode;
#ifdef VOW_TIDSCHED
    int qw;
    TAILQ_ENTRY(ath_atx_tid) wrr_tid_qelem; /* round-robin tid entry */
#endif
} ath_atx_tid_t;

    1.4


/*
 * Transmit queue subtype.  These map directly to
 * WME Access Categories (except for UPSD).  Refer
 * to Table 5 of the WME spec.
 */
typedef enum {
    HAL_WME_AC_BK   = 0,            /* background access category */
    HAL_WME_AC_BE   = 1,            /* best effort access category*/
    HAL_WME_AC_VI   = 2,            /* video access category */
    HAL_WME_AC_VO   = 3,            /* voice access category */
    HAL_WME_UPSD    = 4,            /* uplink power save */
    HAL_XR_DATA     = 5,            /* uplink power save */
} HAL_TX_QUEUE_SUBTYPE;

/*
 * Transmit queue types/numbers.  These are used to tag
 * each transmit queue in the hardware and to identify a set
 * of transmit queues for operations such as start/stop dma.
 */             
typedef enum {
    HAL_TX_QUEUE_INACTIVE   = 0,        /* queue is inactive/unused */
    HAL_TX_QUEUE_DATA       = 1,        /* data xmit q's */
    HAL_TX_QUEUE_BEACON     = 2,        /* beacon xmit q */
    HAL_TX_QUEUE_CAB        = 3,        /* "crap after beacon" xmit q */
    HAL_TX_QUEUE_UAPSD      = 4,        /* u-apsd power save xmit q */
    HAL_TX_QUEUE_PSPOLL     = 5,        /* power-save poll xmit q */
    HAL_TX_QUEUE_CFEND      = 6,         /* CF END queue to send cf-end frames*/
    HAL_TX_QUEUE_PAPRD      = 7         /* PAPRD queue to send PAPRD traningframes*/
} HAL_TX_QUEUE;


1.5

typedef struct {
    u_int32_t               tqi_ver;        /* hal TXQ version */
    HAL_TX_QUEUE_SUBTYPE    tqi_subtype;    /* subtype if applicable */
    HAL_TX_QUEUE_FLAGS      tqi_qflags;     /* flags (see above) */
    u_int32_t               tqi_priority;   /* (not used) */
    u_int32_t               tqi_aifs;       /* aifs */
    u_int32_t               tqi_cwmin;      /* cwMin */
    u_int32_t               tqi_cwmax;      /* cwMax */
    u_int16_t               tqi_shretry;    /* rts retry limit */
    u_int16_t               tqi_lgretry;    /* long retry limit (not used)*/
    u_int32_t               tqi_cbrPeriod;
    u_int32_t               tqi_cbrOverflowLimit;
    u_int32_t               tqi_burstTime;
    u_int32_t               tqi_readyTime; /* specified in msecs */
    u_int32_t               tqi_compBuf;    /* compression buffer phys addr */
} HAL_TXQ_INFO;


1.6 
typedef struct sk_buff *        __adf_nbuf_t;
typedef __adf_nbuf_t            __wbuf_t;
typedef __wbuf_t                wbuf_t;

1.7 
/*
 * Node information. A node could represents a BSS in infrastructure network,
 * or an ad-hoc station in IBSS mode, or an associated station in HOSTAP mode.
 */
typedef struct ieee80211_node {
    TAILQ_ENTRY(ieee80211_node) ni_list; /* node table list */
    LIST_ENTRY(ieee80211_node)  ni_hash; /* hash list */
#ifdef IEEE80211_DEBUG_NODELEAK
    TAILQ_ENTRY(ieee80211_node) ni_alloc_list; /* all allocated nodes */
#endif

#ifdef IEEE80211_DEBUG_REFCNT
#define NUM_TRACE_BUF           (1 << 5)
    const char  *func[NUM_TRACE_BUF];
    int         line[NUM_TRACE_BUF];
    atomic_t    refcnt[NUM_TRACE_BUF];
    atomic_t    index;
#endif

    struct ieee80211_node_table *ni_table;
    struct ieee80211vap     *ni_vap;
    struct ieee80211com     *ni_ic;
    struct ieee80211_node   *ni_bss_node;
    ieee80211_node_state_lock_t    ni_state_lock;   /* Node for the WAR for bug 58187 */
    atomic_t                ni_refcnt;
    u_int8_t                ni_authmode;            /* authentication algorithm */
    u_int32_t               ni_flags;               /* special-purpose state */
#define IEEE80211_NODE_AUTH     0x00000001          /* authorized for data */
#define IEEE80211_NODE_QOS      0x00000002          /* QoS enabled */
#define IEEE80211_NODE_ERP      0x00000004          /* ERP enabled */
#define IEEE80211_NODE_HT       0x00000008          /* HT enabled */
/* NB: this must have the same value as IEEE80211_FC1_PWR_MGT */
#define IEEE80211_NODE_PWR_MGT  0x00000010          /* power save mode enabled */
#define IEEE80211_NODE_UAPSD    0x00000040          /* U-APSD power save enabled */
#define IEEE80211_NODE_UAPSD_TRIG 0x00000080        /* U-APSD triggerable state */
#define IEEE80211_NODE_UAPSD_SP 0x00000100          /* U-APSD SP in progress */
#define IEEE80211_NODE_ATH      0x00000200          /* Atheros Owl or follow-on device */
#define IEEE80211_NODE_OWL_WDSWAR 0x00000400        /* Owl WDS workaround needed*/
#define IEEE80211_NODE_WDS      0x00000800          /* WDS link */
#define	IEEE80211_NODE_NOAMPDU  0x00001000          /* No AMPDU support */
#define IEEE80211_NODE_WEPTKIPAGGR 0x00002000       /* Atheros proprietary wep/tkip aggregation support */
#define IEEE80211_NODE_WEPTKIP  0x00004000
#define IEEE80211_NODE_TEMP     0x00008000          /* temp node (not in the node table) */
#define IEEE80211_NODE_40_INTOLERANT    0x00020000  /* HT40 Intolerant  */
#define IEEE80211_NODE_PAUSED   0x00040000          /* node is  paused*/
#define IEEE80211_NODE_EXTRADELIMWAR 0x00080000
#define IEEE80211_NODE_NAWDS 0x00100000          /* node is an NAWDS repeater */
#define IEEE80211_NODE_TDLS     0x00200000          /* TDLS node */
#define IEEE80211_NODE_REQ_HT20     0x00400000      /* HT20 requesting node */
#define IEEE80211_NODE_ATH_PAUSED 0x00800000         /* all the tid queues in ath layer are paused*/ 
#define IEEE80211_NODE_UAPSD_CREDIT_UPDATE 0x01000000  /*Require credit update*/
#define IEEE80211_NODE_KICK_OUT_DEAUTH 0x02000000  /*Require send deauth when h/w queue no data*/
   
    u_int8_t                ni_ath_flags;       /* Atheros feature flags */
/* NB: These must have the same values as IEEE80211_ATHC_* */
#define IEEE80211_NODE_TURBOP   0x01          /* Turbo prime enable */
#define IEEE80211_NODE_COMP     0x02          /* Compresssion enable */
#define IEEE80211_NODE_FF       0x04          /* Fast Frame capable */
#define IEEE80211_NODE_XR       0x08          /* Atheros WME enable */
#define IEEE80211_NODE_AR       0x10          /* AR capable */
#define IEEE80211_NODE_BOOST    0x80 

    u_int16_t               ni_ath_defkeyindex; /* Atheros def key index */
#define IEEE80211_INVAL_DEFKEY  0x7FFF

    u_int16_t               ni_associd; /* association id */
    u_int32_t               ni_scangen;
    systick_t               ni_assocuptime;  /* association up time */
    systick_t               ni_assocstarttime;   /* association start time */
    systick_t               ni_assoctime;        /* association process time */
    u_int16_t               ni_assocstatus;  /* association status code */
    u_int16_t               ni_txpower; /* current transmit power */

    u_int16_t               ni_vlan;    /* vlan tag */
    u_int32_t               *ni_challenge;  /* shared-key challenge */
    u_int8_t                *ni_wpa_ie;     /* captured WPA/RSN ie */
    u_int8_t                *ni_wps_ie;     /* captured WSC ie */
    u_int8_t                 ni_cc[3];      /* captured country code */

    u_int8_t                *ni_ath_ie; /* captured Atheros ie */
    u_int8_t                *ni_wme_ie; /* captured WME ie */
    u_int16_t               ni_txseqs[IEEE80211_TID_SIZE];      /* tx seq per-tid */
    u_int16_t               ni_rxseqs[IEEE80211_TID_SIZE+1];/* rx seq previous per-tid,
                                                             * the additional one is for check seq on
                                                             * management frames. */
    u_int16_t               ni_last_rxseqs[IEEE80211_TID_SIZE+1];/* rx seq 2nd last(previous) per-tid,
                                                                  * the additional one is for check seq on
                                                                  * management frames. */
    systime_t               ni_rxfragstamp; /* timestamp of last rx frag */
    atomic_t                ni_rxfrag_lock; /* indicate timer handler is currently executing */
    wbuf_t                  ni_rxfrag[1];  /* rx frag reassembly.
                                             * XXX ???: do we have to have a reassembly line
                                             * for each TID */
    
    struct ieee80211_rsnparms ni_rsn;       /* RSN/WPA parameters */
    struct ieee80211_key    ni_ucastkey;    /* unicast key */
    struct {   // for adhoc mcast rx
        struct ieee80211_key nips_hwkey; // allocated clear key used to hand frame to sw
        struct ieee80211_key nips_swkey[IEEE80211_WEP_NKID]; // key used by sw to decrypt
    } ni_persta;
    int                     ni_rxkeyoff;    /* Receive key offset */
#if ATH_SUPPORT_WEP_MBSSID
    struct ieee80211_wep_mbssid ni_wep_mbssid;
#endif /*ATH_SUPPORT_WEP_MBSSID*/

    /*support for WAPI: keys for WAPI*/
#if ATH_SUPPORT_WAPI
	int ni_wkused;
	u32 ni_wapi_rekey_pkthresh;  /*wapi packets threshold for rekey, unicast or multicast depending on node*/
#endif

    /* 11n Capabilities */
    u_int16_t               ni_htcap;              /* HT capabilities */
    u_int16_t               ni_maxampdu;           /* maximum rx A-MPDU length */
    u_int32_t               ni_mpdudensity;        /* MPDU density in nano-sec */

#ifdef ATH_SUPPORT_TxBF
    union ieee80211_hc_txbf ni_txbf;               /* txbf capabilities */
#endif

    u_int8_t                ni_obssnonhtpresent;   /* OBSS non-HT STA present */
    u_int8_t                ni_txburstlimit;       /* TX burst limit */
    u_int8_t                ni_nongfpresent;       /* Non-green field devices present */
    u_int8_t                ni_streams;            /* number of streams supported */

    /* 11n information */
    enum ieee80211_cwm_width ni_chwidth;        /* recommended tx channel width */
    u_int8_t                ni_newchwidth;      /* channel width changed */
    int8_t                  ni_extoffset;       /* recommended ext channel offset */
    u_int8_t                ni_updaterates;     /* update rate table on SM power save */
#define	IEEE80211_NODE_SM_EN                    1
#define	IEEE80211_NODE_SM_PWRSAV_STAT	        2
#define	IEEE80211_NODE_SM_PWRSAV_DYN	        4
#define	IEEE80211_NODE_RATECHG                  8

    /* activity indicators */
    systime_t               ni_beacon_rstamp; /* host timestamp of received beacon and probes */
    u_int32_t               ni_probe_ticks;   /* inactivity mark count */

    u_int8_t		        ni_weptkipaggr_rxdelim; /* number of delimeters required to receive wep/tkip w/aggr */

    /* in activity indicators for AP mode */
    u_int16_t               ni_inact;       /* inactivity mark count */
    u_int16_t               ni_inact_reload;/* inactivity reload value */

#define ATH_TX_MAX_CONSECUTIVE_XRETRIES     50 /* sta gets kicked out after this */
    /* kick out STA when excessive retries occur */
    u_int16_t               ni_consecutive_xretries;

    /* hardware, not just beacon and probes */
    u_int8_t                ni_rssi;    /* recv ssi */

    u_int8_t                ni_macaddr[IEEE80211_ADDR_LEN]; /* MAC address */
    u_int8_t                ni_bssid[IEEE80211_ADDR_LEN]; /* BSSID */

    /* beacon, probe response */
    union {
        u_int8_t            data[8];
        u_int64_t           tsf;
    } ni_tstamp;                        /* from last rcv'd beacon */

    u_int16_t               ni_intval;  /* beacon interval */
    u_int16_t               ni_capinfo; /* negociated capabilities */
    
    u_int16_t               ni_ext_caps;/* exteneded node capabilities */
#define IEEE80211_NODE_C_QOS    0x0002  /* Wmm capable */
#define IEEE80211_NODE_C_UAPSD  0x0004  /* U-APSD capable */  

    u_int8_t                ni_esslen;
    u_int8_t                ni_essid[IEEE80211_NWID_LEN+1];

    struct ieee80211_rateset ni_rates;   /* negotiated rate set */
    struct ieee80211_rateset ni_htrates; /* negotiated ht rate set */
    struct ieee80211_channel *ni_chan;

    u_int8_t                ni_erp;     /* ERP from beacon/probe resp */

    u_int32_t               ni_wait0_ticks;       /* ticks that we stay in dot11_assoc_state_zero state */
    u_int8_t                ni_dtim_period;       /* DTIM period */
    u_int8_t                ni_dtim_count;        /* DTIM count for last bcn */
    u_int16_t               ni_lintval;           /* listen interval */
    u_int8_t                ni_minbasicrate;      /* Min basic rate */

    systime_t               ss_last_data_time;    /* last time data RX/TX time */ 

    u_int16_t               ni_pause_count;
    /* power save queues */
    IEEE80211_NODE_POWERSAVE_QUEUE(ni_dataq)
    IEEE80211_NODE_POWERSAVE_QUEUE(ni_mgmtq)

    /* AP side UAPSD */
    u_int8_t                ni_uapsd;	/* U-APSD per-node flags matching WMM STA Qos Info field */
    u_int8_t                ni_uapsd_maxsp; /* maxsp from flags above */
    u_int16_t               ni_uapsd_trigseq[WME_NUM_AC]; /* trigger suppression on retry */
    u_int8_t                ni_uapsd_ac_trigena[WME_NUM_AC];    /* U-APSD per-node flags matching WMM STA Qos Info field */
    u_int8_t                ni_uapsd_ac_delivena[WME_NUM_AC];    /* U-APSD per-node flags matching WMM STA Qos Info field */

    /* node statistics */
    struct ieee80211_nodestats   ni_stats; 

    u_int8_t                ni_assoc_state; /* IBSS only */
#define IEEE80211_NODE_ADHOC_STATE_ZERO             0
#define IEEE80211_NODE_ADHOC_STATE_UNAUTH_UNASSOC   1
#define IEEE80211_NODE_ADHOC_STATE_AUTH_ASSOC       2

    u_int8_t                ni_chanswitch_tbtt;
#ifdef ATH_SUPPORT_TxBF    
    /* beam forming flag */
    u_int8_t                ni_bf_update_cv        : 1, /* 1: request CV update */                 
                            ni_explicit_noncompbf  : 1, /* 1: set explicit non-compressed bf */    
                            ni_explicit_compbf     : 1, /* 1: set explicit compressed bf*/         
                            ni_implicit_bf         : 1, /* 1: set implicit bf */                   
                            Calibration_Done       : 1,     
                            ni_rpt_received        : 1, /* 1: V/CV report recieved */    
                            ni_txbf_timer_initialized   : 1,    /* 1: txbf related timer initialized */
                            ni_hw_cv_requested     : 1,     /* 1: cv requested by incorrect HW status*/
                            ni_allow_cv_update     : 1;     /* 1: sw time out , allow cv request */    

    u_int8_t                ni_mmss;
    u_int32_t               ni_sw_cv_timeout;
    os_timer_t              ni_cv_timer;
    os_timer_t              ni_report_timer;
    u_int32_t               ni_cvtstamp;
    u_int8_t                ni_cvretry;
#endif

    struct ieee80211vap     *ni_wdsvap;     /* associated WDS vap */
#if ATH_SUPPORT_IQUE
    u_int8_t    ni_hbr_block;
    u_int32_t   ni_ique_flag;    
#endif
#if UMAC_SUPPORT_TDLS
    struct ieee80211_tdls_node *ni_tdls;
#endif /* UMAC_SUPPORT_TDLS */
#ifdef ATH_SUPPORT_P2P
    u_int8_t    ni_p2p_awared;
#endif
#if UMAC_SUPPORT_SMARTANTENNA
    struct per_ratetable permap[MAX_SMART_ANTENNAS][MAX_HT_RATES];
    u_int8_t maxrtsets;
    u_int8_t  current_tx_antenna;
    u_int8_t  default_rx_antenna;
    u_int8_t  reciever_rx_antenna;
    u_int8_t  current_rate_index;
    u_int8_t  smartantenna_state;
    u_int8_t  iteration;
    u_int8_t  is_training;
    u_int8_t  train_type; /* frame based or protocol based */
    u_int32_t pending_pkts; 
    u_int32_t ns_prev_tx_ucast; /* tx unicast packets trasmited in retraining intervel */
    u_int8_t  retrain_miss;  /* Number of consecutive retrain intervels with out doing retraining*/
    u_int8_t  smartant_prev_ratemax;
#endif    
#if UMAC_SUPPORT_INTERNALANTENNA    
    struct smartant_rateset rtset[SMARTANT_MAX_RATESETS];
#endif
#if UMAC_SUPPORT_SMARTANTENNA
    struct smartantenna_state train_state; /* current training sate info */
    TAILQ_ENTRY(ieee80211_node) smartant_nodelist; /* smart antenna node table list */
#endif
#ifdef ATH_SUPPORT_IBSS_NETLINK_NOTIFICATION
    u_int8_t	ni_rssi_class;
#endif
	u_int8_t	ni_lb_auth_num;   /*recive auth reqest packets num*/
} IEEE80211_NODE, *PIEEE80211_NODE;


1.8 

/*
 * Transmitted frames have the following information
 * held in the sk_buff control buffer.  This is used to
 * communicate various inter-procedural state that needs
 * to be associated with the frame for the duration of
 * it's existence.
 *
 * NB: sizeof(cb) == 48 and the vlan code grabs the first
 *     8 bytes so we reserve/avoid it.
 */
struct nbuf_cb {
	u_int8_t		vlan[8];	/* reserve for vlan tag info */
	struct ieee80211_node	*ni;
	void			*context;	/* pointer to context area */
	u_int32_t		flags;
#define	N_LINK0		0x01			/* frame needs WEP encryption */
#define	N_FF		0x02			/* fast frame */
#define	N_PWR_SAV	0x04			/* bypass power save handling */
#define N_UAPSD		0x08			/* frame flagged for u-apsd handling */
#define N_EAPOL		0x10			/* frame flagged for EAPOL handling */
#define N_AMSDU		0x20			/* frame flagged for AMSDU handling */
#define	N_NULL_PWR_SAV	0x40			/* null data with power save bit on */
#define N_PROBING	0x80			/* frame flagged as a probing one */
#define N_ERROR         0x100                   /* frame flagged as a error one */
#define N_MOREDATA      0x200                   /* more data flag */
#define N_SMPSACTM      0x400                   /* This frame is SM power save Action Mgmt frame */
#define N_QOS           0x800                   /* This is a QOS frame*/
#define N_ENCAP_DONE    0x1000              /* This frame is marked as fast-path pkts, some encapsulation work has been done by h/w */
#define N_CLONED		0x2000		/* frame is cloned in rx path */
#ifdef ATH_SUPPORT_WAPI
#define N_WAI           0x4000                  /* frame flagged for WAPI handling */
#endif
	u_int8_t		u_tid;		/* user priority from vlan/ip tos   */
        u_int8_t                exemptiontype;  /* exemption type of this frame (0,1,2)*/
        u_int8_t                type;           /* type of this frame */
#if defined(ATH_SUPPORT_VOWEXT) || defined(ATH_SUPPORT_IQUE) || defined(VOW_LOGLATENCY) || UMAC_SUPPORT_NAWDS != 0
        u_int8_t		firstxmit;
        u_int32_t		firstxmitts;
#endif

#if defined (ATH_SUPPORT_P2P)
        void            *complete_handler;     /* complete handler */ 
        void            *complete_handler_arg; /* complete handler arg */
#endif
#if defined(VOW_TIDSCHED) || defined(ATH_SUPPORT_IQUE) || defined(VOW_LOGLATENCY)
	u_int32_t		qin_timestamp;           /* timestamp of buffer's entry into queue */
#endif
};


1.9

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
