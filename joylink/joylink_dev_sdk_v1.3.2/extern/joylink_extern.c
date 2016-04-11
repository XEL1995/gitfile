/* --------------------------------------------------
 * @brief: 
 *
 * @version: 1.0
 *
 * @date: 10/08/2015 09:28:27 AM
 *
 * --------------------------------------------------
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "joylink.h"
#include "joylink_packets.h"
#include "joylink_extern.h"
#include "joylink_json.h"
#include "joylink_extern_json.h"
#include "joylink_ret_code.h"

typedef struct __attr_{
    char name[128];
    E_JL_DEV_ATTR_GET_CB get;
    E_JL_DEV_ATTR_SET_CB set;
}Attr_t;

typedef struct _attr_manage_{
    Attr_t wlan24g;
    Attr_t subdev;
    Attr_t wlanspeed;
    Attr_t uuid;
    Attr_t feedid;
    Attr_t accesskey;
    Attr_t localkey;
    Attr_t server_st;
	Attr_t macaddr;	
	Attr_t server_info;	
	Attr_t version;
/*
 *ssid and pass_word is option for GW.
 */
	Attr_t ssid;	
	Attr_t pass_word;	

}WIFIManage_t;

WIFIManage_t _g_am, *_g_pam = &_g_am;

extern void
joylink_dev_set_ver(short ver);

extern short
joylink_dev_get_ver();


int DevFeedidSet(const char* json_in)  //add by wangshi
{
	mdev_t * dev;  //flash设备

	flash_drv_init();

	dev = flash_drv_open(0);
	if (dev == NULL)
	{
		wmprintf("Flash driver init is required before open");
		return -1;
	}

	if (flash_drv_erase(dev, FEEDID_ADDR, PAGESIZE) < 0) {
		flash_drv_close(dev);
		wmprintf("Failed to erase partition\r\n");
		return -1;
	}
	if (flash_drv_write(dev, json_in, strlen(json_in), FEEDID_ADDR) != 0)
	{
		wmprintf("Failed to write data\r\n");
		return -1;
	}
	wmprintf("The write address is : %08x\r\n", FEEDID_ADDR);
	return 0;
}

int DevAccesskeyGet(char* buf,unsigned int size)  //add by wangshi
{
	mdev_t * dev;
	flash_drv_init();
	dev = flash_drv_open(0);
	if (dev == NULL) {
		wmprintf("Flash driver init is required before open");
		return -WM_FAIL;
	}

	int rv;
	rv = flash_drv_read(dev, buf, size, ACKEY_ADDR);
	if (rv != WM_SUCCESS) {
		wmprintf("Unable to read from flash: %x\r\n", ACKEY_ADDR);
		return rv;
	}
	wmprintf("read from flash: %x\r\n", ACKEY_ADDR);
}

typedef struct{
	char* buff;
	int len;
	int offset;
}jl_json;

void joylink_json_init(jl_json *jptr, char *buff, int len)
{
	jptr->buff = buff;
	memset(jptr->buff, 0, len);
	jptr->offset = 0;
	jptr->len = len;

}

int joylink_json_start(jl_json *jptr)
{
	if(jptr->offset >= jptr->len -1)
		return -1;
	if(jptr->offset){
		char *buf = str_rev_skip(&(jptr->buff[jptr->offset - 1]));
	    if(*buf == '}')
	    	jptr->buff[jptr->offset++] = ',';
	}
	jptr->buff[jptr->offset++] = '{';
	return 0;
}
int joylink_json_close(jl_json *jptr)
{
	if(jptr->offset >= jptr->len - 1)
		return -1;
	jptr->buff[jptr->offset++] = '}';
	return 0;
}

char *str_rev_skip(char *in)
{
	while(in && (unsigned char)*in < 32)
		in--;
	return in;
}

