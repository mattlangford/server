cc_library(
    name = "resources",
    srcs = ["resources.cc"],
    hdrs = ["resources.hh"]
)

cc_library(
    name = "messages",
    srcs = ["messages.cc"],
    hdrs = ["messages.hh"],
    deps = ["//tcp_server:tcp_server"]
)

cc_library(
    name = "http_server",
    srcs = ["http_server.cc"],
    hdrs = ["http_server.hh"],
    deps = ["//:logging",
            "//tcp_server:tcp_server",
            ":messages",
            ":resources"],
    linkopts = ["-lpthread"],
    visibility = ["//visibility:public"]
)
