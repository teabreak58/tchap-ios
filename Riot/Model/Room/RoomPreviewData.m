/*
 Copyright 2016 OpenMarket Ltd
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

#import "RoomPreviewData.h"

@implementation RoomPreviewData

- (instancetype)initWithRoomId:(NSString *)roomId andSession:(MXSession *)mxSession
{
    self = [super init];
    if (self)
    {
        _roomId = roomId;
        _mxSession = mxSession;
        _numJoinedMembers = -1;
    }
    return self;
}

- (instancetype)initWithRoomId:(NSString *)roomId emailInvitationParams:(NSDictionary *)emailInvitationParams andSession:(MXSession *)mxSession
{
    self = [self initWithRoomId:roomId andSession:mxSession];
    if (self)
    {
        _emailInvitation = [[RoomEmailInvitation alloc] initWithParams:emailInvitationParams];

        // Report decoded data
        _roomName = _emailInvitation.roomName;
        _roomAvatarUrl = _emailInvitation.roomAvatarUrl;
    }
    return self;
}

- (instancetype)initWithPublicRoom:(MXPublicRoom*)publicRoom andSession:(MXSession*)mxSession
{
    self = [self initWithRoomId:publicRoom.roomId andSession:mxSession];
    if (self)
    {
        // Report public room data
        _roomName = publicRoom.name;
        _roomAvatarUrl = publicRoom.avatarUrl;
        _roomTopic = publicRoom.topic;
        _roomAliases = publicRoom.aliases;
        _numJoinedMembers = publicRoom.numJoinedMembers;
        
        if (!_roomName.length)
        {
            // Consider the room aliases to define a default room name.
            _roomName = _roomAliases.firstObject;
        }
    }
    return self;
}

- (void)dealloc
{
    if (_roomDataSource)
    {
        [_roomDataSource destroy];
        _roomDataSource = nil;
    }
    
    _emailInvitation = nil;
}

- (void)peekInRoom:(void (^)(BOOL succeeded))completion
{
    MXWeakify(self);
    [_mxSession peekInRoomWithRoomId:_roomId success:^(MXPeekingRoom *peekingRoom) {
        MXStrongifyAndReturnIfNil(self);

        // Create the room data source
        MXWeakify(self);
        [RoomDataSource loadRoomDataSourceWithPeekingRoom:peekingRoom andInitialEventId:self.eventId onComplete:^(id roomDataSource) {
            MXStrongifyAndReturnIfNil(self);

            self->_roomDataSource = roomDataSource;

            [self.roomDataSource finalizeInitialization];
            self.roomDataSource.markTimelineInitialEvent = YES;

            self->_roomName = peekingRoom.summary.displayname;
            self->_roomAvatarUrl = peekingRoom.summary.avatar;

            self->_roomTopic = [MXTools stripNewlineCharacters:peekingRoom.summary.topic];;
            self->_roomAliases = self.roomDataSource.roomState.aliases;

            // Room members count
            // Note that room members presence/activity is not available
             self->_numJoinedMembers = 0;
            for (MXRoomMember *mxMember in self.roomDataSource.roomState.members.members)
            {
                if (mxMember.membership == MXMembershipJoin)
                {
                     self->_numJoinedMembers ++;
                }
            }

            completion(YES);
        }];

    } failure:^(NSError *error) {
        MXStrongifyAndReturnIfNil(self);
        
        self->_roomName = self->_roomId;
        completion(NO);
    }];
}

@end
