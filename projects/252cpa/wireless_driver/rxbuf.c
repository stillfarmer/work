1. bf->bf_buf_addr[0] = wbuf_map_single(sc->sc_osdev, wbuf, BUS_DMA_FROMDEVICE,
     OS_GET_DMA_MEM_CONTEXT(bf, bf_dmacontext));
 
2. ATH_SET_RX_CONTEXT_BUF(wbuf, bf); 
   
   bf->bf_desc = (void *)rxs;

   TAILQ_INSERT_TAIL(&sc->sc_rxbuf, bf, bf_list);

