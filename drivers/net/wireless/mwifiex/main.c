/*
 * Marvell Wireless LAN device driver: major functions
 *
 * Copyright (C) 2011, Marvell International Ltd.
 *
 * This software file (the "File") is distributed by Marvell International
 * Ltd. under the terms of the GNU General Public License Version 2, June 1991
 * (the "License").  You may use, redistribute and/or modify this File in
 * accordance with the terms and conditions of the License, a copy of which
 * is available by writing to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA or on the
 * worldwide web at http://www.gnu.org/licenses/old-licenses/gpl-2.0.txt.
 *
 * THE FILE IS DISTRIBUTED AS-IS, WITHOUT WARRANTY OF ANY KIND, AND THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY OR FITNESS FOR A PARTICULAR PURPOSE
 * ARE EXPRESSLY DISCLAIMED.  The License provides additional details about
 * this warranty disclaimer.
 */

#include "main.h"
#include "wmm.h"
#include "cfg80211.h"
#include "11n.h"

#define VERSION	"1.0"

const char driver_version[] = "mwifiex " VERSION " (%s) ";

/*
                                                                    
                   
  
                                                          
                                    
                                                             
                                                        
                                     
                                                  
                          
  
                                                                           
                                 
 */
static int mwifiex_register(void *card, struct mwifiex_if_ops *if_ops,
			    void **padapter)
{
	struct mwifiex_adapter *adapter;
	int i;

	adapter = kzalloc(sizeof(struct mwifiex_adapter), GFP_KERNEL);
	if (!adapter)
		return -ENOMEM;

	*padapter = adapter;
	adapter->card = card;

	/*                                               */
	memmove(&adapter->if_ops, if_ops, sizeof(struct mwifiex_if_ops));

	/*                                                             */
	if (adapter->if_ops.init_if(adapter))
		goto error;

	adapter->priv_num = 0;

	/*                                       */
	adapter->priv[0] = kzalloc(sizeof(struct mwifiex_private), GFP_KERNEL);
	if (!adapter->priv[0]) {
		dev_err(adapter->dev,
			"%s: failed to alloc priv[0]\n", __func__);
		goto error;
	}

	adapter->priv_num++;

	adapter->priv[0]->adapter = adapter;
	mwifiex_init_lock_list(adapter);

	init_timer(&adapter->cmd_timer);
	adapter->cmd_timer.function = mwifiex_cmd_timeout_func;
	adapter->cmd_timer.data = (unsigned long) adapter;

	return 0;

error:
	dev_dbg(adapter->dev, "info: leave mwifiex_register with error\n");

	for (i = 0; i < adapter->priv_num; i++)
		kfree(adapter->priv[i]);

	kfree(adapter);

	return -1;
}

/*
                                                                      
            
  
                                                   
                         
                             
                                 
                                
 */
static int mwifiex_unregister(struct mwifiex_adapter *adapter)
{
	s32 i;

	del_timer(&adapter->cmd_timer);

	/*                         */
	for (i = 0; i < adapter->priv_num; i++) {
		if (adapter->priv[i]) {
			mwifiex_free_curr_bcn(adapter->priv[i]);
			kfree(adapter->priv[i]);
		}
	}

	kfree(adapter);
	return 0;
}

/*
                    
  
                                                                               
                                                                       
  
                                                   
                                    
                                                               
                                      
                                                             
                                               
                                  
                                       
 */
