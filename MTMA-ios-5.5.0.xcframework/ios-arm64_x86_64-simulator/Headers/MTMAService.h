//
//  MTMAService.h
//  MTMA
//
//  Created by Shuni Huang on 2024/11/2.
//

#import <Foundation/Foundation.h>

#define MTMA_VERSION_NUMBER 5.5.0

NS_ASSUME_NONNULL_BEGIN
@class MTMAUserID;
@class MTMAUserContact;
@class MTMAEventObject;

/** 独立版 MA SDK 初始化结果。 */
@interface MTMAInitResult : NSObject

@property (nonatomic, assign, readonly) NSInteger code;
@property (nonatomic, copy, readonly) NSString *message;
@property (nonatomic, copy, readonly, nullable) NSString *EUID;
@property (nonatomic, copy, readonly, nullable) NSString *maRID;
@property (nonatomic, assign, readonly, getter=isSuccess) BOOL success;

@end

@interface MTMAConfig : NSObject

/** MA AppKey，与 Push AppKey 相互独立。 */
@property (nonatomic, copy) NSString *appKey;

/* 用户标识模型,当设置该属性时时会在初始化时进行设置用户标识 */
@property (nullable, nonatomic, strong) MTMAUserID *userID;

/** 新版初始化结果回调；设置后优先于 completion。 */
@property (nonatomic, copy) void (^resultCompletion)(MTMAInitResult *result);

/* 请求结果回调, code:0为成功。请改用 resultCompletion。 */
@property (nonatomic, copy) void (^completion)(NSInteger code, NSString * message)
    DEPRECATED_MSG_ATTRIBUTE("Use resultCompletion instead");

@end

@interface MTMACollectControl : NSObject

/* idfa idfa信息。设置为NO,不采集idfa信息。默认为NO。*/
@property (nonatomic, assign) BOOL idfa;
/* idfv idfv信息。设置为NO,不采集idfv信息。默认为NO。*/
@property (nonatomic, assign) BOOL idfv;
/* carrier 运营商信息。设置为NO,不采集运营商信息。默认为YES。 */
@property (nonatomic, assign) BOOL carrier;

@end


@interface MTMAService : NSObject

/*!
 * @abstract 初始化SDK
 * 每次有效调用按顺序初始化；支持同一进程内切换 MA AppKey。
 * 每轮都会请求后端确认 EUID，结果分别回调；前一轮回调结束后执行下一轮。
 *
 */
+ (void)start:(MTMAConfig *)config;

/**
 设置用户标识
 
 @param userID 用户标识模型
 */
+ (void)identifyAccount:(MTMAUserID *)userID;

/**
 设置用户联系方式
 
 @param contact 用户联系方式模型。contacts 的 key 必须为长度 1～256 的 NSString；
 value 必须为 NSString，空字符串表示清空该联系方式，纯空白字符串无效
 */
+ (void)setUserContact:(MTMAUserContact *)contact;

/**
 为第三方 Push 通道设置 RID 或 Token。

 @param channelId MA 控制台中的第三方 Push 通道 ID，必须大于 0
 @param values 当前通道的 RID 或 Token 数组；SDK 去除每个值的首尾空白后发送，
 数组及裁剪后的元素均不能为空
 @param completion 请求结果回调，code 为 0 表示成功
 */
+ (void)setChannelValueWithChannelId:(NSInteger)channelId
                              values:(NSArray<NSString *> *)values
                          completion:(void (^)(NSInteger code,
                                                NSString *message))completion;

/*！自定义事件统计
 *
 * @param event 上报的自定义事件模型
 */
+ (void)eventRecord:(MTMAEventObject *)event;

/**
 设置上报数据间隔，不调用该接口时，默认为10s上报一次事件数据
 上报间隔内存缓存，需要在应用程序每次生命周期中调用才会生效
 
 @param interval 上报间隔，单位s(秒)
 */
