struct nn_device_recpipe {
    int required_checks;
    int (*nn_device_entry)(
        struct nn_device_recipe *device, int s1, int s2, int flags);

    int (*nn_device_twoway)(struct nn_device_recipe *device, int s1, int s2);
    int (*nn_device_oneway)(struct nn_device_recipe *device, int s1, int s2);
    int (*nn_device_loopback)(struct nn_device_recipe *device, int s);
    int (*nn_device_mvmsg)(
        struct nn_device_recipe *device, int from, int to, int flags);
    int (*nn_device_rewritemsg)(struct nn_device_recipe *device, int from,
        int to, int flags, struct nn_msghdr *msghdr, int bytes);
};

int nn_device_loopback(struct nn_device_recipe *device, int s);
int nn_device_twoway(struct nn_device_recipe *device, int s1, int s2);
int nn_device_oneway(struct nn_device_recipe *device, int s1, int s2);
int nn_device_mvmsg(
    struct nn_device_recipe *device, int from, int to, int flags);
int nn_device_entry(struct nn_device_recipe *device, int s1, int s2, int flags);
int nn_device_rewritemsg(struct nn_device_recipe *device, int from, int to,
    int flags, struct nn_msghdr *msghdr, int bytes);

#define NN_CHECK_AT_LEAST_ONE_SOCKET (1 << 0)
#define NN_CHECK_ALLOW_LOOPBACK (1 << 1)
#define NN_CHECK_ALLOW_BIDIRECTIONAL (1 << 2)
#define NN_CHECK_ALLOW_UNIDIRECTIONAL (1 << 3)
#define NN_CHECK_REQUIRE_RAW_SOCKETS (1 << 4)
#define NN_CHECK_SAME_PROTOCOL_FAMILY (1 << 5)
#define NN_CHECK_SOCKET_DIRECTIONALITY (1 << 6)

int nn_custom_device(
    struct nn_device_recipe *device, int s1, int s2, int flags);

static struct nn_device_recipe nn_ordinary_device = {
    NN_CHECK_AT_LEAST_ONE_SOCKET | NN_CHECK_ALLOW_LOOPBACK
        | NN_CHECK_ALLOW_BIDIRECTIONAL | NN_CHECK_REQUIRE_RAW_SOCKETS
        | NN_CHECK_SAME_PROTOCOL_FAMILY | NN_CHECK_SOCKET_DIRECTIONALITY
        | NN_CHECK_ALLOW_UNIDIRECTIONAL,
    nn_device_entry,
    nn_device_twoway,
    nn_device_oneway,
    nn_device_loopback,
    nn_device_mvmsg,
    nn_device_rewritemsg,
};