int mwifiex_main_process(struct mwifiex_adapter *adapter)
{
	int ret = 0;
	unsigned long flags;

	spin_lock_irqsave(&adapter->main_proc_lock, flags);

	/*                             */
	if (adapter->mwifiex_processing) {
		spin_unlock_irqrestore(&adapter->main_proc_lock, flags);
		goto exit_main_proc;
	} else {
		adapter->mwifiex_processing = true;
		spin_unlock_irqrestore(&adapter->main_proc_lock, flags);
	}
process_start:
	do {
		if ((adapter->hw_status == MWIFIEX_HW_STATUS_CLOSING) ||
		    (adapter->hw_status == MWIFIEX_HW_STATUS_NOT_READY))
			break;

		/*                                 */
		if (adapter->int_status) {
			if (adapter->hs_activated)
				mwifiex_process_hs_config(adapter);
			adapter->if_ops.process_int_status(adapter);
		}

		/*                            */
		if ((adapter->ps_state == PS_STATE_SLEEP) &&
		    (adapter->pm_wakeup_card_req &&
		     !adapter->pm_wakeup_fw_try) &&
		    (is_command_pending(adapter) ||
		     !mwifiex_wmm_lists_empty(adapter))) {
			adapter->pm_wakeup_fw_try = true;
			adapter->if_ops.wakeup(adapter);
			continue;
		}
		if (IS_CARD_RX_RCVD(adapter)) {
			adapter->pm_wakeup_fw_try = false;
			if (adapter->ps_state == PS_STATE_SLEEP)
				adapter->ps_state = PS_STATE_AWAKE;
		} else {
			/*                                          */
			if (adapter->pm_wakeup_fw_try)
				break;
			if (adapter->ps_state != PS_STATE_AWAKE ||
			    adapter->tx_lock_flag)
				break;

			if (adapter->scan_processing || adapter->data_sent ||
			    mwifiex_wmm_lists_empty(adapter)) {
				if (adapter->cmd_sent || adapter->curr_cmd ||
				    (!is_command_pending(adapter)))
					break;
			}
		}

		/*                    */
		if (adapter->cmd_resp_received) {
			adapter->cmd_resp_received = false;
			mwifiex_process_cmdresp(adapter);

			/*                                        */
			if (adapter->hw_status == MWIFIEX_HW_STATUS_INIT_DONE) {
				adapter->hw_status = MWIFIEX_HW_STATUS_READY;
				mwifiex_init_fw_complete(adapter);
			}
		}

		/*                 */
		if (adapter->event_received) {
			adapter->event_received = false;
			mwifiex_process_event(adapter);
		}

		/*                                          
                         */
		if (adapter->ps_state == PS_STATE_PRE_SLEEP) {
			if (!adapter->cmd_sent && !adapter->curr_cmd)
				mwifiex_check_ps_cond(adapter);
		}

		/*                                                          
                         
   */
		if ((adapter->ps_state == PS_STATE_SLEEP) ||
		    (adapter->ps_state == PS_STATE_PRE_SLEEP) ||
		    (adapter->ps_state == PS_STATE_SLEEP_CFM) ||
		    adapter->tx_lock_flag)
			continue;

		if (!adapter->cmd_sent && !adapter->curr_cmd) {
			if (mwifiex_exec_next_cmd(adapter) == -1) {
				ret = -1;
				break;
			}
		}

		if (!adapter->scan_processing && !adapter->data_sent &&
		    !mwifiex_wmm_lists_empty(adapter)) {
			mwifiex_wmm_process_tx(adapter);
			if (adapter->hs_activated) {
				adapter->is_hs_configured = false;
				mwifiex_hs_activated_event
					(mwifiex_get_priv
					 (adapter, MWIFIEX_BSS_ROLE_ANY),
					 false);
			}
		}

		if (adapter->delay_null_pkt && !adapter->cmd_sent &&
		    !adapter->curr_cmd && !is_command_pending(adapter) &&
		    mwifiex_wmm_lists_empty(adapter)) {
			if (!mwifiex_send_null_packet
			    (mwifiex_get_priv(adapter, MWIFIEX_BSS_ROLE_STA),
			     MWIFIEX_TxPD_POWER_MGMT_NULL_PACKET |
			     MWIFIEX_TxPD_POWER_MGMT_LAST_PACKET)) {
				adapter->delay_null_pkt = false;
				adapter->ps_state = PS_STATE_SLEEP;
			}
			break;
		}
	} while (true);

	if ((adapter->int_status) || IS_CARD_RX_RCVD(adapter))
		goto process_start;

	spin_lock_irqsave(&adapter->main_proc_lock, flags);
	adapter->mwifiex_processing = false;
	spin_unlock_irqrestore(&adapter->main_proc_lock, flags);

exit_main_proc:
	if (adapter->hw_status == MWIFIEX_HW_STATUS_CLOSING)
		mwifiex_shutdown_drv(adapter);
	return ret;
}

/*
                                             
  
                                                                 
                          
 */
static void mwifiex_free_adapter(struct mwifiex_adapter *adapter)
{
	if (!adapter) {
		pr_err("%s: adapter is NULL\n", __func__);
		return;
	}

	mwifiex_unregister(adapter);
	pr_debug("info: %s: free adapter\n", __func__);
}

