/*
 Copyright 2017 Vector Creations Ltd

 Licensed under the Apache License, Version 2.0 (the "License");
 you may not use this file except in compliance with the License.
 You may obtain a copy of the License at

 http://www.apache.org/licenses/LICENSE-2.0

 Unless required by applicable law or agreed to in writing, software
 distributed under the License is distributed on an "AS IS" BASIS,
 WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 See the License for the specific language governing permissions and
 limitations under the License.
 */

#import <Foundation/Foundation.h>

#import <MatrixSDK/MatrixSDK.h>

#import "Widget.h"

/**
 The type of matrix event used for modular widgets.
 */
FOUNDATION_EXPORT NSString *const kWidgetEventTypeString;

/**
 Known types widgets.
 */
FOUNDATION_EXPORT NSString *const kWidgetTypeJitsi;

/**
 Posted when a widget has been created, updated or disabled.
 The notification object is the `Widget` instance.
 */
FOUNDATION_EXPORT NSString *const kWidgetManagerDidUpdateWidgetNotification;

/**
 `WidgetManager` NSError domain and codes.
 */
FOUNDATION_EXPORT NSString *const WidgetManagerErrorDomain;

typedef enum : NSUInteger
{
    WidgetManagerErrorCodeNotEnoughPower,
    WidgetManagerErrorCodeCreationFailed
}
WidgetManagerErrorCode;


/**
 The `WidgetManager` helps to handle modular widgets.
 */
@interface WidgetManager : NSObject

/**
 Returns the shared widget manager.

 @return the shared widget manager.
 */
+ (instancetype)sharedManager;

/**
 List all active widgets in a room.
 
 @param room the room to check.
 @return a list of widgets.
 */
- (NSArray<Widget*> *)widgetsInRoom:(MXRoom*)room;

/**
 List all active widgets of a given type in a room.

 @param widgetTypes the types of widget to search. Nil means all types.
 @param room the room to check.
 @return a list of widgets.
 */
- (NSArray<Widget*> *)widgetsOfTypes:(NSArray<NSString*>*)widgetTypes inRoom:(MXRoom*)room;

/**
 List all active widgets of a given type in a room, excluding some types.

 @param notWidgetTypes the types of widget to not consider. Nil means all types.
 @param room the room to check.
 @return a list of widgets.
 */
- (NSArray<Widget*> *)widgetsNotOfTypes:(NSArray<NSString*>*)notWidgetTypes inRoom:(MXRoom*)room;

/**
 Add a modular widget to a room.

 @param widgetId the id of the widget.
 @param widgetContent the widget content.
 @param room the room to create the widget to.

 @param success A block object called when the operation succeeds. It provides the newly added widget.
 @param failure A block object called when the operation fails.

 @return a MXHTTPOperation instance.
 */
- (MXHTTPOperation *)createWidget:(NSString*)widgetId
                      withContent:(NSDictionary<NSString*, NSObject*>*)widgetContent
                           inRoom:(MXRoom*)room
                          success:(void (^)(Widget *widget))success
                          failure:(void (^)(NSError *error))failure;

/**
 Add a jitsi conference widget to a room.

 @param room the room to create the widget to.
 @param video the conference type

 @param success A block object called when the operation succeeds. It provides the newly added widget.
 @param failure A block object called when the operation fails.

 @return a MXHTTPOperation instance.
 */
- (MXHTTPOperation *)createJitsiWidgetInRoom:(MXRoom*)room
                                   withVideo:(BOOL)video
                                     success:(void (^)(Widget *jitsiWidget))success
                                     failure:(void (^)(NSError *error))failure;

/**
 Close/Disable a widget in a room.

 @param widgetId the id of the widget to close.
 @param room the room the widget is in.

 @param success A block object called when the operation succeeds.
 @param failure A block object called when the operation fails.

 @return a MXHTTPOperation instance.
 */
- (MXHTTPOperation *)closeWidget:(NSString*)widgetId inRoom:(MXRoom*)room
                         success:(void (^)())success
                         failure:(void (^)(NSError *error))failure;


/**
 Add/remove matrix session.
 
 Registering session allows to generate `kWidgetManagerDidUpdateWidgetNotification` notifications.
 
 @param mxSession the session to add/remove.
 */
- (void)addMatrixSession:(MXSession*)mxSession;
- (void)removeMatrixSession:(MXSession*)mxSession;

/**
 Delete the data associated with an user.
 
@param userId the id of the user.
 */
- (void)deleteDataForUser:(NSString*)userId;

#pragma mark - Modular interface

/**
 Make sure there is a scalar token for the given Matrix session.
 
 If no token was gotten and stored before, the operation will make http requests
 to get one.

 @param mxSession the session to check.
 
 @param success A block object called when the operation succeeds.
 @param failure A block object called when the operation fails.
 */
- (MXHTTPOperation *)getScalarTokenForMXSession:(MXSession*)mxSession
                                        success:(void (^)(NSString *scalarToken))success
                                        failure:(void (^)(NSError *error))failure;

/**
 The current scalar token for the given Matrix session.

 It may be nil if `getScalarTokenForMXSession` was never called before.
 
 @param mxSession the session to check.
 @return the current scalar token .
 */
- (NSString *)scalarTokenForMXSession:(MXSession*)mxSession;

@end