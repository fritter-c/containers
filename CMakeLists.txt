add_subdirectory(tests)

add_library(gtrcontainers INTERFACE allocators/allocators.h
                                    allocator_base.h
                                    circular_buffer.h
                                    deque.h
                                    hashmap.h
                                    linkedlist.h
                                    matrix.h
                                    pair.h
                                    queue.h
                                    stack.h
                                    static_string.h
                                    text.h
                                    tuple.h
                                    vector.h)

add_library(gtr::containers ALIAS gtrcontainers)