/*
                                                       
  
                                               
                                               
                                                       
                                           
                                             
 */
static int mwifiex_init_hw_fw(struct mwifiex_adapter *adapter)
{
	int ret, err;
	struct mwifiex_fw_image fw;

	memset(&fw, 0, sizeof(struct mwifiex_fw_image));

	err = request_firmware(&adapter->firmware, adapter->fw_name,
			       adapter->dev);
	if (err < 0) {
		dev_err(adapter->dev, "request_firmware() returned"
				" error code %#x\n", err);
		ret = -1;
		goto done;
	}
	fw.fw_buf = (u8 *) adapter->firmware->data;
	fw.fw_len = adapter->firmware->size;

	ret = mwifiex_dnld_fw(adapter, &fw);
	if (ret == -1)
		goto done;

	dev_notice(adapter->dev, "WLAN FW is active\n");

	adapter->init_wait_q_woken = false;
	ret = mwifiex_init_fw(adapter);
	if (ret == -1) {
		goto done;
	} else if (!ret) {
		adapter->hw_status = MWIFIEX_HW_STATUS_READY;
		goto done;
	}
	/*                                   */
	wait_event_interruptible(adapter->init_wait_q,
				 adapter->init_wait_q_woken);
	if (adapter->hw_status != MWIFIEX_HW_STATUS_READY) {
		ret = -1;
		goto done;
	}
	ret = 0;

done:
	if (adapter->firmware)
		release_firmware(adapter->firmware);
	if (ret)
		ret = -1;
	return ret;
}

/*
                                       
  
                                                                      
                                                                    
                          
 */
static void
mwifiex_fill_buffer(struct sk_buff *skb)
{
	struct ethhdr *eth;
	struct iphdr *iph;
	struct timeval tv;
	u8 tid = 0;

	eth = (struct ethhdr *) skb->data;
	switch (eth->h_proto) {
	case __constant_htons(ETH_P_IP):
		iph = ip_hdr(skb);
		tid = IPTOS_PREC(iph->tos);
		pr_debug("data: packet type ETH_P_IP: %04x, tid=%#x prio=%#x\n",
			 eth->h_proto, tid, skb->priority);
		break;
	case __constant_htons(ETH_P_ARP):
		pr_debug("data: ARP packet: %04x\n", eth->h_proto);
	default:
		break;
	}
/*                                       */
#define IPTOS_OFFSET 5
	tid = (tid >> IPTOS_OFFSET);
	skb->priority = tid;
	/*                                                       
                                                         
                                                  
                                                       
                                                          
                         
  */
	do_gettimeofday(&tv);
	skb->tstamp = timeval_to_ktime(tv);
}

/*
                                             
  
                         
 */
static int
mwifiex_open(struct net_device *dev)
{
	netif_tx_start_all_queues(dev);
	return 0;
}

/*
                                              
 */
static int
mwifiex_close(struct net_device *dev)
{
	return 0;
}

/*
                                                          
 */
static int
mwifiex_hard_start_xmit(struct sk_buff *skb, struct net_device *dev)
{
	struct mwifiex_private *priv = mwifiex_netdev_get_priv(dev);
	struct sk_buff *new_skb;
	struct mwifiex_txinfo *tx_info;

	dev_dbg(priv->adapter->dev, "data: %lu BSS(%d-%d): Data <= kernel\n",
		jiffies, priv->bss_type, priv->bss_num);

	if (priv->adapter->surprise_removed) {
		kfree_skb(skb);
		priv->stats.tx_dropped++;
		return 0;
	}
	if (!skb->len || (skb->len > ETH_FRAME_LEN)) {
		dev_err(priv->adapter->dev, "Tx: bad skb len %d\n", skb->len);
		kfree_skb(skb);
		priv->stats.tx_dropped++;
		return 0;
	}
	if (skb_headroom(skb) < MWIFIEX_MIN_DATA_HEADER_LEN) {
		dev_dbg(priv->adapter->dev,
			"data: Tx: insufficient skb headroom %d\n",
			skb_headroom(skb));
		/*                                                */
		new_skb =
			skb_realloc_headroom(skb, MWIFIEX_MIN_DATA_HEADER_LEN);
		if (unlikely(!new_skb)) {
			dev_err(priv->adapter->dev, "Tx: cannot alloca new_skb\n");
			kfree_skb(skb);
			priv->stats.tx_dropped++;
			return 0;
		}
		kfree_skb(skb);
		skb = new_skb;
		dev_dbg(priv->adapter->dev, "info: new skb headroomd %d\n",
			skb_headroom(skb));
	}

	tx_info = MWIFIEX_SKB_TXCB(skb);
	tx_info->bss_num = priv->bss_num;
	tx_info->bss_type = priv->bss_type;
	mwifiex_fill_buffer(skb);

	mwifiex_wmm_add_buf_txqueue(priv, skb);
	atomic_inc(&priv->adapter->tx_pending);

	if (atomic_read(&priv->adapter->tx_pending) >= MAX_TX_PENDING) {
		mwifiex_set_trans_start(dev);
		mwifiex_stop_net_dev_queue(priv->netdev, priv->adapter);
	}

	queue_work(priv->adapter->workqueue, &priv->adapter->main_work);

	return 0;
}

