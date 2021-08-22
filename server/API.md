# API specification

## Data structures

```ts
interface SessionData {
	// core data
	token: SessionToken; // TODO: haven't decided on the type yet
	id: UserKey;
}
interface SessionPostData {
	username: string;
	password: string;
}
type SessionPostFailureReason = "invalid credentials";

type hash = std::array<char, 64>;

interface User {
	// core data
	id: UserKey;
	username: string;
	password: hash;
	display_name: string;
	// endpoint-only
	posts: Post[];
}

interface UserCreationData {
	username: string;
	display_name: string;
	password: string;
}
type UserCreationFailureReason = "username taken" | "username invalid" | "display name invalid" | "password invalid";

interface UserEditData {
	display_name?: string;
	password?: string;
}
type UserEditFailureReason = "display name invalid" | "password invalid";

interface Post {
	// core data
	id: PostKey;
	parent: PostKey | null;
	title: string;
	author: UserKey;
	// endpoint-only
	body: string;
	children: Post[];
}

interface PostCreationData {
	parent: PostKey | null;
	title: string;
	body: string;
}
type PostCreationFailureReason = "nonexistent parent";

interface PostEditData {
	title?: string;
	body?: string;
}

interface PostSearchParameters {
	// pagination options
	amount: number;
	page: number; // zero-indexed
	// search options
	title: string;
	author: UserKey;
	// may add body searching later but not now
	parent: PostKey;
}
type PostSearchFailureReason = "amount too large";
```

## Endpoints

Any request with content where the content is not in MessagePack format, or with invalid content at the schema level, will get 400.

For a GET request with query parameters, all of the parameters are optional by definition.

A plus before the method means authentication is required; the request can fail with either 401 (not logged in) or 403 (logged in but without proper permission)

```
GET /users/session : 200 SessionData | 404 (not logged in)
POST /users/session SessionPostData : 201 SessionData (new session from logged-out state) | 205 SessionData (session replaced) | 422 SessionPostFailureReason
DELETE /users/session : 204 (logged out, will contain cookie delete header) | 404 (not logged in)

>> where id is a natural number or "self" for the current user (using self adds 401 to the responses):
GET /users/id/:id : 200 User | 404
+PATCH /users/id/:id UserEditData : 200 User | 404 | 422 UserEditFailureReason
+DELETE /users/id/:id : 204 (user deleted) | 404

POST /users UserCreationData : 201 User | 422 UserCreationFailureReason

>> where name and displayname are strings
GET /users/username/:name : 200 User | 404
GET /users/displayname/:displayname : 200 User[] (which will be empty if there are no users with that display name)

GET /posts PostSearchParameters : 200 Post[] | 422 PostSearchFailureReason
>> where id is a number
GET /posts/:id : 200 Post | 404
+PATCH /posts/:id PostCreationData : 200 Post | 404
+POST /posts PostCreationData : 201 Post | 404
+DELETE /posts/:id : 204 (post deleted)| 422 PostDeletionFailureReason
```

418 = to be implemented
