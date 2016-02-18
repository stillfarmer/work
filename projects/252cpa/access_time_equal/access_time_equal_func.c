1. 

int ath_attach(u_int16_t devid, void *base_addr,
        struct ath_softc_net80211 *scn,
        osdev_t osdev, struct ath_reg_parm *ath_conf_parm,
        struct hal_reg_parm *hal_conf_parm, IEEE80211_REG_PARAMETERS *ieee80211_conf_parm)
{


    error = ath_dev_attach(devid, base_addr,
            ic, &net80211_ops, osdev,
            &scn->sc_dev, &scn->sc_ops,
            scn->amem.handle,
            ath_conf_parm, hal_conf_parm);

    int ath_dev_attach(u_int16_t            devid,
            void                 *bus_context,
            ieee80211_handle_t   ieee,
            struct ieee80211_ops *ieee_ops,
            osdev_t              osdev,
            ath_dev_t            *dev,
            struct ath_ops       **ops,
            asf_amem_instance_handle amem_handle,
            struct ath_reg_parm  *ath_conf_parm,
            struct hal_reg_parm  *hal_conf_parm)
    {
        /* NB: insure BK queue is the lowest priority h/w queue */
        if (!ATH_SET_TX(sc)) {
            error = -EIO;
            goto bad2;
        }    

        int ath_set_tx(struct ath_softc *sc) 
        {
            ath_tx_setup(sc, HAL_WME_AC_BK);
            
            int ath_tx_setup(struct ath_softc *sc, int haltype)
            {
#define    N(a)    (sizeof(a)/sizeof(a[0]))
                struct ath_txq *txq;

                if (haltype >= N(sc->sc_haltype2q)) {   /* 4 == N */
                    printk("HAL AC %u out of range, max %zu!\n",
                            haltype, N(sc->sc_haltype2q));
                    return 0;
                }    
                txq = ath_txq_setup(sc, HAL_TX_QUEUE_DATA, haltype);
                if (txq != NULL) {
                    sc->sc_haltype2q[haltype] = txq->axq_qnum;
                    return 1;
                } else 
                    return 0;
#undef N
            }
        }
    }
}

/**
 * @brief per-frame tx control block
 */
typedef struct 
{
    ath_node_t      an;         /* destination to sent to */
    int             if_id;      /* only valid for cab traffic */
    int             qnum;       /* h/w queue number */

    u_int           ismcast:1;  /* if it's a multicast frame */
    u_int           istxfrag:1; /* if it's a tx fragment */
    u_int           ismgmt:1;   /* if it's a management frame */
    u_int           isdata:1;   /* if it's a data frame */
    u_int           isqosdata:1; /* if it's a qos data frame */
    u_int           ps:1;       /* if one or more stations are in PS mode */
    u_int           shortPreamble:1; /* use short preamble */
    u_int           ht:1;       /* if it can be transmitted using HT */
    u_int           use_minrate:1; /* if this frame should transmitted using specified
                                    * mininum rate */
    u_int           isbar:1;    /* if it is a block ack request */
    u_int           ispspoll:1; /* if it is a PS-Poll frame */
    u_int           calcairtime:1; /* requests airtime be calculated when set for tx frame */
    u_int           iseap:1; /* Is this an EAP packet? */
#ifdef ATH_SUPPORT_UAPSD
    u_int           isuapsd:1;  /* if this frame needs uapsd handling */
#endif
#ifdef ATH_SUPPORT_TxBF
    u_int           isdelayrpt:1;
#endif

    HAL_PKT_TYPE    atype;      /* Atheros packet type */
    u_int32_t       flags;      /* HAL flags */
    u_int32_t       keyix;      /* key index */
    HAL_KEY_TYPE    keytype;    /* key type */
    u_int16_t       txpower;    /* transmit power */
    u_int16_t       seqno;      /* sequence number */
    u_int16_t       tidno;      /* tid number */

    u_int16_t       frmlen;     /* frame length */
#ifdef USE_LEGACY_HAL
    u_int16_t       hdrlen;     /* header length of this frame */
    int             compression; /* compression scheme */
    u_int8_t        ivlen;      /* iv length for compression */
    u_int8_t        icvlen;     /* icv length for compression */
    u_int8_t        antenna;    /* antenna control */
#endif

    int             min_rate;   /* minimum rate */
    int             mcast_rate; /* multicast rate */
    u_int16_t       nextfraglen; /* next fragment length */

    /* below is set only by ath_dev */
    ath_dev_t       dev;        /* device handle */
    u_int8_t        priv[64];   /* private rate control info */

    OS_DMA_MEM_CONTEXT(dmacontext) /* OS specific DMA context */
#ifdef ATH_SUPPORT_HTC
    u_int8_t nodeindex;
    u_int8_t vapindex;
#ifdef ENCAP_OFFLOAD    
    u_int8_t keyid;
    u_int8_t key_mapping_key;
#endif
#endif
} ieee80211_tx_control_t;

