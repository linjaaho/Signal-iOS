//
//  Copyright (c) 2020 Open Whisper Systems. All rights reserved.
//

#import <SignalServiceKit/BaseModel.h>

NS_ASSUME_NONNULL_BEGIN

typedef void (^OWSUserProfileCompletion)(void);

@class OWSAES256Key;
@class SDSAnyReadTransaction;
@class SDSAnyWriteTransaction;
@class SignalServiceAddress;

extern NSNotificationName const kNSNotificationNameProfileWhitelistDidChange;
extern NSNotificationName const kNSNotificationNameLocalProfileDidChange;
extern NSNotificationName const kNSNotificationNameOtherUsersProfileWillChange;
extern NSNotificationName const kNSNotificationNameOtherUsersProfileDidChange;

extern NSString *const kNSNotificationKey_ProfileAddress;
extern NSString *const kNSNotificationKey_ProfileGroupId;

@interface OWSUserProfile : BaseModel

@property (atomic, readonly) SignalServiceAddress *address;
@property (atomic, readonly, nullable) OWSAES256Key *profileKey;
@property (atomic, readonly, nullable) NSString *givenName;
@property (atomic, readonly, nullable) NSString *familyName;
@property (atomic, readonly, nullable) NSPersonNameComponents *nameComponents;
@property (atomic, readonly, nullable) NSString *fullName;
@property (atomic, readonly, nullable) NSString *username;
@property (atomic, readonly) BOOL isUuidCapable;
@property (atomic, readonly, nullable) NSString *avatarUrlPath;
// This filename is relative to OWSProfileManager.profileAvatarsDirPath.
@property (atomic, readonly, nullable) NSString *avatarFileName;

+ (instancetype)new NS_UNAVAILABLE;
- (instancetype)init NS_UNAVAILABLE;
- (nullable instancetype)initWithCoder:(NSCoder *)coder NS_DESIGNATED_INITIALIZER;
- (instancetype)initWithUniqueId:(NSString *)uniqueId NS_UNAVAILABLE;
- (instancetype)initWithGrdbId:(int64_t)grdbId uniqueId:(NSString *)uniqueId NS_UNAVAILABLE;

// This initializer should only be called internally.
- (instancetype)initWithAddress:(SignalServiceAddress *)address NS_DESIGNATED_INITIALIZER;

// --- CODE GENERATION MARKER

// This snippet is generated by /Scripts/sds_codegen/sds_generate.py. Do not manually edit it, instead run `sds_codegen.sh`.

// clang-format off

- (instancetype)initWithGrdbId:(int64_t)grdbId
                      uniqueId:(NSString *)uniqueId
                  avatarFileName:(nullable NSString *)avatarFileName
                   avatarUrlPath:(nullable NSString *)avatarUrlPath
                      familyName:(nullable NSString *)familyName
                   isUuidCapable:(BOOL)isUuidCapable
                      profileKey:(nullable OWSAES256Key *)profileKey
                     profileName:(nullable NSString *)profileName
            recipientPhoneNumber:(nullable NSString *)recipientPhoneNumber
                   recipientUUID:(nullable NSString *)recipientUUID
                        username:(nullable NSString *)username
NS_DESIGNATED_INITIALIZER NS_SWIFT_NAME(init(grdbId:uniqueId:avatarFileName:avatarUrlPath:familyName:isUuidCapable:profileKey:profileName:recipientPhoneNumber:recipientUUID:username:));

// clang-format on

// --- CODE GENERATION MARKER

@property (atomic, readonly, class) SignalServiceAddress *localProfileAddress;
+ (BOOL)isLocalProfileAddress:(SignalServiceAddress *)address;
+ (SignalServiceAddress *)resolveUserProfileAddress:(SignalServiceAddress *)address;

+ (nullable OWSUserProfile *)getUserProfileForAddress:(SignalServiceAddress *)address
                                          transaction:(SDSAnyReadTransaction *)transaction;

+ (OWSUserProfile *)getOrBuildUserProfileForAddress:(SignalServiceAddress *)recipientId
                                        transaction:(SDSAnyWriteTransaction *)transaction;