+ (void)setReportInterval:(NSInteger)interval;

/**
    设置事件缓存上限条数，默认50条，最高不能超过500条
    当超出缓存数量时会上报全部数据
    @param count 事件缓存条数上限
 */
+ (void)setMaxEventCacheCount:(NSInteger)count;

/**
 设置会话超时时间
 APP切换到后台，会话开始超时计时，超时时间内没有活动，就结束当前会话
 
 @param interval 超时时长，单位s(秒)
 */
+ (void)setNoActiveSessionEndDurationTime:(NSInteger)interval;

/**
 获取运营增长EUID
 */
+ (nullable NSString *)EUID;

/**
 获取当前 MA Registration ID。
 */
+ (nullable NSString *)maRID;

/**
 * 设置UTM属性
 *
 * @discussion
 * UTM属性为预置属性
 * 目前能够设置UTM属性为
 * utm_source 广告系列来源
 * utm_medium 广告系列媒介
 * utm_term 广告系列字词
 * utm_content 广告系列内容
 * utm_campaign 广告系列名称
 * utm_id 广告系列ID
 *
 */
+ (void)setUtmProperties:(NSDictionary<NSString*, NSString *> *)property;

/**
 * @abstract
 * 设置用户属性
 *
 * 这些 用户属性 的内容用一个 NSDictionary 来存储
 * 其中的 key 是 用户属性 的名称，必须是 NSString
 * Value 支持 NSString、NSNumber、字符串 NSSet/NSArray、NSDictionary（object）
 * 以及 NSArray<NSDictionary *>（object_array）
 * NSDictionary 的子字段只支持 NSString、NSNumber、字符串集合和 NSNull，
 * NSNull 表示移除该子字段；不支持继续嵌套 object/object_array
 * object 已存在时只合并本次传入的子字段；object_array 已存在时整体替换
 * 其它类型的属性已存在时覆盖，不存在时创建
 *
 * @param userinfo 要替换的那些 用户属性 的内容
 * @param completion 回调
 */
+ (void)setProperty:(NSDictionary *)userinfo completion:(void (^)(NSInteger code, NSString * message))completion;
 
/**
 * @abstract
 * 设置用户的单个 用户属性 的内容
 *
 * Value 支持 NSString、NSNumber、字符串 NSSet/NSArray、NSDictionary（object）
 * 以及 NSArray<NSDictionary *>（object_array）
 * object 已存在时只合并本次传入的子字段；子字段传 NSNull 时移除该子字段
 * object_array 已存在时整体替换；其它类型已存在时覆盖，不存在时创建
 *
 * @param key 用户属性 的名称
 * @param value 用户属性 的内容
 * @param completion 回调
 */
+ (void)setProperty:(NSString *)key to:(id)value completion:(void (^)(NSInteger code, NSString * message))completion;

/**
 * @abstract
 * 局部更新 object_array 中唯一匹配的一个对象
 *
 * identifierKey 对应的子字段必须已经在 object_array 元数据中定义，类型只能是
 * NSString 或非布尔、有限值 NSNumber；identifierValue 类型必须与定义一致，并且在数组中只能匹配一项
 * values 是需要合并到目标对象的非空子字段字典；未传字段保持不变，值为 NSNull 的字段会被移除
 * values 不能修改 identifierKey，也不支持嵌套 object/object_array
 * object_array 属性必须已经存在；首次创建请先用 setProperty:to: 传非空对象数组
 * 当前用户尚无该属性值时，服务端可能返回成功但不产生修改，调用方需核对返回数据
 *
 * @param key object_array 用户属性名称
 * @param identifierKey 用于唯一定位数组元素的子字段名称
 * @param identifierValue 用于匹配数组元素的 NSString 或非布尔、有限值 NSNumber
 * @param values 要更新或移除的子字段
 * @param completion 回调
 */
