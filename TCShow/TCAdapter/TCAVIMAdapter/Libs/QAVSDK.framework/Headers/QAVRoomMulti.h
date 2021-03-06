﻿#import "QAVEndpoint.h"
#import "QAVSDK/QAVCommon.h"
#import <Foundation/Foundation.h>

@protocol QAVRoomDelegate;
@protocol QAVChangeDelegate;
@protocol QAVChangeRoleDelegate;

typedef unsigned long long uint64;

/*!
 @abstract      请求一个画面的回调block。
 */
typedef void (^requestViewBlock)(QAVResult result);

/*!
 @abstract      请求多个成员画面的回调block
 */
typedef void (^RequestViewListBlock)(int result, NSString *error_info);

/*!
 @abstract      取消多个成员画面的回调block
 */
typedef void (^CancelViewListBlock)(int result, NSString *error_info);

/**
 @discussion    多人音视频房间类。
 */
@interface QAVRoomMulti : NSObject 

/*!
 @abstract      房间Id
 @discussion    房间Id是由SDK分配的，APP调用enterRoom()方法成功进入房间之后，SDK
                会为当前房间分配一个Id值，互动直播的录制功能开发  需要用到该值。
*/
@property (readonly, nonatomic) UInt32 roomId;


/*!
 @abstract      同时请求一个或多个成员的视频画面。
 @discussion    同时请求一个或多个成员的视频画面。同一时刻只能请求一次成员的画面，
                并且必须等待异步结果返回后，才能进行新的请求画面操作。
 @param         identifierList  请求的成员id列表。传递成员的identifier(NSString*)
 @param         srcTypeList     视频源类型列表。传递成员的avVideoSrcType(NSNumber*)
 @param         block           返回block (^RequestViewListBlock)。
 @warning       <br/>画面大小可以根据业务层实际需要及硬件能力决定。<br/>
                如果是手机，建议只有其中一路是大画面，其他都是小画面，这样硬件更容易扛得住，同时减少流量。<br/>
                这边把320×240及以上大小的画面认为是大画面；反之，认为是小画面。<br/>
                实际上请求到的画面大小，由发送方决定。如A传的画面是小画面，即使这边即使想请求它的大画面，也只能请求到的小画面。<br/>
                发送方传的画面大小，是否同时有大画面和小画面，由其所设置的编解码参数、场景、硬件、网络等因素决定。<br/>
                requestViewList和cancelAllView不能并发执行，即同一时刻只能进行一种操作。<br/>
                identifierList和srcTypeList的因素个数必须相等，同时每个元素是一一对应的。<br/>
                在请求画面前最好先检查该成员是否有对应的视频源。<br/>
 */
- (void)requestViewList:(NSArray *)identifierList srcTypeList:(NSArray *)srcTypeList ret:(RequestViewListBlock)block;

/*!
 @abstract      取消所有请求的视频画面。
 @param         block           返回block (^CancelViewListBlock)。
 @return        QAV_OK 表示成功操作成功 <br/>
                QAV_ERR_FAIL表示操作失败，可能是因为所请求的成员当前已经没有对应视频源的视
                频、所请求成员已经退出房间等。
 @see           QAVResult
 @warning       requestViewList和cancelAllView不能并发执行，即同一时刻只能进行一种操作。
 */
- (void)cancelAllview:(CancelViewListBlock)block;

/*!
 @abstract      开启/关闭音频数据白名单逻辑。
 @param         identifierList为希望收到其音频数据的成员列表
 @return        QAV_OK 表示成功操作成功 <br/>
                QAV_ERR_FAIL表示操作失败，可能是因为房间不存在,个别成员不在房间内或者转换tinyid失败等（这种情况下名单内若有成员id转换成功，仍然会被加入白名单且白名单生效）。
 @see           QAVResult
 @warning       每次最多设置6个成员id，超过6个将添加vector最后6个到白名单，开启白名单时，若identifierList为空，则默认丢弃任所有音频数据；每次调用，白名单将被重置为新的成员
                列表，而不是累加。需要定制想接收的音频数据才需要调用，不调用则默认接收房间内所有音频数据
 */