+ (nullable OWSUserProfile *)userProfileForUsername:(NSString *)username
                                        transaction:(SDSAnyReadTransaction *)transaction;

+ (BOOL)localUserProfileExistsWithTransaction:(SDSAnyReadTransaction *)transaction;

#pragma mark - Update With... Methods

- (void)updateWithGivenName:(nullable NSString *)givenName
                 familyName:(nullable NSString *)familyName
              avatarUrlPath:(nullable NSString *)avatarUrlPath
             avatarFileName:(nullable NSString *)avatarFileName
                transaction:(SDSAnyWriteTransaction *)transaction
                 completion:(nullable OWSUserProfileCompletion)completion;

- (void)updateWithGivenName:(nullable NSString *)givenName
                 familyName:(nullable NSString *)familyName
                   username:(nullable NSString *)username
              isUuidCapable:(BOOL)isUuidCapable
              avatarUrlPath:(nullable NSString *)avatarUrlPath
                transaction:(SDSAnyWriteTransaction *)transaction
                 completion:(nullable OWSUserProfileCompletion)completion;

- (void)updateWithGivenName:(nullable NSString *)givenName
                 familyName:(nullable NSString *)familyName
                   username:(nullable NSString *)username
              isUuidCapable:(BOOL)isUuidCapable
              avatarUrlPath:(nullable NSString *)avatarUrlPath
             avatarFileName:(nullable NSString *)avatarFileName
                transaction:(SDSAnyWriteTransaction *)transaction
                 completion:(nullable OWSUserProfileCompletion)completion;

- (void)updateWithAvatarFileName:(nullable NSString *)avatarFileName transaction:(SDSAnyWriteTransaction *)transaction;

- (void)updateWithProfileKey:(OWSAES256Key *)profileKey
         wasLocallyInitiated:(BOOL)wasLocallyInitiated
                 transaction:(SDSAnyWriteTransaction *)transaction
                  completion:(nullable OWSUserProfileCompletion)completion;

- (void)updateWithGivenName:(nullable NSString *)givenName
                 familyName:(nullable NSString *)familyName
        wasLocallyInitiated:(BOOL)wasLocallyInitiated
                transaction:(SDSAnyWriteTransaction *)transaction
                 completion:(nullable OWSUserProfileCompletion)completion;

- (void)updateWithGivenName:(nullable NSString *)givenName
                 familyName:(nullable NSString *)familyName
              avatarUrlPath:(nullable NSString *)avatarUrlPath
        wasLocallyInitiated:(BOOL)wasLocallyInitiated
                transaction:(SDSAnyWriteTransaction *)transaction
                 completion:(nullable OWSUserProfileCompletion)completion;

- (void)clearWithProfileKey:(OWSAES256Key *)profileKey
        wasLocallyInitiated:(BOOL)wasLocallyInitiated
                transaction:(SDSAnyWriteTransaction *)transaction
                 completion:(nullable OWSUserProfileCompletion)completion;

- (void)updateWithUsername:(nullable NSString *)username
             isUuidCapable:(BOOL)isUuidCapable
               transaction:(SDSAnyWriteTransaction *)transaction;

#pragma mark - Profile Avatars Directory

+ (NSString *)profileAvatarFilepathWithFilename:(NSString *)filename;
+ (nullable NSError *)migrateToSharedData;
+ (NSString *)legacyProfileAvatarsDirPath;
+ (NSString *)sharedDataProfileAvatarsDirPath;
+ (NSString *)profileAvatarsDirPath;
+ (void)resetProfileStorage;

+ (NSSet<NSString *> *)allProfileAvatarFilePathsWithTransaction:(SDSAnyReadTransaction *)transaction;

+ (void)mergeUserProfilesIfNecessaryForAddress:(SignalServiceAddress *)address
                                   transaction:(SDSAnyWriteTransaction *)transaction;

- (OWSUserProfile *)shallowCopy;

@end

NS_ASSUME_NONNULL_END
