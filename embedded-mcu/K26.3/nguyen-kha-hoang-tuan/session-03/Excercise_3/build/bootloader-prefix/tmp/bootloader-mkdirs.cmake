# Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
# file Copyright.txt or https://cmake.org/licensing for details.

cmake_minimum_required(VERSION 3.5)

file(MAKE_DIRECTORY
  "/home/richard/.espressif/v5.2/esp-idf/components/bootloader/subproject"
  "/home/richard/Programs/esp/homeworks/devlinux/embedded-mcu/K26.3/nguyen-kha-hoang-tuan/session-03/Excercise_3/build/bootloader"
  "/home/richard/Programs/esp/homeworks/devlinux/embedded-mcu/K26.3/nguyen-kha-hoang-tuan/session-03/Excercise_3/build/bootloader-prefix"
  "/home/richard/Programs/esp/homeworks/devlinux/embedded-mcu/K26.3/nguyen-kha-hoang-tuan/session-03/Excercise_3/build/bootloader-prefix/tmp"
  "/home/richard/Programs/esp/homeworks/devlinux/embedded-mcu/K26.3/nguyen-kha-hoang-tuan/session-03/Excercise_3/build/bootloader-prefix/src/bootloader-stamp"
  "/home/richard/Programs/esp/homeworks/devlinux/embedded-mcu/K26.3/nguyen-kha-hoang-tuan/session-03/Excercise_3/build/bootloader-prefix/src"
  "/home/richard/Programs/esp/homeworks/devlinux/embedded-mcu/K26.3/nguyen-kha-hoang-tuan/session-03/Excercise_3/build/bootloader-prefix/src/bootloader-stamp"
)

set(configSubDirs )
foreach(subDir IN LISTS configSubDirs)
    file(MAKE_DIRECTORY "/home/richard/Programs/esp/homeworks/devlinux/embedded-mcu/K26.3/nguyen-kha-hoang-tuan/session-03/Excercise_3/build/bootloader-prefix/src/bootloader-stamp/${subDir}")
endforeach()
if(cfgdir)
  file(MAKE_DIRECTORY "/home/richard/Programs/esp/homeworks/devlinux/embedded-mcu/K26.3/nguyen-kha-hoang-tuan/session-03/Excercise_3/build/bootloader-prefix/src/bootloader-stamp${cfgdir}") # cfgdir has leading slash
endif()
