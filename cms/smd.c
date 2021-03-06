1. struct

    1.1 typedef struct     //contains dynamic information about a dynamically lauched process.
	    {                  // Static information  contained in the CmsEntityInfo structure.
		    DlistNode     dlist;
            const CmsEntityInfo *eInfo;
			DlsState             state;           //  Current state of this dynamically lauchned  process.
			SINT32               serverFd;        //   Server/listening  sock fd
			SINT32               commFd;          // Unix domain TCP connection socket fd on smd for IPC with other processes.
			SINT32               pid;             // pid of the app
			UINBT32              specified;       //app have multi instances , combined pid and eid
            SINT32               numDelayedMsgRequested;  //number of delayed msgs requested by this app.
	        SINT32               numEventInterestRequested; //number of event interests requested by this app.
	        CmsMsgHeader         *msgQueue;    //queueof messages waiting to be delivered to this app.
            char                 optArgs[DLS_OPT_ARGS_LENGTH]; //additional dynamic args, usually sent in by restart msg.
        } DlsInfo


	1.2 typedef struct
		{
			CmsEntityId eid;    /**< The entity id for this entry. */
			UINT16 accessBit;   /**< This entity's bit position in MdmNodeAttributes.accessBitMask (only 15 bits available) */
			char *name;         /**< Short name for this entity. */
			char *path;         /**< Full path to the executable. */
			char *runArgs;      /**< Args to pass to this executable on startup */
			UINT16 flags;       /**< Additional info about this entity, see EIF_xxx */
			UINT8  backLog;     /**< Backlog arg for the server socket */
			UINT16 port;        /**< If server, port number for the server socket. */
			UBOOL8 isFeatureCompiledIn; /**< Is this feature selected via make menuconfig */
			UINT32 maxMemUsage;    /**< Maximum memory usage, in KB, 0 if unknown. */
			UINT32 normalMemUsage; /**< Typical memory usage, in KB, 0 if unknown. */
			UINT32 minMemUsage;    /**< Minimum memory usage, in KB, 0 if unknown. */
		} CmsEntityInfo;    
		/*included in eid.c*/
	
	1.3 typedef struct
	    {
			struct dlist_node *next;
			struct dlist_node *prev;
		}DlistNode;
	
		DLIST_HEAD(dlsInfoHead);
	    struct dlist_node dlsInfoHead = DLIST_HEAD_INIT(dlsInfoHead);
		                              = {&dlsInfoHead, &dlsInfoHead};

	1.4
	/** Return byte offset of the specified member.
	 *
	 * This is defined in stddef.h for MIPS, but not defined
	 * on LINUX desktop systems.  Play it safe and just define
	 * it here for all build types.
	 */
	#undef offsetof
	#define offsetof(TYPE, MEMBER) ((size_t) &((TYPE *)0)->MEMBER)


	/** cast a member of a structure out to the containing structure
	 *
	 * @ptr:    the pointer to the member.
	 * @type:   the type of the container struct this is embedded in.
	 * @member: the name of the member within the struct.
	 *
	 */
	#define container_of(ptr, type, member) ({          \
		const typeof( ((type *)0)->member ) *__mptr = (ptr);    \
		(type *)( (char *)__mptr - offsetof(type,member) );})


	#define dlist_entry(ptr, type, member) \
		container_of(ptr, type, member)




	#define dlist_entry(ptr, type, member) \
		container_of(ptr, type, member)

	/** Create a for loop over all entries in the dlist.
	 *
	 * @param pos A variable that is the type of the structure which
	 *            contains the DlistNode.
	 * @param head Pointer to the head of the dlist.
	 * @param member The field name of the DlistNode field in the
	 *               containing structure.
	 *
	 */
	#define dlist_for_each_entry(pos, head, member)  \
		for ( pos = dlist_entry( (head)->next, typeof(*pos), member);
				&pos->member != (head);
				pos = dlist_entry( pos->member.next, typeof(*pos), member))



