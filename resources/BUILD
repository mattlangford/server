cc_library(
    name = "abstract_resource",
    hdrs = ["abstract_resource.hh"],
    visibility = ["//visibility:public"]
)

cc_library(
    name = "file_resource",
    srcs = ["file_resource.cc"],
    hdrs = ["file_resource.hh"],
    deps = [":abstract_resource"],
    visibility = ["//visibility:public"]
)

cc_library(
    name = "json_resource",
    hdrs = ["json_resource.hh"],
    deps = ["//json:json",
            ":abstract_resource"],
    visibility = ["//visibility:public"]
)

