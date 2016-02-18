1.
/*
 * This structure defines the management hooks for network devices.
 * The following hooks can be defined; unless noted otherwise, they are
 * optional and can be filled with a null pointer.
 *
 * int (*ndo_init)(struct net_device *dev);
 *     This function is called once when network device is registered.
 *     The network device can use this to any late stage initializaton
 *     or semantic validattion. It can fail with an error code which will
 *     be propogated back to register_netdev
 *
 * void (*ndo_uninit)(struct net_device *dev);
 *     This function is called when device is unregistered or when registration
 *     fails. It is not called if init fails.
 *
 * int (*ndo_open)(struct net_device *dev);
 *     This function is called when network device transistions to the up
 *     state.
 *
 * int (*ndo_stop)(struct net_device *dev);
 *     This function is called when network device transistions to the down
 *     state.
 *
 * int (*ndo_start_xmit)(struct sk_buff *skb, struct net_device *dev);
 *	Called when a packet needs to be transmitted.
 *	Must return NETDEV_TX_OK , NETDEV_TX_BUSY, or NETDEV_TX_LOCKED,
 *	Required can not be NULL.
 *
 * u16 (*ndo_select_queue)(struct net_device *dev, struct sk_buff *skb);
 *	Called to decide which queue to when device supports multiple
 *	transmit queues.
 *
 * void (*ndo_change_rx_flags)(struct net_device *dev, int flags);
 *	This function is called to allow device receiver to make
 *	changes to configuration when multicast or promiscious is enabled.
 *
 * void (*ndo_set_rx_mode)(struct net_device *dev);
 *	This function is called device changes address list filtering.
 *
 * void (*ndo_set_multicast_list)(struct net_device *dev);
 *	This function is called when the multicast address list changes.
 *
 * int (*ndo_set_mac_address)(struct net_device *dev, void *addr);
 *	This function  is called when the Media Access Control address
 *	needs to be changed. If this interface is not defined, the
 *	mac address can not be changed.
 *
 * int (*ndo_validate_addr)(struct net_device *dev);
 *	Test if Media Access Control address is valid for the device.
 *
 * int (*ndo_do_ioctl)(struct net_device *dev, struct ifreq *ifr, int cmd);
 *	Called when a user request an ioctl which can't be handled by
 *	the generic interface code. If not defined ioctl's return
 *	not supported error code.
 *
 * int (*ndo_set_config)(struct net_device *dev, struct ifmap *map);
 *	Used to set network devices bus interface parameters. This interface
 *	is retained for legacy reason, new devices should use the bus
 *	interface (PCI) for low level management.
 *
 * int (*ndo_change_mtu)(struct net_device *dev, int new_mtu);
 *	Called when a user wants to change the Maximum Transfer Unit
 *	of a device. If not defined, any request to change MTU will
 *	will return an error.
 *
 * void (*ndo_tx_timeout)(struct net_device *dev);
 *	Callback uses when the transmitter has not made any progress
 *	for dev->watchdog ticks.
 *
 * struct net_device_stats* (*get_stats)(struct net_device *dev);
 *	Called when a user wants to get the network device usage
 *	statistics. If not defined, the counters in dev->stats will
 *	be used.
 *
 * void (*ndo_vlan_rx_register)(struct net_device *dev, struct vlan_group *grp);
 *	If device support VLAN receive accleration
 *	(ie. dev->features & NETIF_F_HW_VLAN_RX), then this function is called
 *	when vlan groups for the device changes.  Note: grp is NULL
 *	if no vlan's groups are being used.
 *
 * void (*ndo_vlan_rx_add_vid)(struct net_device *dev, unsigned short vid);
 *	If device support VLAN filtering (dev->features & NETIF_F_HW_VLAN_FILTER)
 *	this function is called when a VLAN id is registered.
 *
 * void (*ndo_vlan_rx_kill_vid)(struct net_device *dev, unsigned short vid);
 *	If device support VLAN filtering (dev->features & NETIF_F_HW_VLAN_FILTER)
 *	this function is called when a VLAN id is unregistered.
 *
 * void (*ndo_poll_controller)(struct net_device *dev);
 */
