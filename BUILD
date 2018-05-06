cc_binary(
    name = "main",
    srcs = ["main.cc"],
    deps = [":http_server"],
)

cc_library(
    name = "logging",
    hdrs = ["logging.hh"]
)

cc_library(
    name = "http_messages",
    srcs = ["messages.cc"],
    hdrs = ["messages.hh"],
    deps = ["//tcp_server:tcp_server"]
)

cc_library(
    name = "http_server",
    srcs = ["http_server.cc"],
    hdrs = ["http_server.hh"],
    deps = ["//tcp_server:tcp_server",
            ":http_messages",
            ":logging"],
    linkopts = ["-lpthread"]
)

cc_binary(
    name = "message_tests",
    srcs = ["message_tests.cc"],
    deps = [":http_messages"]
)