- (QAVResult)requestAudioList:(NSArray*)identifierList;

/*!
 @abstract      关闭音频数据白名单逻辑。
 @return        QAV_OK 表示成功操作成功 <br/>
                QAV_ERR_FAIL表示操作失败，可能是因为房间不存在等。
 @see           QAVResult
 @warning       关闭白名单逻辑则主动接收房间内所有音频数据。
 */
- (QAVResult)cancelAudioList;

/*!
 @abstract      获取房间成员对象列表。
 @discussion    获取当前正在房间内的成员对象列表,当房间成员小于等于50人时，返回全体房间成员列表，
                当房间成员数大于50人时，只能获取拥有音视频态的成员列表。
 @return        正在房间内的成员对象列表。如果失败则返回nil。
 @warning       所返回的成员对象的生命周期由SDK侧负责，业务侧不需要关心。最好业务侧不要一直持有
                它，可以在每次需要时去重新获取它即可。
 */
- (NSArray *)GetEndpointList;

/*!
 @abstract      获取房间成员对象。
 @discussion    根据成员id获取成员对象。
 @param         identifier      要获取的成员对象的成员id。
 @return        返回对应的成员对象。如果失败时则返回nil。
 @warning       所返回的成员对象的生命周期由SDK侧负责，业务侧不需要关心。最好业务侧不要一直持有
                它，可以在每次需要时去重新获取它即可。
 */
- (QAVEndpoint *)GetEndpointById:(NSString *)identifier;

/*!
 @abstract      修改通话能力权限。
 @discussion    修改通话能力权限。通话能力权限包括是否可以创建房间/进入房间/发送音频/接收音频/
                发送摄像头视频/接收摄像头视频/发送屏幕视频/接收屏幕视频等。
 @param         auth_Bit        通话能力权限位。
 @param         buff            通话能力权限位的加密串。
 @param         dlg             回调函数。
 @warning       废弃接口，修改权限的逻辑业务测只需要调用ChangeAVControlRole: delegate:方法
 */
- (void)ChangeAuthoritybyBit:(uint64)auth_Bit
                    orstring:(NSData *)buff
                    delegate:(id<QAVChangeDelegate>)dlg
                    __attribute((deprecated(
        "废弃接口，修改权限的逻辑业务测只需要调用ChangeAVControlRole: delegate:方法")));


/*!
 @abstract      修改角色。
 @discussion    此前，角色被设定为在进入房间之前指定、进入房间之后不能动态修改。这个接口的作用
                就是修改这一设定，即：在进入房间之后也能动态修改角色。业务测可以通过此接口让用
                户在房间内动态调整音视频、网络参数，如将视频模式从清晰切换成流畅。
 @param         role            角色字符串。
 @param         dlg             回调函数。
 @warning       [改动]这个接口现在已经实现了普通修改角色，修改音视频上下行的权限和切换音频场景
                的功能，以前切换角色后仍然需要切换权限和切换音频场景，新的接口结合了这三个功能。
 */
- (void)ChangeAVControlRole:(NSString *)role
                   delegate:(id<QAVChangeRoleDelegate>)dlg;

/*!
 @abstract      获取音视频通话的实时通话质量的相关信息。该函数主要用来查看实时通话质量、排查问题
                等，业务侧可以不用关心它。
 @return        返回音视频通话的实时通话质量的相关参数，并以字符串形式返回。
 */
- (NSString *)getQualityTips;