int JySetJson(jl_json *jptr, char *name, char *str,int value ,int data)
{
	char *buf;

	if(jptr->offset >= jptr->len - 1)
		return -1;

	buf = str_rev_skip(&(jptr->buff[jptr->offset - 1]));
	if(*buf != '{')
		jptr->buff[jptr->offset++] = ',';

	switch(data)
	{
	case 1:  //string
		snprintf(&jptr->buff[jptr->offset],
			 jptr->len - jptr->offset, "\"%s\":\"%s\"",
			 name, str);
	case 2:  //number
		snprintf(&jptr->buff[jptr->offset],
			 jptr->len - jptr->offset, "\"%s\":%d",
			 name, value);
	default:
		printf("Invalid case\r\n");
	}

	jptr->offset = strlen(jptr->buff);
}

/**
 * brief:
 * Check dev net is ok.
 * @Param: st
 *
 * @Returns: 
 *  E_RET_TRUE
 *  E_RET_FAIL
 */
E_JLRetCode_t
joylink_dev_is_net_ok()
{
    /**
     *FIXME:must to do
     */
	char *p;
	char *ip;
	short *port;

    int ret = E_RET_TRUE;

    char *p = NULL;
    struct in_addr addr;
    char str_tmp[256] = { 0 };

    char dns_buff[1024];
    struct hostent hostinfo, *phost;
    struct sockaddr_in addr4;
    int rc;
    unsigned int ip4;

    if (_g_pdev->jlp.joylink_server  == NULL)
        return -1;

    memcpy(str_tmp, _g_pdev->jlp.joylink_server, sizeof(_g_pdev->jlp.ip));

    bzero(&addr4, sizeof(struct sockaddr_in));
    if (1 == inet_pton(AF_INET, str_tmp, &(addr4.sin_addr)))
        return (addr4.sin_addr.s_addr);

    //返回对应于主机名的主机信息
    if (0 == gethostbyname_r(str_tmp/*点分10进制指针*/, &hostinfo/*主机信息结构体指针*/,
    		                 dns_buff/**/, 8192, &phost, &rc))
    {
    	addr.s_addr = *(unsigned int *) (hostinfo.h_addr);
    }
    else
    {
    	addr.s_addr = 0;
    	return -WM_FAIL;
    }

    char *ipstr = inet_ntoa(addr);
    strcpy(ip, ipstr);
    CH_P("ip=%s,port=%d\r\n", ip, *port);

    return WM_SUCCESS;
}


/**
 * brief:
 * When connecting server st changed,
 * this fun will be called.
 *
 * @Param: st
 * JL_SERVER_ST_INIT      (0)
 * JL_SERVER_ST_AUTH      (1)
 * JL_SERVER_ST_WORK      (2)
 *
 * @Returns: 
 *  E_RET_OK
 *  E_RET_ERROR
 */
E_JLRetCode_t
joylink_dev_set_connect_st(int st)
{
    /**
     *FIXME:must to do
     */
    int ret = E_RET_OK;
    return ret;
}

/**
 * brief:
 * Save joylink protocol info in flash.
 *
 * @Param:jlp 
 *
 * @Returns: 
 *  E_RET_OK
 *  E_RET_ERROR
 */
E_JLRetCode_t
joylink_dev_set_attr_jlp(JLPInfo_t *jlp)
{
    if(NULL == jlp){
        return E_RET_ERROR;
    }
    /**
     *FIXME:must to do
     */

    int ret = E_RET_ERROR;
    char buff[256];

    bzero(buff, sizeof(buff));
    if(strlen(jlp->feedid)){

    	joylink_dev_register_attr_cb("feedid",1,DevFeedidSet,DevFeedidGet);//add by wangshi

        sprintf(buff, "{\"feedid\":\"%s\"}", jlp->feedid);
        log_debug("--set buff:%s", buff);
        if(_g_pam->feedid.set){
            if(_g_pam->feedid.set(buff)){
                log_error("set error");
                goto RET;
            }
        }
    }

    bzero(buff, sizeof(buff));
    if(strlen(jlp->accesskey)){
        sprintf(buff, "{\"accesskey\":\"%s\"}", jlp->accesskey);
        log_debug("--set buff:%s", buff);
        if(_g_pam->accesskey.set){
            if(_g_pam->accesskey.set(buff)){
                log_error("set error");
                goto RET;
            }
        }
    }

    bzero(buff, sizeof(buff));
    if(strlen(jlp->localkey)){
        sprintf(buff, "{\"localkey\":\"%s\"}", jlp->localkey);
        log_debug("--set buff:%s", buff);
        if(_g_pam->localkey.set){
            if(_g_pam->localkey.set(buff)){
                log_error("set error");
                goto RET;
            }
        }
    }

    bzero(buff, sizeof(buff));
    if(strlen(jlp->joylink_server)){
        sprintf(buff, "%s:%d", jlp->joylink_server, jlp->server_port);
        log_debug("--set buff:%s", buff);
        if(_g_pam->server_info.set){
            if(_g_pam->server_info.set(buff)){
                log_error("set error");
                goto RET;
            }
        }
    }

    bzero(buff, sizeof(buff));
    sprintf(buff, "{\"version\":%d}", jlp->version);
    log_debug("--set buff:%s", buff);
    if(_g_pam->version.set){
        if(_g_pam->version.set(buff)){
            log_error("set error");
            goto RET;
        }
    }

    ret = E_RET_OK;
RET:

    return ret;

}

