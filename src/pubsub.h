#ifndef PUBSUB_H_INCLUDED
#define PUBSUB_H_INCLUDED

#ifdef __cplusplus
extern "C" {
#endif

#define NN_PROTO_PUBSUB (2)

#define NN_PUB (NN_PROTO_PUBSUB * 16 + 0)
#define NN_SUB (NN_PROTO_PUBSUB * 16 + 1)

#define NN_SUB_SUBSCRIBE (1)
#define NN_SUB_UNSUBSCRIBE (2)

#ifdef __cplusplus
}
#endif

#endif // !PUBSUB_H_INCLUDED