/*!
 @abstract      获取音视频通话的实时通话质量的相关信息。该函数主要用来查看实时通话质量、排查问题
                等，业务侧可以用其来做些提高用户体验的功能，如提示网络差，通话质量也较差。
                具体分为如下几个部分1.SDK基本信息参数2.网络层相关的统计参数3.视频相关的统计参数4.音频相关的统计参数
 @discussion    ROOMSTATPARATICKCOUNTBEGIN;            统计开始时间<br/>
                ROOMSTATPARATICKCOUNTBEGIN;            统计开始时间<br/>
                ROOMSTATPARATICKCOUNTEND;              统计结束时间<br/>
                ROOMSTATPARABASICINFO;                 基本信息<br/>
                ROOMSTATPARANETWORK;                   网络信息<br/>
                ROOMSTATPARAVIDEOPARAM;                视频信息<br/>
                ROOMSTATPARAAPPCPURATE;                app的cpu占用率<br/>
                ROOMSTATPARASYSCPURATE;                系统的cpu占用率<br/>
                ROOMSTATPARASDKVERSION;                sdk版本号<br/>
                ROOMSTATPARAINTERFACEIP;               接口机IP<br/>
                ROOMSTATPARAINTERFACEPORT;             接口机端口<br/>
                ROOMSTATPARACLIENTIP;                  客户端ip<br/>
                ROOMSTATPARAKBPSRECV;                  收包速率,单位kbps<br/>
                ROOMSTATPARAPACKETRECV;                收包速率,每秒收包的个数<br/>
                ROOMSTATPARAKBPSSEND;                  发包速率,单位kbps<br/>
                ROOMSTATPARAPACKETSEND;                发包速率,每秒发包的个数<br/>
                ROOMSTATPARARTT;                       往返时延<br/>
                ROOMSTATPARALOSSMODELRECV;             下行平均连续丢包个数<br/>
                ROOMSTATPARALOSSRATERECV;              下行丢包率<br/>
                ROOMSTATPARALOSSMODELSEND;             上行平均连续丢包个数，从Hello的回包中获<br/>
                ROOMSTATPARALOSSRATESEND;              上行丢包率，从Hello的回包中获得<br/>
                ROOMSTATPARALOSSRATESENDUDT;           上行UDT丢包率<br/>
                ROOMSTATPARALOSSRATERECVUDT;           下行UDT丢包率<br/>
                ROOMSTATPARACAPTURE;                   视频采集信息<br/>
                ROOMSTATPARACAPTUREWIDTH;              视频采集宽度<br/>
                ROOMSTATPARACAPTUREHEIGHT;             视频采集高度<br/>
                ROOMSTATPARACAPTUREFPS;                视频采集帧率<br/>
                ROOMSTATPARAVIDEOENCODE;               视频编码参数<br/>
                ROOMSTATPARAVIDEOENCODEVIEWTYPE;       视频类型<br/>
                ROOMSTATPARAVIDEOENCODEWIDTH;          视频编码宽度<br/>
                ROOMSTATPARAVIDEOENCODEHEIGHT;         视频编码高度<br/>
                ROOMSTATPARAVIDEOENCODEFPS;            视频编码帧率<br/>
                ROOMSTATPARAVIDEOENCODEBR;             视频编码码率<br/>
                ROOMSTATPARAVIDEOENCODEAG;             视频编码角度<br/>
                ROOMSTATPARAVIDEOENCODETYPE;           视频编码类型<br/>
                ROOMSTATPARAVIDEOENCODEHW;             是否开启硬编解码<br/>
                ROOMSTATPARAVIDEOENCODECOUNT;          视频编码路数<br/>
                ROOMSTATPARAVIDEOSENDPARAM;            视频发送参数<br/>
                ROOMSTATPARAVIDEOSENDLOSSRATE;         视频发送参数丢包率<br/>
                ROOMSTATPARAVIDEOSENDIFEC;             视频发送参数I帧fec<br/>
                ROOMSTATPARAVIDEOSENDSPFEC;            视频发送参数sp帧<br/>
                ROOMSTATPARAVIDEOSENDPKT;              视频发送参数包数<br/>
                ROOMSTATPARAVIDEOISANCHOR;             是否为主播<br/>
                ROOMSTATPARAVIDEOSENDSTNSBG;           人为补零 //人为丢包<br/>
                ROOMSTATPARAVIDEOSENDSTNSBD;           解码端统计的丢包 //jitter中统计的EOS、大于一定数量的连续丢包<br/>
                ROOMSTATPARAVIDEOSENDSTNSBB;           卡顿次数 //可能的小卡顿次数 //连续plc<br/>
                ROOMSTATPARAVIDEODECODEPARAM;          视频解码参数<br/>
                ROOMSTATPARAVIDEODECODEVIEWTYPE;       视频类型<br/>
                ROOMSTATPARAVIDEODECODETINYID;         视频对应用户<br/>
                ROOMSTATPARAVIDEODECODEWIDTH;          视频编码宽度<br/>
                ROOMSTATPARAVIDEODECODEHEIGHT;         视频编码高度<br/>
                ROOMSTATPARAVIDEODECODEFPS;            视频编码帧率<br/>
                ROOMSTATPARAVIDEODECODEBR;             视频编码码率<br/>
                ROOMSTATPARAVIDEODECODETYPE;           视频编码类型<br/>
                ROOMSTATPARAVIDEODECODEHW;             是否开启硬编解码<br/>
                ROOMSTATPARAVIDEODECODECOUNT;          视频解码路数<br/>
                ROOMSTATPARAVIDEORECVLOSSRATE;         视频接收丢包率<br/>
                ROOMSTATPARAVIDEORECVJITTER;           视频抖动<br/>
                ROOMSTATPARAVIDEORECVBR;               视频接收码率<br/>
                ROOMSTATPARASAMPLERATE;                音频流控下发参数采样率<br/>
                ROOMSTATPARACODECTYPE;                 音频流控下发参数通道数，1表示单声道（mono），2表示立体声（stereo）<br/>
                ROOMSTATPARACHANNELCOUNT;              音频流控下发参数编码类型<br/>
                ROOMSTATPARAQOSMAINBIG;                视频流控下发参数大画面宽<br/>
                ROOMSTATPARAQOSWIDT;                   视频流控下发参数大画面宽<br/>
                ROOMSTATPARAQOSHEIGHT;                 视频流控下发参数大画面高<br/>
                ROOMSTATPARAQOSFPS;                    视频流控下发参数大画面帧率<br/>
                ROOMSTATPARAQOSBITRATE;                视频流控下发参数大画面码率<br/>
                ROOMSTATPARAQOSENCODETYPE;             视频流控下发参数编码类型<br/>
                ROOMSTATPARAQOSMINQP;                  视频流控下发最小质量<br/>
                ROOMSTATPARAQOSMAXQP;                  视频流控下发最大质量<br/>
                ROOMSTATPARAQOSFECTYPE;                视频流控下发参数fec类型<br/>
                ROOMSTATPARAQOSIFEC;                   视频流控下发i帧fec<br/>
                ROOMSTATPARAQOSSPFEC;                  视频流控下发sp帧fec<br/>
                ROOMSTATPARAQOSPFEC;                   视频流控下发p帧fec<br/>
                ROOMSTATPARAQOSIMTU;                   视频流控下发参数I帧mtu<br/>
                ROOMSTATPARAQOSSPMTU;                  视频流控下发参数sp帧mtu<br/>
                ROOMSTATPARAQOSPMTU;                   视频流控下发参数p帧mtu<br/>
                ROOMSTATPARAQOSIMINPKG;                视频流控下发参数i帧最小包<br/>
                ROOMSTATPARAQOSSPMINPKG;               视频流控下发参数sp帧最小包<br/>
                ROOMSTATPARAQOSPMINPKG;                视频流控下发参数p帧最小包<br/>
                ROOMSTATPARAQOSIMINSIZE;               视频流控下发参数i帧最小包大小<br/>
                ROOMSTATPARAQOSSPMINSIZE;              视频流控下发参数sp帧最小包大小<br/>
                ROOMSTATPARAQOSPMINSIZE;               视频流控下发参数p帧最小包大小<br/>
                ROOMSTATPARAQOSGOPTYPE;                视频流控下发参数gop类型<br/>
                ROOMSTATPARAQOSGOP;                    视频流控下发参数gop<br/>
                ROOMSTATPARAQOSHW;                     视频流控下发参数硬编码<br/>
                ROOMSTATPARAAUDIOPARAM;                音频流控参数<br/>
                ROOMSTATPARAAUDIOCAPTURE;              音频采集参数<br/>
                ROOMSTATPARAAUDIOCAPTURESAMPLERATE;    音频采集参数<br/>
                ROOMSTATPARAAUDIOCAPTURECHANNELCOUNT;  音频采集参数<br/>
                ROOMSTATPARAAUDIOENCODEPARAM;          音频编码参数<br/>
                ROOMSTATPARAAUDIOENCODETYPE;           音频编码类型<br/>
                ROOMSTATPARAAUDIOENCODEBR;             音频编码码率<br/>
                ROOMSTATPARAAUDIOENCODEVAD;            音频采集参数<br/>
                ROOMSTATPARAAUDIOSENDPARAM;            音频发送参数<br/>
                ROOMSTATPARAAUDIOSENDLOSSRATE;         音频发送对丢包率<br/>
                ROOMSTATPARAAUDIOSENDFEC;              音频发送的fec<br/>
                ROOMSTATPARAAUDIOSENDJETTER;           音频抖动<br/>
                ROOMSTATPARAAUDIOSENDBR;               音频发送码率<br/>
                ROOMSTATPARAAUDIOSENDBRUDT;            音频发送码率udt<br/>
                ROOMSTATPARAAUDIODECODECOUNT;          音频解码路数<br/>
                ROOMSTATPARAAUDIODECODETYPE;           音频解码类型<br/>
                ROOMSTATPARAAUDIODECODESAMPLETYPE;     音频解码采样率<br/>
                ROOMSTATPARAAUDIODECODECHANNELCOUNT;   音频解码通道数<br/>
                ROOMSTATPARAAUDIODECODETINYID;         对应的tinyid<br/>
                ROOMSTATPARAAUDIORECVPARAM;            音频接收参数<br/>
                ROOMSTATPARAAUDIORECVPLAYDELAY;        音频播放时延<br/>
                ROOMSTATPARAAUDIORECVLOSSRATE;         收包丢包率<br/>
                ROOMSTATPARAAUDIOCATEGORY;             音频场景<br/>
                ROOMSTATPARAQOSSMALLWIDTH;             视频流控下发参数小画面宽<br/>
                ROOMSTATPARAQOSSMALLHEIGHT;            视频流控下发参数小画面高<br/>
                ROOMSTATPARAQOSSMALLFPS;               视频流控下发参数小画面帧率<br/>
                ROOMSTATPARAQOSSMALLBITRATE;           视频流控下发参数小画面码率<br/>
                ROOMSTATPARAEAUDIOQOSPARAM;            音频流控下发参数<br/>
                ROOMSTATPARAEAUDIOQOSSAMPLERATE;       音频流控下发码率<br/>
                ROOMSTATPARAEAUDIOQOSENCODETYPE;       音频流控下发编码类型<br/>
                ROOMSTATPARAEAUDIOQOSCHANNELCOUNT;     音频流控下发采集通道数<br/>
                ROOMSTATPARAEAUDIOQOSBITRATE;          音频流控下发码率<br/>
                ROOMSTATPARAEAUDIOQOSAEC;              音频流控下发aec<br/>
                ROOMSTATPARAEAUDIOQOSAGC;              音频流控下发agc<br/>
                ROOMSTATPARAEAUDIOQOSFEC;              音频流控下发fec<br/>
                ROOMSTATPARAEAUDIOQOSVAD;              音频流控下发vad<br/>
                ROOMSTATPARAEAUDIOQOSPACK;             音频流控下发pack<br/>
                ROOMSTATPARAEAUDIOQOSRECN;             音频流控下发recn<br/>
                ROOMSTATPARAEAUDIOQOSRECM;             音频流控下发recm<br/>
                ROOMSTATPARAEAUDIOQOSMTU;              音频流控下发mtu<br/>
                ROOMSTATPARAEAUDIOQOSJITTERMIN;        音频流控下发，Jitter的最小延时，单位毫秒<br/>
                ROOMSTATPARAEAUDIOQOSJITTERMINMAX;     音频流控下发，Jitter最大延时的最小阀值<br/>
                ROOMSTATPARAEAUDIOQOSJITTERMAXMAX;     音频流控下发，Jitter最大延时的最大阀值<br/>
                ROOMSTATPARAEAUDIOQOSJITTERDROP;       音频流控下发，Jitter最大延时的损失比例<br/>
                ROOMSTATPARAUINO;                              <br/>
                ROOMSTATPARAUIN1;                              <br/>
                ROOMSTATPARAUIN2;                              <br/>
                ROOMSTATPARAUIN3;                              <br/>
                ROOMSTATPARAUIN4;                       解码画面，最多4路主路＋1路辅路,包含tinyid,分辨率，帧率(*10),码率
 @return        以NSDictionary形式返回音视频通话的实时通话质量的相关参数。
 
 */