/**
 * brief:
 * get joylink protocol info.
 *
 * @Param:jlp 
 *
 * @Returns: 
 *  E_RET_OK
 *  E_RET_ERROR
 */
E_JLRetCode_t
joylink_dev_get_jlp_info(JLPInfo_t *jlp)
{

    if(NULL == jlp){
        return -1;
    }
    /**
     *FIXME:must to do
     */
    int ret = -1;
    char buff[256];

    if(_g_pam->accesskey.get){

    	joylink_dev_register_attr_cb("accesskey",1,DevAccesskeySet,DevAccesskeyGet); //add by wangshi

        bzero(buff, sizeof(buff));
        if(!_g_pam->accesskey.get(buff, sizeof(buff))){
            log_debug("-->accesskey:%s\n", buff);
            jl_parse_jlp(jlp, buff);
        }else{
            log_error("get accesskey error");
            goto RET;
        }
    }

    if(_g_pam->localkey.get){
        bzero(buff, sizeof(buff));
        if(!_g_pam->localkey.get(buff, sizeof(buff))){
            log_debug("-->localkey:%s\n", buff);
            jl_parse_jlp(jlp, buff);
        }else{
            log_error("get localkey error");
            goto RET;
        }
    }

    if(_g_pam->feedid.get){
        bzero(buff, sizeof(buff));
        if(!_g_pam->feedid.get(buff, sizeof(buff))){
            log_debug("-->feedid:%s\n", buff);
            jl_parse_jlp(jlp, buff);
        }else{
            log_error("get feedid error");
            goto RET;
        }
    }

    if(_g_pam->uuid.get){
        bzero(buff, sizeof(buff));
        if(!_g_pam->uuid.get(buff, sizeof(buff))){
            log_debug("-->uuid:%s\n", buff);
            jl_parse_jlp(jlp, buff);
        }else{
            log_error("get uuid error");
            goto RET;
        }
    }

    if(_g_pam->macaddr.get){
        bzero(buff, sizeof(buff));
        if(!_g_pam->macaddr.get(buff, sizeof(buff))){
            log_info("-->mac:%s\n", buff);
            jl_parse_jlp(jlp, buff);
        }else{
            log_error("get mac error");
            goto RET;
        }
    }

    if(_g_pam->server_info.get){
        bzero(buff, sizeof(buff));
        if(!_g_pam->server_info.get(buff, sizeof(buff))){
            log_info("-->server_info:%s\n", buff);
            joylink_util_cut_ip_port(buff, jlp->joylink_server, &jlp->server_port);
        }else{
            log_error("get mac error");
            goto RET;
        }
    }

    if(_g_pam->version.get){
        bzero(buff, sizeof(buff));
        if(!_g_pam->version.get(buff, sizeof(buff))){
            log_info("-->version:%s\n", buff);
            jl_parse_jlp(jlp, buff);
        }else{
            log_error("get mac error");
            goto RET;
        }
    }
    ret = E_RET_OK;

RET:
    return ret;
}

