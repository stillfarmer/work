

struct ath_rx_edma {
    wbuf_t                  *rxfifo;            /* 硬件缓冲区对应的缓存  */
    u_int8_t                rxfifoheadindex;    /* 当有一段缓存由硬件交由软件处理时， rxfifoheadindex++ */
    u_int8_t                rxfifotailindex;
    u_int8_t                rxfifodepth;        /* count of RXBPs pushed into fifo */
                                                /* when putrxbuf, rxfifodepth++ */
                                                /* 硬件缓冲区缓存可用的数量  */

    u_int32_t               rxfifohwsize;       /* Rx FIFO size from HAL */
    ath_bufhead             rxqueue;            /* 延后队列待软件处理的报文队列 */
    spinlock_t              rxqlock;
};

struct ath_softc
{
    struct ath_rx_edma sc_rxedma[HAL_NUM_RX_QUEUES];
   
    struct
    

}
