cc_library(
    name = "messages",
    srcs = ["messages.cc"],
    hdrs = ["messages.hh"],
    deps = ["//tcp_server:tcp_server"],
    visibility = ["//visibility:public"]
)

cc_library(
    name = "http_server",
    srcs = ["http_server.cc"],
    hdrs = ["http_server.hh"],
    deps = ["//:logging",
            "//tcp_server:tcp_server",
            "//resources:abstract_resource",
            ":messages"],
    linkopts = ["-lpthread"],
    visibility = ["//visibility:public"]
)
