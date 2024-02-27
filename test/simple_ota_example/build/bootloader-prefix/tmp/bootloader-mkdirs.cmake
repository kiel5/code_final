# Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
# file Copyright.txt or https://cmake.org/licensing for details.

cmake_minimum_required(VERSION 3.5)

file(MAKE_DIRECTORY
  "C:/Espressif/frameworks/esp-idf-v5.0/components/bootloader/subproject"
  "C:/Users/kien/Downloads/esp32/simple_ota_example/build/bootloader"
  "C:/Users/kien/Downloads/esp32/simple_ota_example/build/bootloader-prefix"
  "C:/Users/kien/Downloads/esp32/simple_ota_example/build/bootloader-prefix/tmp"
  "C:/Users/kien/Downloads/esp32/simple_ota_example/build/bootloader-prefix/src/bootloader-stamp"
  "C:/Users/kien/Downloads/esp32/simple_ota_example/build/bootloader-prefix/src"
  "C:/Users/kien/Downloads/esp32/simple_ota_example/build/bootloader-prefix/src/bootloader-stamp"
)

set(configSubDirs )
foreach(subDir IN LISTS configSubDirs)
    file(MAKE_DIRECTORY "C:/Users/kien/Downloads/esp32/simple_ota_example/build/bootloader-prefix/src/bootloader-stamp/${subDir}")
endforeach()
if(cfgdir)
  file(MAKE_DIRECTORY "C:/Users/kien/Downloads/esp32/simple_ota_example/build/bootloader-prefix/src/bootloader-stamp${cfgdir}") # cfgdir has leading slash
endif()