- (NSDictionary *)GetQualityParam;

/*!
 @abstract      获取音视频无参考评分。
 @return        返回音视频通话的实时通话质量的相关参数，并以字符串形式返回。
 */
- (NSArray *)getStatisticsParam;
@end


/*!
 @discussion    房间委托协议。业务侧需要实现它来处理房间异步操作返回的结果。
 */
@protocol QAVRoomDelegate <NSObject>

@required

/*!
 @abstract      返回QAVContext::EnterRoom()的异步操作结果的函数。
 @discussion    此函数用来返回QAVContext::EnterRoom()的异步操作结果。
 @param         result          返回码。SDK的各种返回码的定义和其他详细说明参考QAVError.h。
 @param         error_info      错误码描述信息
 */
- (void)OnEnterRoomComplete:(int)result WithErrinfo:(NSString *)error_info;

/*!
 @abstract      返回switchRoom()的异步操作结果的函数。
 @discussion    APP使用switchRoom可以快速切换房间，在使用switchRoom之后可以通过这个回调拿到
                执行结果，APP需要根据结果进行相应处理.retCode要关注以下三个取值：<br/>
                QAV_OK,<br/>
                QAV_ERR_NOT_TRY_NEW_ROOM,<br/>
                QAV_ERR_TRY_NEW_ROOM_FAILED.<br/>
 @param         result          QAV_OK表示切换成功，需要APP侧将前一个房间的成员、以及视频相关内容清空;<br/>
                                QAV_ERR_NOT_TRY_NEW_ROOM表示切换中途停止而仍然留在原房间;<br/>
                                QAV_ERR_TRY_NEW_ROOM_FAILED表示原房间已退出，而新房间进入失败，APP
                                侧要做退出房间的处理
 @param         error_info      错误信息描述
 */
