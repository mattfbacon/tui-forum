# API specification

## Data structures

```ts
type Token = string; // base-64 encoded

interface SessionData {
	id: UserKey;
}
interface SessionCreationData {
	token: Token;
}
interface SessionPostData {
	username: string;
	password: string;
}

interface User {
	// core data
	id: UserKey;
	username: string;
	display_name: string;
	// endpoint-only
	posts: Post[];
}

interface UserCreationData {
	username: string;
	display_name: string;
	password: string;
}
type UserCreationFailureReason =
	| "username taken"
	| "username invalid"
	| "display name invalid"
	| "password invalid";

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

### Indication of Completion

There is a three-check system:

1. Implemented on a basic level
2. Has authorization (including `self` as ID)
3. Verifies its operands

In terms of implementation priority, I will start with the first check, then work on the second, then the third.

Even if an item doesn't have authorization/verification, I will put an `x`, and check it at its time. The reason for this is that it might turn out that it does need that, and I don't want to ignore it.

A lack of any `✓`s or `x`s implies three `x`s.

```
GET /users/session : 200 SessionData | 404 (not logged in)
POST /users/session SessionPostData : 200 SessionCreationData | 403 Forbidden (invalid credentials)
DELETE /users/session : 204 (logged out) | 404 (not logged in)

>> where id is a natural number or "self" for the current user (using self adds 401 to the responses):
✓xx GET /users/id/:id : 200 User | 404
✓xx +PATCH /users/id/:id UserEditData : 200 User | 404 | 422 UserEditFailureReason
✓xx +DELETE /users/id/:id : 204 (user deleted) | 404

✓xx POST /users UserCreationData : 201 User | 422 UserCreationFailureReason

>> where name and display_name are strings
✓xx GET /users/username/:name : 200 User | 404
✓xx GET /users/display_name/:display_name : 200 User[] (which will be empty if there are no users with that display name)

GET /posts PostSearchParameters : 200 Post[] | 422 PostSearchFailureReason
>> where id is a number
GET /posts/:id : 200 Post | 404
+PATCH /posts/:id PostCreationData : 200 Post | 404
+POST /posts PostCreationData : 201 Post | 404
+DELETE /posts/:id : 204 (post deleted)| 422 PostDeletionFailureReason
```

418 = to be implemented