#define HAVE_NET_DEVICE_OPS
struct net_device_ops {
	int			(*ndo_init)(struct net_device *dev);
	void			(*ndo_uninit)(struct net_device *dev);
	int			(*ndo_open)(struct net_device *dev);
	int			(*ndo_stop)(struct net_device *dev);
	int			(*ndo_start_xmit) (struct sk_buff *skb,
						   struct net_device *dev);
	u16			(*ndo_select_queue)(struct net_device *dev,
						    struct sk_buff *skb);
#define HAVE_CHANGE_RX_FLAGS
	void			(*ndo_change_rx_flags)(struct net_device *dev,
						       int flags);
#define HAVE_SET_RX_MODE
	void			(*ndo_set_rx_mode)(struct net_device *dev);
#define HAVE_MULTICAST
	void			(*ndo_set_multicast_list)(struct net_device *dev);
#define HAVE_SET_MAC_ADDR
	int			(*ndo_set_mac_address)(struct net_device *dev,
						       void *addr);
#define HAVE_VALIDATE_ADDR
	int			(*ndo_validate_addr)(struct net_device *dev);
#define HAVE_PRIVATE_IOCTL
	int			(*ndo_do_ioctl)(struct net_device *dev,
					        struct ifreq *ifr, int cmd);
#define HAVE_SET_CONFIG
	int			(*ndo_set_config)(struct net_device *dev,
					          struct ifmap *map);
#define HAVE_CHANGE_MTU
	int			(*ndo_change_mtu)(struct net_device *dev,
						  int new_mtu);
	int			(*ndo_neigh_setup)(struct net_device *dev,
						   struct neigh_parms *);
#define HAVE_TX_TIMEOUT
	void			(*ndo_tx_timeout) (struct net_device *dev);

	struct net_device_stats* (*ndo_get_stats)(struct net_device *dev);

	void			(*ndo_vlan_rx_register)(struct net_device *dev,
						        struct vlan_group *grp);
	void			(*ndo_vlan_rx_add_vid)(struct net_device *dev,
						       unsigned short vid);
	void			(*ndo_vlan_rx_kill_vid)(struct net_device *dev,
						        unsigned short vid);
#ifdef CONFIG_NET_POLL_CONTROLLER
#define HAVE_NETDEV_POLL
	void                    (*ndo_poll_controller)(struct net_device *dev);
#endif
#if defined(CONFIG_FCOE) || defined(CONFIG_FCOE_MODULE)
	int			(*ndo_fcoe_ddp_setup)(struct net_device *dev,
						      u16 xid,
						      struct scatterlist *sgl,
						      unsigned int sgc);
	int			(*ndo_fcoe_ddp_done)(struct net_device *dev,
						     u16 xid);
#endif
};


2. 
/** 
 *	struct sk_buff - socket buffer
 *	@next: Next buffer in list
 *	@prev: Previous buffer in list
 *	@sk: Socket we are owned by
 *	@tstamp: Time we arrived
 *	@dev: Device we arrived on/are leaving by
 *	@transport_header: Transport layer header
 *	@network_header: Network layer header
 *	@mac_header: Link layer header
 *	@_skb_dst: destination entry
 *	@sp: the security path, used for xfrm
 *	@cb: Control buffer. Free for use by every layer. Put private vars here
 *	@len: Length of actual data
 *	@data_len: Data length
 *	@mac_len: Length of link layer header
 *	@hdr_len: writable header length of cloned skb
 *	@csum: Checksum (must include start/offset pair)
 *	@csum_start: Offset from skb->head where checksumming should start
 *	@csum_offset: Offset from csum_start where checksum should be stored
 *	@local_df: allow local fragmentation
 *	@cloned: Head may be cloned (check refcnt to be sure)
 *	@nohdr: Payload reference only, must not modify header
 *	@pkt_type: Packet class
 *	@fclone: skbuff clone status
 *	@ip_summed: Driver fed us an IP checksum
 *	@priority: Packet queueing priority
 *	@users: User count - see {datagram,tcp}.c
 *	@protocol: Packet protocol from driver
 *	@truesize: Buffer size 
 *	@head: Head of buffer
 *	@data: Data head pointer
 *	@tail: Tail pointer
 *	@end: End pointer
 *	@destructor: Destruct function
 *	@mark: Generic packet mark
 *	@nfct: Associated connection, if any
 *	@ipvs_property: skbuff is owned by ipvs
 *	@peeked: this packet has been seen already, so stats have been
 *		done for it, don't do them again
 *	@nf_trace: netfilter packet trace flag
 *	@nfctinfo: Relationship of this skb to the connection
 *	@nfct_reasm: netfilter conntrack re-assembly pointer
 *	@nf_bridge: Saved data about a bridged frame - see br_netfilter.c
 *	@iif: ifindex of device we arrived on
 *	@queue_mapping: Queue mapping for multiqueue devices
 *	@tc_index: Traffic control index
 *	@tc_verd: traffic control verdict
 *	@ndisc_nodetype: router type (from link layer)
 *	@do_not_encrypt: set to prevent encryption of this frame
 *	@dma_cookie: a cookie to one of several possible DMA operations
 *		done by skb DMA functions
 *	@secmark: security marking
 *	@vlan_tci: vlan tag control information
 */

