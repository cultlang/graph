workspace(name = "cultlang_graph")

load("@bazel_tools//tools/build_defs/repo:http.bzl", "http_archive")
load("@bazel_tools//tools/build_defs/repo:git.bzl", "git_repository")


########################
# cultlang
git_repository(
    name = "cultlang_stdext",
    remote = "https://github.com/cultlang/stdext",
    commit = "4cc13150915535fea70e651934d0a9a76c8311a2",
    shallow_since = "1568672074 -0700",
)

########################
# catch
git_repository(
    name = "catch",
    remote = "https://github.com/cgrinker/Catch2",
    commit = "5e6488fd9949cb41d717a72c8c4603b7e37d68cd",
    shallow_since = "1560275997 -0700"
)

########################
# spdlog:
git_repository(
    name = "spdlog",
    remote = "https://github.com/cgrinker/spdlog",
    commit = "3bf4a07dc286e10fa32d6503fded647c9ee02d26",
    shallow_since = "1560271912 -0700"
)

########################
# fmt
git_repository(
    name = "fmt",
    remote = "https://github.com/cgrinker/fmt",
    commit = "f16f77297e6bb4df38d4c858edb3295f55716cb4",
    shallow_since = "1560552811 -0700"
)

########################
# replxx
git_repository(
    name = "replxx",
    remote = "https://github.com/cgrinker/replxx",
    commit = "46dfaf9bbebe517bfd1930f2e11f4b47b1a0b7f1",
    shallow_since = "1560556649 -0700",
)

########################
# pegtl
git_repository(
    name = "pegtl",
    remote = "https://github.com/mason-bially/PEGTL",
    commit = "7c6b89f5ee75fc7e47ff69a18851d841acd05ed6",
)

