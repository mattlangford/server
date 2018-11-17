cc_binary(
    name = "main",
    srcs = ["main.cc"],
    deps = ["//http_server:http_server",
            "//resources:file_resource",
            ":logging"],
)

cc_library(
    name = "logging",
    hdrs = ["logging.hh"],
    visibility = ["//visibility:public"]
)
