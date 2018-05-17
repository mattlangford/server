cc_library(
    name = "json",
    srcs = ["json.cc"],
    hdrs = ["json.hh"],
    deps = ["@variant//:variant"],
    visibility = ["//visibility:public"]
)

cc_test(
    name = "json_tests",
    srcs = ["json_tests.cc"],
    deps = [":json"]
)
