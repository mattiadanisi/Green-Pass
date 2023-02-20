cmake_minimum_required(VERSION 3.23)
project(green_pass C)

set(CMAKE_C_STANDARD 99)

add_executable(GREEN_PASS ServerV.c ServerV.h CentroVaccinale.c CentroVaccinale.h ServerG.c ServerG.h Wrapper.c Wrapper.h ClientS.c ClientA.c ClientT.c)
