project (base)
set(BASE_SOURCE
        aligned_alloc.h
        aligned_alloc.cpp
        bit_processor.h
        bit_processor.cpp
        bitrate_statist.h
        byte_array.h
        byte_array.cpp
        byte_fifo.h
        byte_fifo.cpp
        clock.h
        clock.cpp
        concurrent_queue.h
        crash_utils.h
        debug.h
        double_buffer.h
        endian.h
        errno.h
        errno.cpp
        event_loop.h
        event_loop.cpp
        frame_rate_statist.h
        global.h
        global.cpp
        log.h
        log.cpp
        performance_counter.h
        recyclable_object.h
        recyclable_object.cpp
        thread.h
        thread.cpp
        thread_pool.h
        uuid.h
        uuid.cpp
        url.h
        url.cpp
        encrypt/base64.h
        encrypt/base64.cpp
        encrypt/md5.h
        encrypt/md5.cpp
        gfx/color.h
        gfx/layout.h
        gfx/layout.cpp
        gfx/point.h
        gfx/rect.h
        gfx/size.h
        gfx/style_sheet.h
        gfx/style_sheet.cpp
        io/io_context.h
        io/io_context.cpp
        io/io_device.h
        io/io_device.cpp
        io/timer.h
        io/timer.cpp
        #json/json_helper.h
        #json/json_object.h
        #json/json_object.cpp
        #json/json2.hpp
        string/string_utils.h
        string/string_utils.cpp
        string/text_codec.cpp
        string/text_codec.h
        string/text_codec_icu.cpp
        string/text_codec_icu.h
        string/text_codec_icu_mib.cpp
        win/getopt.h
        win/getopt.c)

set(BASE_UNIT_TEST_SOURCE
        aligned_alloc_unittest.cpp
        bit_processor_unittest.cpp
        bitrate_statist_unittest.cpp
        byte_array_unittest.cpp
        byte_fifo_unittest.cpp
        clock_unittest.cpp
        concurrent_queue_unittest.cpp
        double_buffer_unittest.cpp
        endian_unittest.cpp
        errno_unittest.cpp
        event_loop_unittest.cpp
        frame_rate_statist_unittest.cpp
        global_unittest.cpp
        log_unittest.cpp
        performance_counter_unittest.cpp
        recyclable_object_unittest.cpp
        thread_unittest.cpp
        thread_pool_unittest.cpp
        uuid_unittest.cpp
        url_unittest.cpp
        encrypt/base64_unittest.cpp
        encrypt/md5_unittest.cpp
        gfx/color_unittest.cpp
        gfx/layout_unittest.cpp
        gfx/point_unittest.cpp
        gfx/rect_unittest.cpp
        gfx/size_unittest.cpp
        gfx/style_sheet_unittest.cpp
        io/io_device_unittest.cpp
        io/io_context_unittest.cpp
        io/timer_unittest.cpp
        string/text_codec_unittest.cpp
        unittest_main.cpp)

if (CMAKE_SYSTEM_NAME STREQUAL "Android")
    set(BASE_SOURCE ${BASE_SOURCE} crash_utils_android.cpp)
else ()
    set(BASE_SOURCE ${BASE_SOURCE} crash_utils.cpp)
endif ()

add_library(${PROJECT_NAME} STATIC ${BASE_SOURCE})
add_executable(${PROJECT_NAME}_unittest ${BASE_UNIT_TEST_SOURCE})

if (CMAKE_SYSTEM_NAME STREQUAL "Windows")
    target_link_libraries(${PROJECT_NAME}_unittest ${GTEST_LIBS} base ${ICU_LIBS})
elseif (CMAKE_SYSTEM_NAME STREQUAL "Android")
    target_link_libraries(${PROJECT_NAME} ${UUID_LIBS} ${ICU_LIBS} log)
    target_link_libraries(${PROJECT_NAME}_unittest ${GTEST_LIBS} ${UUID_LIBS} base)
elseif (CMAKE_SYSTEM_NAME STREQUAL "Linux")
    target_link_libraries(${PROJECT_NAME} ${UUID_LIBS} ${ICU_LIBS})
    target_link_libraries(${PROJECT_NAME}_unittest ${GTEST_LIBS} base pthread dl)
elseif (CMAKE_SYSTEM_NAME STREQUAL "Darwin")
    target_link_libraries(${PROJECT_NAME} ${UUID_LIBS} ${ICU_LIBS})
    target_link_libraries(${PROJECT_NAME}_unittest ${GTEST_LIBS} base pthread)
endif ()

add_test(unittest_for_base_library
         ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${PROJECT_NAME}_unittest)

# Arrange project structure for visual studio
assign_source_group(${BASE_SOURCE})
assign_source_group(${BASE_UNIT_TEST_SOURCE})
set_target_properties(${PROJECT_NAME} PROPERTIES FOLDER Libraries)
set_target_properties(${PROJECT_NAME}_unittest PROPERTIES FOLDER Libraries/UnitTests)

if (CMAKE_SYSTEM_NAME STREQUAL "Windows")
    set_target_properties(${PROJECT_NAME} PROPERTIES COMPILE_FLAGS "/MP" )
    set_target_properties(${PROJECT_NAME}_unittest PROPERTIES COMPILE_FLAGS "/MP" )
endif ()

