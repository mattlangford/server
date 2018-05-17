cc_library(
    name = "json",
    srcs = ["json.cc",
            "parser.cc"],
    hdrs = ["json.hh",
            "parser.hh"],
    deps = ["@variant//:variant"],
    visibility = ["//visibility:public"]
)

cc_test(
    name = "json_tests",
    srcs = ["json_tests.cc"],
    deps = [":json"]
)