+ (void)updateObjectArrayProperty:(NSString *)key
                    identifierKey:(NSString *)identifierKey
                  identifierValue:(id)identifierValue
                           values:(NSDictionary<NSString *, id> *)values
                       completion:(void (^)(NSInteger code, NSString * message))completion;

/**
 * @abstract
 * 往 object_array 末尾追加一个对象元素
 *
 * object 必须是非空 NSDictionary，且至少保留一个非 NSNull 子字段；
 * 子字段规则与 setProperty:to: 的 object 一致，不支持嵌套 object/object_array
 * 与 setProperty:to: 传整个数组不同，本接口只追加一个元素，不影响已有元素
 * 该属性必须已经在服务端元数据中定义为 object_array；尚未定义时服务端可能
 * 返回成功但不产生修改，请先用 setProperty:to: 传非空对象数组完成首次创建
 *
 * @param key object_array 用户属性名称
 * @param object 要追加的对象元素
 * @param completion 回调
 */
+ (void)addObjectArrayProperty:(NSString *)key
                           object:(NSDictionary<NSString *, id> *)object
                       completion:(void (^)(NSInteger code, NSString * message))completion;

/**
 * @abstract
 * 从 object_array 中移除唯一匹配的一个对象元素
 *
 * identifierKey 与 identifierValue 的约束和 updateObjectArrayProperty: 完全一致：
 * 子字段必须已在元数据中定义，类型只能是 NSString 或非布尔、有限值 NSNumber，
 * 且在数组中只能匹配一项
 * 本接口移除的是整个数组元素；只想移除元素里的某个子字段请用
 * updateObjectArrayProperty:，把该子字段传 NSNull
 * 该属性尚未在服务端定义，或当前用户没有该属性值时，服务端会忽略本次请求并
 * 返回成功，调用方需核对返回数据
 *
 * @param key object_array 用户属性名称
 * @param identifierKey 用于唯一定位数组元素的子字段名称
 * @param identifierValue 用于匹配数组元素的 NSString 或非布尔、有限值 NSNumber
 * @param completion 回调
 */
+ (void)removeObjectArrayProperty:(NSString *)key
                    identifierKey:(NSString *)identifierKey
                  identifierValue:(id)identifierValue
                       completion:(void (^)(NSInteger code, NSString * message))completion;
 
/**
 * @abstract
 * 给一个数值类型的 用户属性 增加一个数值
 *
 * 只能对 NSNumber 类型的 用户属性 调用这个接口，否则会被忽略
 * 如果这个 用户属性 之前不存在，则初始值当做 0 来处理
 *
 * @param key  待增加数值的 用户属性 的名称
 * @param amount   要增加的数值
 * @param completion 回调
 */
+ (void)increaseProperty:(NSString *)key by:(NSNumber *)amount completion:(void (^)(NSInteger code, NSString * message))completion;
 
/**
 * @abstract
 * 给多个数值类型的 用户属性 增加数值
 *
 * userinfo 中，key 是 NSString ，value 是 NSNumber
 * 其它与 - (void)increase:by: 相同
 *
 * @param userinfo 多个
 * @param completion 回调
 */
+ (void)increaseProperty:(NSDictionary *)userinfo completion:(void (^)(NSInteger code, NSString * message))completion;
 
/**
 * @abstract
 * 向一个 NSSet 或者 NSArray 类型的属性 添加一些值
 *
 * 如前面所述，这个 NSSet 或者 NSArray 的元素必须是 NSString，否则，会忽略
 * 同时，如果要 append 的 用户属性 之前不存在，会初始化一个空的 NSSet 或者 NSArray
 *
 * @param key key
 * @param content  NSSet 或者 NSArray
 * @param completion 回调
 */
+ (void)addProperty:(NSString *)key by:(NSObject<NSFastEnumeration> *)content completion:(void (^)(NSInteger code, NSString * message))completion;

