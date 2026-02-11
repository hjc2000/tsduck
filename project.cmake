add_library(${ProjectName} OBJECT)

# 将资源目录用预定义传给 C++ 程序
target_compile_definitions(${ProjectName} PRIVATE TS_NO_SRT=1)
target_compile_definitions(${ProjectName} PRIVATE TS_NO_RIST=1)

target_import_src(${ProjectName})
target_import_base(${ProjectName} PUBLIC)
target_link_libraries(${ProjectName} PUBLIC winmm Userenv Ws2_32)
