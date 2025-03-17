# Benchmarks

I ran multiple time instantiation and setting of component data.

Bitmask impl:

Debug build:
1,864,243      stalled-cycles-backend:u         #   48.31% backend cycles idle
release build
1,864,243      stalled-cycles-backend:u         #   40.31% backend cycles idle

Component id list impl:

Debug build:
1,864,243      stalled-cycles-backend:u         #   40.31% backend cycles idle
release build
1,864,243      stalled-cycles-backend:u         #   40.31% backend cycles idle

The number of stalled cycles doesn't change in debug or release
we have an actual bottleneck in memory allocation