- (void)OnSwitchRoomComplete:(int)result WithErrinfo:(NSString *)error_info;

/*!
 @abstract      退出房间完成回调。
 @discussion    APP调用ExitRoom()后，SDK通过此回调通知APP成功退出了房间。
 */
- (void)OnExitRoomComplete;

/*!
 @abstract      SDK主动退出房间提示。
 @discussion    该回调方法表示SDK内部主动退出了房间。SDK内部会因为30s心跳包超时等原因主动退出
                房间，APP需要监听此退出房间事件并对该事件进行相应处理
 @param         result          退出房间的原因，具体值见返回码。SDK的各种返回码的定义和其他
                                详细说明参考QAVError.h。
 @param         error_info      错误信息描述
 */
- (void)OnRoomDisconnect:(int)result WithErrinfo:(NSString *)error_info;

/*!
 @abstract      房间成员状态变化通知的函数。
 @discussion    当房间成员发生状态变化(如是否发音频、是否发视频等)时，会通过该函数通知业务侧。
 @param         eventID         状态变化id，详见QAVUpdateEvent的定义。
 @param         endpoints       发生状态变化的成员id列表。
 */
- (void)OnEndpointsUpdateInfo:(QAVUpdateEvent)eventID endpointlist:(NSArray *)endpoints;