/**
 * @abstract
 * 向多个 NSSet 或者 NSArray 类型的属性 添加一些值
 *
 * userinfo 中，key 是 NSString ，value 是 NSSet 或者 NSArray
 * 其它与 - (void)append:by: 相同
 *
 * @param userinfo 多个
 * @param completion 回调
 */
+ (void)addProperty:(NSDictionary *)userinfo completion:(void (^)(NSInteger code, NSString * message))completion;


/**
 * @abstract
 * 向一个 NSSet 或者 NSArray 类型的属性 删除一些值
 *
 * @param key key
 * @param content NSSet 或者 NSArray 类型，里面的元素必须是 NSString
 * @param completion 回调
 */
+ (void)removeProperty:(NSString * )key by:(NSObject<NSFastEnumeration> *)content completion:(void (^)(NSInteger code, NSString * message))completion;

     
/**
 * @abstract
 * 删除某个 用户属性 的全部内容
 * 如果这个 用户属性 之前不存在，则直接忽略
 *
 * @param key 用户属性 的名称
 * @param completion 回调
 */
+ (void)deleteProperty:(NSString *)key completion:(void (^)(NSInteger code, NSString * message))completion;

/**
 数据采集控制
 
 @param control 数据采集配置。
 */
+ (void)setCollectControl:(MTMACollectControl *)control;


@end

// 用户标识模型。userID、anonymousID、email、phone 共用清洗规则：
// 先去除首尾空格，空值及 0/null/undefined/nan（忽略大小写）均按未传处理，
// 不参与本次身份匹配，也不产生逐字段结果。SDK 不修改调用方持有的对象。
// 非 NSString、超长或格式错误只剔除对应字段并记录本地结果，不阻断初始化。
@interface MTMAUserID : NSObject

/* userID，最长 255 个 Unicode 字符；空值和保留值按上述统一清洗规则处理 */
@property (nullable, nonatomic, copy) NSString *userID;

/* anonymousID，最长 256 个 Unicode 字符；空值和保留值按上述统一清洗规则处理 */
@property (nullable, nonatomic, copy) NSString *anonymousID;

/* email，最长 256 个 Unicode 字符，须包含一个非首尾的 @，不能包含 ASCII 空白；空值和保留值按上述统一清洗规则处理 */
@property (nullable, nonatomic, copy) NSString *email;

/* phone，须符合 E.164 格式：+ 后为 2 至 15 位数字，首位数字不能为 0；空值和保留值按上述统一清洗规则处理 */
@property (nullable, nonatomic, copy) NSString *phone;

/**
 主线程异步回调，code=0 不代表所有标识都已绑定。
 start: 的 code 表示初始化结果；成功 message 为空字符串或仅含本地拒绝字段 JSON，
 不返回服务端 resultMap。无可用标识时仍按普通初始化继续。
 identifyAccount: 成功时合并服务端 resultMap 与本地拒绝结果，两者都没有时 message 为 success；
 无可用标识时返回 -3。message 不保证是 JSON，缺少某字段不表示该字段成功。
 */
@property (nullable, nonatomic, copy) void (^completion)(NSInteger code, NSString * message);

@end


/**
 * @abstract 自定义事件对象
 *
 * @discussion 所有的字符串属性长度不能超过50字节（包括extra的key）
 *
 */
@interface MTMAEventObject : NSObject
//事件ID，必填，非空，不能使用el/engagelab/metaverse前缀
@property (nonatomic, copy, nonnull) NSString * eventName;
//自定义属性（<=100个）key为NSString，只能包含数字小写字母下划线，以及以字母开头
@property (nonatomic, strong, nonnull) NSDictionary<NSString *, id> * property;

@end


//用户联系方式模型
@interface MTMAUserContact: NSObject

@property (nonatomic, strong) NSDictionary<NSString *, NSString *> *contacts;

/* 请求结果回调, code:0为成功 */
@property (nullable, nonatomic, copy) void (^completion)(NSInteger code, NSString * message);

@end


NS_ASSUME_NONNULL_END