/**
 * brief:
 * Save dev attr info to flash.
 *
 * @Param:wi 
 * "wi" is only a example, replace with dev attr.
 *
 * @Returns: 
 *  E_RET_OK
 *  E_RET_ERROR
 */
E_JLRetCode_t
joylink_dev_set_attr(WIFICtrl_t *wi)
{
    if(NULL == wi){
        return -1;
    }
    /**
     *FIXME:must to do
     */
    int ret = E_RET_ERROR;

    return ret;
}

/**
 * brief:
 * Get dev snap shot.
 *
 * @Param:out_snap
 *
 * @Returns: snap shot len.
 */
int
joylink_dev_get_snap_shot(char *out_snap, int32_t out_max)
{
    if(NULL == out_snap || out_max < 0){
        return 0;
    }
    /**
     *FIXME:must to do
     */
    int len = 0;

    return len;
}

/**
 * brief:
 * Get dev snap shot.
 *
 * @Param:out_snap
 *
 * @Returns: snap shot len.
 */
int
joylink_dev_get_json_snap_shot(char *out_snap, int32_t out_max, int code, char *feedid)
{
    /**
     *FIXME:must to do
     */
	jl_json DevJson;
	joylink_json_init(&DevJson,out_snap,out_max);
	joylink_json_start(&DevJson);
	JySetJson(&DevJson,"ac0x0001",NULL,DeviceState.power,2);

	joylink_json_close();
	/*************************/

	sprintf(out_snap, "{\"code\":%d, \"feedid\":\"%s\"}", code, feedid);

    return strlen(out_snap);
}

typedef struct {
	char type[20];
	char  ordervalue[200];
	int 	ordertype;
	int 	ordercode;
	char  from[12];
	char  msgtype[20];
	char  target[33];

}Xmpp_Process_Data;

/**
 * brief:
 * json ctrl.
 *
 * @Param:json_cmd
 *
 * @Returns: 
 *  E_RET_OK
 *  E_RET_ERROR
 */
E_JLRetCode_t 
joylink_dev_lan_json_ctrl(const char *json_cmd)
{
    /**
     *FIXME:must to do
     */

    char tmpJson[1024];
    memset(tmpJson,0,1024);
    memcpy(tmpJson,json_cmd,1024);

	Xmpp_Process_Data recmsg;
	strstr(tmpJson,"magtype");
	strstr(tmpJson,)
    strcmp(recmsg.msgtype,"getGroupSwitch");

    strcmp(recmsg.msgtype,"getMulityModes");

    strcmp(recmsg.msgtype,"instantstatus");

    strcmp(recmsg.msgtype,"order");

    log_debug("json ctrl:%s", json_cmd);

    return E_RET_OK;
}

/**
 * brief:
 * script control.
 * @Param: 
 *
 * @Returns: 
 *  E_RET_OK
 *  E_RET_ERROR
 */
E_JLRetCode_t 
joylink_dev_script_ctrl(const char *cmd, JLContrl_t *ctr, int from_server)
{
    if(NULL == cmd|| NULL == ctr){
        return -1;
    }
    /**
     *FIXME:must to do
     */
    int ret = E_RET_ERROR;
    int offset = 0;
    int time_tmp;
    memcpy(&time_tmp, cmd + offset, 4);
    offset +=4;
    memcpy(&ctr->biz_code, cmd + offset, 4);
    offset +=4;
    memcpy(&ctr->serial, cmd + offset, 4);
    offset +=4;

    if(ctr->biz_code == JL_BZCODE_GET_SNAPSHOT){
        /*
         *Nothing to do!
         */
        ret = 0;
    }else if(ctr->biz_code == JL_BZCODE_CTRL){
        /**
         *Must to do!
         */
        log_debug("script ctrl:%s", cmd+offset);
    }
    
    return ret;
}

/**
 * brief:
 * dev ota update
 * @Param: JLOtaOrder_t *otaOrder
 *
 * @Returns: 
 *  E_RET_OK
 *  E_RET_ERROR
 */