/*!
 @abstract      房间成员无某个通话能力权限却去使用相关通话能力而导致的异常通知的函数。
 @discussion    当房间成员使用某个通话能力(如发送视频)且这时并没有这个通话能力权限，就会通过这个
                函数通知业务侧。
 @param         privilege       当前SDK侧所记录的该房间成员的通话能力权限值。
 */
- (void)OnPrivilegeDiffNotify:(int)privilege;

/*!
 @abstract      半自动模式接收摄像头视频的事件通知。
 @discussion    半自动模式接收摄像头视频的事件通知。也就是当进入房间时，如果已经有人发送了视频，
                则会自动接收这些视频，不用手动去请求。当然，进入房间后，如何其他人再发送的视频，
                则不会自动接收。
 @param         identifierList  自动接收的摄像头视频所对应的成员id列表。
 */
- (void)OnSemiAutoRecvCameraVideo:(NSArray *)identifierList;

/*!
 @abstract      摄像头分辨率和帧率修改通知。
 @discussion    客户如果使用外部输入流的话（enableExternalCapture）需要根据此通知中的分辨率和
                帧率动态修改摄像头的分辨率和帧率。分辨率的设置优先选比通知中的分辨率同比高档次的分
                辨率，其次选高档次分辨率，如果以上两种分辨率都没，建议设置为640*480
 @param         width           分辨率宽
 @param         height          分辨率高
 @param         fps             帧率
 */