/*
                                                            
 */
static int
mwifiex_set_mac_address(struct net_device *dev, void *addr)
{
	struct mwifiex_private *priv = mwifiex_netdev_get_priv(dev);
	struct sockaddr *hw_addr = addr;
	int ret;

	memcpy(priv->curr_addr, hw_addr->sa_data, ETH_ALEN);

	/*                          */
	ret = mwifiex_send_cmd_sync(priv, HostCmd_CMD_802_11_MAC_ADDRESS,
				    HostCmd_ACT_GEN_SET, 0, NULL);

	if (!ret)
		memcpy(priv->netdev->dev_addr, priv->curr_addr, ETH_ALEN);
	else
		dev_err(priv->adapter->dev,
			"set mac address failed: ret=%d\n", ret);

	memcpy(dev->dev_addr, priv->curr_addr, ETH_ALEN);

	return ret;
}

/*
                                                               
 */
static void mwifiex_set_multicast_list(struct net_device *dev)
{
	struct mwifiex_private *priv = mwifiex_netdev_get_priv(dev);
	struct mwifiex_multicast_list mcast_list;

	if (dev->flags & IFF_PROMISC) {
		mcast_list.mode = MWIFIEX_PROMISC_MODE;
	} else if (dev->flags & IFF_ALLMULTI ||
		   netdev_mc_count(dev) > MWIFIEX_MAX_MULTICAST_LIST_SIZE) {
		mcast_list.mode = MWIFIEX_ALL_MULTI_MODE;
	} else {
		mcast_list.mode = MWIFIEX_MULTICAST_MODE;
		if (netdev_mc_count(dev))
			mcast_list.num_multicast_addr =
				mwifiex_copy_mcast_addr(&mcast_list, dev);
	}
	mwifiex_request_set_multicast_list(priv, &mcast_list);
}

/*
                                                             
 */
static void
mwifiex_tx_timeout(struct net_device *dev)
{
	struct mwifiex_private *priv = mwifiex_netdev_get_priv(dev);

	dev_err(priv->adapter->dev, "%lu : Tx timeout, bss_type-num = %d-%d\n",
		jiffies, priv->bss_type, priv->bss_num);
	mwifiex_set_trans_start(dev);
	priv->num_tx_timeout++;
}

/*
                                                             
 */
static struct net_device_stats *mwifiex_get_stats(struct net_device *dev)
{
	struct mwifiex_private *priv = mwifiex_netdev_get_priv(dev);

	return &priv->stats;
}

/*                         */
static const struct net_device_ops mwifiex_netdev_ops = {
	.ndo_open = mwifiex_open,
	.ndo_stop = mwifiex_close,
	.ndo_start_xmit = mwifiex_hard_start_xmit,
	.ndo_set_mac_address = mwifiex_set_mac_address,
	.ndo_tx_timeout = mwifiex_tx_timeout,
	.ndo_get_stats = mwifiex_get_stats,
	.ndo_set_rx_mode = mwifiex_set_multicast_list,
};

/*
                                                              
  
                                              
                          
                            
                               
  
                                                    
                                          
                                             
                                                     
                                              
                                             
                                          
                                                        
  
                                                        
 */
