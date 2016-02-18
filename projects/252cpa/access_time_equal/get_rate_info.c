1. userspace 
  
    list_stations()
    {
        struct iwreq iwr;
        (void) strncpy(iwr.ifr_name, ifname, sizeof(iwr.ifr_name));
        iwr.u.data.pointer = (void *) buf;
        iwr.u.data.length = LIST_STATION_ALLOC_SIZE;
        ioctl(s, IEEE80211_IOCTL_STA_INFO, &iwr);
    }

2. kernel 

    int ieee80211_ioctl(struct net_device *dev, struct ifreq *ifr, int cmd)
    {
        /* dev->name = ath0; */
        case IEEE80211_IOCTL_STA_INFO:
            return ieee80211_ioctl_getstainfo(dev, (struct iwreq *) ifr);
           
            2.1 int ieee80211_ioctl_getstainfo(struct net_device *dev, struct iwreq *iwr)
                {
                    #define ath_netdev_priv(d)  netdev_priv(d)
                    #define netdev_priv(dev)   (char *)dev + ALIGN(sizeof(struct net_device), NETDEV_ALIGN);
                    osif_dev  *osifp = ath_netdev_priv(dev);
                    wlan_if_t vap = osifp->os_if;
                    struct stainforeq req;

                    wlan_iterate_station_list(vap, get_sta_space, &req);

                    if (req.space > iwr->u.data.length)
                    {
                        req.space = iwr->u.data.length;
                    }

                    if (req.space > 0)
                    {
                        p = (void *)OS_MALLOC(osifp->os_handle, space, GFP_KERNEL);

                        req.si = (struct ieee80211req_sta_info *)p;
                        wlan_iterate_station_list(vap, get_sta_info, &req);

                        2.1.1 int32_t wlan_iterate_station_list(wlan_if_t vap,ieee80211_sta_iter_func iter_func,void *arg)
                              {
                                  return ieee80211_iterate_node_list(vap, iter_func, arg, 
                                                                    IEEE80211_NODE_ITER_F_ASSOC_STA);

                                         2.1.1.1 int ieee80211_iterate_node_list(wlan_if_t vap,ieee80211_sta_iter_func iter_func,void *arg, u_int32_t flag)
                                                 {
                                                     struct ieee80211_iter_arg itr_arg;

                                                     itr_arg.count=0;
                                                     itr_arg.vap=vap;
                                                     itr_arg.flag=flag;
                                                     ieee80211_iterate_node(ic,ieee80211_node_iter,(void *)&itr_arg);

                                                     2.1.1.1.1 void ieee80211_iterate_node(struct ieee80211com *ic, ieee80211_iter_func *func, void *arg)
                                                               {
                                                                   struct ieee80211_node_table *nt = &ic->ic_sta;
                                                                   struct ieee80211_node *ni = NULL, *next=NULL;

                                                                   TAILQ_FOREACH_SAFE(ni, &nt->nt_node, ni_list, next) 
                                                                   {
                                                                       ieee80211_ref_node(ni);
                                                                       (*func)(arg, ni);
                                                                       ieee80211_free_node(ni);
                                                                   }
                                                               }

                                                     for (i=0;i<itr_arg.count; ++i)
                                                     {
                                                         if (i == IEEE80211_AID_DEF) break;   
                                                         if (iter_func) {
                                                             /* 
                                                              * node has been refed in ieee80211_sta_iter
                                                              * so safe to acces the contentes of the node.
                                                              */
                                                             (* iter_func) (arg, itr_arg.nodes[i]);
                                                         }
                                                         /* decrement the ref count which is incremented above in ieee80211_sta_iter */
                                                         ieee80211_free_node(itr_arg.nodes[i]);
                                                     }

                                                 }
                              }

                        iwr->u.data.length = space - req.space;

                        copy_to_user(iwr->u.data.pointer, p, iwr->u.data.length);
                    }
                }
    }