2. code

	main     // main.c

	2.1 smd_init

		2.1.1 system_init
		/* signal handler setup & write to proc file */

		e.g. signal( SIGTERM, SIGIGN);
			 e.g. cmsFil_writeToProc("/proc/sys/net/ipv4/ip_forward", "1");


		2.1.2 prmgr_init
	         /*  it may be designed to handle the proc file, but do nothing here */


		2.1.3 oalEvent_init
		    /*  init Unix domain socket
			    add stage 1 dInfo to dlsInfoHead , in state 1, only ssk dInfo is added
				oal_launchOnBoot(1) */

			2.1.3.1 interest_init
				/*  do nothing */

			2.1.3.2 sched_init

			    /* init smd tmrHandle */

			2.1.3.3 ipcListenFd = initUnixDomainServerSocket()
				/* initialize my own Unix domain listening socket for other apps to connect to me */

				    

			2.1.3.4 FD_SET(ipcListenedFd, &readFdsMaster);
			        UPDATE_MAXFD(ipcListenedFd);

			2.1.3.5 initDls(state)    //stage = 1
                /* Initialize dynamic launch entries for the stage 1 apps */

				2.1.3.5.1 eInfo = cmsEid_getFirstEntityInfo();
				          /* get entiryInfoArr addr*/

				2.1.3.5.2 numEntries = cmsEid_getNumberOfEntityInfoEntries();
				          /* get entiryInfoArr num */

				2.1.3.5.3 if ( 1 == stage)
				          {/* find process, EIF_LAUNCH_IN_STAGE_1 */

                              for ( i = 0; i < numEntries; i++)
						      {
						          if (..&EIF_LAUNCH_IN_STAGE_1 ..)
							      {
						  	          dInfo = insertDlsInfoEntry(eInfo->eid);

								      2.1.3.5.3.1 insertDlsInfoEntry()
								      {
									      dInfo = cmsMem_alloc();
									      dInfo->eInfo =  cmsEid_getEntityInfo(GENERIC_EID(eid));

									      dInfo->state = DLS_NOT_RUNNING;
									      dInfo->commFd = CMS_INVALID_FD;
									      dInfo->serverFd = CMS_INVALID_FD;
									      dInfo->pid = CMS_INVALID_PID;

									      if ( dInfo->eInfo->flags & EIF_SERVER)
								          {
										      dInfo->serverFd = initInetServerSocket(domain,
																			   dInfo->eInfo->port,
																			   socketType,
																			   dInfo->eInfo->backlog)
								              FD_SET( dInfo->serverFd, &readFdsMaster);
										      UPDATE_MAXFD(dInfo->serverFd);
									      }

									      dlist_prepend((DlistNode *) dInfo, &dlsInfoHead);

									      /* add a new entry in front of an existing list element */
									      /* after this action, dlsInfoHead.prev = dInfo, dlsInfoHead.next = firstAdded node */

									      2.1.3.5.3.1.1 dlist_prepend;
									                {
														existing->prev->next = new_node;       /* dlsInfoHead.prev->next = dInfo */

														new_node->next = existing;             /* dInfo->next = &dlsInfoHead */
														new_node->prev = existing->prev;       /* dInfo->prev = dlsInfoHead.prev */

														existing->prev = new_node;             /* dlsInfoHead.prev = dInfo */
													}

													/* dlsInfoHead.prev = dInfo; dInfo->next = &dlsInfoHead; d */

									      return dInfo;
							          }
						          }
						      }
					      }

                          /* this is stage 2*/
						  else
						  {
						      /* initialize stage 2 apps (which include the launch-on-boot apps) */
						      for (i=0; i < numEntries; i++) 
							  {    
						          if ((eInfo->isFeatureCompiledIn) &&
									 ((eInfo->flags & (EIF_LAUNCH_ON_BOOT|EIF_SERVER)) ||
						             (eInfo->eid == EID_CONSOLED)))
								  {    
								      if ((dInfo = getExistingDlsInfo(eInfo->eid)) != NULL)
									  {    
									      /** A server app, such as tr69c, could have been launched
										   * during ssk initialization of the MDM.  So if there is
										   * a dInfo structure already, don't insert it again.
										   */
										  cmsLog_debug("dInfo entry for %s already inserted", eInfo->name);
									  }    

									  else 
									  {    
									      cmsLog_debug("inserting stage 2 entity: %s (%d)", eInfo->name, eInfo->eid);
										  dInfo = insertDlsInfoEntry(eInfo->eid);
										  if (dInfo == NULL)
										  {    
										      cmsLog_error("failed to initialize dInfo for %s", eInfo->name);
											  return CMSRET_INTERNAL_ERROR;
										  }    
									  }
								  }
						      eInfo++;
							}

						  }

						  /*End of oalEvent_init, 2.1.3 
			    			    add stage 1 dInfo to dlsInfoHead , in state 1, only ssk dInfo is added  */

		2.1.4 oal_LaunchOnBoot(1);
			
			2.1.4.1 oal_LaunchOnboot(1)
			        {
						DlsInfo *dInfo;
						dlist_for_each_entry(dInfo, &dlsInfoHead, dlist)
						{
							if (((stage == 1) && (dInfo->eInfo->flags & EIF_LAUNCH_IN_STAGE_1)) ||
									((stage == 2) && (dInfo->eInfo->flags & EIF_LAUNCH_ON_BOOT)))
							{
								if ( dInfo->eInfo->flags & EIF_MESSAGING_CAPABLE)              /* ssk has the flag */
								{
									msg->type = CMS_MSG_SYSTEM_BOOT;
									msg->src  = EID_SMD;
									msg->dst  = dInfo->eInfo->eid;
									msg->flags_event = 1;

									/* app which was marked as launch_on_boot during stage 2 cloud have been already
									   launched during ssk's call to cmsMdm_init, so must use sendMessageByState 
									   to deliver the msg, which will either queue or send the msg depending on the app's state */
									sendMessageByState(dInfo, &msg);

									2.1.4.1.1 
									sendMessageByState
									{
										switch( dInfo->state)
										{
											case DLS_NOT_RUNNING:

												  /* append msg to dInfo->msgQueue */
												  queueMsg(dInfo, msg);

												  launchApp(dInfo);
												  /* when to launchApp? 
												    (1) sendMessageByState */
													(2) listen on server id and FD_ISSET

												  2.1.4.1.1.1
												  launchApp
												  {
												  	  dInfo->pid = CMS_INVALID_PID;


												      spawnInfo.exe = exeBuf;    /* exeBuf = dInfo->eInfo->path */
													  spawnInfo.args = argsBuf;  
													  spawnInfo.spawnMode = SPAWN_AND_RETURN;
													  spawnInfo.stdinFd = 0;
													  spawnInfo.stdoutFd = 1;
													  spawnInfo.stderrFd = 2;
													  spawnInfo.serverFd = dInfo->serverFd;
													  spawnInfo.maxFd = maxFd;
													  spawnInfo.inheritSigint = TRUE;
													  
													  prctl_spawnProcess(&spawnInfo, &procInfo);

													  2.1.4.1.1.1.1

													  /* prctl_spawnProcess start */
													  prctl_spawnProcess()
													  {
													      oal_spawnProcess()

														  2.1.4.1.1.1.1.1
														  /* oal_spawnProcess start */
														  {
														      pid = fork();

															  if ( 0 == pid)
															  {
															      /*This is child */
																  1. stdin, stdout, stderror handled.
																  2. signal handler handled.
																  3. serverFd handled.
																     dup2(spawnInfo->serverFd, CMS_DYNAMIC_LAUNCH_SERVER_FD); 
																	 /* CMS_DYNAMIC_LAUNCH_SERVER_FD = 3 */

																  execv(spawnInfo->exe, argv);
															  }
														  }
														  /* oal_spawnProcess end */
													  }
													  /* prctl_spawnPrcess end */

													  dInfo->pid = procInfo->pid;

   													  if (dInfo->eInfo->flags & EIF_MESSAGING_CAPABLE)
      												  {
	        										      dInfo->state = DLS_LAUNCHED;
			   										  }
			      									  else 
				     								  {
					       							      /*  message capable, mark it as RUNNING without conf." */
						         					      dInfo->state = DLS_RUNNING;
								    				  }

													  /* once we launch an app, stop monitoring its fd */
													  if ((dInfo->eInfo->flags & EIF_SERVER) ||
															(dInfo->eInfo->eid == EID_CONSOLED))
													  {
													      FD_CLR(dInfo->serverFd, &readFdsMaster);
													  }
												  }
												  /* end of launchApp 2.1.4.1.1.1 */

												  break;
										    ....
										}
									}
								}
							}
						}
					}
    /* end of smd_init 2.1 */

	2.2 smd_main
	    {
		    while (keepLooping)
			{
			    /* do periodic/timer event processing */
			    sched_process();

				2.2.1 
				sched_process
				{
					cmsTmr_executeExpiredEvents(tmrHandle);
				}
				/*end of 2.2.1 sched_process */

				oal_processEvents();

				2.2.2
				oal_processEvents
				{
				    for ( i = 0; i < (maxFd + 1); i++)
					{
					    if ( !FD_ISSET(i, &readFds))
						{
						    /* no message, no socket */
						    continue;
						}

                        /* listen on server fd; on launch, only ssk added to the dls info */
						if ( NULL != ( dInfo = getDlsInfoByServerFd(i)))
						{
						    2.2.2.1 getDlsInfoByServerFd
						    /* check dls list if the serverFd == i, return dInfo*/

						    if (dInfo->state != DLS_NOT_RUNNING)
						    {    
							    /* if the app is running , it should listen the socket and its fd should be FD_CLR in launchApp */
						        cmsLog_error("got activity on server socket while app %s is in state %d",
											   dInfo->eInfo->name, dInfo->state);
					        }    
							else 
							{    
							    /* launch the application process */
								cmsLog_debug("got activity on server socket %d for %s", dInfo->serverFd, dInfo->eInfo->name);
								launchApp(dInfo);
						    }
						}

						/* check for activity on the ipc comm sockets to the individual apps */
						/* on launch, only ssk info add, and when ssk cmsMsg_init not complete, dInfo will be null */
				        if ((dInfo = getDlsInfoByCommFd(i)) != NULL)
						{
							CmsMsgHeader *msg;
							CmsRet ret;

							cmsLog_notice("detected message on fd %d from %s", dInfo->commFd, dInfo->eInfo->name);

							ret = oalMsg_receive(dInfo->commFd, &msg, NULL);
							if (ret == CMSRET_SUCCESS)
							{
								processMessage(dInfo, &msg);

								2.2.2.2 processMessage(DlsInfo *dInfo, CmsMsgHeader **msg)
								        {
										    if ((*msg)->flags_requeue == 1) /*to resolve*/
											{
											}

											if ( (*msg)->dst == EID_SMD)
											{
												/* ssk send to smd to tell shmId when mdm_init finished */
											    if ( (*msg)->type == CMS_MSG_SHMID)    
												{
												}
                                            	/* this is very important, when ssk init mdm finished, it will send the message
											   		and then smd will enter stage 2 */
												else if ((*msg)->type ==  CMS_MSG_MDM_INITIALIZED)
												{
												    stage = 2;
													cmsMdm_init;
													initLoggingFromConfig;

													initDls(stage);
													oal_launchOnBoot(stage);
												}
											}

											else
											{
											    routeMessage(msg);

												2.2.2.2.1 routeMessage()
												          {
														      dstDInfo = getExistingDlsInfo(dstEid);
														      if ((*msg)->flags_bounceIfNotRunning)
															  {
															  }

															  if (dstDInfo == NULL)
															  {
															  }

															  sendMessageByState(dstDInfo, msg);
														  }
											}
										}
							}
							else if (ret == CMSRET_DISCONNECTED)
							{
								CmsEntityId exitEid = dInfo->eInfo->eid;
								cmsLog_notice("detected exit of %s (pid=%d) on fd %d", dInfo->eInfo->name, dInfo->pid, dInfo->commFd);
								collectApp(&dInfo);
								if (exitEid == EID_SSK)
								{
									cmsLog_error("ssk has died.  smd must exit now!");
									return CMSRET_INTERNAL_ERROR;
								}
							}
							else
							{
								cmsLog_error("error on read from fd %d for %s", dInfo->commFd, dInfo->eInfo->name);
							}
						}

						/*   
						 * check for newly launched processes (either launched by smd or on
						 * command line) connecting back to smd.
						 */

						/* this was first detect, when ssk cmsMsg_init send CMS_MSG_APP_LAUNCHED */
						if (i == ipcListenFd)
						{    
						    fd = accept(ipcListenFd, (struct sockaddr *)&clientAddr, &sockAddrSize);

							ret = oalMsg_receive(fd, &msg, &timeout);

							dInfo = validateAppLaunchedMsg(msg);
							        /* check if launched message, if it is
									   call getDlsInfo to check if it is already in dls
									   if it is not already in, call getNewDlsInfo( in getDlsInfo) 
									   to insertDlsInfoEntry */
							        2.2.2.3
							dInfo->commFd = fd;
							processLaunchConfirmation(dInfo);

							2.2.2.4 processLaunchConfirmation
							        {
									    dInfo->state = DLS_RUNNING;

										/* after this two actions, app send msg will not enter judge ( i = ipcListendFd ) 
										   because connect has finished */
										FD_SET(dInfo->commFd, &readFdsMaster);
										UPDATE_MAXFD(dInfo->commFd);
									}
						}
					}
					/* end of 2.2.2 oal_processEvents */

				}

			}
			/* end of smd_main 2.2 */
      


3. interest list

    3.1 struct

        /** Structure to track a single entity that is interested in a particular event.
        *
        */
        typedef struct event_interest_info
        {
            DlistNode          dlist;     /**< generic doubly linked list handle */
            CmsEntityId        eid;       /**< eid of interested app. */
            char *             matchData; /**< extra matching data beyond just the msgType. */
        } EvtInterestInfo;


        /** Structure to keep track of all entities that are interested in a particular event.
         */
        typedef struct event_interest
        {
            DlistNode      dlist;       /**< generic doubly linked list handle */           
                                        /*该链表节点所接链表， 是interst msg type 所接链表 */

            CmsMsgType     type;        /**< Event of interest, which is just a notification msg type */

            DlistNode      evtInfoHead; /**< Doubly linked list of EvtInterestInfo's for this event. */  
                                    /* 该链表节点所接链表， 是对该msg type 感兴趣的eid的链表， 链表节点见 3.1.1*/
        } EvtInterest;                  


   3.2
       in processMessage oal_event.c

       if ( EID_SMD == (*msg)->dst)
       {
           if ( (*msg)->flags_event)
           {
               This is an event msg sent to smd.
           }

           else
           {
               /* this is an request msg sent to smd */

               case CMS_MSG_REGISTER_EVENT_INTEREST:
                   processRegisterEventInterest(dInfo, msg, TRUE);

                   3.2.1 processRegisterEventInterest(DlsInfo *dInfo, CmsMsgHeader **msg, UBOOL8 positive)
                   {
                       interest_register((*msg)->wordData, (*msg)->src, matchData)

                           3.2.1.1 interest_register(CmsMsgType msgType, CmsEntityId interestedEid, const char *matchData) /* mathc*/
                                   {
                                       if((evtInterest = interest_findType(msgType)) == NULL)
                                       {
                                           /* 该消息类型初次注册， 添加该消息事件 */
                                           evtInterest = (EvtInterest *) cmsMem_alloc(sizeof(EvtInterest), ALLOC_ZEROIZE);

                                           evtInterest->type = msgType;
                                           DLIST_HEAD_IN_STRUCT_INIT(evtInterest->evtInfoHead);   /* 初始化对该msg感兴趣的eid的链表头 */

                                           /* link new evtInterest onto list */
                                           dlist_append((struct dlist_node *) evtInterest, &evtInterestHead); /* 将该事件添加到interst 链表里 */
                                       }

                                       /*  
                                        * at this point, evtInterest is pointing to the event of interest,
                                        * check for duplicate registration. 检查是否有同一eid对同一msgtype重复注册。
                                        */
                                       if ((evtInfo = interest_findInfo(evtInterest, interestedEid)) != NULL)
                                       {   
                                           if (cmsUtl_strcmp(evtInfo->matchData, matchData))
                                           {   
                                               cmsLog_error("different match data fields for same eid %d, ignore new matchData", evtInfo->eid);
                                           }   

                                           cmsLog_debug("duplicate registration for event 0x%x eid %d (0x%x), ignore registration",
                                                   evtInterest->type, interestedEid, interestedEid);
                                           return CMSRET_INVALID_ARGUMENTS;
                                       }   

                                       /* No duplicate found, allocate a new EvtInterestInfo and link it into list */
                                       evtInfo = (EvtInterestInfo *) cmsMem_alloc(sizeof(EvtInterestInfo), ALLOC_ZEROIZE);
                                       if (evtInfo == NULL)
                                       {   
                                           cmsLog_error("Could not allocate EvtInterestInfo");
                                           return CMSRET_RESOURCE_EXCEEDED;
                                       }   

                                       evtInfo->eid = interestedEid;
                                       evtInfo->matchData = cmsMem_strdup(matchData);

                                       dlist_append((DlistNode *) evtInfo, &(evtInterest->evtInfoHead));
                                   }
                   }

                   break;

               case CMS_MSG_UNREGISTER_EVENT_INTEREST:
                   processRegisterEventInterest(dInfo, msg, FALSE);
                   break;

               case CMS_MSG_REGISTER_DELAYED_MSG:
                   processRegisterDelayedMsg(dInfo, msg, TRUE);

                   3.2.2 
                   break;

               case CMS_MSG_UNREGISTER_DELAYED_MSG:
                   processRegisterDelayedMsg(dInfo, msg, FALSE);
                   break;
           }
       }












