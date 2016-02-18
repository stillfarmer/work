#define BUS_DMA_FROMDEVICE 0

bf->bf_buf_addr[0] = wbuf_map_single(sc->sc_osdev, wbuf, BUS_DMA_FROMDEVICE,
                                              OS_GET_DMA_MEM_CONTEXT(bf, bf_dmacontext));

1.  static INLINE dma_addr_t
    wbuf_map_single(osdev_t os_handle, wbuf_t wbuf, int direction, dma_context_t context)
    {
        return __wbuf_map_single(os_handle, wbuf, direction, context);
    }

    1.1 dma_addr_t
        __wbuf_map_single(osdev_t osdev, adf_nbuf_t nbf, int direction, dma_addr_t *pa)
       {
       /*  
        * NB: do NOT use nbf->len, which is 0 on initialization.
        * Use nbf's entire data area instead.
        */
           #define UNI_SKB_END_POINTER(skb)   (skb)->end
           *pa = bus_map_single(osdev, nbf->data, UNI_SKB_END_POINTER(nbf) - nbf->data, direction);

           1.1.1 dma_addr_t bus_map_single(void *hwdev, void *ptr,
                       size_t size, int direction)
                 {
                     osdev_t devhandle = (osdev_t)hwdev; 
                     HAL_BUS_CONTEXT *bc = &(devhandle->bc);

                     if (!(devhandle->bdev) || bc->bc_bustype == HAL_BUS_TYPE_AHB) {
                         dma_map_single(devhandle->bdev, ptr, size,
                           (direction == BUS_DMA_TODEVICE)? DMA_TO_DEVICE : DMA_FROM_DEVICE);  /* DMA_FROM_DEVICE */

                         1.1.1.1 
                     } 
                     else 
                     {
                         pci_map_single(devhandle->bdev, ptr, size,
                           (direction == BUS_DMA_TODEVICE)? PCI_DMA_TODEVICE : PCI_DMA_FROMDEVICE);
                     }   

               return __pa(ptr);
           }
           EXPORT_SYMBOL(bus_map_single);


           return *pa;
       }