void mwifiex_init_priv_params(struct mwifiex_private *priv,
						struct net_device *dev)
{
	dev->netdev_ops = &mwifiex_netdev_ops;
	/*                              */
	priv->current_key_index = 0;
	priv->media_connected = false;
	memset(&priv->nick_name, 0, sizeof(priv->nick_name));
	priv->num_tx_timeout = 0;
	memcpy(dev->dev_addr, priv->curr_addr, ETH_ALEN);
}

/*
                                             
 */
int is_command_pending(struct mwifiex_adapter *adapter)
{
	unsigned long flags;
	int is_cmd_pend_q_empty;

	spin_lock_irqsave(&adapter->cmd_pending_q_lock, flags);
	is_cmd_pend_q_empty = list_empty(&adapter->cmd_pending_q);
	spin_unlock_irqrestore(&adapter->cmd_pending_q_lock, flags);

	return !is_cmd_pend_q_empty;
}

/*
                                        
  
                                                                  
                     
 */
static void mwifiex_main_work_queue(struct work_struct *work)
{
	struct mwifiex_adapter *adapter =
		container_of(work, struct mwifiex_adapter, main_work);

	if (adapter->surprise_removed)
		return;
	mwifiex_main_process(adapter);
}

/*
                                                            
                      
 */
static void
mwifiex_terminate_workqueue(struct mwifiex_adapter *adapter)
{
	flush_workqueue(adapter->workqueue);
	destroy_workqueue(adapter->workqueue);
	adapter->workqueue = NULL;
}

/*
                               
  
                                                                         
                                                                          
                                                                         
                           
                                   
                                              
                             
                                          
                                
 */
int
mwifiex_add_card(void *card, struct semaphore *sem,
		 struct mwifiex_if_ops *if_ops, u8 iface_type)
{
	struct mwifiex_adapter *adapter;
	char fmt[64];
	struct mwifiex_private *priv;

	if (down_interruptible(sem))
		goto exit_sem_err;

	if (mwifiex_register(card, if_ops, (void **)&adapter)) {
		pr_err("%s: software init failed\n", __func__);
		goto err_init_sw;
	}

	adapter->iface_type = iface_type;

	adapter->hw_status = MWIFIEX_HW_STATUS_INITIALIZING;
	adapter->surprise_removed = false;
	init_waitqueue_head(&adapter->init_wait_q);
	adapter->is_suspended = false;
	adapter->hs_activated = false;
	init_waitqueue_head(&adapter->hs_activate_wait_q);
	adapter->cmd_wait_q_required = false;
	init_waitqueue_head(&adapter->cmd_wait_q.wait);
	adapter->cmd_wait_q.status = 0;
	adapter->scan_wait_q_woken = false;

	adapter->workqueue = create_workqueue("MWIFIEX_WORK_QUEUE");
	if (!adapter->workqueue)
		goto err_kmalloc;

	INIT_WORK(&adapter->main_work, mwifiex_main_work_queue);

	/*                                                                      
                                                                */
	if (adapter->if_ops.register_dev(adapter)) {
		pr_err("%s: failed to register mwifiex device\n", __func__);
		goto err_registerdev;
	}

	if (mwifiex_init_hw_fw(adapter)) {
		pr_err("%s: firmware init failed\n", __func__);
		goto err_init_fw;
	}

	priv = adapter->priv[0];

	if (mwifiex_register_cfg80211(priv) != 0) {
		dev_err(adapter->dev, "cannot register netdevice"
			       " with cfg80211\n");
			goto err_init_fw;
	}

	rtnl_lock();
	/*                                     */
	if (!mwifiex_add_virtual_intf(priv->wdev->wiphy, "mlan%d",
				      NL80211_IFTYPE_STATION, NULL, NULL)) {
		rtnl_unlock();
		dev_err(adapter->dev, "cannot create default station"
				" interface\n");
		goto err_add_intf;
	}

	rtnl_unlock();

	up(sem);

	mwifiex_drv_get_driver_version(adapter, fmt, sizeof(fmt) - 1);
	dev_notice(adapter->dev, "driver_version = %s\n", fmt);

