{
  "targets": [
    {
      "target_name": "ina226_native",
      "sources": [ 
          "src/c-src/errors.c",
          "src/c-src/extension.c",
          "src/c-src/fn-bindings.c",
          "src/c-src/driver_ina226_interface.c",
          "src/c-src/structs.c",
          "src/ina226/src/driver_ina226.c"
      ],
      "include_dirs": [
        # "/usr/local/include/",  # Uncomment if needed 
        "src/c-include/",
        "src/ina226/src/"
      ],
      "cflags!": [ 
            "-Wall",               # basic warnings
            "-Wextra",             # extra warnings
            "-Wpedantic",          # strict ISO compliance
            "-Wconversion",        # implicit conversions
            "-Wshadow",            # variable shadowing
            "-Wcast-align",        # alignment casts
            "-Wformat-security",   # printf/scanf security
            "-Wnull-dereference",  # null pointer checks
            "-Werror"              # turn *all* warnings into errors
        ],
    }
  ]
}
