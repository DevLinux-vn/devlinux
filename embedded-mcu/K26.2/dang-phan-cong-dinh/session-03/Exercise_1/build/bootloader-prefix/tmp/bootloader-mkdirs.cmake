# Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
# file Copyright.txt or https://cmake.org/licensing for details.

cmake_minimum_required(VERSION 3.5)

file(MAKE_DIRECTORY
  "C:/Users/hanph/esp/esp-idf/components/bootloader/subproject"
  "C:/Users/hanph/ESP32S3/03_GPIO/Button-Driven/build/bootloader"
  "C:/Users/hanph/ESP32S3/03_GPIO/Button-Driven/build/bootloader-prefix"
  "C:/Users/hanph/ESP32S3/03_GPIO/Button-Driven/build/bootloader-prefix/tmp"
  "C:/Users/hanph/ESP32S3/03_GPIO/Button-Driven/build/bootloader-prefix/src/bootloader-stamp"
  "C:/Users/hanph/ESP32S3/03_GPIO/Button-Driven/build/bootloader-prefix/src"
  "C:/Users/hanph/ESP32S3/03_GPIO/Button-Driven/build/bootloader-prefix/src/bootloader-stamp"
)

set(configSubDirs )
foreach(subDir IN LISTS configSubDirs)
    file(MAKE_DIRECTORY "C:/Users/hanph/ESP32S3/03_GPIO/Button-Driven/build/bootloader-prefix/src/bootloader-stamp/${subDir}")
endforeach()
if(cfgdir)
  file(MAKE_DIRECTORY "C:/Users/hanph/ESP32S3/03_GPIO/Button-Driven/build/bootloader-prefix/src/bootloader-stamp${cfgdir}") # cfgdir has leading slash
endif()