E_JLRetCode_t
joylink_dev_ota(JLOtaOrder_t *otaOrder)
{
    if(NULL == otaOrder){
        return -1;
    }
    /**
     *FIXME:must to do
     */
    int ret = E_RET_OK;
    log_debug("serial:%d | feedid:%s | productuuid:%s | version:%d | versionname:%s | crc32:%d | url:%s\n",
     otaOrder->serial, otaOrder->feedid, otaOrder->productuuid, otaOrder->version, 
     otaOrder->versionname, otaOrder->crc32, otaOrder->url);

    return ret;
}

/**
 * brief:
 * dev ota status upload
 * @Param: 
 *
 * @Returns: 
 *  E_RET_OK
 *  E_RET_ERROR
 */
void
joylink_dev_ota_status_upload()
{
    JLOtaUpload_t otaUpload;
    strcpy(otaUpload.feedid, _g_pdev->jlp.feedid);
    strcpy(otaUpload.productuuid, _g_pdev->jlp.uuid);

    /**
     *FIXME:must to do
     *status,status_desc, progress
     */
    joylink_server_ota_status_upload_req(&otaUpload);
}

/**
 * brief:1 start a softap with ap_ssid 
 *       2 option to do it.
 *
 * @Returns: 
 *  E_RET_OK
 *  E_RET_ERROR
 */
E_JLRetCode_t
joylink_dev_start_softap(char *ap_ssid)
{
    if(NULL == ap_ssid){
        return E_RET_ERROR;
    }
    int ret = E_RET_OK;
    return ret;
}

/**
 * brief:1 stop softap
 *       2 option to do it.
 *
 * @Returns: 
 *  E_RET_OK
 *  E_RET_ERROR
 */
E_JLRetCode_t
joylink_dev_stop_softap()
{
    int ret = E_RET_OK;
    return ret;
}

/**
 * brief:
 * attr set and get callback register.
 * @Param:
 *
 * @Returns: 
 *  E_RET_OK
 *  E_RET_ERROR
 */
int 
joylink_dev_register_attr_cb(
        const char *name,
        E_JL_DEV_ATTR_TYPE type,
        E_JL_DEV_ATTR_GET_CB attr_get_cb,
        E_JL_DEV_ATTR_SET_CB attr_set_cb)
{
    if(NULL == name){
        return -1;
    }
    int ret = -1;
    log_debug("regster %s", name);
    if(!strcmp(name, JL_ATTR_WLAN24G)){
       _g_pam->wlan24g.get = attr_get_cb; 
       _g_pam->wlan24g.set = attr_set_cb; 
       ret = 0;
    }else if(!strcmp(name, JL_ATTR_SUBDEVS)){
       _g_pam->subdev.get = attr_get_cb; 
       _g_pam->subdev.set = attr_set_cb; 
       ret = 0;
    }else if(!strcmp(name, JL_ATTR_WAN_SPEED)){
       _g_pam->wlanspeed.get = attr_get_cb; 
       _g_pam->wlanspeed.set = attr_set_cb; 
       ret = 0;
    }else if(!strcmp(name, JL_ATTR_UUID)){
       _g_pam->uuid.get = attr_get_cb; 
       _g_pam->uuid.set = attr_set_cb; 
       ret = 0;
    }else if(!strcmp(name, JL_ATTR_FEEDID)){
       _g_pam->feedid.get = attr_get_cb; 
       _g_pam->feedid.set = attr_set_cb; 
       ret = 0;
    }else if(!strcmp(name, JL_ATTR_ACCESSKEY)){
       _g_pam->accesskey.get = attr_get_cb; 
       _g_pam->accesskey.set = attr_set_cb; 
       ret = 0;
    }else if(!strcmp(name, JL_ATTR_LOCALKEY)){
       _g_pam->localkey.get = attr_get_cb; 
       _g_pam->localkey.set = attr_set_cb; 
       ret = 0;
    }else if(!strcmp(name, JL_ATTR_CONN_STATUS)){
       _g_pam->server_st.set = attr_set_cb; 
       ret = 0;
    }else if(!strcmp(name, JL_ATTR_MACADDR)){
       _g_pam->macaddr.get = attr_get_cb; 
       ret = 0;
    }

    log_info("regster %s:ret:%d", name, ret);
    return ret;
}