2. 
#if LINUX_VERSION_CODE > KERNEL_VERSION(2,6,30)
static const struct net_device_ops athdev_net_ops = {
    .ndo_open    = ath_netdev_open,
    .ndo_stop    = ath_netdev_stop,
    .ndo_start_xmit = ath_netdev_hardstart,
    .ndo_set_mac_address = ath_netdev_set_macaddr,
    .ndo_tx_timeout = ath_netdev_tx_timeout,
    .ndo_get_stats = ath_getstats,
    .ndo_change_mtu = ath_change_mtu,
    .ndo_set_multicast_list = ath_netdev_set_mcast_list,
    .ndo_do_ioctl = ath_ioctl,
};
#endif

static int
ath_netdev_hardstart(struct sk_buff *skb, struct net_device *dev)
{
    do_ath_netdev_hardstart(skb,dev);
}

    2.1 #define do_ath_netdev_hardstart(_skb,_dev) do{\
        if(lmac_run_aponly(_skb,_dev))\

            2.1.1 static inline
                  int lmac_run_aponly(struct sk_buff *skb,struct net_device* dev)
                  {
                      struct ieee80211_node *ni = wbuf_get_node((wbuf_t)skb);
                                2.1.1.1 struct ieee80211_node *wbuf_get_node(wbuf_t wbuf) { return __wbuf_get_node(wbuf); }

                                    2.1.1.1.1 #define __wbuf_get_node                 N_NODE_GET
                                              #define N_NODE_GET(_nbf) (((struct nbuf_cb *)(_nbf)->cb)->ni)

                      vap = ni->ni_vap;
                      opmode = ieee80211vap_get_opmode(vap);

                      /*
                       * Run aponly code --ONLY-- when all the following conditions are true:
                       * (0) Running on Linux (aponly is not currently supported on any other OS)
                       * (1) NAWDS disabled
                       * (2) WDS disabled 
                       * (3) vap is an AP (and not IBSS or STA)
                       */
                      if(opmode == IEEE80211_M_HOSTAP &&
#if UMAC_SUPPORT_NAWDS
                              vap->iv_nawds.mode == IEEE80211_NAWDS_DISABLED &&
#endif
                              !IEEE80211_VAP_IS_WDS_ENABLED(vap)) {
                          return 1;
                      }
                      return 0;
                  }

        return ath_netdev_hardstart_aponly(_skb,_dev);\

            2.1.2 int ath_netdev_hardstart_aponly(struct sk_buff *skb, struct net_device *dev)
                  {
                      struct ath_softc_net80211 *scn = ath_netdev_priv(dev);
                                                                                                       /* 32 */
                      2.1.2.1 ath_net_dev_priv { return (char *)dev + ALIGN(sizeof(struct net_device), NETDEV_ALIGN);}

                      struct ether_header *eh = (struct ether_header *)skb->data;
                      int    ismulti = IEEE80211_IS_MULTICAST(eh->ether_dhost) ? 1 : 0;

                      cb = (struct ieee80211_cb *)skb->cb;
                      ni = cb->ni;

                      if (unlikely(ni == NULL)) {
                          dev_kfree_skb(skb);
                          return 0;
                      }    

                      txq_depth = ath_txq_depth_aponly(scn->sc_dev, scn->sc_ac2q[skb->priority]);
                          2.1.2.2 uint_32_t ath_txq_depth_aponly(ath_dev_t dev, int qnum)
                                  {
                                      #define ATH_DEV_TO_SC(_dev)         ((struct ath_softc *)(_dev))
                                      struct ath_softc *sc = ATH_DEV_TO_SC(dev);
                                      return sc->sc_txq[qnum].axq_depth;
                                   }
                          txq_aggr_depth = ath_txq_aggr_depth_aponly(scn->sc_dev, scn->sc_ac2q[skb->priority]);
                          ath_net80211_addba_status_aponly(ni,cb->u_tid,&addba_status);
                  }
        else \
            return ath_netdev_hardstart_generic(_skb,_dev);\
        }while(0)