struct sk_buff {
	/* These two members must be first. */
	struct sk_buff		*next;
	struct sk_buff		*prev;

	struct sock		*sk;
	ktime_t			tstamp;
	struct net_device	*dev;

	unsigned long		_skb_dst;
#ifdef CONFIG_XFRM
	struct	sec_path	*sp;
#endif
	/*
	 * This is the control buffer. It is free to use for every
	 * layer. Please put your private variables there. If you
	 * want to keep them across layers you have to do a skb_clone()
	 * first. This is owned by whoever has the skb queued ATM.
	 */
	char			cb[48];
#if defined(CONFIG_IMQ) || defined(CONFIG_IMQ_MODULE)
	void			*cb_next;
#endif

	unsigned int		len,
				data_len;
	__u16			mac_len,
				hdr_len;
	union {
		__wsum		csum;
		struct {
			__u16	csum_start;
			__u16	csum_offset;
		};
	};
	__u32			priority;
	kmemcheck_bitfield_begin(flags1);
	__u8			local_df:1,
				cloned:1,
				ip_summed:2,
				nohdr:1,
				nfctinfo:3;
	__u8			pkt_type:3,
				fclone:2,
				ipvs_property:1,
				peeked:1,
				nf_trace:1;
	kmemcheck_bitfield_end(flags1);
	__be16			protocol;

	void			(*destructor)(struct sk_buff *skb);
#if defined(CONFIG_NF_CONNTRACK) || defined(CONFIG_NF_CONNTRACK_MODULE)
	struct nf_conntrack	*nfct;
	struct sk_buff		*nfct_reasm;
#endif
#if defined(CONFIG_IMQ) || defined(CONFIG_IMQ_MODULE)
	struct nf_queue_entry	*nf_queue_entry;
#endif
#ifdef CONFIG_BRIDGE_NETFILTER
	struct nf_bridge_info	*nf_bridge;
#endif

	int			iif;
	__u16			queue_mapping;
#ifdef CONFIG_NET_SCHED
	__u16			tc_index;	/* traffic control index */
#ifdef CONFIG_NET_CLS_ACT
	__u16			tc_verd;	/* traffic control verdict */
#endif
#endif

	kmemcheck_bitfield_begin(flags2);
#ifdef CONFIG_IPV6_NDISC_NODETYPE
	__u8			ndisc_nodetype:2;
#endif
#if defined(CONFIG_MAC80211) || defined(CONFIG_MAC80211_MODULE)
	__u8			do_not_encrypt:1;
#endif
	kmemcheck_bitfield_end(flags2);

	/* 0/13/14 bit hole */
#if defined(CONFIG_IMQ) || defined(CONFIG_IMQ_MODULE)
	__u8			imq_flags:IMQ_F_BITS;
#endif

#ifdef CONFIG_NET_DMA
	dma_cookie_t		dma_cookie;
#endif
#ifdef CONFIG_NETWORK_SECMARK
	__u32			secmark;
#endif

	__u32			mark;

	__u16			vlan_tci;

	sk_buff_data_t		transport_header;
	sk_buff_data_t		network_header;
	sk_buff_data_t		mac_header;
	/* These elements must be at the end, see alloc_skb() for details.  */
	sk_buff_data_t		tail;
	sk_buff_data_t		end;
	unsigned char		*head,
				*data;
	unsigned int		truesize;
	atomic_t		users;
#ifdef CONFIG_ATHRS_HW_NAT
        __u32                   ath_hw_nat_fw_flags;
#endif
};