	return 0;

err_add_intf:
	rtnl_lock();
	mwifiex_del_virtual_intf(priv->wdev->wiphy, priv->netdev);
	rtnl_unlock();
err_init_fw:
	pr_debug("info: %s: unregister device\n", __func__);
	adapter->if_ops.unregister_dev(adapter);
err_registerdev:
	adapter->surprise_removed = true;
	mwifiex_terminate_workqueue(adapter);
err_kmalloc:
	if ((adapter->hw_status == MWIFIEX_HW_STATUS_FW_READY) ||
	    (adapter->hw_status == MWIFIEX_HW_STATUS_READY)) {
		pr_debug("info: %s: shutdown mwifiex\n", __func__);
		adapter->init_wait_q_woken = false;

		if (mwifiex_shutdown_drv(adapter) == -EINPROGRESS)
			wait_event_interruptible(adapter->init_wait_q,
						 adapter->init_wait_q_woken);
	}

	mwifiex_free_adapter(adapter);

err_init_sw:
	up(sem);

exit_sem_err:
	return -1;
}
EXPORT_SYMBOL_GPL(mwifiex_add_card);

/*
                                  
  
                                                                         
                           
                           
                                       
                                  
                               
                                    
 */
int mwifiex_remove_card(struct mwifiex_adapter *adapter, struct semaphore *sem)
{
	struct mwifiex_private *priv = NULL;
	int i;

	if (down_interruptible(sem))
		goto exit_sem_err;

	if (!adapter)
		goto exit_remove;

	adapter->surprise_removed = true;

	/*           */
	for (i = 0; i < adapter->priv_num; i++) {
		priv = adapter->priv[i];
		if (priv && priv->netdev) {
			if (!netif_queue_stopped(priv->netdev))
				mwifiex_stop_net_dev_queue(priv->netdev,
							   adapter);
			if (netif_carrier_ok(priv->netdev))
				netif_carrier_off(priv->netdev);
		}
	}

	dev_dbg(adapter->dev, "cmd: calling mwifiex_shutdown_drv...\n");
	adapter->init_wait_q_woken = false;

	if (mwifiex_shutdown_drv(adapter) == -EINPROGRESS)
		wait_event_interruptible(adapter->init_wait_q,
					 adapter->init_wait_q_woken);
	dev_dbg(adapter->dev, "cmd: mwifiex_shutdown_drv done\n");
	if (atomic_read(&adapter->rx_pending) ||
	    atomic_read(&adapter->tx_pending) ||
	    atomic_read(&adapter->cmd_pending)) {
		dev_err(adapter->dev, "rx_pending=%d, tx_pending=%d, "
		       "cmd_pending=%d\n",
		       atomic_read(&adapter->rx_pending),
		       atomic_read(&adapter->tx_pending),
		       atomic_read(&adapter->cmd_pending));
	}

	for (i = 0; i < adapter->priv_num; i++) {
		priv = adapter->priv[i];

		if (!priv)
			continue;

		rtnl_lock();
		if (priv->wdev && priv->netdev)
			mwifiex_del_virtual_intf(priv->wdev->wiphy,
						 priv->netdev);
		rtnl_unlock();
	}

	priv = adapter->priv[0];
	if (!priv)
		goto exit_remove;

	if (priv->wdev) {
		wiphy_unregister(priv->wdev->wiphy);
		wiphy_free(priv->wdev->wiphy);
		kfree(priv->wdev);
	}

	mwifiex_terminate_workqueue(adapter);

	/*                   */
	dev_dbg(adapter->dev, "info: unregister device\n");
	adapter->if_ops.unregister_dev(adapter);
	/*                        */
	dev_dbg(adapter->dev, "info: free adapter\n");
	mwifiex_free_adapter(adapter);

exit_remove:
	up(sem);
exit_sem_err:
	return 0;
}
EXPORT_SYMBOL_GPL(mwifiex_remove_card);

/*
                                        
  
                                                  
 */
static int
mwifiex_init_module(void)
{
#ifdef CONFIG_DEBUG_FS
	mwifiex_debugfs_init();
#endif
	return 0;
}

/*
                                      
  
                                        
 */
static void
mwifiex_cleanup_module(void)
{
#ifdef CONFIG_DEBUG_FS
	mwifiex_debugfs_remove();
#endif
}

module_init(mwifiex_init_module);
module_exit(mwifiex_cleanup_module);

MODULE_AUTHOR("Marvell International Ltd.");
MODULE_DESCRIPTION("Marvell WiFi-Ex Driver version " VERSION);
MODULE_VERSION(VERSION);
MODULE_LICENSE("GPL v2");