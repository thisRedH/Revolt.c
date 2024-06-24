
# API Notes

## Endpoints

| URL                           |  Release   | Description                 |
| ----------------------------- | ---------- | --------------------------- |
| `https://api.revolt.chat`     | Production | Primary API endpoint        |
| `https://app.revolt.chat/api` | Production | API endpoint for old client |
| `https://revolt.chat/api`     | Staging    | API endpoint for new client |

| URL                              |  Release   | Description                    |
| -------------------------------- | ---------- | ------------------------------ |
| `wss://ws.revolt.chat`           | Production | Primary events endpoint        |
| `https://app.revolt.chat/events` | Production | Events endpoint for old client |
| `https://revolt.chat/events`     | Staging    | Events endpoint for new client |

## Routes

### USERS

#### User Information

| Method    | Route                          | Description                                                         |
| --------- | ------------------------------ | ------------------------------------------------------------------- |
| **GET**   | /users/@me                     | Retrieve your user information.                                     |
| **PATCH** | /users/@me/username            | Change your username.                                               |
| **GET**   | /users/{target}                | Retrieve a user's information.                                      |
| **PATCH** | /users/{target}                | Edit currently authenticated user. [Parameters][patch_users_target] |
| **GET**   | /users/{target}/flags          | Retrieve a user's flags.                                            |
| **GET**   | /users/{target}/default_avatar | This returns a default avatar based on the given id.                |
| **GET**   | /users/{target}/profile        | Retrieve a user's profile data.                                     |

[patch_users_target]: https://developers.revolt.chat/developers/api/reference.html#tag/user-information/patch/users/{target}

#### Direct Messaging​

| Method  | Route              | Description                                                                                  |
| ------- | ------------------ | -------------------------------------------------------------------------------------------- |
| **GET** | /users/dms         | This fetches your direct messages, including any DM and group DM conversations.              |
| **GET** | /users/{target}/dm | Open a DM with another user. If the target is oneself, a saved messages channel is returned. |

#### Relationships

| Method     | Route                  | Description                                                            |
| ---------- | ---------------------- | ---------------------------------------------------------------------- |
| **GET**    | /users/{target}/mutual | Retrieve a list of mutual friends and servers with another user.       |
| **POST**   | /users/friend          | Send a friend request to another user. [Parameters][post_users_friend] |
| **PUT**    | /users/{target}/friend | Accept another user's friend request.                                  |
| **DELETE** | /users/{target}/friend | Denies another user's friend request or removes an existing friend.    |
| **PUT**    | /users/{target}/block  | Block another user by their id.                                        |
| **DELETE** | /users/{target}/block  | Unblock another user by their id.                                      |

[post_users_friend]: https://developers.revolt.chat/developers/api/reference.html#tag/relationships/post/users/friend

### BOTS

Ignored

### Report Content

| Method   | Route          | Description                                                                        |
| -------- | -------------- | ---------------------------------------------------------------------------------- |
| **POST** | /safety/report | Report a piece of content to the moderation team. [Parameters][post_safety_report] |

[post_safety_report]: https://developers.revolt.chat/developers/api/reference.html#tag/user-safety