- (void)OnCameraSettingNotify:(int)width Height:(int)height Fps:(int)fps;

/*!
 @abstract      sdk事件通知
 @discussion    用于通知从创建房间到退出时的一些事件，目前只通知连接某个接口机ip超时
 @param         type            事件主类型  
 @param         subtype         事件子类型  
 @param         data            事件数据
 */
- (void)OnRoomEvent:(int)type subtype:(int)subtype data:(void *)data;

/*!
 @abstract      硬件状态事件通知
 @discussion    用于通知硬件软件切换的状态
 @param         isEncoder       编码还是解码选择
 @param         isMain          主路辅路选择
 @param         IsswitchToSoft  是不是切换到软件选择
 @param         Uin             多路解码情况，解码的uin号码
 */
-(void)OnHwStateChangeNotify:(BOOL)isEncoder isMain:(BOOL)isMainVideo isSwitchToSoft:(BOOL)switchToSoft identifier:(NSString *)identifier;
@end

/*!
 @discussion    切换权限委托协议
 */
@protocol QAVChangeDelegate <NSObject>
@required

/*!
 @abstract      修改权限的回调。
 @discussion    当用户的权限被修改后，假如继承了QAVChangeDelegate，将会回调到此方法中。
 @param         result          返回QAV_OK表示修改成功，失败则返回其他值，参照QAVResult
 @see           QAVResult
 @param         error_info      错误信息描述
 */
- (void)OnChangeAuthority:(int)result WithErrinfo:(NSString *)error_info;
@end
/*!
 @discussion    切换角色委托协议
 */
@protocol QAVChangeRoleDelegate <NSObject>
@required

/*!
 @abstract      修改角色的回调。
 @discussion    当用户的角色被修改后，假如继承了QAVChangeRoleDelegate，将会回调到此方法中。
 @param         result          返回QAV_OK表示修改成功，失败则返回其他值，参照QAVResult
 @see           QAVResult
 @param         error_info      错误信息描述
 */
- (void)OnChangeRoleDelegate:(int)result WithErrinfo:(NSString *)error_info;

@end
